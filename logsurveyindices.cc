#include "logsurveyindices.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "areatime.h"
#include "errorhandler.h"
#include "stock.h"
#include "logsionstep.h"
#include "gadget.h"

LogSurveyIndices::LogSurveyIndices(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper, double likweight, const char* name)
  : Likelihood(LOGSURVEYLIKELIHOOD, likweight), surveyname(name) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j;

  intmatrix Ages;
  doublevector Lengths;
  int numarea = 0;
  charptrvector areaindex;
  charptrvector ageindex;
  charptrvector lenindex;

  char datafilename[MaxStrLength];
  char aggfilename[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  strncpy(aggfilename, "", MaxStrLength);

  ifstream datafile;
  CommentStream subdata(datafile);

  ReadWordAndValue(infile, "datafile", datafilename);
  //Read in area aggregation from file
  ReadWordAndValue(infile, "areaaggfile", aggfilename);
  datafile.open(aggfilename);
  CheckIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  numarea = ReadAggregation(subdata, areas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Check if we read sufficient input.
  if (numarea != 1)
    handle.Message("Error - there should be one area for the log survey indices");

  //Check area data
  for (i = 0; i < areas.Size(); i++)
    if ((areas[i] = Area->InnerArea(areas[i])) == -1)
      handle.UndefinedArea(areas[i]);

  ReadWordAndValue(infile, "lenaggfile", aggfilename);
  datafile.open(aggfilename);
  CheckIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  i = ReadLengthAggregation(subdata, Lengths, lenindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  ReadWordAndValue(infile, "ageaggfile", aggfilename);
  datafile.open(aggfilename);
  CheckIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  i = ReadAggregation(subdata, Ages, ageindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Store minage & maxage for later use in printing
  minage = Ages[0][0];
  maxage = minage;
  for (i = 0; i < Ages.Nrow(); i++) {
    for (j = 0; j < Ages[i].Size(); j++) {
      if (Ages[i][j] < minage)
        minage = Ages[i][j];
      if (Ages[i][j] > maxage)
        maxage = Ages[i][j];
    }
  }

  //Read in the stocknames
  i = 0;
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
  SI = new LogSIOnStep(infile, areas, Lengths, Ages, TimeInfo,
      keeper, lenindex, ageindex, areaindex[0], datafilename);

  //prepare for next likelihood component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[component]") == 0))
      handle.Unexpected("[component]", text);
  }
}

LogSurveyIndices::~LogSurveyIndices() {
  int i;
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
  delete SI;
}

void LogSurveyIndices::LikelihoodPrint(ofstream& outfile) const {
  int i;
  outfile << "\nLogSurvey Indices\nlikelihood " << likelihood
    << "\nStocknames: " << sep;
  for (i = 0; i < stocknames.Size(); i++)
    outfile << stocknames[i] << sep;
  outfile << endl;
  Likelihood::LikelihoodPrint(outfile);
  outfile << " Areas\n";
  for (i = 0; i < areas.Size(); i++)
    outfile << areas[i] << TAB;
  outfile << endl;
  SI->LikelihoodPrint(outfile);
}

void LogSurveyIndices::AddToLikelihood(const TimeClass* const TimeInfo) {
  likelihood = 0.0;
  SI->Sum(TimeInfo);
  if (TimeInfo->CurrentTime() == TimeInfo->TotalNoSteps())
    likelihood += SI->Regression();
}

void LogSurveyIndices::SetStocks(Stockptrvector& Stocks) {
  int i, j;
  int found;
  Stockptrvector s;
  for (i = 0; i < stocknames.Size(); i++) {
    found = 0;
    for (j = 0; j < Stocks.Size(); j++)
      if (strcasecmp(stocknames[i], Stocks[j]->Name()) == 0) {
        found = 1;
        s.resize(1, Stocks[j]);
      }
    if (found == 0) {
      cerr << "Error when searching for names of stocks for survey indices\n"
        << "Did not find any name matching " << stocknames[i] << endl;
      exit(EXIT_FAILURE);
    }
  }
  SI->SetStocks(s);
}

void LogSurveyIndices::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  SI->Reset(keeper);
}

void LogSurveyIndices::Print(ofstream& outfile) const {
  int i;
  outfile << "\nLog Survey Indices " << surveyname << "\nlikelihood " << likelihood
    << "\nStocknames: ";
  for (i = 0; i < stocknames.Size(); i++)
    outfile << stocknames[i] << sep;
  outfile << endl;
  SI->Print(outfile);
}

void LogSurveyIndices::printHeader(ofstream& surveyfile, const PrintInfo& print) {
  timeindex = 0;
  int i;
  if (print.surveyprint) {
    surveyfile << "\nsurvey " << surveyname << "\nstocks";
    for (i = 0; i<stocknames.Size(); i++)
      surveyfile << sep <<stocknames[i];

    surveyfile << "\nminage " << minage << " maxage " << maxage
      << " minlen " << min_length << " maxlen " << max_length << " lenstep "
      << dl << endl << endl;
  }
}

void LogSurveyIndices::print(ofstream& catchfile, const TimeClass& time, const PrintInfo& print) {
  if (print.catchprint)
    timeindex++;
}
