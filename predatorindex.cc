#include "predatorindex.h"
#include "pionstep.h"
#include "predator.h"
#include "prey.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "areatime.h"
#include "errorhandler.h"
#include "predatoraggregator.h"
#include "stock.h"
#include "gadget.h"

PredatorIndices::PredatorIndices(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, double likweight)
  :Likelihood(PREDATORINDICESLIKELIHOOD, likweight) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;

  int numarea = 0;
  intmatrix tmpareas;
  charptrvector areaindex;
  doublevector predatorlengths;
  doublevector preylengths;
  charptrvector predlenindex;
  charptrvector preylenindex;

  char datafilename[MaxStrLength];
  char aggfilename[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  strncpy(aggfilename, "", MaxStrLength);

  ifstream datafile;
  CommentStream subdata(datafile);

  ReadWordAndValue(infile, "datafile", datafilename);

  //Read in the predator names and lengths
  i = 0;
  infile >> text >> ws;
  if (!(strcasecmp(text, "predatornames") == 0))
    handle.Unexpected("predatornames", text);
  infile >> text >> ws;
  while (!infile.eof() && !(strcasecmp(text, "predlenfile") == 0)) {
    predatornames.resize(1);
    predatornames[i] = new char[strlen(text) + 1];
    strcpy(predatornames[i++], text);
    infile >> text >> ws;
  }

  if (!(strcasecmp(text, "predlenfile") == 0))
    handle.Unexpected("predlenfile", text);
  infile >> aggfilename >> ws;
  datafile.open(aggfilename);
  CheckIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  i = ReadLengthAggregation(subdata, predatorlengths, predlenindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Read in the prey names and lengths
  i = 0;
  infile >> text >> ws;
  if (!(strcasecmp(text, "preynames") == 0))
    handle.Unexpected("preynames", text);
  infile >> text >> ws;
  while (!infile.eof() && !(strcasecmp(text, "preylenfile") == 0)) {
    preynames.resize(1);
    preynames[i] = new char[strlen(text) + 1];
    strcpy(preynames[i++], text);
    infile >> text >> ws;
  }

  if (!(strcasecmp(text, "preylenfile") == 0))
    handle.Unexpected("preylenfile", text);
  infile >> aggfilename >> ws;
  datafile.open(aggfilename);
  CheckIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  i = ReadLengthAggregation(subdata, preylengths, preylenindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Read in area aggregation from file
  ReadWordAndValue(infile, "areaaggfile", aggfilename);
  datafile.open(aggfilename);
  CheckIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  numarea = ReadAggregation(subdata, tmpareas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Check if we read correct input
  if (tmpareas.Nrow() != 1)
    handle.Message("Error - there should be only one area for the predator indices");

  for (i = 0; i < tmpareas.Ncol(0); i++)
    areas.resize(1, tmpareas[0][i]);

  //Check area data
  for (i = 0; i < areas.Size(); i++)
    if ((areas[i] = Area->InnerArea(areas[i])) == -1)
      handle.UndefinedArea(areas[i]);

  int biomass;
  ReadWordAndVariable(infile, "biomass", biomass);
  PI = new PIOnStep(infile, areas, predatorlengths, preylengths, TimeInfo, biomass,
    areaindex[0], preylenindex, predlenindex, datafilename);

  //prepare for next likelihood component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[component]") == 0))
      handle.Unexpected("[component]", text);
  }

  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < preylenindex.Size(); i++)
    delete[] preylenindex[i];
  for (i = 0; i < predlenindex.Size(); i++)
    delete[] predlenindex[i];
}

void PredatorIndices::SetPredatorsAndPreys(Predatorptrvector& Predators, Preyptrvector& Preys) {
  int i, j;
  int found = 0;
  Predatorptrvector predators;
  for (i = 0; i < predatornames.Size(); i++) {
    found = 0;
    for (j = 0; j < Predators.Size(); j++)
      if (strcasecmp(predatornames[i], Predators[j]->Name()) == 0) {
        found = 1;
        predators.resize(1, Predators[j]);
      }
    if (found == 0) {
      cerr << "Error when searching for names of predators for PredatorIndices\n"
        << "Did not find any name matching " << predatornames[i] << endl;
      exit(EXIT_FAILURE);
    }
  }

  Preyptrvector preys;
  for (i = 0; i < preynames.Size(); i++) {
    found = 0;
    for (j = 0; j < Preys.Size(); j++)
      if (strcasecmp(preynames[i], Preys[j]->Name()) == 0) {
        found = 1;
        preys.resize(1, Preys[j]);
      }
    if (found == 0) {
      cerr << "Error when searching for names of preys for PredatorIndices\n"
        << "Did not find any name matching " << preynames[i] << endl;
      exit(EXIT_FAILURE);
    }
  }
  PI->SetPredatorsAndPreys(predators, preys);
}

void PredatorIndices::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  PI->Reset(keeper);
}

void PIOnStep::SetPredatorsAndPreys(const Predatorptrvector& predators, const Preyptrvector& preys) {
  intmatrix areas(1, Areas.Size());
  int i;
  for (i = 0; i < Areas.Size(); i++)
    areas[0][i] = Areas[i];
  aggregator = new PredatorAggregator(predators, preys, areas, PredatorLgrpDiv, PreyLgrpDiv);
}

PIOnStep::PIOnStep(CommentStream& infile, const intvector& areas,
  const doublevector& predatorlengths, const doublevector& preylengths,
  const TimeClass* const TimeInfo, int biomass, const char* arealabel,
  const charptrvector& preylenindex, const charptrvector& predlenindex, const char* datafilename)
  : SIOnStep(infile, datafilename, arealabel, TimeInfo, predlenindex.Size() * preylenindex.Size(), predlenindex, preylenindex),
    PredatorLgrpDiv(0), PreyLgrpDiv(0), Biomass(biomass), Areas(areas), aggregator(0) {

  PredatorLgrpDiv = new LengthGroupDivision(predatorlengths);
  if (PredatorLgrpDiv->Error())
    LengthGroupPrintError(predatorlengths, "predator index, predator lengths");
  PreyLgrpDiv = new LengthGroupDivision(preylengths);
  if (PreyLgrpDiv->Error())
    LengthGroupPrintError(preylengths, "predator index, prey lengths");

  //read the predator indices data from the datafile
  ErrorHandler handle;
  ifstream datafile;
  CommentStream subdata(datafile);
  datafile.open(datafilename);
  CheckIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  ReadPredatorData(subdata, arealabel, predlenindex, preylenindex, TimeInfo);
  handle.Close();
  datafile.close();
  datafile.clear();
}

void PIOnStep::ReadPredatorData(CommentStream& infile, const char* arealabel,
  const charptrvector& predlenindex, const charptrvector& preylenindex, const TimeClass* TimeInfo) {

  int i;
  int year, step;
  double tmpnumber;
  char tmparea[MaxStrLength], tmppred[MaxStrLength], tmpprey[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmppred, "", MaxStrLength);
  strncpy(tmpprey, "", MaxStrLength);
  int keepdata, timeid, predid, preyid, colid;
  int numprey = preylenindex.Size();
  ErrorHandler handle;

  //Check the number of columns in the inputfile
  if (CountColumns(infile) != 6)
    handle.Message("Wrong number of columns in inputfile - should be 6");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> tmparea >> tmppred >> tmpprey >> tmpnumber >> ws;

    //check if the year and step are in the simulation
    timeid = -1;
    if (TimeInfo->IsWithinPeriod(year, step)) {
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;
    } else
      keepdata = 1;

    //if tmparea matches strlabel keep data, else dont keep the data
    if (!(strcasecmp(arealabel, tmparea) == 0))
      keepdata = 1;

    //if tmppred is in predlenindex keep data, else dont keep the data
    predid = -1;
    for (i = 0; i < predlenindex.Size(); i++)
      if (strcasecmp(predlenindex[i], tmppred) == 0)
        predid = i;

    if (predid == -1)
      keepdata = 1;

    //if tmpprey is in preylenindex keep data, else dont keep the data
    preyid = -1;
    for (i = 0; i < preylenindex.Size(); i++)
      if (strcasecmp(preylenindex[i], tmpprey) == 0)
        preyid = i;

    if (preyid == -1)
      keepdata = 1;

    if (keepdata == 0) {
      //survey indices data is required, so store it
      colid = preyid + (numprey * predid);
      //JMB - this should really be stored as [time][prey][pred]
      Indices[timeid][colid] = tmpnumber;
    }
  }
}

void PIOnStep::Sum(const class TimeClass* const TimeInfo) {
  if (!(this->IsToSum(TimeInfo)))
    return;
  if (Biomass)
    aggregator->Sum();
  else
    aggregator->NumberSum();

  const bandmatrixvector* cons = &(aggregator->ReturnSum());
  int ncol = (*cons)[0].Nrow() * (*cons)[0].Ncol();
  doublevector numbers(ncol);
  int k = 0;
  int predlen, preylen;
  for (predlen = 0; predlen < (*cons)[0].Nrow(); predlen ++)
    for (preylen = 0; preylen < (*cons)[0].Ncol(); preylen++) {
      numbers[k] = (*cons)[0][predlen][preylen];
      k++;
    }
  this->KeepNumbers(numbers);
}

PIOnStep::~PIOnStep() {
  delete aggregator;
  delete PredatorLgrpDiv;
  delete PreyLgrpDiv;
}

PredatorIndices::~PredatorIndices() {
  int i;
  for (i = 0; i < predatornames.Size(); i++)
    delete[] predatornames[i];
  for (i = 0; i < preynames.Size(); i++)
    delete[] preynames[i];
  delete PI;
}

void PredatorIndices::AddToLikelihood(const TimeClass* const TimeInfo) {
  likelihood = 0;
  PI->Sum(TimeInfo);
  if (TimeInfo->CurrentTime() == TimeInfo->TotalNoSteps())
    likelihood += PI->Regression();
}
