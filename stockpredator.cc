#include "stockpredator.h"
#include "keeper.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "prey.h"
#include "areatime.h"
#include "suits.h"
#include "readword.h"
#include "gadget.h"

extern ErrorHandler handle;

StockPredator::StockPredator(CommentStream& infile, const char* givenname, const IntVector& Areas,
  const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv,
  int minage, int maxage, const TimeClass* const TimeInfo, Keeper* const keeper)
  : PopPredator(givenname, Areas, OtherLgrpDiv, GivenLgrpDiv),
    maxconbylength(areas.Size(), GivenLgrpDiv->numLengthGroups(), 0.0) {

  keeper->addString("predator");
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  infile >> text >> ws;
  if (!(strcasecmp(text, "suitability") == 0))
    handle.Unexpected("suitability", text);

  this->readSuitabilityMatrix(infile, "maxconsumption", TimeInfo, keeper);

  keeper->addString("maxconsumption");
  maxconsumption.resize(4, keeper);
  if (!(infile >> maxconsumption))
    handle.Message("Error in stock file - incorrect format of maxconsumption vector");
  maxconsumption.Inform(keeper);

  keeper->clearLast();
  keeper->addString("halffeedingvalue");
  readWordAndFormula(infile, "halffeedingvalue", halfFeedingValue);
  halfFeedingValue.Inform(keeper);
  keeper->clearLast();
  keeper->clearLast();

  //Everything read from infile.
  IntVector size(maxage - minage + 1, GivenLgrpDiv->numLengthGroups());
  IntVector minlength(maxage - minage + 1, 0);

  int numlength = LgrpDiv->numLengthGroups();
  int numarea = areas.Size();
  Alkeys.resize(numarea, minage, minlength, size);
  BandMatrix bm(minlength, size, minage); //default initialization to 0.
  Alprop.resize(numarea, bm);
  Phi.AddRows(numarea, numlength, 0.0);
  fphi.AddRows(numarea, numlength, 0.0);
  fphI.AddRows(numarea, numlength, 0.0);
  //Predator::setPrey will call resizeObjects.
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
    Alkeys[area].printNumbers(outfile);
  }
  for (area = 0; area < areas.Size(); area++) {
    outfile << "Alkeys (mean weights) on internal area " << areas[area] << endl;
    Alkeys[area].printWeights(outfile);
  }
  for (area = 0; area < areas.Size(); area++) {
    outfile << "Age-length proportion on internal area " << areas[area] << endl;
    Alprop[area].Print(outfile);
  }
  outfile << "Maximum consumption by length.\n";
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tInternal area " << areas[area] << TAB;
    for (i = 0; i < maxconbylength.Ncol(); i++) {
      outfile.precision(smallprecision);
      outfile.width(smallwidth);
      outfile << sep << maxconbylength[area][i];
    }
    outfile << endl;
  }
  outfile << endl;
}

void StockPredator::resizeObjects() {
  PopPredator::resizeObjects();
}

void StockPredator::Sum(const AgeBandMatrix& stock, int area) {
  int length;
  const int inarea = AreaNr[area];
  Alkeys[inarea].setToZero();
  Alkeys[inarea].Add(stock, *CI);
  for (length = 0; length < Prednumber.Ncol(inarea); length++)
    Prednumber[inarea][length].N = 0.0;
  Alkeys[inarea].Colsum(Prednumber[inarea]);
}

void StockPredator::Reset(const TimeClass* const TimeInfo) {
  this->PopPredator::Reset(TimeInfo);
  int a, l, age;
  if (TimeInfo->CurrentTime() == 1) {
    for (a = 0; a < areas.Size(); a++) {
      for (l = 0; l < LgrpDiv->numLengthGroups(); l++) {
        Phi[a][l] = 0.0;
        fphi[a][l] = 0.0;
        maxconbylength[a][l] = 0.0;
      }
    }
    for (a = 0; a < areas.Size(); a++) {
      for (age = Alkeys[a].minAge(); age <= Alkeys[a].maxAge(); age++) {
        for (l = Alkeys[a].minLength(age); l < Alkeys[a].maxLength(age); l++) {
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
  for (prey = 0; prey < numPreys(); prey++)
    if (strcasecmp(Preyname(prey), preyname) == 0)
      return Preys(prey)->NumberPriortoEating(area);

  handle.logFailure("Error in stockpredator - failed to match prey", preyname);
  exit(EXIT_FAILURE);
}
