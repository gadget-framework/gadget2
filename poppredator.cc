#include "poppredator.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

PopPredator::PopPredator(const char* givenname, const IntVector& Areas,
  const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv)
  : Predator(givenname, Areas) {

  int i;
  if (isZero(GivenLgrpDiv->dl())) {
    DoubleVector dv(GivenLgrpDiv->numLengthGroups() + 1);
    for (i = 0; i < dv.Size() - 1; i++)
      dv[i] = GivenLgrpDiv->minLength(i);
    dv[i] = GivenLgrpDiv->maxLength(i - 1);
    LgrpDiv = new LengthGroupDivision(dv);
  } else
    LgrpDiv = new LengthGroupDivision(*GivenLgrpDiv);

  CI = new ConversionIndex(OtherLgrpDiv, LgrpDiv);
}

PopPredator::~PopPredator() {
  delete LgrpDiv;
  delete CI;
}

void PopPredator::Print(ofstream& outfile) const {
  Predator::Print(outfile);
  int i, area;

  outfile << TAB;
  LgrpDiv->Print(outfile);
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tNumber of predators on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
      outfile.precision(smallprecision);
      outfile.width(smallwidth);
      outfile << sep << Prednumber[area][i].N;
    }
    outfile << "\n\tWeight of predators on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
      outfile.precision(smallprecision);
      outfile.width(smallwidth);
      outfile << sep << Prednumber[area][i].W;
    }
    outfile << "\n\tTotal amount eaten on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
      outfile.precision(smallprecision);
      outfile.width(smallwidth);
      outfile << sep << totalconsumption[area][i];
    }
    outfile << "\n\tOverconsumption on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
      outfile.precision(smallprecision);
      outfile.width(smallwidth);
      outfile << sep << overconsumption[area][i];
    }
    outfile << endl;
  }
}

const BandMatrix& PopPredator::Consumption(int area, const char* preyname) const {
  int prey;
  for (prey = 0; prey < numPreys(); prey++)
    if (strcasecmp(Preyname(prey), preyname) == 0)
      return consumption[AreaNr[area]][prey];

  handle.logFailure("Error in poppredator - failed to match prey", preyname);
  exit(EXIT_FAILURE);
}

const double PopPredator::consumedBiomass(int prey, int area) const{
  int age, len;
  double tons = 0.0;
  const BandMatrix& bio = consumption[area][prey];
  for (age = bio.minAge(); age <= bio.maxAge(); age++)
    for (len = bio.minLength(age); len < bio.maxLength(age); len++)
      tons += bio[age][len];

  return tons;
}

void PopPredator::DeleteParametersForPrey(int prey, Keeper* const keeper) {
  Predator::DeleteParametersForPrey(prey, keeper);
}

void PopPredator::Reset(const TimeClass* const TimeInfo) {
  this->Predator::Reset(TimeInfo);
  //Now the matrices Suitability(prey) are of the correct size.
  //We must adjust the size of consumption accordingly.
  int i, j, area, prey;
  for (area = 0; area < areas.Size(); area++) {
    for (prey = 0; prey < numPreys(); prey++) {
      if (this->DidChange(prey, TimeInfo)) {
        //adjust the size of consumption[area][prey].
        cons.ChangeElement(area, prey, Suitability(prey));
        consumption.ChangeElement(area, prey, Suitability(prey));
        for (i = 0; i < Suitability(prey).Nrow(); i++)
          for (j = consumption[area][prey].minCol(i);
              j < consumption[area][prey].maxCol(i); j++)
            consumption[area][prey][i][j] = 0.0;
      }
    }
  }
  if (TimeInfo->CurrentTime() == 1) {
    for (area = 0; area < areas.Size(); area++) {
      for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
        Prednumber[area][i].N = 0.0;
        Prednumber[area][i].W = 0.0;
        overconsumption[area][i] = 0.0;
        totalconsumption[area][i] = 0.0;
      }
    }
  }
  handle.logMessage("Reset predatation data for predator", this->Name());
}

void PopPredator::resizeObjects() {
  Predator::resizeObjects();
  while (consumption.Nrow())
    consumption.DeleteRow(0);
  while (cons.Nrow())
    cons.DeleteRow(0);
  while (totalconsumption.Nrow())
    totalcons.DeleteRow(0);
  while (totalcons.Nrow())
    totalconsumption.DeleteRow(0);
  while (Prednumber.Nrow())
    Prednumber.DeleteRow(0);

  PopInfo nullpop;
  //Add rows to matrices and initialise
  int numareas = areas.Size();
  int numlengths = LgrpDiv->numLengthGroups();
  cons.AddRows(numareas, numPreys());
  totalcons.AddRows(numareas, numlengths, 0.0);
  overcons.AddRows(numareas, numlengths, 0.0);
  consumption.AddRows(numareas, numPreys());
  totalconsumption.AddRows(numareas, numlengths, 0.0);
  overconsumption.AddRows(numareas, numlengths, 0.0);
  Prednumber.AddRows(numareas, numlengths, nullpop);
}

void PopPredator::Multiply(AgeBandMatrix& stock_alkeys, const DoubleVector& ratio) {
  //written by kgf 31/7 98
  //Note! ratio is supposed to have equal dimensions to PopPredator.
  stock_alkeys.Multiply(ratio, *CI);
}
