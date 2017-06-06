#include "stomachcontent.h"
#include "areatime.h"
#include "predator.h"
#include "stockpredator.h"
#include "prey.h"
#include "predatoraggregator.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "multinomial.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

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
  readWordAndValue(infile, "datafile", datafilename);

  if (strcasecmp(functionname, "scnumbers") == 0) {
    StomCont = new SCNumbers(infile, Area, TimeInfo, keeper, datafilename, this->getName());

  } else if (strcasecmp(functionname, "scratios") == 0) {
    char numfilename[MaxStrLength];
    strncpy(numfilename, "", MaxStrLength);
    readWordAndValue(infile, "numberfile", numfilename);
    StomCont = new SCRatios(infile, Area, TimeInfo, keeper, datafilename, numfilename, this->getName());

  } else if (strcasecmp(functionname, "scamounts") == 0) {
    char numfilename[MaxStrLength];
    strncpy(numfilename, "", MaxStrLength);
    readWordAndValue(infile, "numberfile", numfilename);
    StomCont = new SCAmounts(infile, Area, TimeInfo, keeper, datafilename, numfilename, this->getName());

  } else if (strcasecmp(functionname, "scsimple") == 0) {
    StomCont = new SCSimple(infile, Area, TimeInfo, keeper, datafilename, this->getName());

  } else {
    handle.logFileMessage(LOGFAIL, "\nError in stomachcontent - unrecognised function", functionname);
  }
}

StomachContent::~StomachContent() {
  delete StomCont;
  delete[] functionname;
}

void StomachContent::addLikelihood(const TimeClass* const TimeInfo) {
  if (isZero(weight))
    return;
  likelihood += StomCont->calcLikelihood(TimeInfo);
}

void StomachContent::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  if (isZero(weight))
    handle.logMessage(LOGWARN, "Warning in stomachcontent - zero weight for", this->getName());
  StomCont->Reset();
}

void StomachContent::Print(ofstream& outfile) const {
  outfile << "\nStomach Content " << this->getName() << " - likelihood value " << likelihood
    << "\n\tFunction " << functionname << endl;
  StomCont->Print(outfile);
}

// ********************************************************
// Functions for base SC
// ********************************************************
SC::SC(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo,
  Keeper* const keeper, const char* datafilename, const char* givenname)
    : HasName(givenname), aggregator(0), preyLgrpDiv(0), predLgrpDiv(0), dptr(0) {

  int i, j;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int numpred = 0;
  int numarea = 0;

  timeindex = 0;
  usepredages = 0;

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

  if (epsilon < verysmall) {
    handle.logFileMessage(LOGWARN, "epsilon should be a positive integer - set to default value 10");
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
      areas[i][j] = Area->getInnerArea(areas[i][j]);

  //read in the predators
  i = 0;
  infile >> text >> ws;
  if ((strcasecmp(text, "predators") != 0) && (strcasecmp(text, "predatornames") != 0))
    handle.logFileUnexpected(LOGFAIL, "predatornames", text);
  infile >> text >> ws;
  while (!infile.eof() && ((strcasecmp(text, "predatorlengths") != 0)
      && (strcasecmp(text, "predatorages") != 0))) {
    predatornames.resize(new char[strlen(text) + 1]);
    strcpy(predatornames[i++], text);
    infile >> text >> ws;
  }
  if (predatornames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "\nError in stomachcontent - failed to read predators");
  handle.logMessage(LOGMESSAGE, "Read predator data - number of predators", predatornames.Size());

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
    handle.logFileUnexpected(LOGFAIL, "predatorlengths", text);

  //read in the preys
  readWordAndValue(infile, "preyaggfile", aggfilename);
  datafile.open(aggfilename, ios::in);
  handle.checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  i = readPreyAggregation(subdata, preynames, preylengths, digestioncoeff, preyindex, keeper);
  handle.Close();
  datafile.close();
  datafile.clear();

  //prepare for next likelihood component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (strcasecmp(text, "[component]") != 0)
      handle.logFileUnexpected(LOGFAIL, "[component]", text);
  }
}

void SC::aggregate(int i) {
  aggregator[i]->Sum();
}

double SC::calcLikelihood(const TimeClass* const TimeInfo) {

  if (!(AAT.atCurrentTime(TimeInfo)))
    return 0.0;

  int i, a, k, p;
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Calculating likelihood score for stomachcontent component", this->getName());

  timeindex = -1;
  for (i = 0; i < Years.Size(); i++)
    if ((Years[i] == TimeInfo->getYear()) && (Steps[i] == TimeInfo->getStep()))
      timeindex = i;
  if (timeindex == -1)
    handle.logMessage(LOGFAIL, "Error in stomachcontent - invalid timestep");

  //Get the consumption from aggregator, indexed the same way as in obsConsumption
  int numprey = 0;
  for (i = 0; i < preyindex.Size(); i++) {
    this->aggregate(i);
    for (a = 0; a < areas.Nrow(); a++) {
      dptr = aggregator[i]->getSum()[a];
      for (k = 0; k < dptr->Nrow(); k++)
        for (p = 0; p < dptr->Ncol(k); p++)
          (*modelConsumption[timeindex][a])[k][numprey + p] = (*dptr)[k][p] * digestion[i][p];

    }
    numprey += preylengths[i].Size() - 1;
  }

  //Now calculate likelihood score
  double l = calcLikelihood();
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "The likelihood score for this component on this timestep is", l);
  return l;
}

void SC::Reset() {
  //JMB - calculate the digestion coefficiant matrix
  int i, j;
  if (digestion.Nrow() != digestioncoeff.Nrow())
    handle.logMessage(LOGFAIL, "Error in stomachcontent - missing digestion coefficient data");

  for (i = 0; i < digestion.Nrow(); i++)
    for (j = 0; j < digestion.Ncol(i); j++)
      digestion[i][j] = digestioncoeff[i][0] + digestioncoeff[i][1] *
                          pow(preyLgrpDiv[i]->meanLength(j), digestioncoeff[i][2]);

  for (i = 0; i < modelConsumption.Nrow(); i++)
    for (j = 0; j < modelConsumption.Ncol(i); j++)
      (*modelConsumption[i][j]).setToZero();

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset stomachcontent component", this->getName());
}

void SC::printSummary(ofstream& outfile, double weight) {
  int year, area;

  for (year = 0; year < likelihoodValues.Nrow(); year++) {
    for (area = 0; area < likelihoodValues.Ncol(year); area++) {
      outfile << setw(lowwidth) << Years[year] << sep << setw(lowwidth)
        << Steps[year] << sep << setw(printwidth) << areaindex[area] << sep
        << setw(largewidth) << this->getName() << sep << setprecision(smallprecision)
        << setw(smallwidth) << weight << sep << setprecision(largeprecision)
        << setw(largewidth) << likelihoodValues[year][area] << endl;
    }
  }
  outfile.flush();
}

void SC::printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo) {

  if (!AAT.atCurrentTime(TimeInfo))
    return;

  int i, area, pred, prey;
  timeindex = -1;
  for (i = 0; i < Years.Size(); i++)
    if ((Years[i] == TimeInfo->getYear()) && (Steps[i] == TimeInfo->getStep()))
      timeindex = i;
  if (timeindex == -1)
    handle.logMessage(LOGFAIL, "Error in stomachcontent - invalid timestep");

  for (area = 0; area < modelConsumption.Ncol(timeindex); area++) {
    for (pred = 0; pred < modelConsumption[timeindex][area]->Nrow(); pred++) {
      for (prey = 0; prey < modelConsumption[timeindex][area]->Ncol(pred); prey++) {
        outfile << setw(lowwidth) << Years[timeindex] << sep << setw(lowwidth)
          << Steps[timeindex] << sep << setw(printwidth) << areaindex[area] << sep
          << setw(printwidth) << predindex[pred] << sep << setw(printwidth)
          << preyindex[prey] << sep << setprecision(largeprecision) << setw(largewidth);

        //JMB crude filter to remove the 'silly' values from the output
        if ((*modelConsumption[timeindex][area])[pred][prey] < rathersmall)
          outfile << 0 << endl;
        else
          outfile << (*modelConsumption[timeindex][area])[pred][prey] << endl;
      }
    }
  }
}

SC::~SC() {
  int i, j;
  for (i = 0; i < obsConsumption.Nrow(); i++) {
    for (j = 0; j < obsConsumption[i].Size(); j++) {
      delete obsConsumption[i][j];
      delete modelConsumption[i][j];
    }
  }

  for (i = 0; i < preyindex.Size(); i++) {
    delete aggregator[i];
    delete preyLgrpDiv[i];
    for (j = 0; j < preynames[i].Size(); j++)
      delete[] preynames[i][j];
  }

  if (aggregator != 0) {
    delete[] aggregator;
    aggregator = 0;
  }
  if (preyLgrpDiv != 0) {
    delete[] preyLgrpDiv;
    preyLgrpDiv = 0;
  }

  if (!usepredages)
    delete predLgrpDiv;

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
  int i, j, k, l, found;
  int minage, maxage;
  PredatorPtrVector predators;
  aggregator = new PredatorAggregator*[preyindex.Size()];

  for (i = 0; i < predatornames.Size(); i++) {
    found = 0;
    for (j = 0; j < Predators.Size(); j++)
      if (strcasecmp(predatornames[i], Predators[j]->getName()) == 0) {
        found++;
        predators.resize(Predators[j]);
      }

    if (found == 0)
      handle.logMessage(LOGFAIL, "Error in stomachcontent - failed to match predator", predatornames[i]);
  }

  //check predator areas
  if (handle.getLogLevel() >= LOGWARN) {
    for (j = 0; j < areas.Nrow(); j++) {
      found = 0;
      for (i = 0; i < predators.Size(); i++)
        for (k = 0; k < areas.Ncol(j); k++)
          if (predators[i]->isInArea(areas[j][k]))
            found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in stomachcontent - predator not defined on all areas");
    }
  }

  //check that the predators are stocks and not fleets
  for (i = 0; i < predators.Size(); i++)
    if (predators[i]->getType() != STOCKPREDATOR)
      handle.logMessage(LOGFAIL, "Error in stomachcontent - cannot aggregate predator", predators[i]->getName());

  preyLgrpDiv = new LengthGroupDivision*[preyindex.Size()];
  if (!usepredages) {
    predLgrpDiv = new LengthGroupDivision(predatorlengths);
    if (predLgrpDiv->Error())
      handle.logFileMessage(LOGFAIL, "\nError in stomachcontent - failed to create length group");
  }

  if (handle.getLogLevel() >= LOGWARN) {
    if (usepredages) {
      //check predator ages
      minage = 9999;
      maxage = -1;
      for (j = 0; j < predatorages.Nrow(); j++) {
        for (k = 0; k < predatorages.Ncol(j); k++) {
          minage = min(predatorages[j][k], minage);
          maxage = max(predatorages[j][k], maxage);
        }
      }

      found = 0;
      for (j = 0; j < predators.Size(); j++)
        if (minage >= ((StockPredator*)predators[j])->minAge())
          found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in stomachcontent - minimum age less than predator age");

      found = 0;
      for (j = 0; j < predators.Size(); j++)
        if (maxage <= ((StockPredator*)predators[j])->maxAge())
          found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in stomachcontent - maximum age greater than predator age");

    } else {
      //check predator lengths
      found = 0;
      for (j = 0; j < predators.Size(); j++)
        if (predLgrpDiv->maxLength(0) > predators[j]->getLengthGroupDiv()->minLength())
          found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in stomachcontent - minimum length group less than predator length");

      found = 0;
      for (j = 0; j < predators.Size(); j++)
        if (predLgrpDiv->minLength(predLgrpDiv->numLengthGroups()) < predators[j]->getLengthGroupDiv()->maxLength())
          found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in stomachcontent - maximum length group greater than predator length");
    }
  }

  for (i = 0; i < preynames.Nrow(); i++) {
    PreyPtrVector preys;
    for (j = 0; j < preynames.Ncol(i); j++) {
      found = 0;
      for (k = 0; k < Preys.Size(); k++)
        if (strcasecmp(preynames[i][j], Preys[k]->getName()) == 0) {
          found++;
          preys.resize(Preys[k]);
        }

      if (found == 0)
        handle.logMessage(LOGFAIL, "Error in stomachcontent - failed to match prey", preynames[i][j]);
    }

    //check prey areas
    if (handle.getLogLevel() >= LOGWARN) {
      for (j = 0; j < areas.Nrow(); j++) {
        found = 0;
        for (k = 0; k < preys.Size(); k++)
          for (l = 0; l < areas.Ncol(j); l++)
            if (preys[k]->isInArea(areas[j][l]))
              found++;
        if (found == 0)
          handle.logMessage(LOGWARN, "Warning in stomachcontent - prey not defined on all areas");
      }
    }

    //resize the digestion matrix
    digestion.AddRows(1, preylengths[i].Size(), 0.0);
    preyLgrpDiv[i] = new LengthGroupDivision(preylengths[i]);
    if (preyLgrpDiv[i]->Error())
      handle.logFileMessage(LOGFAIL, "\nError in stomachcontent - failed to create length group");

    if (usepredages)
      aggregator[i] = new PredatorAggregator(predators, preys, areas, predatorages, preyLgrpDiv[i]);
    else
      aggregator[i] = new PredatorAggregator(predators, preys, areas, predLgrpDiv, preyLgrpDiv[i]);
  }
}

void SC::Print(ofstream& outfile) const {
  int i, j;

  outfile << "\tPredators:\n\t\t";
  for (i = 0; i < predatornames.Size(); i++)
    outfile << predatornames[i] << sep;

  if (usepredages) {
    outfile << "\n\t\tages:";
    for (i = 0; i < predatorages.Nrow(); i++) {
      outfile << "\n\t\t\t";
      for (j = 0; j < predatorages.Ncol(i); j++)
        outfile << predatorages[i][j] << sep;
    }
    outfile << endl;
  } else {
    outfile << "\n\t\tlengths: ";
    for (i = 0; i < predatorlengths.Size(); i++)
      outfile << predatorlengths[i] << sep;
    outfile << endl;
  }

  outfile << "\n\tPreys:";
  for (i = 0; i < preyindex.Size(); i++) {
    outfile << "\n\t\t" << preyindex[i] << "\n\t\t";
    for (j = 0; j < preynames[i].Size(); j++)
      outfile << preynames[i][j] << sep;
    outfile << "\n\t\tlengths: ";
    for (j = 0; j < preylengths[i].Size(); j++)
      outfile << preylengths[i][j] << sep;
    outfile << endl;
    aggregator[i]->Print(outfile);
  }
  outfile.flush();
}

// ********************************************************
// Functions for SCNumbers
// ********************************************************
SCNumbers::SCNumbers(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper,
  const char* datafilename, const char* givenname)
  : SC(infile, Area, TimeInfo, keeper, datafilename, givenname) {

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

  MN = Multinomial();
  MN.setValue(epsilon);
  mndist.resize(likelihoodValues.Nrow(), 0.0);
  mndata.resize(likelihoodValues.Nrow(), 0.0);
}

void SCNumbers::readStomachNumberContent(CommentStream& infile, const TimeClass* const TimeInfo) {

  int i, year, step, count, reject;
  double tmpnumber;
  char tmparea[MaxStrLength], tmppred[MaxStrLength], tmpprey[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmppred, "", MaxStrLength);
  strncpy(tmpprey, "", MaxStrLength);
  int keepdata, timeid, areaid, predid, preyid;

  if (usepredages) //age structured predator
    handle.logMessage(LOGFAIL, "Error in stomachcontent - age based predators cannot be used with scnumbers");

  int numpred = predatorlengths.Size() - 1;
  int numarea = areas.Nrow();
  int numprey = 0;
  for (i = 0; i < preylengths.Nrow(); i++)
    numprey += preylengths[i].Size() - 1;
  if (numprey == 0)
    handle.logMessage(LOGWARN, "Warning in stomachcontents - no prey found for", this->getName());

  //Check the number of columns in the inputfile
  infile >> ws;
  if (countColumns(infile) != 6)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 6");

  year = step = count = reject = 0;
  while (!infile.eof()) {
    keepdata = 1;
    infile >> year >> step >> tmparea >> tmppred >> tmpprey >> tmpnumber >> ws;

    //crude check to see if something has gone wrong and avoid infinite loops
    if (strlen(tmparea) == 0)
      handle.logFileMessage(LOGFAIL, "failed to read data from file");

    //if tmparea is in areaindex find areaid, else dont keep the data
    areaid = -1;
    for (i = 0; i < areaindex.Size(); i++)
      if (strcasecmp(areaindex[i], tmparea) == 0)
        areaid = i;

    if (areaid == -1)
      keepdata = 0;

    //if tmppred is in predindex find predid, else dont keep the data
    predid = -1;
    for (i = 0; i < predindex.Size(); i++)
      if (strcasecmp(predindex[i], tmppred) == 0)
        predid = i;

    if (predid == -1)
      keepdata = 0;

    //if tmpprey is in preyindex find preyid, else dont keep the data
    preyid = -1;
    for (i = 0; i < preyindex.Size(); i++)
      if (strcasecmp(preyindex[i], tmpprey) == 0)
        preyid = i;

    if (preyid == -1)
      keepdata = 0;

    //check if the year and step are in the simulation
    timeid = -1;
    if ((TimeInfo->isWithinPeriod(year, step)) && (keepdata == 1)) {
      //if this is a new timestep, resize to store the data
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        timeid = Years.Size() - 1;

        obsConsumption.resize();
        modelConsumption.resize();
        likelihoodValues.AddRows(1, numarea, 0.0);
        for (i = 0; i < numarea; i++) {
          obsConsumption[timeid].resize(new DoubleMatrix(numpred, numprey, 0.0));
          modelConsumption[timeid].resize(new DoubleMatrix(numpred, numprey, 0.0));
        }
      }

    } else
      keepdata = 0;

    if (keepdata == 1) {
      //stomach content data is required, so store it
      count++;
      (*obsConsumption[timeid][areaid])[predid][preyid] = tmpnumber;
    } else
      reject++;  //count number of rejected data points read from file
  }

  AAT.addActions(Years, Steps, TimeInfo);
  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in stomachcontent - found no data in the data file for", this->getName());
  if (reject != 0)
    handle.logMessage(LOGMESSAGE, "Discarded invalid stomachcontent data - number of invalid entries", reject);
  handle.logMessage(LOGMESSAGE, "Read stomachcontent data file - number of entries", count);
}

double SCNumbers::calcLikelihood() {
  int a, pred, prey;
  MN.Reset();
  for (a = 0; a < areas.Nrow(); a++) {
    likelihoodValues[timeindex][a] = 0.0;
    for (prey = 0; prey < obsConsumption[timeindex][a]->Ncol(0); prey++) {
      for (pred = 0; pred < mndata.Size(); pred++) {
        mndata[pred] = (*obsConsumption[timeindex][a])[pred][prey];
        mndist[pred] = (*modelConsumption[timeindex][a])[pred][prey];
      }
      likelihoodValues[timeindex][a] += MN.calcLogLikelihood(mndata, mndist);
    }
  }
  return MN.getLogLikelihood();
}

void SCNumbers::aggregate(int i) {
  aggregator[i]->NumberSum();
}

// ********************************************************
// Functions for SCAmounts
// ********************************************************
SCAmounts::SCAmounts(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper,
  const char* datafilename, const char* numfilename, const char* givenname)
  : SC(infile, Area, TimeInfo, keeper, datafilename, givenname) {

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

  int i, year, step, count, reject;
  double tmpnumber, tmpstddev;
  char tmparea[MaxStrLength], tmppred[MaxStrLength], tmpprey[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmppred, "", MaxStrLength);
  strncpy(tmpprey, "", MaxStrLength);
  int keepdata, timeid, areaid, predid, preyid;

  int numpred = 0;
  if (usepredages) //age structured predator
    numpred = predatorages.Nrow();
  else
    numpred = predatorlengths.Size() - 1;

  int numarea = areas.Nrow();
  int numprey = 0;
  for (i = 0; i < preylengths.Nrow(); i++)
    numprey += preylengths[i].Size() - 1;
  if (numprey == 0)
    handle.logMessage(LOGWARN, "Warning in stomachcontents - no prey found for", this->getName());

  //Check the number of columns in the inputfile
  infile >> ws;
  if (countColumns(infile) != 7)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 7");

  year = step = count = reject = 0;
  while (!infile.eof()) {
    keepdata = 1;
    infile >> year >> step >> tmparea >> tmppred >> tmpprey >> tmpnumber >> tmpstddev >> ws;

    //crude check to see if something has gone wrong and avoid infinite loops
    if (strlen(tmparea) == 0)
      handle.logFileMessage(LOGFAIL, "failed to read data from file");

    //if tmparea is in areaindex find areaid, else dont keep the data
    areaid = -1;
    for (i = 0; i < areaindex.Size(); i++)
      if (strcasecmp(areaindex[i], tmparea) == 0)
        areaid = i;

    if (areaid == -1)
      keepdata = 0;

    //if tmppred is in predindex find predid, else dont keep the data
    predid = -1;
    for (i = 0; i < predindex.Size(); i++)
      if (strcasecmp(predindex[i], tmppred) == 0)
        predid = i;

    if (predid == -1)
      keepdata = 0;

    //if tmpprey is in preyindex find preyid, else dont keep the data
    preyid = -1;
    for (i = 0; i < preyindex.Size(); i++)
      if (strcasecmp(preyindex[i], tmpprey) == 0)
        preyid = i;

    if (preyid == -1)
      keepdata = 0;

    //check if the year and step are in the simulation
    timeid = -1;
    if ((TimeInfo->isWithinPeriod(year, step)) && (keepdata == 1)) {
      //if this is a new timestep, resize to store the data
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        timeid = Years.Size() - 1;

        obsConsumption.resize();
        modelConsumption.resize();
        stddev.resize();
        likelihoodValues.AddRows(1, numarea, 0.0);
        for (i = 0; i < numarea; i++) {
          obsConsumption[timeid].resize(new DoubleMatrix(numpred, numprey, 0.0));
          modelConsumption[timeid].resize(new DoubleMatrix(numpred, numprey, 0.0));
          stddev[timeid].resize(new DoubleMatrix(numpred, numprey, 0.0));
        }
      }

    } else
      keepdata = 0;

    if (keepdata == 1) {
      //stomach content data is required, so store it
      count++;
      (*obsConsumption[timeid][areaid])[predid][preyid] = tmpnumber;
      (*stddev[timeid][areaid])[predid][preyid] = tmpstddev;
    } else
      reject++;  //count number of rejected data points read from file
  }

  AAT.addActions(Years, Steps, TimeInfo);
  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in stomachcontent - found no data in the data file for", this->getName());
  if (reject != 0)
    handle.logMessage(LOGMESSAGE, "Discarded invalid stomachcontent data - number of invalid entries", reject);
  handle.logMessage(LOGMESSAGE, "Read stomachcontent data file - number of entries", count);
}

void SCAmounts::readStomachSampleContent(CommentStream& infile, const TimeClass* const TimeInfo) {

  int i, year, step, count, reject;
  double tmpnumber;
  int keepdata, timeid, areaid, predid;
  char tmparea[MaxStrLength], tmppred[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmppred, "", MaxStrLength);

  int numpred = 0;
  if (usepredages) //age structured predator
    numpred = predatorages.Nrow();
  else
    numpred = predatorlengths.Size() - 1;

  //We know the size that numbers[] will be from obsConsumption
  int numarea = areas.Nrow();
  for (i = 0; i < obsConsumption.Nrow(); i++)
    number.resize(new DoubleMatrix(numarea, numpred, 0.0));

  //Check the number of columns in the inputfile
  infile >> ws;
  if (countColumns(infile) != 5)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 5");

  year = step = count = reject = 0;
  while (!infile.eof()) {
    keepdata = 1;
    infile >> year >> step >> tmparea >> tmppred >> tmpnumber >> ws;

    //crude check to see if something has gone wrong and avoid infinite loops
    if (strlen(tmparea) == 0)
      handle.logFileMessage(LOGFAIL, "failed to read data from file");

    //check if the year and step are in the simulation
    timeid = -1;
    if (TimeInfo->isWithinPeriod(year, step))
      //find the timeid from Years and Steps
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

    if (timeid == -1)
      keepdata = 0;

    //if tmparea is in areaindex find areaid, else dont keep the data
    areaid = -1;
    for (i = 0; i < areaindex.Size(); i++)
      if (strcasecmp(areaindex[i], tmparea) == 0)
        areaid = i;

    if (areaid == -1)
      keepdata = 0;

    //if tmppred is in predindex find predid, else dont keep the data
    predid = -1;
    for (i = 0; i < predindex.Size(); i++)
      if (strcasecmp(predindex[i], tmppred) == 0)
        predid = i;

    if (predid == -1)
      keepdata = 0;

    if (keepdata == 1) {
      //stomach content data is required, so store it
      count++;
      (*number[timeid])[areaid][predid] = tmpnumber;
    } else
      reject++;  //count number of rejected data points read from file
  }

  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in stomachcontent - found no data in the data file for", this->getName());
  if (reject != 0)
    handle.logMessage(LOGMESSAGE, "Discarded invalid stomachcontent data - number of invalid entries", reject);
  handle.logMessage(LOGMESSAGE, "Read stomachcontent data file - number of entries", count);
}

SCAmounts::~SCAmounts() {
  int i, j;
  for (i = 0; i < stddev.Nrow(); i++) {
    delete number[i];
    for (j = 0; j < stddev[i].Size(); j++)
      delete stddev[i][j];
  }
}

//JMB - note this ignores the number of samples ...
void SCAmounts::printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo) {

  if (!AAT.atCurrentTime(TimeInfo))
    return;

  int i, area, pred, prey;
  timeindex = -1;
  for (i = 0; i < Years.Size(); i++)
    if ((Years[i] == TimeInfo->getYear()) && (Steps[i] == TimeInfo->getStep()))
      timeindex = i;
  if (timeindex == -1)
    handle.logMessage(LOGFAIL, "Error in stomachcontent - invalid timestep");

  for (area = 0; area < modelConsumption.Ncol(timeindex); area++) {
    for (pred = 0; pred < modelConsumption[timeindex][area]->Nrow(); pred++) {
      for (prey = 0; prey < modelConsumption[timeindex][area]->Ncol(pred); prey++) {
        outfile << setw(lowwidth) << Years[timeindex] << sep << setw(lowwidth)
          << Steps[timeindex] << sep << setw(printwidth) << areaindex[area] << sep
          << setw(printwidth) << predindex[pred] << sep << setw(printwidth)
          << preyindex[prey] << sep << setprecision(largeprecision) << setw(largewidth);

        //JMB crude filter to remove the 'silly' values from the output
        if ((*modelConsumption[timeindex][area])[pred][prey] < rathersmall)
          outfile << 0;
        else
          outfile << (*modelConsumption[timeindex][area])[pred][prey];

        outfile << sep << setprecision(largeprecision) << setw(largewidth)
          << (*stddev[timeindex][area])[pred][prey] << endl;
      }
    }
  }
}

double SCAmounts::calcLikelihood() {
  int a, pred, prey;
  double tmplik, lik = 0.0;

  for (a = 0; a < areas.Nrow(); a++) {
    likelihoodValues[timeindex][a] = 0.0;
    for (pred = 0; pred < obsConsumption[timeindex][a]->Nrow(); pred++) {
      if (!(isZero((*number[timeindex])[a][pred]))) {
        tmplik = 0.0;
        for (prey = 0; prey < obsConsumption[timeindex][a]->Ncol(pred); prey++) {
          if (!(isZero((*stddev[timeindex][a])[pred][prey])))
            tmplik += ((*modelConsumption[timeindex][a])[pred][prey] -
              (*obsConsumption[timeindex][a])[pred][prey]) *
              ((*modelConsumption[timeindex][a])[pred][prey] -
              (*obsConsumption[timeindex][a])[pred][prey]) /
              ((*stddev[timeindex][a])[pred][prey] * (*stddev[timeindex][a])[pred][prey]);
        }
        tmplik *= (*number[timeindex])[a][pred];
        likelihoodValues[timeindex][a] += tmplik;
      }
    }
    lik += likelihoodValues[timeindex][a];
  }
  return lik;
}

// ********************************************************
// Functions for SCRatios
// ********************************************************
void SCRatios::setPredatorsAndPreys(PredatorPtrVector& Predators, PreyPtrVector& Preys) {
  int i, j, k, l;
  double tmpdivide, scale;
  SC::setPredatorsAndPreys(Predators, Preys);
  //Scale each row such that it sums up to 1
  for (i = 0; i < obsConsumption.Nrow(); i++) {
    for (j = 0; j < obsConsumption.Ncol(i); j++) {
      for (k = 0; k < obsConsumption[i][j]->Nrow(); k++) {
        scale = 0.0;
        for (l = 0; l < obsConsumption[i][j]->Ncol(k); l++)
          scale += (*obsConsumption[i][j])[k][l];

        if (!(isZero(scale))) {
          tmpdivide = 1.0 / scale;
          for (l = 0; l < obsConsumption[i][j]->Ncol(k); l++)
            (*obsConsumption[i][j])[k][l] *= tmpdivide;
        }
      }
    }
  }
}

double SCRatios::calcLikelihood() {
  int a, pred, prey;
  double scale, tmplik, tmpdivide;
  double lik = 0.0;

  for (a = 0; a < areas.Nrow(); a++) {
    likelihoodValues[timeindex][a] = 0.0;
    for (pred = 0; pred < obsConsumption[timeindex][a]->Nrow(); pred++) {
      scale = 0.0;
      for (prey = 0; prey < modelConsumption[timeindex][a]->Ncol(pred); prey++)
        scale += (*modelConsumption[timeindex][a])[pred][prey];

      if (!(isZero(scale))) {
        tmpdivide = 1.0 / scale;
        for (prey = 0; prey < obsConsumption[timeindex][a]->Ncol(pred); prey++)
          (*modelConsumption[timeindex][a])[pred][prey] *= tmpdivide;

        if (!(isZero((*number[timeindex])[a][pred]))) {
          tmplik = 0.0;
          for (prey = 0; prey < obsConsumption[timeindex][a]->Ncol(pred); prey++) {
            if (!(isZero((*stddev[timeindex][a])[pred][prey])))
              tmplik += ((*modelConsumption[timeindex][a])[pred][prey]  -
                (*obsConsumption[timeindex][a])[pred][prey]) *
                ((*modelConsumption[timeindex][a])[pred][prey]  -
                (*obsConsumption[timeindex][a])[pred][prey]) /
                ((*stddev[timeindex][a])[pred][prey] * (*stddev[timeindex][a])[pred][prey]);
          }
          tmplik *= (*number[timeindex])[a][pred];
          likelihoodValues[timeindex][a] += tmplik;
        }
      }
    }
    lik += likelihoodValues[timeindex][a];
  }
  return lik;
}

// ********************************************************
// Functions for SCSimple
// ********************************************************
SCSimple::SCSimple(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper,
  const char* datafilename, const char* givenname)
  : SC(infile, Area, TimeInfo, keeper, datafilename, givenname) {

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

  int i, year, step, count, reject;
  double tmpnumber;
  char tmparea[MaxStrLength], tmppred[MaxStrLength], tmpprey[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmppred, "", MaxStrLength);
  strncpy(tmpprey, "", MaxStrLength);
  int keepdata, timeid, areaid, predid, preyid;

  int numpred = 0;
  if (usepredages) //age structured predator
    numpred = predatorages.Nrow();
  else
    numpred = predatorlengths.Size() - 1;

  int numarea = areas.Nrow();
  int numprey = 0;
  for (i = 0; i < preylengths.Nrow(); i++)
    numprey += preylengths[i].Size() - 1;
  if (numprey == 0)
    handle.logMessage(LOGWARN, "Warning in stomachcontents - no prey found for", this->getName());

  //Check the number of columns in the inputfile
  infile >> ws;
  if (countColumns(infile) != 6)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 6");

  year = step = count = reject = 0;
  while (!infile.eof()) {
    keepdata = 1;
    infile >> year >> step >> tmparea >> tmppred >> tmpprey >> tmpnumber >> ws;

    //crude check to see if something has gone wrong and avoid infinite loops
    if (strlen(tmparea) == 0)
      handle.logFileMessage(LOGFAIL, "failed to read data from file");

    //if tmparea is in areaindex find areaid, else dont keep the data
    areaid = -1;
    for (i = 0; i < areaindex.Size(); i++)
      if (strcasecmp(areaindex[i], tmparea) == 0)
        areaid = i;

    if (areaid == -1)
      keepdata = 0;

    //if tmppred is in predindex find predid, else dont keep the data
    predid = -1;
    for (i = 0; i < predindex.Size(); i++)
      if (strcasecmp(predindex[i], tmppred) == 0)
        predid = i;

    if (predid == -1)
      keepdata = 0;

    //if tmpprey is in preyindex find preyid, else dont keep the data
    preyid = -1;
    for (i = 0; i < preyindex.Size(); i++)
      if (strcasecmp(preyindex[i], tmpprey) == 0)
        preyid = i;

    if (preyid == -1)
      keepdata = 0;

    //check if the year and step are in the simulation
    timeid = -1;
    if ((TimeInfo->isWithinPeriod(year, step)) && (keepdata == 1)) {
      //if this is a new timestep, resize to store the data
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        timeid = Years.Size() - 1;

        obsConsumption.resize();
        modelConsumption.resize();
        likelihoodValues.AddRows(1, numarea, 0.0);
        for (i = 0; i < numarea; i++) {
          obsConsumption[timeid].resize(new DoubleMatrix(numpred, numprey, 0.0));
          modelConsumption[timeid].resize(new DoubleMatrix(numpred, numprey, 0.0));
        }
      }

    } else
      keepdata = 0;

    if (keepdata == 1) {
      //stomach content data is required, so store it
      count++;
      (*obsConsumption[timeid][areaid])[predid][preyid] = tmpnumber;
    } else
      reject++;  //count number of rejected data points read from file
  }

  AAT.addActions(Years, Steps, TimeInfo);
  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in stomachcontent - found no data in the data file for", this->getName());
  if (reject != 0)
    handle.logMessage(LOGMESSAGE, "Discarded invalid stomachcontent data - number of invalid entries", reject);
  handle.logMessage(LOGMESSAGE, "Read stomachcontent data file - number of entries", count);
}

void SCSimple::setPredatorsAndPreys(PredatorPtrVector& Predators, PreyPtrVector& Preys) {
  int i, j, k, l;
  double tmpdivide, scale;
  SC::setPredatorsAndPreys(Predators, Preys);
  //Scale each row such that it sums up to 1
  for (i = 0; i < obsConsumption.Nrow(); i++) {
    for (j = 0; j < obsConsumption.Ncol(i); j++) {
      for (k = 0; k < obsConsumption[i][j]->Nrow(); k++) {
        scale = 0.0;
        for (l = 0; l < obsConsumption[i][j]->Ncol(k); l++)
          scale += (*obsConsumption[i][j])[k][l];

        if (!(isZero(scale))) {
          tmpdivide = 1.0 / scale;
          for (l = 0; l < obsConsumption[i][j]->Ncol(k); l++)
            (*obsConsumption[i][j])[k][l] *= tmpdivide;
        }
      }
    }
  }
}

double SCSimple::calcLikelihood() {
  int a, pred, prey;
  double scale, tmplik, tmpdivide;
  double lik = 0.0;
  
  for (a = 0; a < areas.Nrow(); a++) {
    likelihoodValues[timeindex][a] = 0.0;
    for (pred = 0; pred < obsConsumption[timeindex][a]->Nrow(); pred++) {
      scale = 0.0;
      for (prey = 0; prey < modelConsumption[timeindex][a]->Ncol(pred); prey++)
        scale += (*modelConsumption[timeindex][a])[pred][prey];
      
      if (!(isZero(scale))) //{
        tmpdivide = 1.0 / scale;
      else 
	tmpdivide = 0.0;
      tmplik = 0.0;
      for (prey = 0; prey < obsConsumption[timeindex][a]->Ncol(pred); prey++) {
	(*modelConsumption[timeindex][a])[pred][prey] *= tmpdivide;
	tmplik += ((*modelConsumption[timeindex][a])[pred][prey] -
		   (*obsConsumption[timeindex][a])[pred][prey]) *
	  ((*modelConsumption[timeindex][a])[pred][prey] -
	   (*obsConsumption[timeindex][a])[pred][prey]);
      }
      likelihoodValues[timeindex][a] += tmplik;
      //}
    }
    lik += likelihoodValues[timeindex][a];
  }
  return lik;
}
