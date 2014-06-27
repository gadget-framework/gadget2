#include "poppredator.h"
#include "mathfunc.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

PopPredator::PopPredator(const char* givenname, const IntVector& Areas,
  const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv)
  : Predator(givenname, Areas) {

  LgrpDiv = new LengthGroupDivision(*GivenLgrpDiv);
  if (LgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in poppredator - failed to create length group");
  CI = new ConversionIndex(OtherLgrpDiv, LgrpDiv);
  if (CI->Error())
    handle.logMessage(LOGFAIL, "Error in poppredator - error when checking length structure");
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
      delete usesuit[i][j];
    }
  }
  for (i = 0; i < predratio.Size(); i++)
    delete predratio[i];
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

double PopPredator::getConsumptionBiomass(int prey, int area) const {
  int inarea = this->areaNum(area);
  if (inarea == -1)
    return 0.0;

  int i, j;
  double kilos = 0.0;
  for (i = 0; i < (*consumption[inarea][prey]).Nrow(); i++)
    for (j = 0; j < (*consumption[inarea][prey]).Ncol(i); j++)
      kilos += (*consumption[inarea][prey])[i][j];

  return kilos;
}

void PopPredator::Reset(const TimeClass* const TimeInfo) {
  Predator::Reset(TimeInfo);

  int i, area;
  if (TimeInfo->getSubStep() == 1) {
    for (area = 0; area < areas.Size(); area++) {
      totalconsumption[area].setToZero();
      (*predratio[area]).setToZero();
      for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
        prednumber[area][i].setToZero();
      for (i = 0; i < this->numPreys(); i++)
        (*consumption[area][i]).setToZero();

      //JMB only reset these if they are needed ...
      for (i = 0; i < this->numPreys(); i++)
        if ((hasoverconsumption[area]) || (this->didChange(i, TimeInfo)))
          (*usesuit[area][i]) = this->getSuitability(i);

      if (hasoverconsumption[area]) {
        hasoverconsumption[area] = 0;
        overconsumption[area].setToZero();
      }
    }
  }

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset predatation data for predator", this->getName());
}

void PopPredator::setPrey(PreyPtrVector& preyvec, Keeper* const keeper) {
  Predator::setPrey(preyvec, keeper);

  int i, j, numlen, numarea, preylen;
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
    if (CI->Error())
      handle.logMessage(LOGFAIL, "Error in poppredator - error when checking length structure");
  }

  //now we need to initialise things
  Predator::Initialise();
  PopInfo nullpop;
  numlen = LgrpDiv->numLengthGroups();
  numarea = areas.Size();
  for (i = 0; i < numarea; i++) {
    cons.resize();
    consumption.resize();
    usesuit.resize();
    predratio.resize(new DoubleMatrix(this->numPreys(), numlen, 0.0));
    for (j = 0; j < this->numPreys(); j++) {
      preylen = this->getPrey(j)->getLengthGroupDiv()->numLengthGroups();
      cons[i].resize(new DoubleMatrix(numlen, preylen, 0.0));
      consumption[i].resize(new DoubleMatrix(numlen, preylen, 0.0));
      usesuit[i].resize(new DoubleMatrix(numlen, preylen, 0.0));
    }
  }

  hasoverconsumption.resize(numarea, 0);
  totalcons.AddRows(numarea, numlen, 0.0);
  overcons.AddRows(numarea, numlen, 0.0);
  totalconsumption.AddRows(numarea, numlen, 0.0);
  overconsumption.AddRows(numarea, numlen, 0.0);
  prednumber.AddRows(numarea, numlen, nullpop);
}

double PopPredator::getTotalOverConsumption(int area) const {
  int inarea = this->areaNum(area);
  if (inarea == -1)
    return 0.0;

  int i;
  double total = 0.0;
  for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
    total += overconsumption[inarea][i];
  return total;
}

const PopInfoVector& PopPredator::getConsumptionPopInfo(int area, const char* preyname) const {
  int prey;
  for (prey = 0; prey < this->numPreys(); prey++)
    if (strcasecmp(this->getPreyName(prey), preyname) == 0)
      return this->getPrey(prey)->getConsumptionPopInfo(area);

  handle.logMessage(LOGFAIL, "Error in poppredator - failed to match prey", preyname);
  exit(EXIT_FAILURE);
}
