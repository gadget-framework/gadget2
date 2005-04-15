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
#include "gadget.h"

extern ErrorHandler handle;

// ********************************************************
// Functions for main likelihood component StomachContent
// ********************************************************
StomachContent::StomachContent(CommentStream& infile,
  const AreaClass* const Area, const TimeClass* const TimeInfo,
  Keeper* const keeper, double weight, const char* name)
  : Likelihood(STOMACHCONTENTLIKELIHOOD, weight, name) {

  functionname = new char[MaxStrLength];
  strncpy(functionname, "", MaxStrLength);
  readWordAndValue(infile, "function", functionname);

  char datafilename[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  char numfilename[MaxStrLength];
  strncpy(numfilename, "", MaxStrLength);
  readWordAndValue(infile, "datafile", datafilename);

  int functionnumber = 0;
  if (strcasecmp(functionname, "scnumbers") == 0)
    functionnumber = 1;
  else if (strcasecmp(functionname, "scratios") == 0)
    functionnumber = 2;
  else if (strcasecmp(functionname, "scamounts") == 0)
    functionnumber = 3;
  else if (strcasecmp(functionname, "scsimple") == 0)
    functionnumber = 4;
  else
    handle.Message("Error in stomachcontent - unrecognised function", functionname);

  switch(functionnumber) {
    case 1:
      StomCont = new SCNumbers(infile, Area, TimeInfo, keeper, datafilename, this->getName());
      break;
    case 2:
      readWordAndValue(infile, "numberfile", numfilename);
      StomCont = new SCRatios(infile, Area, TimeInfo, keeper, datafilename, numfilename, this->getName());
      break;
    case 3:
      readWordAndValue(infile, "numberfile", numfilename);
      StomCont = new SCAmounts(infile, Area, TimeInfo, keeper, datafilename, numfilename, this->getName());
      break;
    case 4:
      StomCont = new SCSimple(infile, Area, TimeInfo, keeper, datafilename, this->getName());
      break;
    default:
      handle.Message("Error in stomachcontent - unrecognised function", functionname);
  }
}

StomachContent::~StomachContent() {
  delete StomCont;
  delete[] functionname;
}

void StomachContent::Print(ofstream& outfile) const {
  outfile << "\nStomach Content " << this->getName() << " - likelihood value " << likelihood
    << "\n\tFunction " << functionname << endl;
  StomCont->Print(outfile);
}

// ********************************************************
// Functions for base SC
// ********************************************************
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
    epsilon = 10.0;

  if (epsilon <= 0) {
    handle.Warning("Epsilon should be a positive integer - set to default value 10");
    epsilon = 10.0;
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
  if (!((strcasecmp(text, "predators") == 0) || (strcasecmp(text, "predatornames") == 0)))
    handle.Unexpected("predatornames", text);
  infile >> text >> ws;
  while (!infile.eof() && ((strcasecmp(text, "predatorlengths") != 0)
      && (strcasecmp(text, "predatorages") != 0))) {
    predatornames.resize(1);
    predatornames[i] = new char[strlen(text) + 1];
    strcpy(predatornames[i++], text);
    infile >> text >> ws;
  }
  if (predatornames.Size() == 0)
    handle.Message("Error in stomachcontent - failed to read predators");
  handle.logMessage("Read predator data - number of predators", predatornames.Size());

  if (strcasecmp(text, "predatorlengths") == 0) { //read predator lengths
    usepredages = 0; //predator is length structured
    readWordAndValue(infile, "lenaggfile", aggfilename);
    datafile.open(aggfilename, ios::in);
    handle.checkIfFailure(datafile, aggfilename);
    handle.Open(aggfilename);
    numpred = readLengthAggregation(subdata, predatorlengths, predindex);
    handle.Close();
    datafile.close();
    datafile.clear();
  } else if (strcasecmp(text, "predatorages") == 0) { //read predator ages
    handle.Message("Stomach content data for age-based predators is currently not suppported");
    usepredages = 1; //predator is age structured
    readWordAndValue(infile, "ageaggfile", aggfilename);
    datafile.open(aggfilename, ios::in);
    handle.checkIfFailure(datafile, aggfilename);
    handle.Open(aggfilename);
    numpred = readAggregation(subdata, predatorages, predindex);
    handle.Close();
    datafile.close();
    datafile.clear();
  } else
    handle.Unexpected("predatorlengths", text);

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
  aggregator[i]->Sum();
}

double SC::calcLikelihood(const TimeClass* const TimeInfo) {
  if (!AAT.AtCurrentTime(TimeInfo))
    return 0.0;

  int i, a, k;
  int prey_g = 0, prey_l = 0;
  double length = 0.0, mult = 0.0;

  int pred_size = 0;
  if (usepredages) //age structured predator
    pred_size = predatorages.Size();
  else
    pred_size = predatorlengths.Size() - 1;

  //The vector which will keep the consumption, indexed the same way as in obsConsumption
  DoubleMatrixPtrVector consumption(areas.Nrow());
  for (a = 0; a < areas.Nrow(); a++)
    consumption[a] = new DoubleMatrix(obsConsumption[timeindex][0]->Nrow(),
      obsConsumption[timeindex][0]->Ncol(), 0.0);

  handle.logMessage("Calculating likelihood score for stomachcontent component", scname);
  //Get the consumption from aggregator, indexed the same way as in obsConsumption
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
  double l = calcLikelihood(consumption, sum);
  for (a = 0; a < areas.Nrow(); a++)
    delete consumption[a];
  timeindex++;
  handle.logMessage("The likelihood score for this component on this timestep is", l);
  return l;
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

void SC::SummaryPrint(ofstream& outfile, double weight) {
  int year, area;

  for (year = 0; year < likelihoodValues.Nrow(); year++) {
    for (area = 0; area < likelihoodValues.Ncol(year); area++) {
      outfile << setw(lowwidth) << Years[year] << sep << setw(lowwidth)
        << Steps[year] << sep << setw(printwidth) << areaindex[area] << sep
        << setw(largewidth) << scname << sep << setprecision(smallprecision)
        << setw(smallwidth) << weight << sep << setprecision(largeprecision)
        << setw(largewidth) << likelihoodValues[year][area] << endl;
    }
  }
  outfile.flush();
}

//JMB - note this ignores the standard deviation and number of samples ...
void SC::LikelihoodPrint(ofstream& outfile, const TimeClass* const TimeInfo) {

  if (!AAT.AtCurrentTime(TimeInfo))
    return;

  int t, area, pred, prey;
  t = timeindex - 1; //timeindex was increased before this is called

  if ((t >= Years.Size()) || t < 0)
    handle.logFailure("Error in stomachcontents - invalid timestep", t);

  for (area = 0; area < modelConsumption.Ncol(t); area++) {
    for (pred = 0; pred < modelConsumption[t][area]->Nrow(); pred++) {
      for (prey = 0; prey < modelConsumption[t][area]->Ncol(pred); prey++) {
        //JMB - might need to be careful with pred and prey
        outfile << setw(lowwidth) << Years[t] << sep << setw(lowwidth)
          << Steps[t] << sep << setw(printwidth) << areaindex[area] << sep
          << setw(printwidth) << predatornames[pred] << sep << setw(printwidth)
          << preynames[pred][prey] << sep << setprecision(largeprecision) << setw(largewidth)
          << (*modelConsumption[t][area])[pred][prey] << endl;
      }
    }
  }
}

SC::~SC() {
  int i, j;
  for (i = 0; i < obsConsumption.Nrow(); i++)
    for (j = 0; j < obsConsumption[i].Size(); j++) {
      delete obsConsumption[i][j];
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

  if (usepredages == 0)
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

  for (i = 0; i < predatornames.Size(); i++) {
    found = 0;
    for (j = 0; j < Predators.Size(); j++)
      if (strcasecmp(predatornames[i], Predators[j]->getName()) == 0) {
        found++;
        predators.resize(1, Predators[j]);
      }

    if (found == 0)
      handle.logFailure("Error in stomachcontent - failed to match predator", predatornames[i]);
  }

  //check predator areas
  for (j = 0; j < areas.Nrow(); j++) {
    found = 0;
    for (i = 0; i < predators.Size(); i++)
      for (k = 0; k < areas.Ncol(j); k++)
        if (predators[i]->isInArea(areas[j][k]))
          found++;
    if (found == 0)
      handle.logWarning("Warning in stomachcontent - predator not defined on all areas");
  }

  preyLgrpDiv = new LengthGroupDivision*[preynames.Nrow()];
  if (usepredages == 0)
    predLgrpDiv = new LengthGroupDivision*[preynames.Nrow()];

  for (i = 0; i < preynames.Nrow(); i++) {
    PreyPtrVector preys;
    for (j = 0; j < preynames[i].Size(); j++) {
      found = 0;
      for (k = 0; k < Preys.Size(); k++)
        if (strcasecmp(preynames[i][j], Preys[k]->getName()) == 0) {
          found++;
          preys.resize(1, Preys[k]);
        }

      if (found == 0)
        handle.logFailure("Error in stomachcontent - failed to match prey", preynames[i][j]);

    }

    //check prey areas
    for (j = 0; j < areas.Nrow(); j++) {
      found = 0;
      for (i = 0; i < preys.Size(); i++)
        for (k = 0; k < areas.Ncol(j); k++)
          if (preys[i]->isInArea(areas[j][k]))
            found++;
      if (found == 0)
        handle.logWarning("Warning in stomachcontent - prey not defined on all areas");
    }

    preyLgrpDiv[i] = new LengthGroupDivision(preylengths[i]);
    if (usepredages == 0) { //length structured predator
      predLgrpDiv[i] = new LengthGroupDivision(predatorlengths);

      //check predator lengths
      found = 0;
      for (j = 0; j < predators.Size(); j++)
        if (predLgrpDiv[i]->maxLength(0) > predators[j]->returnLengthGroupDiv()->minLength())
          found++;
      if (found == 0)
        handle.logWarning("Warning in stomachcontent - minimum length group less than predator length");

      found = 0;
      for (j = 0; j < predators.Size(); j++)
        if (predLgrpDiv[i]->minLength(predLgrpDiv[i]->numLengthGroups()) < predators[j]->returnLengthGroupDiv()->maxLength())
          found++;
      if (found == 0)
        handle.logWarning("Warning in stomachcontent - maximum length group greater than predator length");

      aggregator[i] = new PredatorAggregator(predators, preys, areas, predLgrpDiv[i], preyLgrpDiv[i]);
    } else
      handle.logMessage("Stomach contents data for age-based predators is currently not supported");
  }
}

void SC::Print(ofstream& outfile) const {
  int i, j, r, t;

  outfile << "\tPredators:\n\t\t";
  for (i = 0; i < predatornames.Size(); i++)
    outfile << predatornames[i] << sep;

  if (usepredages) {
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

  outfile << "\tPreys:";
  for (i = 0; i < preynames.Nrow(); i++) {
    outfile << "\n\t\t";
    for (j = 0; j < preynames[i].Size(); j++)
      outfile << preynames[i][j] << sep;
    outfile << "\n\t\tlengths: ";
    for (j = 0; j < preylengths[i].Size(); j++)
      outfile << preylengths[i][j] << sep;
  }

  //timeindex was increased before this is called, so we subtract 1
  t = timeindex - 1;

  if (t < 0) {
    //this has been called at the very start of the model (ie -printinitial)
    //so the modelConsumption object hasnt been initialised yet ...
    for (r = 0; r < obsConsumption[0].Size(); r++) {
      outfile << "\n\tInternal areas" << sep << r;
      for (i = 0; i < obsConsumption[0][r]->Nrow(); i++) {
        outfile << "\n\t\t";
        for (j = 0; j < (*obsConsumption[0][r])[i].Size(); j++) {
          outfile.width(smallwidth);
          outfile << 0 << sep;
        }
      }
    }

  } else {
    for (r = 0; r < modelConsumption[t].Size(); r++) {
      outfile << "\n\tInternal areas" << sep << r;
      for (i = 0; i < modelConsumption[t][r]->Nrow(); i++) {
        outfile << "\n\t\t";
        for (j = 0; j < (*modelConsumption[t][r])[i].Size(); j++) {
          outfile.width(smallwidth);
          outfile << (*modelConsumption[t][r])[i][j] << sep;
        }
      }
    }
  }
  outfile << endl;
}

// ********************************************************
// Functions for SCNumbers
// ********************************************************
SCNumbers::SCNumbers(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper,
  const char* datafilename, const char* name)
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
  if (usepredages) //age structured predator
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
    if ((TimeInfo->isWithinPeriod(year, step)) && (keepdata == 0)) {
      //if this is a new timestep, resize to store the data
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        timeid = Years.Size() - 1;

        obsConsumption.AddRows(1, numarea, 0);
        modelConsumption.AddRows(1, numarea, 0);
        likelihoodValues.AddRows(1, numarea, 0.0);
        for (i = 0; i < numarea; i++)
          obsConsumption[timeid][i] = new DoubleMatrix(pred_size, nopreygroups, 0.0);
      }

    } else {
      //dont keep the data
      keepdata = 1;
    }

    if (keepdata == 0) {
      //stomach content data is required, so store it
      count++;
      (*obsConsumption[timeid][areaid])[predid][preyid] = tmpnumber;
    }
  }

  AAT.addActions(Years, Steps, TimeInfo);
  if (count == 0)
    handle.logWarning("Warning in stomachcontent - found no data in the data file for", scname);
  handle.logMessage("Read stomachcontent data file - number of entries", count);
}

//This code will probably be simplified a bit if the Multinomial
//class is changed to take integers.  (Factorial replaced by the gamma function)
double SCNumbers::calcLikelihood(DoubleMatrixPtrVector& consumption, DoubleMatrix& sum) {
  Multinomial MN(epsilon);
  int a, predl, preyl;
  double tmp;
  for (a = 0; a < consumption.Size(); a++) {
    likelihoodValues[timeindex][a] = 0.0;
    for (predl = 0; predl < consumption[a]->Nrow(); predl++) {
      if (!(isZero(sum[a][predl]))) {
        DoubleVector* numbers = new DoubleVector(consumption[a]->Ncol(), 0.0);
        for (preyl = 0; preyl < consumption[a]->Ncol(); preyl++)
          (*numbers)[preyl] = (*obsConsumption[timeindex][a])[predl][preyl];

        likelihoodValues[timeindex][a] += MN.calcLogLikelihood(*numbers, (*consumption[a])[predl]);

        delete numbers;
      }
    }
  }
  return MN.returnLogLikelihood();
}

void SCNumbers::Aggregate(int i) {
  aggregator[i]->NumberSum();
}

// ********************************************************
// Functions for SCAmounts
// ********************************************************
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
  if (usepredages) //age structured predator
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
    if ((TimeInfo->isWithinPeriod(year, step)) && (keepdata == 0)) {
      //if this is a new timestep, resize to store the data
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        timeid = Years.Size() - 1;

        obsConsumption.AddRows(1, numarea, 0);
        modelConsumption.AddRows(1, numarea, 0);
        stddev.AddRows(1, numarea, 0);
        likelihoodValues.AddRows(1, numarea, 0.0);
        for (i = 0; i < numarea; i++) {
          obsConsumption[timeid][i] = new DoubleMatrix(pred_size, nopreygroups, 0.0);
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
      (*obsConsumption[timeid][areaid])[predid][preyid] = tmpnumber;
      (*stddev[timeid][areaid])[predid][preyid] = tmpstddev;
    }
  }

  AAT.addActions(Years, Steps, TimeInfo);
  if (count == 0)
    handle.logWarning("Warning in stomachcontent - found no data in the data file for", scname);
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
  if (usepredages) //age structured predator
    pred_size = predatorages.Size();
  else
    pred_size = predatorlengths.Size() - 1;

  //We know the size that numbers[] will be from obsConsumption
  int numarea = areas.Nrow();
  number.resize(obsConsumption.Nrow());
  for (i = 0; i < obsConsumption.Nrow(); i++)
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
    if (TimeInfo->isWithinPeriod(year, step))
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

SCAmounts::~SCAmounts() {
  int i, j;
  for (i = 0; i < stddev.Nrow(); i++) {
    delete number[i];
    for (j = 0; j < stddev[i].Size(); j++)
      delete stddev[i][j];
  }
}

double SCAmounts::calcLikelihood(DoubleMatrixPtrVector& consumption, DoubleMatrix& sum) {
  int a, k, preyl;
  double tmplik, lik = 0.0;

  for (a = 0; a < consumption.Size(); a++) {
    likelihoodValues[timeindex][a] = 0.0;
    for (k = 0; k < consumption[a]->Nrow(); k++) {
      tmplik = 0.0;
      for (preyl = 0; preyl < consumption[a]->Ncol(); preyl++) {
        if (!(isZero((*stddev[timeindex][a])[k][preyl])))
          tmplik += ((*consumption[a])[k][preyl] -
            (*obsConsumption[timeindex][a])[k][preyl]) *
            ((*consumption[a])[k][preyl] - (*obsConsumption[timeindex][a])[k][preyl]) /
            ((*stddev[timeindex][a])[k][preyl] * (*stddev[timeindex][a])[k][preyl]);
      }
      tmplik *= (*number[timeindex])[a][k];
      lik += tmplik;
      likelihoodValues[timeindex][a] += tmplik;
    }
  }
  return lik;
}

// ********************************************************
// Functions for SCRatios
// ********************************************************
void SCRatios::setPredatorsAndPreys(PredatorPtrVector& Predators, PreyPtrVector& Preys) {
  int i, j, k, l;
  double tmpdivide, sum = 0.0;
  SC::setPredatorsAndPreys(Predators, Preys);
  //Scale each row such that it sums up to 1
  for (i = 0; i < obsConsumption.Nrow(); i++) {
    for (j = 0; j < obsConsumption.Ncol(i); j++) {
      for (k = 0; k < obsConsumption[i][j]->Nrow(); k++) {
        sum = 0.0;
        for (l = 0; l < obsConsumption[i][j]->Ncol(k); l++)
          sum += (*obsConsumption[i][j])[k][l];

        if (!(isZero(sum))) {
          tmpdivide = 1.0 / sum;
          for (l = 0; l < obsConsumption[i][j]->Ncol(k); l++)
            (*obsConsumption[i][j])[k][l] *= tmpdivide;
        }
      }
    }
  }
}

double SCRatios::calcLikelihood(DoubleMatrixPtrVector& consumption, DoubleMatrix& sum) {
  int a, predl, preyl;
  double tmplik, tmpdivide;
  double lik = 0.0;

  for (a = 0; a < consumption.Size(); a++) {
    likelihoodValues[timeindex][a] = 0.0;
    for (predl = 0; predl < consumption[a]->Nrow(); predl++) {
      if (!(isZero(sum[a][predl]))) {
        tmpdivide = 1.0 / sum[a][predl];
        tmplik = 0.0;
        for (preyl = 0; preyl < consumption[a]->Ncol(); preyl++) {
          if (!(isZero((*stddev[timeindex][a])[predl][preyl])))
            tmplik += ((*consumption[a])[predl][preyl] * tmpdivide -
              (*obsConsumption[timeindex][a])[predl][preyl]) *
              ((*consumption[a])[predl][preyl] * tmpdivide -
              (*obsConsumption[timeindex][a])[predl][preyl]) /
              ((*stddev[timeindex][a])[predl][preyl] *
              (*stddev[timeindex][a])[predl][preyl]);
        }
        tmplik *= (*number[timeindex])[a][predl];
        lik += tmplik;
        likelihoodValues[timeindex][a] += tmplik;
      }
    }
  }
  return lik;
}

// ********************************************************
// Functions for SCSimple
// ********************************************************
SCSimple::SCSimple(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper,
  const char* datafilename, const char* name)
  : SC(infile, Area, TimeInfo, keeper, datafilename, name) {

  ifstream datafile;
  CommentStream subdata(datafile);
  //read in stomach content from file
  datafile.open(datafilename, ios::in);
  handle.checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  readStomachSimpleContent(subdata, TimeInfo);
  handle.Close();
  datafile.close();
  datafile.clear();
}

void SCSimple::readStomachSimpleContent(CommentStream& infile, const TimeClass* const TimeInfo) {

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
  if (usepredages) //age structured predator
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
    if ((TimeInfo->isWithinPeriod(year, step)) && (keepdata == 0)) {
      //if this is a new timestep, resize to store the data
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        timeid = Years.Size() - 1;

        obsConsumption.AddRows(1, numarea, 0);
        modelConsumption.AddRows(1, numarea, 0);
        likelihoodValues.AddRows(1, numarea, 0.0);
        for (i = 0; i < numarea; i++)
          obsConsumption[timeid][i] = new DoubleMatrix(pred_size, nopreygroups, 0.0);
      }

    } else {
      //dont keep the data
      keepdata = 1;
    }

    if (keepdata == 0) {
      //stomach content data is required, so store it
      count++;
      (*obsConsumption[timeid][areaid])[predid][preyid] = tmpnumber;
    }
  }

  AAT.addActions(Years, Steps, TimeInfo);
  if (count == 0)
    handle.logWarning("Warning in stomachcontent - found no data in the data file for", scname);
  handle.logMessage("Read stomachcontent data file - number of entries", count);
}

void SCSimple::setPredatorsAndPreys(PredatorPtrVector& Predators, PreyPtrVector& Preys) {
  int i, j, k, l;
  double tmpdivide, sum = 0.0;
  SC::setPredatorsAndPreys(Predators, Preys);
  //Scale each row such that it sums up to 1
  for (i = 0; i < obsConsumption.Nrow(); i++) {
    for (j = 0; j < obsConsumption.Ncol(i); j++) {
      for (k = 0; k < obsConsumption[i][j]->Nrow(); k++) {
        sum = 0.0;
        for (l = 0; l < obsConsumption[i][j]->Ncol(k); l++)
          sum += (*obsConsumption[i][j])[k][l];

        if (!(isZero(sum))) {
          tmpdivide = 1.0 / sum;
          for (l = 0; l < obsConsumption[i][j]->Ncol(k); l++)
            (*obsConsumption[i][j])[k][l] *= tmpdivide;
        }
      }
    }
  }
}

double SCSimple::calcLikelihood(DoubleMatrixPtrVector& consumption, DoubleMatrix& sum) {
  int a, predl, preyl;
  double tmplik, tmpdivide;
  double lik = 0.0;

  for (a = 0; a < consumption.Size(); a++) {
    likelihoodValues[timeindex][a] = 0.0;
    for (predl = 0; predl < consumption[a]->Nrow(); predl++) {
      if (!(isZero(sum[a][predl]))) {
        tmpdivide = 1.0 / sum[a][predl];
        tmplik = 0.0;
        for (preyl = 0; preyl < consumption[a]->Ncol(); preyl++) {
          tmplik += ((*consumption[a])[predl][preyl] * tmpdivide -
              (*obsConsumption[timeindex][a])[predl][preyl]) *
              ((*consumption[a])[predl][preyl] * tmpdivide -
              (*obsConsumption[timeindex][a])[predl][preyl]);
        }
        lik += tmplik;
        likelihoodValues[timeindex][a] += tmplik;
      }
    }
  }
  return lik;
}
