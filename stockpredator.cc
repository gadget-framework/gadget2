#include "stockpredator.h"
#include "keeper.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "prey.h"
#include "areatime.h"
#include "print.h"
#include "suits.h"
#include "readword.h"
#include "gadget.h"

StockPredator::StockPredator(CommentStream& infile, const char* givenname, const IntVector& Areas,
  const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv,
  int minage, int maxage, const TimeClass* const TimeInfo, Keeper* const keeper)
  : PopPredator(givenname, Areas, OtherLgrpDiv, GivenLgrpDiv),
    MaxconByLength(areas.Size(), GivenLgrpDiv->NoLengthGroups(), 0.0) {

  ErrorHandler handle;
  keeper->AddString("predator");
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  infile >> text >> ws;
  if (!(strcasecmp(text, "suitability") == 0))
    handle.Unexpected("suitability", text);

  this->ReadSuitabilityMatrix(infile, "maxconsumption", TimeInfo, keeper);

  keeper->AddString("maxconsumption");
  maxConsumption.resize(4, keeper);  //Maxnumber of constants is 4
  if (!(infile >> maxConsumption))
    handle.Message("Incorrect format of maxconsumption vector");
  maxConsumption.Inform(keeper);

  keeper->ClearLastAddString("halffeedingvalue");
  readWordAndFormula(infile, "halffeedingvalue", halfFeedingValue);
  halfFeedingValue.Inform(keeper);
  keeper->ClearLast();

  keeper->ClearLast();
  //Everything read from infile.
  IntVector size(maxage - minage + 1, GivenLgrpDiv->NoLengthGroups());
  IntVector minlength(maxage - minage + 1, 0);

  int numlength = LgrpDiv->NoLengthGroups();
  int numarea = areas.Size();
  Alkeys.resize(numarea, minage, minlength, size);
  BandMatrix bm(minlength, size, minage); //default initialization to 0.
  Alprop.resize(numarea, bm);
  Phi.AddRows(numarea, numlength, 0.0);
  fphi.AddRows(numarea, numlength, 0.0);
  fphI.AddRows(numarea, numlength, 0.0);
  //Predator::SetPrey will call ResizeObjects.
}

void StockPredator::Print(ofstream& outfile) const {
  int i, area;
  PopPredator::Print(outfile);
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tPhi on internal area " << areas[area] << endl << TAB;
    for (i = 0; i < Phi.Ncol(area); i++) {
      outfile.precision(smallprecision);
      outfile.width(smallwidth);
      outfile << Phi[area][i] << sep;
    }
    outfile << endl;
  }
  for (area = 0; area < areas.Size(); area++) {
    outfile << "Alkeys (numbers) on internal area " << areas[area] << endl;
    Printagebandm(outfile, Alkeys[area]);
  }
  for (area = 0; area < areas.Size(); area++) {
    outfile << "Alkeys (mean weights) on internal area " << areas[area] << endl;
    PrintWeightinagebandm(outfile, Alkeys[area]);
  }
  for (area = 0; area < areas.Size(); area++) {
    outfile << "Age-length proportion on internal area " << areas[area] << endl;
    BandmatrixPrint(Alprop[area], outfile);
  }
  outfile << "Maximum consumption by length.\n";
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tInternal area " << areas[area] << TAB;
    for (i = 0; i < MaxconByLength.Ncol(); i++) {
      outfile.precision(smallprecision);
      outfile.width(smallwidth);
      outfile << sep << MaxconByLength[area][i];
    }
    outfile << endl;
  }
  outfile << endl;
}

void StockPredator::ResizeObjects() {
  PopPredator::ResizeObjects();
}

void StockPredator::Sum(const AgeBandMatrix& stock, int area) {
  const int inarea = AreaNr[area];
  Alkeys[inarea].setToZero();
  AgebandmAdd(Alkeys[inarea], stock, *CI);
  int length;
  for (length = 0; length < Prednumber.Ncol(inarea); length++)
    Prednumber[inarea][length].N = 0.0;
  Alkeys[inarea].Colsum(Prednumber[inarea]);
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

const PopInfoVector& StockPredator::NumberPriortoEating(int area, const char* preyname) const {
  int prey;
  for (prey = 0; prey < NoPreys(); prey++)
    if (strcasecmp(Preyname(prey), preyname) == 0)
      return Preys(prey)->NumberPriortoEating(area);

  cerr << "Predator " << this->Name() << " was asked for consumption\n"
    << "of prey " << preyname << " which he does not eat\n";
  exit(EXIT_FAILURE);
}
