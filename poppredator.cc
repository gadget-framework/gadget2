#include "poppredator.h"
#include "mathfunc.h"
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

PopPredator::PopPredator(const char* givenname, const IntVector& Areas)
  : Predator(givenname, Areas), LgrpDiv(0), CI(0) {
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
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << prednumber[area][i].N << sep;
    outfile << "\n\tWeight of predators on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << prednumber[area][i].W << sep;
    outfile << "\n\tTotal amount eaten on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << totalconsumption[area][i] << sep;
    outfile << "\n\tOverconsumption on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << overconsumption[area][i] << sep;
    outfile << endl;
  }
}

const BandMatrix& PopPredator::getConsumption(int area, const char* preyname) const {
  int prey;
  for (prey = 0; prey < this->numPreys(); prey++)
    if (strcasecmp(getPreyName(prey), preyname) == 0)
      return consumption[this->areaNum(area)][prey];

  handle.logMessage(LOGFAIL, "Error in poppredator - failed to match prey", preyname);
  exit(EXIT_FAILURE);
}

const double PopPredator::getConsumptionBiomass(int prey, int area) const{
  int age, len;
  double kilos = 0.0;
  //Note area is already the internal area ...
  for (age = consumption[area][prey].minAge(); age <= consumption[area][prey].maxAge(); age++)
    for (len = consumption[area][prey].minLength(age); len < consumption[area][prey].maxLength(age); len++)
      kilos += (consumption[area][prey])[age][len];

  return kilos;
}

void PopPredator::Reset(const TimeClass* const TimeInfo) {
  Predator::Reset(TimeInfo);
  //Now the matrices Suitability(prey) are of the correct size.
  //We must adjust the size of consumption accordingly.
  int i, j, area, prey;
  for (area = 0; area < areas.Size(); area++) {
    for (prey = 0; prey < this->numPreys(); prey++) {
      if (this->didChange(prey, TimeInfo)) {
        cons.changeElement(area, prey, Suitability(prey));
        consumption.changeElement(area, prey, Suitability(prey));
      }
    }
  }

  if (TimeInfo->getSubStep() == 1) {
    for (area = 0; area < areas.Size(); area++) {
      for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
        prednumber[area][i].N = 0.0;
        prednumber[area][i].W = 0.0;
        overconsumption[area][i] = 0.0;
        totalconsumption[area][i] = 0.0;
        for (prey = 0; prey < this->numPreys(); prey++)
          for (j = consumption[area][prey].minCol(i); j < consumption[area][prey].maxCol(i); j++)
            consumption[area][prey][i][j] = 0.0;
      }
    }
  }

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset predatation data for predator", this->getName());
}

void PopPredator::setPrey(PreyPtrVector& preyvec, Keeper* const keeper) {
  Predator::setPrey(preyvec, keeper);

  if (LgrpDiv == 0) {
    //need to construct length group based on the min/max lengths of the preys
    int i;
    double minl, maxl;
    minl = 9999.0;
    maxl = 0.0;
    for (i = 0; i < this->numPreys(); i++) {
      minl = min(Preys(i)->returnLengthGroupDiv()->minLength(), minl);
      maxl = max(Preys(i)->returnLengthGroupDiv()->maxLength(), maxl);
    }
    LgrpDiv = new LengthGroupDivision(minl, maxl, maxl - minl);
    if (LgrpDiv->Error())
      handle.logMessage(LOGFAIL, "Error in poppredator - failed to create length group");
    CI = new ConversionIndex(LgrpDiv, LgrpDiv);
  }

  PopInfo nullpop;
  //add rows to matrices and initialise
  cons.AddRows(areas.Size(), this->numPreys());
  totalcons.AddRows(areas.Size(), LgrpDiv->numLengthGroups(), 0.0);
  overcons.AddRows(areas.Size(), LgrpDiv->numLengthGroups(), 0.0);
  consumption.AddRows(areas.Size(), this->numPreys());
  totalconsumption.AddRows(areas.Size(), LgrpDiv->numLengthGroups(), 0.0);
  overconsumption.AddRows(areas.Size(), LgrpDiv->numLengthGroups(), 0.0);
  prednumber.AddRows(areas.Size(), LgrpDiv->numLengthGroups(), nullpop);
}

double PopPredator::getTotalOverConsumption(int area) const {
  int i, inarea = this->areaNum(area);
  double total;
  total = 0.0;
  for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
    total += overconsumption[inarea][i];
  return total;
}
