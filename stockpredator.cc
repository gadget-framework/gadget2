#include "stockpredator.h"
#include "keeper.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "prey.h"
#include "conversion.h"
#include "areatime.h"
#include "print.h"
#include "suits.h"
#include "gadget.h"

StockPredator::StockPredator(CommentStream& infile, const char* givenname, const intvector& Areas,
  const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv,
  int minage, int maxage, const TimeClass* const TimeInfo, Keeper* const keeper)
  : PopPredator(givenname, Areas, OtherLgrpDiv, GivenLgrpDiv),
  MaxconByLength(areas.Size(), GivenLgrpDiv->NoLengthGroups(), 0) {

  //NumberOfMaxConsumptionConstants = 4;
  ErrorHandler handle;
  keeper->AddString("predator");
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  infile >> text >> ws;
  if (!(strcasecmp(text, "suitability") == 0))
    handle.Unexpected("suitability", text);

  this->ReadSuitabilityMatrix(infile, "Maxconsumption", TimeInfo, keeper);

  keeper->AddString("maxconsumption");
  maxConsumption.resize(4, keeper);  //Maxnumber of constants is 4
  if (!(infile >> maxConsumption))
    handle.Message("Incorrect format of growthPar vector");
  maxConsumption.Inform(keeper);

  infile >> text;
  keeper->ClearLastAddString("halffeedingvalue");
  if (strcasecmp(text, "halffeedingvalue") == 0) {
    if (!(infile >> halfFeedingValue))
      handle.Message("Could not read HalfFeedingValue in stockpredator");
    halfFeedingValue.Inform(keeper);
  } else
    handle.Unexpected("halffeedingvalue", text);

  keeper->ClearLast();
  keeper->ClearLast();
  //Everything read from infile.
  intvector size(maxage - minage + 1, GivenLgrpDiv->NoLengthGroups());
  intvector minlength(maxage - minage + 1, 0);
  Alkeys.resize(areas.Size(), minage, minlength, size);
  bandmatrix bm(minlength, size, minage); //default initialization to 0.
  Alprop.resize(areas.Size(), bm);
  Phi.AddRows(areas.Size(), LgrpDiv->NoLengthGroups(), 0.0);
  fphi.AddRows(areas.Size(), LgrpDiv->NoLengthGroups(), 0.0);
  fphI.AddRows(areas.Size(), LgrpDiv->NoLengthGroups(), 0.0);
  //Predator::SetPrey will call ResizeObjects.
}

StockPredator::~StockPredator() {
}

void StockPredator::Print(ofstream& outfile) const {
  int i, area;
  PopPredator::Print(outfile);
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tPhi on area " << areas[area] <<  endl << TAB;
    for (i = 0; i < Phi.Ncol(area); i++) {
      outfile.precision(smallprecision);
      outfile.width(printwidth);
      outfile << sep << Phi[area][i];
    }
    outfile << endl;
  }
  for (area = 0; area < areas.Size(); area++) {
    outfile << "Alkeys (numbers) on area " << areas[area] << endl;
    Printagebandm(outfile, Alkeys[area]);
  }
  for (area = 0; area < areas.Size(); area++) {
    outfile << "Alkeys (mean weights) on area " << areas[area] << endl;
    PrintWeightinagebandm(outfile, Alkeys[area]);
  }
  for (area = 0; area < areas.Size(); area++) {
    outfile << "Age-length proportion on area " << areas[area] << endl;
    BandmatrixPrint(Alprop[area], outfile);
  }
  outfile << "Maximum consumption by length.\n";
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tArea " << areas[area] << TAB;
    for (i = 0; i < MaxconByLength.Ncol(); i++) {
      outfile.precision(smallprecision);
      outfile.width(printwidth);
      outfile << sep << MaxconByLength[area][i];
    }
    outfile << endl;
  }
  outfile << endl;
}

const doublevector& StockPredator::FPhi(int area) const {
  return fphi[AreaNr[area]];
}

const doublevector& StockPredator::MaxConByLength(int area) const {
  return MaxconByLength[AreaNr[area]];
}

void StockPredator::ResizeObjects() {
  PopPredator::ResizeObjects();
}

void StockPredator::Sum(const Agebandmatrix& stock, int area) {
  const int inarea = AreaNr[area];
  Alkeys[inarea].SettoZero();
  AgebandmAdd(Alkeys[inarea], stock, *CI);
  int length;
  for (length = 0; length < Prednumber.Ncol(inarea); length++)
    Prednumber[inarea][length].N = 0.0;
  Alkeys[inarea].Colsum(Prednumber[inarea]);
}

const bandmatrix& StockPredator::Alproportion(int area) const {
  return Alprop[AreaNr[area]];
}

void StockPredator::Reset(const TimeClass* const TimeInfo) {
  this->PopPredator::Reset(TimeInfo);
  int a, l, age;
  if (TimeInfo->CurrentTime() == 1) {
    for (a = 0; a < areas.Size(); a++) {
      for (l = 0; l < LgrpDiv->NoLengthGroups(); l++) {
        Phi[a][l] = 0.0;
        fphi[a][l] = 0.0;
        MaxconByLength[a][l] = 0.0;
      }
    }
    for (a = 0; a < areas.Size(); a++) {
      for (age = Alkeys[a].Minage(); age <= Alkeys[a].Maxage(); age++) {
        for (l = Alkeys[a].Minlength(age); l < Alkeys[a].Maxlength(age); l++) {
          Alkeys[a][age][l].N = 0.0;
          Alkeys[a][age][l].W = 0.0;
          Alprop[a][age][l] = 0.0;
        }
      }
    }
  }
}

const popinfovector& StockPredator::NumberPriortoEating(int area, const char* preyname) const {
  int prey;
  for (prey = 0; prey < NoPreys(); prey++)
    if (strcasecmp(Preyname(prey), preyname) == 0)
      return Preys(prey)->NumberPriortoEating(area);

  cerr << "Predator " << this->Name() << " was asked for consumption\n"
    << "of prey " << preyname << " which he does not eat\n";
  exit(EXIT_FAILURE);
}
