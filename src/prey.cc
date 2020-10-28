#include "prey.h"
#include "errorhandler.h"
#include "readword.h"
#include "readaggregation.h"
#include "keeper.h"
#include "mathfunc.h"
#include "gadget.h"
#include "global.h"

Prey::Prey(CommentStream& infile, const IntVector& Areas,
  const char* givenname, const TimeClass* const TimeInfo, Keeper* const keeper)
  : HasName(givenname), LivesOnAreas(Areas), CI(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;

  //read the prey length group data
  DoubleVector preylengths;
  CharPtrVector preylenindex;
  char aggfilename[MaxStrLength];
  strncpy(aggfilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  readWordAndValue(infile, "preylengths", aggfilename);
  datafile.open(aggfilename, ios::binary);
  handle.checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  i = readLengthAggregation(subdata, preylengths, preylenindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  LgrpDiv = new LengthGroupDivision(preylengths);
  if (LgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in prey - failed to create length group");

  //read the energy content of this prey
  infile >> ws;
  char c = infile.peek();
  if ((c == 'e') || (c == 'E'))
    readWordAndModelVariable(infile, "energycontent", energy, TimeInfo, keeper);
  else
    energy.setValue(1.0);

  //read from file - initialise things
  int numlen = LgrpDiv->numLengthGroups();
  int numarea = areas.Size();
  PopInfo nullpop;

  preynumber.AddRows(numarea, numlen, nullpop);
  biomass.AddRows(numarea, numlen, 0.0);
  cons.AddRows(numarea, numlen, 0.0);
  consumption.AddRows(numarea, numlen, 0.0);
  isoverconsumption.resize(numarea, 0);
  total.resize(numarea, 0.0);
  ratio.AddRows(numarea, numlen, 0.0);
  useratio.AddRows(numarea, numlen, 0.0);
  consratio.AddRows(numarea, numlen, 0.0);
  overconsumption.AddRows(numarea, numlen, 0.0);

  //preylenindex is not required - free up memory
  for (i = 0; i < preylenindex.Size(); i++)
    delete[] preylenindex[i];
}

Prey::Prey(CommentStream& infile, const char* givenname,
  const IntVector& Areas, const TimeClass* const TimeInfo, Keeper* const keeper)
  : HasName(givenname), LivesOnAreas(Areas) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  //read the length information
  DoubleVector lengths(2, 0.0);
  infile >> text >> ws;
  if (strcasecmp(text, "lengths") != 0)
    handle.logFileUnexpected(LOGFAIL, "lengths", text);
  infile >> lengths[0] >> lengths[1] >> ws;

  LgrpDiv = new LengthGroupDivision(lengths);
  if (LgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in prey - failed to create length group");
  CI = new ConversionIndex(LgrpDiv, LgrpDiv);
  if (CI->Error())
    handle.logMessage(LOGFAIL, "Error in prey - error when checking length structure");

  //read the energy content of this prey
  infile >> ws;
  char c = infile.peek();
  if ((c == 'e') || (c == 'E'))
    readWordAndModelVariable(infile, "energycontent", energy, TimeInfo, keeper);
  else
    energy.setValue(1.0);

  int numlen = LgrpDiv->numLengthGroups();
  int numarea = areas.Size();
  PopInfo nullpop;

  preynumber.AddRows(numarea, numlen, nullpop);
  biomass.AddRows(numarea, numlen, 0.0);
  cons.AddRows(numarea, numlen, 0.0);
  consumption.AddRows(numarea, numlen, 0.0);
  isoverconsumption.resize(numarea, 0);
  total.resize(numarea, 0.0);
  ratio.AddRows(numarea, numlen, 0.0);
  useratio.AddRows(numarea, numlen, 0.0);
  consratio.AddRows(numarea, numlen, 0.0);
  overconsumption.AddRows(numarea, numlen, 0.0);
}

Prey::~Prey() {
  delete CI;
  delete LgrpDiv;
}

void Prey::setCI(const LengthGroupDivision* const GivenLDiv) {
  if (!checkLengthGroupStructure(GivenLDiv, LgrpDiv))
    handle.logMessage(LOGFAIL, "Error in prey - invalid length group structure for consumption of", this->getName());
  if (GivenLDiv->minLength() < LgrpDiv->minLength())
    handle.logMessage(LOGFAIL, "Error in prey - invalid minimum length group for consumption of", this->getName());
  if (!isSmall(LgrpDiv->minLength() - GivenLDiv->minLength()))
    handle.logMessage(LOGWARN, "Warning in prey - minimum lengths don't match for consumption of", this->getName());
  if (GivenLDiv->maxLength() > LgrpDiv->maxLength())
    handle.logMessage(LOGFAIL, "Error in prey - invalid maximum length group for consumption of", this->getName());
  if (!isSmall(LgrpDiv->maxLength() - GivenLDiv->maxLength()))
    handle.logMessage(LOGWARN, "Warning in prey - maximum lengths don't match for consumption of", this->getName());

  CI = new ConversionIndex(GivenLDiv, LgrpDiv);
  if (CI->Error())
    handle.logMessage(LOGFAIL, "Error in prey - error when checking length structure for", this->getName());
}

void Prey::Print(ofstream& outfile) const {
  int i, area;
  outfile << "\nPrey\n\tName " << this->getName() << "\n\tEnergy content " << energy << "\n\t";
  LgrpDiv->Print(outfile);
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tNumber of prey on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << preynumber[area][i].N << sep;
    outfile << "\n\tWeight of prey on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << preynumber[area][i].W << sep;
    outfile << "\n\tConsumption of prey on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << consumption[area][i] << sep;
    outfile << "\n\tOverconsumption of prey on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << overconsumption[area][i] << sep;
    outfile << endl;
  }
}

//reduce the population of the stock by the consumption
void Prey::Subtract(AgeBandMatrix& Alkeys, int area) {
  Alkeys.Subtract(consratio[this->areaNum(area)], *CI);
}

//adds the consumption by biomass
void Prey::addBiomassConsumption(int area, const DoubleVector& predcons) {
  int i, inarea = this->areaNum(area);
  if (predcons.Size() != cons[inarea].Size())
    handle.logMessage(LOGFAIL, "Error in consumption - cannot add different size vectors");
  for (i = 0; i < predcons.Size(); i++)
    cons[inarea][i] += predcons[i];
}

//adds the consumption by numbers
void Prey::addNumbersConsumption(int area, const DoubleVector& predcons) {
  int i, inarea = this->areaNum(area);
  if (predcons.Size() != cons[inarea].Size())
    handle.logMessage(LOGFAIL, "Error in consumption - cannot add different size vectors");
  for (i = 0; i < predcons.Size(); i++)
    cons[inarea][i] += (predcons[i] * preynumber[inarea][i].W);
}

//check if more is consumed of prey than was available.  If this is
//the case a flag is set. Changed 22 - May 1997  so that only 95% of a prey
//in an area can be eaten in one timestep.  This is to avoid problems
//with survey indices etc.
void Prey::checkConsumption(int area, const TimeClass* const TimeInfo) {
  int i, over = 0;
  int inarea = this->areaNum(area);

  double timeratio = 1.0;
  double maxRatio = TimeInfo->getMaxRatioConsumed();
  if (TimeInfo->numSubSteps() != 1) {
    timeratio = 1.0 / TimeInfo->getSubStep();
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      useratio[inarea][i] *= (1.0 - timeratio);
  }

  for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
    if (isZero(biomass[inarea][i])) {
      //no prey biomass available to consume
      ratio[inarea][i] = 0.0;
      consratio[inarea][i] = 0.0;
      if (!(isZero(cons[inarea][i]))) {
        //consumption required but no prey exists
        over = 1;
        overconsumption[inarea][i] += cons[inarea][i];
      }

    } else {
      //prey available to consume so only need to check overconsumption
      ratio[inarea][i] = cons[inarea][i] / biomass[inarea][i];
      if (ratio[inarea][i] > maxRatio) {
        over = 1;
        overconsumption[inarea][i] += (ratio[inarea][i] - maxRatio) * biomass[inarea][i];
        consratio[inarea][i] = 1.0 - maxRatio;
        useratio[inarea][i] += (timeratio * maxRatio);
        cons[inarea][i] = biomass[inarea][i] * maxRatio;
      } else {
        consratio[inarea][i] = 1.0 - ratio[inarea][i];
        useratio[inarea][i] += (timeratio * ratio[inarea][i]);
      }
      //finally add the consumption
      consumption[inarea][i] += cons[inarea][i];
    }
  }

  //JMB changed to deal with substeps a little better
  if (over)
    isoverconsumption[inarea] = over;
}

void Prey::Reset(const TimeClass* const TimeInfo) {
  consumption.setToZero();
  overconsumption.setToZero();
  useratio.setToZero();
  isoverconsumption.setToZero();

  energy.Update(TimeInfo);
  if (isZero(energy))
    handle.logMessage(LOGWARN, "Warning in prey - energy content should be non-zero");

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset consumption data for prey", this->getName());
}

int Prey::isPreyArea(int area) {
  if (this->isInArea(area) == 0)
    return 0;
  if (total[this->areaNum(area)] < 0.0)
    handle.logMessage(LOGWARN, "Warning in prey - negative amount consumed for", this->getName());
  if (isZero(total[this->areaNum(area)]))
    return 0;
  return 1;
}

int Prey::isOverConsumption(int area) {
  if (this->isInArea(area) == 0)
    return 0;
  return isoverconsumption[this->areaNum(area)];
}

double Prey::getTotalOverConsumption(int area) const {
  int inarea = this->areaNum(area);
  if (inarea == -1)
    return 0.0;

  int i;
  double total = 0.0;
  for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
    total += overconsumption[inarea][i];
  return total;
}
