#include "surveyindices.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "areatime.h"
#include "errorhandler.h"
#include "stock.h"
#include "sibylengthonstep.h"
#include "sibylengthandageonstep.h"
#include "sibyageonstep.h"
#include "gadget.h"

extern ErrorHandler handle;

SurveyIndices::SurveyIndices(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper, double weight, const char* name)
  : Likelihood(SURVEYINDICESLIKELIHOOD, weight) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j;

  IntMatrix ages;
  DoubleVector lengths;
  CharPtrVector areaindex;
  CharPtrVector ageindex;
  CharPtrVector lenindex;

  char datafilename[MaxStrLength];
  char aggfilename[MaxStrLength];
  char sitype[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  strncpy(aggfilename, "", MaxStrLength);
  strncpy(sitype, "", MaxStrLength);

  ifstream datafile;
  CommentStream subdata(datafile);

  surveyname = new char[strlen(name) + 1];
  strcpy(surveyname, name);
  readWordAndValue(infile, "datafile", datafilename);
  readWordAndValue(infile, "sitype", sitype);

  //read in area aggregation from file
  readWordAndValue(infile, "areaaggfile", aggfilename);
  datafile.open(aggfilename, ios::in);
  handle.checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  i = readAggregation(subdata, areas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Check if we read correct input
  if (areas.Nrow() != 1)
    handle.Message("Error - there should be only one area for the survey indices");

  //Must change from outer areas to inner areas.
  for (i = 0; i < areas.Nrow(); i++)
    for (j = 0; j < areas.Ncol(i); j++)
      if ((areas[i][j] = Area->InnerArea(areas[i][j])) == -1)
        handle.UndefinedArea(areas[i][j]);

  if (strcasecmp(sitype, "lengths") == 0) {
    readWordAndValue(infile, "lenaggfile", aggfilename);
    datafile.open(aggfilename, ios::in);
    handle.checkIfFailure(datafile, aggfilename);
    handle.Open(aggfilename);
    i = readLengthAggregation(subdata, lengths, lenindex);
    handle.Close();
    datafile.close();
    datafile.clear();

  } else if (strcasecmp(sitype, "ages") == 0) {
    readWordAndValue(infile, "ageaggfile", aggfilename);
    datafile.open(aggfilename, ios::in);
    handle.checkIfFailure(datafile, aggfilename);
    handle.Open(aggfilename);
    i = readAggregation(subdata, ages, ageindex);
    handle.Close();
    datafile.close();
    datafile.clear();

  } else if (strcasecmp(sitype, "ageandlengths") == 0) {
    readWordAndValue(infile, "lenaggfile", aggfilename);
    datafile.open(aggfilename, ios::in);
    handle.checkIfFailure(datafile, aggfilename);
    handle.Open(aggfilename);
    i = readLengthAggregation(subdata, lengths, lenindex);
    handle.Close();
    datafile.close();
    datafile.clear();

    readWordAndValue(infile, "ageaggfile", aggfilename);
    datafile.open(aggfilename, ios::in);
    handle.checkIfFailure(datafile, aggfilename);
    handle.Open(aggfilename);
    i = readAggregation(subdata, ages, ageindex);
    handle.Close();
    datafile.close();
    datafile.clear();

  } else
    handle.Unexpected("lengths, ages or ageandlengths", sitype);

  //read in the stocknames
  i = 0;
  infile >> text >> ws;
  if (!(strcasecmp(text, "stocknames") == 0))
    handle.Unexpected("stocknames", text);
  infile >> text;
  while (!infile.eof() && !(strcasecmp(text, "fittype") == 0)) {
    stocknames.resize(1);
    stocknames[i] = new char[strlen(text) + 1];
    strcpy(stocknames[i++], text);
    infile >> text >> ws;
  }

  //We have now read in all the data from the main likelihood file
  if (strcasecmp(sitype, "lengths") == 0) {
    SI = new SIByLengthOnStep(infile, areas, lengths, areaindex,
      lenindex, TimeInfo, datafilename, surveyname);

  } else if (strcasecmp(sitype, "ages") == 0) {
    SI = new SIByAgeOnStep(infile, areas, ages, areaindex,
      ageindex, TimeInfo, datafilename, surveyname);

  } else if (strcasecmp(sitype, "ageandlengths") == 0) {
    SI = new SIByLengthAndAgeOnStep(infile, areas, lengths, ages, TimeInfo,
      keeper, lenindex, ageindex, areaindex, datafilename, surveyname);

  } else
    handle.Message("Error in surveyindex - unrecognised type", sitype);

  //prepare for next likelihood component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[component]") == 0))
      handle.Unexpected("[component]", text);
  }

  //index ptrvectors are not required - free up memory
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < ageindex.Size(); i++)
    delete[] ageindex[i];
  for (i = 0; i < lenindex.Size(); i++)
    delete[] lenindex[i];
}

SurveyIndices::~SurveyIndices() {
  int i;
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
  delete SI;
  delete[] surveyname;
}

void SurveyIndices::LikelihoodPrint(ofstream& outfile) {
  int i, j;
  outfile << "\nSurvey Indices\n\nLikelihood " << likelihood
    << "\nWeight " << weight << "\nStock names: ";
  for (i = 0; i < stocknames.Size(); i++)
    outfile << stocknames[i] << sep;
  outfile << "\nInternal areas";
  for (i = 0; i < areas.Nrow(); i++) {
    outfile << endl;
    for (j = 0; j < areas.Ncol(i); j++)
      outfile << areas[i][j] << sep;
  }
  outfile << endl;
  SI->LikelihoodPrint(outfile);
  outfile << endl;
  outfile.flush();
}

void SurveyIndices::addLikelihood(const TimeClass* const TimeInfo) {
  SI->Sum(TimeInfo);
  if (TimeInfo->CurrentTime() == TimeInfo->TotalNoSteps())
    likelihood += SI->Regression();
}

void SurveyIndices::setStocks(StockPtrVector& Stocks) {
  int i, j;
  int found = 0;
  StockPtrVector s;
  for (i = 0; i < stocknames.Size(); i++) {
    found = 0;
    for (j = 0; j < Stocks.Size(); j++)
      if (strcasecmp(stocknames[i], Stocks[j]->Name()) == 0) {
        found = 1;
        s.resize(1, Stocks[j]);
      }
    if (found == 0)
      handle.logFailure("Error in surveyindex - failed to match stock", stocknames[i]);

  }
  SI->setStocks(s);
}

void SurveyIndices::Reset(const Keeper* const keeper) {
  SI->Reset(keeper);
  Likelihood::Reset(keeper);
}

void SurveyIndices::Print(ofstream& outfile) const {
  int i;
  outfile << "\nSurvey Indices " << surveyname << " - likelihood value " << likelihood << "\n\tStock names: ";
  for (i = 0; i < stocknames.Size(); i++)
    outfile << stocknames[i] << sep;
  outfile << endl;
  SI->Print(outfile);
}
