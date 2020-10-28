#include "surveyindices.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "areatime.h"
#include "errorhandler.h"
#include "stock.h"
#include "sibylengthonstep.h"
#include "sibyageonstep.h"
#include "sibyfleetonstep.h"
#include "sibyeffortonstep.h"
#include "sibyacousticonstep.h"
#include "gadget.h"
#include "global.h"

SurveyIndices::SurveyIndices(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, double weight, const char* name)
  : Likelihood(SURVEYINDICESLIKELIHOOD, weight, name) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j, biomass;

  IntMatrix ages;
  DoubleVector lengths;
  CharPtrVector charindex, areaindex;

  char datafilename[MaxStrLength];
  char aggfilename[MaxStrLength];
  char sitype[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  strncpy(aggfilename, "", MaxStrLength);
  strncpy(sitype, "", MaxStrLength);

  ifstream datafile;
  CommentStream subdata(datafile);

  readWordAndValue(infile, "datafile", datafilename);
  readWordAndValue(infile, "sitype", sitype);

  //JMB - added the optional reading of the biomass flag
  infile >> ws;
  char c = infile.peek();
  biomass = 0;
  if ((c == 'b') || (c == 'B'))
    readWordAndVariable(infile, "biomass", biomass);
  if (biomass != 0 && biomass != 1)
    handle.logFileMessage(LOGFAIL, "\nError in surveyindex - biomass must be 0 or 1");

  //read in area aggregation from file
  readWordAndValue(infile, "areaaggfile", aggfilename);
  datafile.open(aggfilename, ios::binary);
  handle.checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  i = readAggregation(subdata, areas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Must change from outer areas to inner areas.
  for (i = 0; i < areas.Nrow(); i++)
    for (j = 0; j < areas.Ncol(i); j++)
      areas[i][j] = Area->getInnerArea(areas[i][j]);

  if ((strcasecmp(sitype, "lengths") == 0) || (strcasecmp(sitype, "length") == 0)) {
    readWordAndValue(infile, "lenaggfile", aggfilename);
    datafile.open(aggfilename, ios::binary);
    handle.checkIfFailure(datafile, aggfilename);
    handle.Open(aggfilename);
    i = readLengthAggregation(subdata, lengths, charindex);
    handle.Close();
    datafile.close();
    datafile.clear();
    //read the word 'stocknames'
    infile >> text >> ws;

  } else if ((strcasecmp(sitype, "ages") == 0) || (strcasecmp(sitype, "age") == 0)) {
    readWordAndValue(infile, "ageaggfile", aggfilename);
    datafile.open(aggfilename, ios::binary);
    handle.checkIfFailure(datafile, aggfilename);
    handle.Open(aggfilename);
    i = readAggregation(subdata, ages, charindex);
    handle.Close();
    datafile.close();
    datafile.clear();
    //read the word 'stocknames'
    infile >> text >> ws;

  } else if ((strcasecmp(sitype, "fleets") == 0) || (strcasecmp(sitype, "fleet") == 0)) {
    readWordAndValue(infile, "lenaggfile", aggfilename);
    datafile.open(aggfilename, ios::binary);
    handle.checkIfFailure(datafile, aggfilename);
    handle.Open(aggfilename);
    i = readLengthAggregation(subdata, lengths, charindex);
    handle.Close();
    datafile.close();
    datafile.clear();

    //read in the fleetnames
    i = 0;
    infile >> text >> ws;
    if (strcasecmp(text, "fleetnames") != 0)
      handle.logFileUnexpected(LOGFAIL, "fleetnames", text);
    infile >> text;
    while (!infile.eof() && (strcasecmp(text, "stocknames") != 0)) {
      fleetnames.resize(new char[strlen(text) + 1]);
      strcpy(fleetnames[i++], text);
      infile >> text >> ws;
    }
    if (fleetnames.Size() == 0)
      handle.logFileMessage(LOGFAIL, "\nError in surveyindex - failed to read fleets");
    handle.logMessage(LOGMESSAGE, "Read fleet data - number of fleets", fleetnames.Size());

  } else if ((strcasecmp(sitype, "effort") == 0) || (strcasecmp(sitype, "cpue") == 0)) {
    //read in the fleetnames
    i = 0;
    infile >> text >> ws;
    if (strcasecmp(text, "fleetnames") != 0)
      handle.logFileUnexpected(LOGFAIL, "fleetnames", text);
    infile >> text;
    while (!infile.eof() && (strcasecmp(text, "stocknames") != 0)) {
      fleetnames.resize(new char[strlen(text) + 1]);
      strcpy(fleetnames[i++], text);
      infile >> text >> ws;
    }
    if (fleetnames.Size() == 0)
      handle.logFileMessage(LOGFAIL, "\nError in surveyindex - failed to read fleets");
    handle.logMessage(LOGMESSAGE, "Read fleet data - number of fleets", fleetnames.Size());

    if (biomass) //JMB the biomass flag is probably not needed for effort index
      handle.logMessage(LOGWARN, "Warning in surveyindex - biomass flag specified for effort index");

  } else if (strcasecmp(sitype, "acoustic") == 0) {
    //read in the fleetnames
    i = 0;
    infile >> text >> ws;
    if (strcasecmp(text, "surveynames") != 0)
      handle.logFileUnexpected(LOGFAIL, "surveynames", text);
    infile >> text;
    while (!infile.eof() && (strcasecmp(text, "stocknames") != 0)) {
      charindex.resize(new char[strlen(text) + 1]);
      strcpy(charindex[i++], text);
      infile >> text >> ws;
    }
    if (charindex.Size() == 0)
      handle.logFileMessage(LOGFAIL, "\nError in surveyindex - failed to read acoustic survey names");
    handle.logMessage(LOGMESSAGE, "Read acoustic survey data - number of survey names", charindex.Size());

    if (biomass) //JMB the biomass flag is probably not needed for acoustic index
      handle.logMessage(LOGWARN, "Warning in surveyindex - biomass flag specified for acoustic index");

  } else if (strcasecmp(sitype, "ageandlengths") == 0) {
    handle.logFileMessage(LOGFAIL, "\nThe ageandlengths surveyindex likelihood component is no longer supported\nUse the surveydistribution likelihood component instead");

  } else
    handle.logFileMessage(LOGFAIL, "\nError in surveyindex - unrecognised type", sitype);

  if (strcasecmp(text, "stocknames") != 0)
    handle.logFileUnexpected(LOGFAIL, "stocknames", text);

  i = 0;
  infile >> text;
  //read in the stocknames
  while (!infile.eof() && (strcasecmp(text, "fittype") != 0)) {
    stocknames.resize(new char[strlen(text) + 1]);
    strcpy(stocknames[i++], text);
    infile >> text >> ws;
  }
  if (stocknames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "\nError in surveyindex - failed to read stocks");
  handle.logMessage(LOGMESSAGE, "Read stock data - number of stocks", stocknames.Size());

  //We have now read in all the data from the main likelihood file
  if ((strcasecmp(sitype, "lengths") == 0) || (strcasecmp(sitype, "length") == 0)) {
    SI = new SIByLengthOnStep(infile, areas, lengths, areaindex,
      charindex, TimeInfo, datafilename, this->getName(), biomass);

  } else if ((strcasecmp(sitype, "ages") == 0) || (strcasecmp(sitype, "age") == 0)) {
    SI = new SIByAgeOnStep(infile, areas, ages, areaindex,
      charindex, TimeInfo, datafilename, this->getName(), biomass);

  } else if ((strcasecmp(sitype, "fleets") == 0) || (strcasecmp(sitype, "fleet") == 0)) {
    SI = new SIByFleetOnStep(infile, areas, lengths, areaindex,
      charindex, TimeInfo, datafilename, this->getName(), biomass);

  } else if ((strcasecmp(sitype, "effort") == 0) || (strcasecmp(sitype, "cpue") == 0)) {
    SI = new SIByEffortOnStep(infile, areas, areaindex,
      fleetnames, TimeInfo, datafilename, this->getName(), biomass);

  } else if (strcasecmp(sitype, "acoustic") == 0) {
    SI = new SIByAcousticOnStep(infile, areas, areaindex,
      charindex, TimeInfo, datafilename, this->getName(), biomass);

  } else
    handle.logFileMessage(LOGFAIL, "\nError in surveyindex - unrecognised type", sitype);

  //delete objects that are no longer needed to free memory
  for (i = 0; i < charindex.Size(); i++)
    delete[] charindex[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];

  //prepare for next likelihood component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (strcasecmp(text, "[component]") != 0)
      handle.logFileUnexpected(LOGFAIL, "[component]", text);
  }
}

SurveyIndices::~SurveyIndices() {
  int i;
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
  for (i = 0; i < fleetnames.Size(); i++)
    delete[] fleetnames[i];
  delete SI;
}

void SurveyIndices::addLikelihood(const TimeClass* const TimeInfo) {
  SI->Sum(TimeInfo);
  if (TimeInfo->getTime() == TimeInfo->numTotalSteps())
    likelihood += SI->calcSSE();
}

void SurveyIndices::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, j, k, found;
  FleetPtrVector f;
  StockPtrVector s;

  for (i = 0; i < fleetnames.Size(); i++) {
    found = 0;
    for (j = 0; j < Fleets.Size(); j++) {
      if (strcasecmp(fleetnames[i], Fleets[j]->getName()) == 0) {
        found++;
        f.resize(Fleets[j]);
      }
    }
    if (found == 0)
      handle.logMessage(LOGFAIL, "Error in surveyindex - failed to match fleet", fleetnames[i]);
  }

  for (i = 0; i < f.Size(); i++)
    for (j = 0; j < f.Size(); j++)
      if ((strcasecmp(f[i]->getName(), f[j]->getName()) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in surveyindex - repeated fleet", f[i]->getName());

  for (i = 0; i < stocknames.Size(); i++) {
    found = 0;
    for (j = 0; j < Stocks.Size(); j++) {
      if (strcasecmp(stocknames[i], Stocks[j]->getName()) == 0) {
        found++;
        s.resize(Stocks[j]);
      }
    }
    if (found == 0)
      handle.logMessage(LOGFAIL, "Error in surveyindex - failed to match stock", stocknames[i]);
  }

  for (i = 0; i < s.Size(); i++)
    for (j = 0; j < s.Size(); j++)
      if ((strcasecmp(s[i]->getName(), s[j]->getName()) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in surveyindex - repeated stock", s[i]->getName());

  //check fleet and stock areas
  if (handle.getLogLevel() >= LOGWARN) {
    if (fleetnames.Size() > 0) {  //JMB might not be any fleets
      for (j = 0; j < areas.Nrow(); j++) {
        found = 0;
        for (i = 0; i < f.Size(); i++)
          for (k = 0; k < areas.Ncol(j); k++)
            if (f[i]->isInArea(areas[j][k]))
              found++;
        if (found == 0)
          handle.logMessage(LOGWARN, "Warning in surveyindex - fleet not defined on all areas");
      }
    }

    for (j = 0; j < areas.Nrow(); j++) {
      found = 0;
      for (i = 0; i < s.Size(); i++)
        for (k = 0; k < areas.Ncol(j); k++)
          if (s[i]->isInArea(areas[j][k]))
            found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in surveyindex - stock not defined on all areas");
    }
  }

  SI->setFleetsAndStocks(f, s);
}

void SurveyIndices::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  SI->Reset();
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset surveyindex component", this->getName());
}

void SurveyIndices::Print(ofstream& outfile) const {
  int i;
  outfile << "\nSurvey Indices " << this->getName() << " - likelihood value " << likelihood;
  if (stocknames.Size() > 0) {
    outfile << "\n\tStock names: ";
    for (i = 0; i < stocknames.Size(); i++)
      outfile << stocknames[i] << sep;
  }
  if (fleetnames.Size() > 0) {
    outfile << "\n\tFleet names: ";
    for (i = 0; i < fleetnames.Size(); i++)
      outfile << fleetnames[i] << sep;
  }
  outfile << endl;
  SI->Print(outfile);
  outfile.flush();
}
