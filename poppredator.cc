#include "poppredator.h"
#include "mathfunc.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

PopPredator::PopPredator(const char* givenname, const IntVector& Areas,
  const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv)
  : Predator(givenname, Areas) {

  LgrpDiv = new LengthGroupDivision(*GivenLgrpDiv);
  CI = new ConversionIndex(OtherLgrpDiv, LgrpDiv);
}

PopPredator::PopPredator(const char* givenname, const IntVector& Areas)
  : Predator(givenname, Areas), LgrpDiv(0), CI(0) {
}

PopPredator::~PopPredator() {
  int i, j;
  delete LgrpDiv;
  delete CI;
  for (i = 0; i < consumption.Nrow(); i++) {
    for (j = 0; j < consumption[i].Size(); j++) {
      delete consumption[i][j];
      delete cons[i][j];
    }
  }
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

const DoubleMatrix& PopPredator::getConsumption(int area, const char* preyname) const {
  int prey;
  for (prey = 0; prey < this->numPreys(); prey++)
    if (strcasecmp(this->getPreyName(prey), preyname) == 0)
      return (*consumption[this->areaNum(area)][prey]);

  handle.logMessage(LOGFAIL, "Error in poppredator - failed to match prey", preyname);
  exit(EXIT_FAILURE);
}

const double PopPredator::getConsumptionBiomass(int prey, int area) const{
  int i, j;
  int inarea = this->areaNum(area);
  double kilos = 0.0;
  for (i = 0; i < (*consumption[inarea][prey]).Nrow(); i++)
    for (j = 0; j < (*consumption[inarea][prey]).Ncol(i); j++)
      kilos += (*consumption[inarea][prey])[i][j];

  return kilos;
}

void PopPredator::Reset(const TimeClass* const TimeInfo) {
  Predator::Reset(TimeInfo);

  int i, j, area, prey;
  if (TimeInfo->getSubStep() == 1) {
    for (area = 0; area < areas.Size(); area++) {
      for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
        prednumber[area][i].N = 0.0;
        prednumber[area][i].W = 0.0;
        overconsumption[area][i] = 0.0;
        totalconsumption[area][i] = 0.0;
        for (prey = 0; prey < this->numPreys(); prey++)
          for (j = 0; j < (*consumption[area][prey]).Ncol(i); j++)
            (*consumption[area][prey])[i][j] = 0.0;
      }
    }
  }

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset predatation data for predator", this->getName());
}

void PopPredator::setPrey(PreyPtrVector& preyvec, Keeper* const keeper) {
  Predator::setPrey(preyvec, keeper);

  int i, j;
  if (LgrpDiv == 0) {
    //need to construct length group based on the min/max lengths of the preys
    double minl, maxl;
    minl = 9999.0;
    maxl = 0.0;
    for (i = 0; i < this->numPreys(); i++) {
      minl = min(this->getPrey(i)->getLengthGroupDiv()->minLength(), minl);
      maxl = max(this->getPrey(i)->getLengthGroupDiv()->maxLength(), maxl);
    }
    LgrpDiv = new LengthGroupDivision(minl, maxl, maxl - minl);
    if (LgrpDiv->Error())
      handle.logMessage(LOGFAIL, "Error in poppredator - failed to create length group");
    CI = new ConversionIndex(LgrpDiv, LgrpDiv);
  }

  //now we need to initialise things
  this->Initialise();
  PopInfo nullpop;
  for (i = 0; i < areas.Size(); i++) {
    cons.resize();
    consumption.resize();
    for (j = 0; j < this->numPreys(); j++) {
      cons[i].resize(new DoubleMatrix(LgrpDiv->numLengthGroups(), this->getPrey(j)->getLengthGroupDiv()->numLengthGroups(), 0.0));
      consumption[i].resize(new DoubleMatrix(LgrpDiv->numLengthGroups(),  this->getPrey(j)->getLengthGroupDiv()->numLengthGroups(), 0.0));
    }
  }

  totalcons.AddRows(areas.Size(), LgrpDiv->numLengthGroups(), 0.0);
  overcons.AddRows(areas.Size(), LgrpDiv->numLengthGroups(), 0.0);
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

const PopInfoVector& PopPredator::getNumberPriorToEating(int area, const char* preyname) const {
  int prey;
  for (prey = 0; prey < this->numPreys(); prey++)
    if (strcasecmp(this->getPreyName(prey), preyname) == 0)
      return this->getPrey(prey)->getNumberPriorToEating(area);

  handle.logMessage(LOGFAIL, "Error in poppredator - failed to match prey", preyname);
  exit(EXIT_FAILURE);
}
