#include "stomachcontent.h"
#include "areatime.h"
#include "predator.h"
#include "prey.h"
#include "predatoraggregator.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "multinomial.h"
#include "errorhandler.h"
#include "formatedprinting.h"
#include "gadget.h"

extern ErrorHandler handle;

StomachContent::StomachContent(CommentStream& infile,
  const AreaClass* const Area, const TimeClass* const TimeInfo,
  Keeper* const keeper, double weight, const char* name)
  : Likelihood(STOMACHCONTENTLIKELIHOOD, weight) {

  char datafilename[MaxStrLength];
  char numfilename[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  strncpy(numfilename, "", MaxStrLength);

  stomachname = new char[strlen(name) + 1];
  strcpy(stomachname, name);

  functionname = new char[MaxStrLength];
  strncpy(functionname, "", MaxStrLength);
  readWordAndValue(infile, "function", functionname);
  readWordAndValue(infile, "datafile", datafilename);

  if (strcasecmp(functionname, "scnumbers") == 0)
    functionnumber = 1;
  else if (strcasecmp(functionname, "scratios") == 0)
    functionnumber = 2;
  else if (strcasecmp(functionname, "scamounts") == 0)
    functionnumber = 3;
  else
    handle.Message("Error in stomachcontent - unrecognised function", functionname);

  switch(functionnumber) {
    case 1:
      StomCont = new SCNumbers(infile, Area, TimeInfo, keeper, datafilename, numfilename, stomachname);
      break;
    case 2:
      readWordAndValue(infile, "numberfile", numfilename);
      StomCont = new SCRatios(infile, Area, TimeInfo, keeper, datafilename, numfilename, stomachname);
      break;
    case 3:
      readWordAndValue(infile, "numberfile", numfilename);
      StomCont = new SCAmounts(infile, Area, TimeInfo, keeper, datafilename, numfilename, stomachname);
      break;
    default:
      handle.Message("Unrecognized functionnumber");
  }
}

StomachContent::~StomachContent() {
  delete StomCont;
  delete[] stomachname;
  delete[] functionname;
}

void StomachContent::addLikelihood(const TimeClass* const TimeInfo) {
  double l = StomCont->Likelihood(TimeInfo);
  if (!(isZero(l))) {
    likelihood += l;
    handle.logMessage("The likelihood score for this component has increased to", likelihood);
  }
}

void StomachContent::Print(ofstream& outfile) const {
  outfile << "\nStomach Content " << stomachname << " - likelihood value " << likelihood
    << "\n\tfunction " << functionname << endl;
  StomCont->Print(outfile);
}

SC::SC(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper,
  const char* datafilename, const char* name) {

  int i, j;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int numarea = 0;
  int numpred = 0;
  int numprey = 0;

  aggregator = 0;
  preyLgrpDiv = 0;
  predLgrpDiv = 0;
  timeindex = 0;

  scname = new char[strlen(name) + 1];
  strcpy(scname, name);

  char aggfilename[MaxStrLength];
  strncpy(aggfilename, "", MaxStrLength);

  ifstream datafile;
  CommentStream subdata(datafile);

  //JMB - changed to make the reading of minimum probability optional
  infile >> ws;
  char c = infile.peek();
  if ((c == 'm') || (c == 'M'))
    readWordAndVariable(infile, "minimumprobability", epsilon);
  else if ((c == 'e') || (c == 'E'))
    readWordAndVariable(infile, "epsilon", epsilon);
  else
    epsilon = 10;

  if (epsilon <= 0) {
    handle.Warning("Epsilon should be a positive integer - set to default value 10");
    epsilon = 10;
  }

  //read in area aggregation from file
  readWordAndValue(infile, "areaaggfile", aggfilename);
  datafile.open(aggfilename, ios::in);
  handle.checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  numarea = readAggregation(subdata, areas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Must change from outer areas to inner areas.
  for (i = 0; i < areas.Nrow(); i++)
    for (j = 0; j < areas.Ncol(i); j++)
      if ((areas[i][j] = Area->InnerArea(areas[i][j])) == -1)
        handle.UndefinedArea(areas[i][j]);

  //read in the predators
  i = 0;
  infile >> text >> ws;
  if (strcasecmp(text, "predators") != 0)
    handle.Unexpected("predators", text);
  infile >> text >> ws;
  while (!infile.eof() && ((strcasecmp(text, "predatorlengths") != 0)
      && (strcasecmp(text, "predatorages") != 0))) {
    predatornames.resize(1);
    predatornames[i] = new char[strlen(text) + 1];
    strcpy(predatornames[i++], text);
    infile >> text >> ws;
  }

  if (strcasecmp(text, "predatorlengths") == 0) { //read predator lengths
    age_pred = 0; //predator is length structured
    readWordAndValue(infile, "lenaggfile", aggfilename);
    datafile.open(aggfilename, ios::in);
    handle.checkIfFailure(datafile, aggfilename);
    handle.Open(aggfilename);
    numpred = readLengthAggregation(subdata, predatorlengths, predindex);
    handle.Close();
    datafile.close();
    datafile.clear();
  } else if (strcasecmp(text, "predatorages") == 0) { //read predator ages
    age_pred = 1; //predator is age structured
    readWordAndValue(infile, "ageaggfile", aggfilename);
    datafile.open(aggfilename, ios::in);
    handle.checkIfFailure(datafile, aggfilename);
    handle.Open(aggfilename);
    numpred = readAggregation(subdata, predatorages, predindex);
    handle.Close();
    datafile.close();
    datafile.clear();
  } else
    handle.Unexpected("predatorlengths or predatorages", text);

  //read in the preys
  readWordAndValue(infile, "preyaggfile", aggfilename);
  datafile.open(aggfilename, ios::in);
  handle.checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  numprey = readPreyAggregation(subdata, preynames, preylengths, digestioncoeff, preyindex, keeper);
  handle.Close();
  datafile.close();
  datafile.clear();

  //prepare for next likelihood component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[component]") == 0))
      handle.Unexpected("[component]", text);
  }
}

void SC::Aggregate(int i) {
  if (age_pred)
    aggregator[i]->MeanSum();
  else
    aggregator[i]->Sum();
}

double SC::Likelihood(const TimeClass* const TimeInfo) {
  if (!AAT.AtCurrentTime(TimeInfo))
    return 0.0;


  int i, a, k;
  int prey_g = 0, prey_l = 0;
  double length = 0.0, mult = 0.0;

  int pred_size = 0;
  if (age_pred) //age structured predator
    pred_size = predatorages.Size();
  else
    pred_size = predatorlengths.Size() - 1;

  //The vector which will keep the consumption, indexed the same way as in stomachcontent
  DoubleMatrixPtrVector consumption(areas.Nrow());
  for (a = 0; a < areas.Nrow(); a++)
    consumption[a] = new DoubleMatrix(stomachcontent[timeindex][0]->Nrow(),
      stomachcontent[timeindex][0]->Ncol(), 0.0);

  handle.logMessage("Calculating likelihood score for stomachcontent component", scname);
  //Get the consumption from aggregator, indexed the same way as in stomachcontent
  //While we're at it, get the sum of each row, for later scaling.
  DoubleMatrix sum(areas.Nrow(), pred_size, 0.0);
  for (i = 0; i < preynames.Nrow(); i++) {
    Aggregate(i);
    const BandMatrixVector* cons = &aggregator[i]->returnSum();
    for (a = 0; a < areas.Nrow(); a++) {
      for (k = 0; k < (*cons)[a].Nrow(); k++) {
        for (prey_l = 0; prey_l < (*cons)[a].Ncol(); prey_l++) {
          length = (preylengths[i][prey_l] + preylengths[i][prey_l + 1]) * 0.5;
          mult = digestioncoeff[i][0] + digestioncoeff[i][1] *
            pow(length, digestioncoeff[i][2]);
          (*consumption[a])[k][prey_g + prey_l] = (*cons)[a][k][prey_l] * mult;
          sum[a][k] += (*cons)[a][k][prey_l] * mult;
        }
      }
    }
    prey_g += (preylengths[i].Size() - 1);
  }

  for (a = 0; a < consumption.Size(); a++)
    modelConsumption[timeindex][a] = new DoubleMatrix(*consumption[a]);

  //Now, calculate likelihood and cleanup
  double l = CalculateLikelihood(consumption, sum);
  for (a = 0; a < areas.Nrow(); a++)
    delete consumption[a];
  timeindex++;
  return l;
}

SCNumbers::SCNumbers(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper,
  const char* datafilename, const char* numfilename, const char* name)
  : SC(infile, Area, TimeInfo, keeper, datafilename, name) {

  ifstream datafile;
  CommentStream subdata(datafile);
  //read in stomach content from file
  datafile.open(datafilename, ios::in);
  handle.checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  readStomachNumberContent(subdata, TimeInfo);
  handle.Close();
  datafile.close();
  datafile.clear();
}

SCAmounts::SCAmounts(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper,
  const char* datafilename, const char* numfilename, const char* name)
  : SC(infile, Area, TimeInfo, keeper, datafilename, name) {

  ifstream datafile;
  CommentStream subdata(datafile);
  //read in stomach content amounts from file
  datafile.open(datafilename, ios::in);
  handle.checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  readStomachAmountContent(subdata, TimeInfo);
  handle.Close();
  datafile.close();
  datafile.clear();

  //read in stomach content sample size from file
  datafile.open(numfilename, ios::in);
  handle.checkIfFailure(datafile, numfilename);
  handle.Open(numfilename);
  readStomachSampleContent(subdata, TimeInfo);
  handle.Close();
  datafile.close();
  datafile.clear();
}

void SCNumbers::readStomachNumberContent(CommentStream& infile, const TimeClass* const TimeInfo) {

  int i;
  int year, step;
  double tmpnumber;
  char tmparea[MaxStrLength], tmppred[MaxStrLength], tmpprey[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmppred, "", MaxStrLength);
  strncpy(tmpprey, "", MaxStrLength);
  int count = 0;
  int keepdata, timeid, areaid, predid, preyid;

  int pred_size = 0;
  if (age_pred) //age structured predator
    pred_size = predatorages.Size();
  else
    pred_size = predatorlengths.Size() - 1;

  int numarea = areas.Nrow();
  int nopreygroups = 0;
  for (i = 0; i < preylengths.Nrow(); i++)
    nopreygroups += preylengths[i].Size() - 1;

  //Find start of distribution data in datafile
  infile >> ws;
  char c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  //Check the number of columns in the inputfile
  if (countColumns(infile) != 6)
    handle.Message("Wrong number of columns in inputfile - should be 6");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> tmparea >> tmppred >> tmpprey >> tmpnumber >> ws;

    //if tmparea is in areaindex find areaid, else dont keep the data
    areaid = -1;
    for (i = 0; i < areaindex.Size(); i++)
      if (strcasecmp(areaindex[i], tmparea) == 0)
        areaid = i;

    if (areaid == -1)
      keepdata = 1;

    //if tmppred is in predindex find predid, else dont keep the data
    predid = -1;
    for (i = 0; i < predindex.Size(); i++)
      if (strcasecmp(predindex[i], tmppred) == 0)
        predid = i;

    if (predid == -1)
      keepdata = 1;

    //if tmpprey is in preyindex find preyid, else dont keep the data
    preyid = -1;
    for (i = 0; i < preyindex.Size(); i++)
      if (strcasecmp(preyindex[i], tmpprey) == 0)
        preyid = i;

    if (preyid == -1)
      keepdata = 1;

    //check if the year and step are in the simulation
    timeid = -1;
    if ((TimeInfo->IsWithinPeriod(year, step)) && (keepdata == 0)) {
      //if this is a new timestep, resize to store the data
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        timeid = Years.Size() - 1;

        stomachcontent.AddRows(1, numarea);
        for (i = 0; i < numarea; i++)
          stomachcontent[timeid][i] = new DoubleMatrix(pred_size, nopreygroups, 0.0);
      }

    } else {
      //dont keep the data
      keepdata = 1;
    }

    if (keepdata == 0) {
      //stomach content data is required, so store it
      count++;
      (*stomachcontent[timeid][areaid])[predid][preyid] = tmpnumber;
    }
  }

  AAT.addActions(Years, Steps, TimeInfo);
  if (count == 0)
    handle.logWarning("Warning in stomachcontent - found no data in the data file for", scname);
  else
    modelConsumption.AddRows(stomachcontent.Nrow(), stomachcontent.Ncol(), 0);
  handle.logMessage("Read stomachcontent data file - number of entries", count);
}

void SCAmounts::readStomachAmountContent(CommentStream& infile, const TimeClass* const TimeInfo) {

  int i;
  int year, step;
  double tmpnumber, tmpstddev;
  char tmparea[MaxStrLength], tmppred[MaxStrLength], tmpprey[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmppred, "", MaxStrLength);
  strncpy(tmpprey, "", MaxStrLength);
  int count = 0;
  int keepdata, timeid, areaid, predid, preyid;

  int pred_size = 0;
  if (age_pred) //age structured predator
    pred_size = predatorages.Size();
  else
    pred_size = predatorlengths.Size() - 1;

  int numarea = areas.Nrow();
  int nopreygroups = 0;
  for (i = 0; i < preylengths.Nrow(); i++)
    nopreygroups += preylengths[i].Size() - 1;

  //Find start of distribution data in datafile
  infile >> ws;
  char c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  //Check the number of columns in the inputfile
  if (countColumns(infile) != 7)
    handle.Message("Wrong number of columns in inputfile - should be 7");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> tmparea >> tmppred >> tmpprey >> tmpnumber >> tmpstddev >> ws;

    //if tmparea is in areaindex find areaid, else dont keep the data
    areaid = -1;
    for (i = 0; i < areaindex.Size(); i++)
      if (strcasecmp(areaindex[i], tmparea) == 0)
        areaid = i;

    if (areaid == -1)
      keepdata = 1;

    //if tmppred is in predindex find predid, else dont keep the data
    predid = -1;
    for (i = 0; i < predindex.Size(); i++)
      if (strcasecmp(predindex[i], tmppred) == 0)
        predid = i;

    if (predid == -1)
      keepdata = 1;

    //if tmpprey is in preyindex find preyid, else dont keep the data
    preyid = -1;
    for (i = 0; i < preyindex.Size(); i++)
      if (strcasecmp(preyindex[i], tmpprey) == 0)
        preyid = i;

    if (preyid == -1)
      keepdata = 1;

    //check if the year and step are in the simulation
    timeid = -1;
    if ((TimeInfo->IsWithinPeriod(year, step)) && (keepdata == 0)) {
      //if this is a new timestep, resize to store the data
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        timeid = Years.Size() - 1;

        stomachcontent.AddRows(1, numarea);
        stddev.AddRows(1, numarea);
        for (i = 0; i < numarea; i++) {
          stomachcontent[timeid][i] = new DoubleMatrix(pred_size, nopreygroups, 0.0);
          stddev[timeid][i] = new DoubleMatrix(pred_size, nopreygroups, 0.0);
        }
      }

    } else {
      //dont keep the data
      keepdata = 1;
    }

    if (keepdata == 0) {
      //stomach content data is required, so store it
      count++;
      (*stomachcontent[timeid][areaid])[predid][preyid] = tmpnumber;
      (*stddev[timeid][areaid])[predid][preyid] = tmpstddev;
    }
  }

  AAT.addActions(Years, Steps, TimeInfo);
  if (count == 0)
    handle.logWarning("Warning in stomachcontent - found no data in the data file for", scname);
  else
    modelConsumption.AddRows(stomachcontent.Nrow(), stomachcontent.Ncol(), 0);
  handle.logMessage("Read stomachcontent data file - number of entries", count);
}

void SCAmounts::readStomachSampleContent(CommentStream& infile, const TimeClass* const TimeInfo) {

  int i;
  int year, step;
  double tmpnumber;
  int keepdata, timeid, areaid, predid;
  char tmparea[MaxStrLength], tmppred[MaxStrLength];
  int count = 0;
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmppred, "", MaxStrLength);

  int pred_size = 0;
  if (age_pred) //age structured predator
    pred_size = predatorages.Size();
  else
    pred_size = predatorlengths.Size() - 1;

  //We know the size that numbers[] will be from stomachcontent
  int numarea = areas.Nrow();
  number.resize(stomachcontent.Nrow());
  for (i = 0; i < stomachcontent.Nrow(); i++)
    number[i] = new DoubleMatrix(numarea, pred_size, 0.0);

  //Find start of distribution data in datafile
  infile >> ws;
  char c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  //Check the number of columns in the inputfile
  if (countColumns(infile) != 5)
    handle.Message("Wrong number of columns in inputfile - should be 5");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> tmparea >> tmppred >> tmpnumber >> ws;

    //check if the year and step are in the simulation
    timeid = -1;
    if (TimeInfo->IsWithinPeriod(year, step))
      //find the timeid from Years and Steps
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

    if (timeid == -1)
      keepdata = 1;

    //if tmparea is in areaindex find areaid, else dont keep the data
    areaid = -1;
    for (i = 0; i < areaindex.Size(); i++)
      if (strcasecmp(areaindex[i], tmparea) == 0)
        areaid = i;

    if (areaid == -1)
      keepdata = 1;

    //if tmppred is in predindex find predid, else dont keep the data
    predid = -1;
    for (i = 0; i < predindex.Size(); i++)
      if (strcasecmp(predindex[i], tmppred) == 0)
        predid = i;

    if (predid == -1)
      keepdata = 1;

    if (keepdata == 0) {
      //stomach content data is required, so store it
      count++;
      (*number[timeid])[areaid][predid] = tmpnumber;
    }
  }
  if (count == 0)
    handle.logWarning("Warning in stomachcontent - found no data in the data file for", scname);
  handle.logMessage("Read stomachcontent data file - number of entries", count);
}

SC::~SC() {
  int i, j;
  for (i = 0; i < stomachcontent.Nrow(); i++)
    for (j = 0; j < stomachcontent[i].Size(); j++) {
      delete stomachcontent[i][j];
      delete modelConsumption[i][j];
    }

  for (i = 0; i < preynames.Nrow(); i++) {
    delete aggregator[i];
    delete preyLgrpDiv[i];
    for (j = 0; j < preynames[i].Size(); j++)
      delete[] preynames[i][j];
  }
  delete[] scname;

  if (aggregator != 0) {
    delete[] aggregator;
    aggregator = 0;
  }
  if (preyLgrpDiv != 0) {
    delete[] preyLgrpDiv;
    preyLgrpDiv = 0;
  }

  if (age_pred == 0)
    for (i = 0; i < preynames.Nrow(); i++)
      delete predLgrpDiv[i];

  if (predLgrpDiv != 0) {
    delete[] predLgrpDiv;
    predLgrpDiv = 0;
  }

  for (i = 0; i < predatornames.Size(); i++)
    delete[] predatornames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < predindex.Size(); i++)
    delete[] predindex[i];
  for (i = 0; i < preyindex.Size(); i++)
    delete[] preyindex[i];
}

void SC::setPredatorsAndPreys(PredatorPtrVector& Predators, PreyPtrVector& Preys) {
  int i, j, k, found;
  PredatorPtrVector predators;
  aggregator = new PredatorAggregator*[preynames.Nrow()];

  if (age_pred == 0) { //the test is done in aggregator for age structured predators
    for (i = 0; i < predatornames.Size(); i++) {
      found = 0;
      for (j = 0; j < Predators.Size(); j++)
        if (strcasecmp(predatornames[i], Predators[j]->Name()) == 0) {
          found++;
          predators.resize(1, Predators[j]);
        }

      if (found == 0)
        handle.logFailure("Error in stomachcontent - failed to match predator", predatornames[i]);

    }
  }

  preyLgrpDiv = new LengthGroupDivision*[preynames.Nrow()];
  if (age_pred == 0)
    predLgrpDiv = new LengthGroupDivision*[preynames.Nrow()];

  for (i = 0; i < preynames.Nrow(); i++) {
    PreyPtrVector preys;
    for (j = 0; j < preynames[i].Size(); j++) {
      found = 0;
      for (k = 0; k < Preys.Size(); k++)
        if (strcasecmp(preynames[i][j], Preys[k]->Name()) == 0) {
          found++;
          preys.resize(1, Preys[k]);
        }

      if (found == 0)
        handle.logFailure("Error in stomachcontent - failed to match prey", preynames[i][j]);

    }

    preyLgrpDiv[i] = new LengthGroupDivision(preylengths[i]);
    if (age_pred == 0) { //length structured predator
      predLgrpDiv[i] = new LengthGroupDivision(predatorlengths);
      aggregator[i] = new PredatorAggregator(predators, preys, areas, predLgrpDiv[i], preyLgrpDiv[i]);
    } else
      aggregator[i] = new PredatorAggregator(predatornames, preys, areas, predatorages, preyLgrpDiv[i]);
  }
}

void SC::Print(ofstream& outfile) const {
  int i, j, y, ar;

  outfile << "\tPreys:\n";
  for (i = 0; i < preynames.Nrow(); i++) {
    outfile << "\t\t";
    for (j = 0; j < preynames[i].Size(); j++)
      outfile << preynames[i][j] << sep;
    outfile << "\n\t\tlengths: ";
    for (j = 0; j < preylengths[i].Size(); j++)
      outfile << preylengths[i][j] << sep;
    outfile << endl;
  }

  outfile << "\tPredators: " << sep;
  for (i = 0; i < predatornames.Size(); i++)
    outfile << predatornames[i] << sep;

  if (age_pred) {
    outfile << "\n\t\tages: ";
    for (i = 0; i < predatorages.Size(); i++)
      outfile << predatorages[i] << sep;
    outfile << endl;
  } else {
    outfile << "\n\t\tlengths: ";
    for (i = 0; i < predatorlengths.Size(); i++)
      outfile << predatorlengths[i] << sep;
    outfile << endl;
  }

  outfile << "\tStomach Content Data:";
  for (y = 0; y < stomachcontent.Nrow(); y++) {
    outfile << "\n\trow " << y;
    for (ar = 0; ar < stomachcontent[y].Size(); ar++) {
      outfile << "\n\tinternal areas" << sep << ar << "\n\t\t";
      for (i = 0; i < stomachcontent[y][ar]->Nrow(); i++) {
        for (j = 0; j < (*stomachcontent[y][ar])[i].Size(); j++) {
          outfile.width(smallwidth);
          outfile.precision(smallprecision);
          outfile << (*stomachcontent[y][ar])[i][j] << sep;
        }
        outfile << "\n\t\t";
      }
    }
  }
  outfile << endl;
}

void SCNumbers::Aggregate(int i) {
  aggregator[i]->NumberSum();
}

SCAmounts::~SCAmounts() {
  int i, j;
  for (i = 0; i < stddev.Nrow(); i++) {
    delete number[i];
    for (j = 0; j < stddev[i].Size(); j++)
      delete stddev[i][j];
  }
}

double SCAmounts::CalculateLikelihood(DoubleMatrixPtrVector& consumption, DoubleMatrix& sum) {
  int a, k, preyl;
  double tmplik, lik = 0.0;

  for (a = 0; a < consumption.Size(); a++) {
    for (k = 0; k < consumption[a]->Nrow(); k++) {
      tmplik = 0.0;
      for (preyl = 0; preyl < consumption[a]->Ncol(); preyl++) {
        if ((*stddev[timeindex][a])[k][preyl] != 0)
          tmplik += ((*consumption[a])[k][preyl] -
            (*stomachcontent[timeindex][a])[k][preyl]) *
            ((*consumption[a])[k][preyl] - (*stomachcontent[timeindex][a])[k][preyl]) /
            ((*stddev[timeindex][a])[k][preyl] * (*stddev[timeindex][a])[k][preyl]);
      }
      lik += tmplik * (*number[timeindex])[a][k];
    }
  }
  return lik;
}

void SCAmounts::PrintLikelihood(ofstream& outfile, const TimeClass& TimeInfo) {
  if (!AAT.AtCurrentTime(&TimeInfo))
    return;

  outfile.setf(ios::fixed);
  int a, pd, py;

  //timeindex was increased before this is called, so we subtract 1.
  int time = timeindex - 1;

  outfile << "\nTime:    Year " << TimeInfo.CurrentYear()
    << " Step " << TimeInfo.CurrentStep() << "\nName:    " << scname << endl;
  for (a = 0; a < modelConsumption[time].Size(); a++) {
    outfile << "Internal area  :" << a << "\nObserved:\n";
    for (pd = 0; pd < stomachcontent[time][a]->Nrow(); pd++) {
      for (py = 0; py < (*stomachcontent[time][a])[pd].Size(); py++) {
        outfile.precision(printprecision);
        outfile.width(printwidth);
        outfile << (*stomachcontent[time][a])[pd][py] << sep;
      }
      outfile << endl;
    }
    outfile << "Modelled:\n";
    for (pd = 0; pd < modelConsumption[time][a]->Nrow(); pd++) {
      for (py = 0; py < (*modelConsumption[time][a])[pd].Size(); py++) {
        outfile.precision(printprecision);
        outfile.width(printwidth);
        outfile << (*modelConsumption[time][a])[pd][py] << sep;
      }
      outfile << endl;
    }
    outfile << "Standard deviation:\n";
    for (pd = 0; pd < modelConsumption[time][a]->Nrow(); pd++) {
      for (py = 0; py < (*modelConsumption[time][a])[pd].Size(); py++) {
        outfile.precision(printprecision);
        outfile.width(printwidth);
        outfile << (*stddev[time][a])[pd][py] << sep;
      }
      outfile << endl;
    }
    outfile << "Number of stomachs:\n";
    for (pd = 0; pd < modelConsumption[time][a]->Nrow(); pd++) {
      outfile.precision(lowprecision);
      outfile.width(lowwidth);
      outfile << (*number[time])[a][pd] << sep;
    }
    outfile << endl;
  }
  outfile.flush();
}

void SCAmounts::PrintLikelihoodHeader(ofstream& outfile) {
  int i, j;

  outfile << "Likelihood:       stomachcontent - " << scname << "\nFunction:         -\n"
    << "Calculated every: step\nFilter:           default\nPredation by:     ";

  if (age_pred)
    outfile << "age\n";
  else
    outfile << "length\n";

  outfile << "Name:             " << scname << endl;
  for (i = 0; i < preynames.Nrow(); i++) {
    outfile << "Preys:           ";
    for (j = 0; j < preynames[i].Size(); j++)
      outfile << sep << preynames[i][j];
    outfile << " lengths:";
    for (j = 0; j < preylengths[i].Size(); j++)
      outfile << sep << preylengths[i][j];
    outfile << endl;
  }

  outfile << "Predators:       ";
  for (i = 0; i < predatornames.Size(); i++)
    outfile << sep << predatornames[i];
  if (age_pred) {
    outfile << "\nAges:            ";
    for (i = 0; i < predatorages.Size(); i++)
      outfile << sep << predatorages[i];
    outfile << endl;
  } else {
    outfile << "\nLengths:         ";
    for (i = 0; i < predatorlengths.Size(); i++)
      outfile << sep << predatorlengths[i];
    outfile << endl;
  }
  outfile.flush();
}

void SCRatios::setPredatorsAndPreys(PredatorPtrVector& Predators, PreyPtrVector& Preys) {
  int i, j, k, l;
  double sum = 0.0;
  SC::setPredatorsAndPreys(Predators, Preys);
  //Scale each row such that it sums up to 1
  for (i = 0; i < stomachcontent.Nrow(); i++)
    for (j = 0; j < stomachcontent.Ncol(i); j++)
      for (k = 0; k < stomachcontent[i][j]->Nrow(); k++) {
        sum = 0.0;
        for (l = 0; l < stomachcontent[i][j]->Ncol(k); l++)
          sum += (*stomachcontent[i][j])[k][l];

        if (sum > 0)
          for (l = 0; l < stomachcontent[i][j]->Ncol(k); l++)
            (*stomachcontent[i][j])[k][l] /= sum;
      }
}

double SCRatios::CalculateLikelihood(DoubleMatrixPtrVector& consumption, DoubleMatrix& sum) {
  int a, predl, preyl;
  double tmplik, tmpdivide;
  double lik = 0.0;

  for (a = 0; a < consumption.Size(); a++) {
    for (predl = 0; predl < consumption[a]->Nrow(); predl++) {
      if (sum[a][predl] != 0) {
        tmpdivide = 1 / sum[a][predl];
        tmplik = 0.0;
        for (preyl = 0; preyl < consumption[a]->Ncol(); preyl++) {
          if ((*stddev[timeindex][a])[predl][preyl] != 0)
            tmplik += ((*consumption[a])[predl][preyl] * tmpdivide -
              (*stomachcontent[timeindex][a])[predl][preyl]) *
              ((*consumption[a])[predl][preyl] * tmpdivide -
              (*stomachcontent[timeindex][a])[predl][preyl]) /
              ((*stddev[timeindex][a])[predl][preyl] *
              (*stddev[timeindex][a])[predl][preyl]);
        }
        tmplik *= (*number[timeindex])[a][predl];
        lik += tmplik;
      }
    }
  }
  return lik;
}

//This code will probably be simplified a bit if the Multinomial
//class is changed to take integers.  (Factorial replaced by the gamma function)
double SCNumbers::CalculateLikelihood(DoubleMatrixPtrVector& consumption, DoubleMatrix& sum) {
  Multinomial MN(epsilon);
  int a, predl, preyl;
  double tmp;
  for (a = 0; a < consumption.Size(); a++) {
    for (predl = 0; predl < consumption[a]->Nrow(); predl++) {
      if (!(isZero(sum[a][predl]))) {
        DoubleVector* numbers = new DoubleVector(consumption[a]->Ncol(), 0.0);
        for (preyl = 0; preyl < consumption[a]->Ncol(); preyl++)
          (*numbers)[preyl] = (*stomachcontent[timeindex][a])[predl][preyl];

        tmp = MN.calcLogLikelihood(*numbers, (*consumption[a])[predl]);
        delete numbers;
      }
    }
  }
  return MN.returnLogLikelihood();
}

void SC::Reset() {
  timeindex = 0;
  int i, j;
  for (i = 0; i < modelConsumption.Nrow(); i++)
    for (j = 0; j < modelConsumption.Ncol(); j++)
      if (modelConsumption[i][j] != 0) {
        delete modelConsumption[i][j];
        modelConsumption[i][j] = 0;
      }
  handle.logMessage("Reset stomachcontent component", scname);
}
