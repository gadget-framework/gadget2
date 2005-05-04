#include "prey.h"
#include "errorhandler.h"
#include "readword.h"
#include "readaggregation.h"
#include "keeper.h"
#include "mathfunc.h"
#include "gadget.h"

extern ErrorHandler handle;

Prey::Prey(CommentStream& infile, const IntVector& Areas, const char* givenname,
  Keeper* const keeper) : HasName(givenname), LivesOnAreas(Areas), CI(0) {

  keeper->addString("prey");
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
  datafile.open(aggfilename, ios::in);
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
  readWordAndVariable(infile, "energycontent", energy);

  this->InitialiseObjects();
  keeper->clearLast();

  //preylenindex is not required - free up memory
  for (i = 0; i < preylenindex.Size(); i++)
    delete[] preylenindex[i];
}

Prey::Prey(const DoubleVector& lengths, const IntVector& Areas,
  double Energy, const char* givenname)
  : HasName(givenname), LivesOnAreas(Areas), energy(Energy) {

  LgrpDiv = new LengthGroupDivision(lengths);
  if (LgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in prey - failed to create length group");
  this->InitialiseObjects();
  CI = new ConversionIndex(LgrpDiv, LgrpDiv);
}

void Prey::InitialiseObjects() {
  int numlen = LgrpDiv->numLengthGroups();
  int numarea = areas.Size();
  PopInfo nullpop;

  Number.AddRows(numarea, numlen, nullpop);
  numberPriorToEating.AddRows(numarea, numlen, nullpop);
  biomass.AddRows(numarea, numlen, 0.0);
  cons.AddRows(numarea, numlen, 0.0);
  consumption.AddRows(numarea, numlen, 0.0);
  tooMuchConsumption.resize(numarea, 0);
  total.resize(numarea, 0.0);
  ratio.AddRows(numarea, numlen, 0.0);
  overcons.AddRows(numarea, numlen, 0.0);
  overconsumption.AddRows(numarea, numlen, 0.0);
}

Prey::~Prey() {
  delete CI;
  delete LgrpDiv;
}

void Prey::setCI(const LengthGroupDivision* const GivenLDiv) {
  CI = new ConversionIndex(GivenLDiv, LgrpDiv);
}

void Prey::Print(ofstream& outfile) const {
  int i, area;

  outfile << "\nPrey\n\tName " << this->getName() << "\n\tEnergy content " << energy << "\n\t";
  LgrpDiv->Print(outfile);
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tNumber of prey on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << Number[area][i].N << sep;
    outfile << "\n\tWeight of prey on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << Number[area][i].W << sep;
    outfile << "\n\tConsumption of prey on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << consumption[area][i] << sep;
    outfile << "\n\tOverconsumption of prey on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << overconsumption[area][i] << sep;
    outfile << endl;
  }
}

//Reduce the population of the stock by the consumption.
void Prey::Subtract(AgeBandMatrix& Alkeys, int area) {
  int inarea = this->areaNum(area);
  DoubleVector subConsume(cons[inarea].Size(), 0.0);
  int len;
  for (len = 0; len < subConsume.Size(); len++)
    if (!(isZero(Number[inarea][len].W)))
      subConsume[len] = cons[inarea][len] / Number[inarea][len].W ;

  Alkeys.Subtract(subConsume, *CI, Number[inarea]);
}

//adds the consumption by biomass
void Prey::addBiomassConsumption(int area, const DoubleIndexVector& predconsumption) {
  int i, inarea = this->areaNum(area);
  if ((predconsumption.minCol() < 0) || (predconsumption.maxCol() > cons[inarea].Size()))
    handle.logMessage(LOGFAIL, "Error in prey - invalid length group when adding consumption");
  for (i = predconsumption.minCol(); i < predconsumption.maxCol(); i++)
    cons[inarea][i] += predconsumption[i];
}

//adds the consumption by numbers
void Prey::addNumbersConsumption(int area, const DoubleIndexVector& predconsumption) {
  int i, inarea = this->areaNum(area);
  if ((predconsumption.minCol() < 0) || (predconsumption.maxCol() > cons[inarea].Size()))
    handle.logMessage(LOGFAIL, "Error in prey - invalid length group when adding consumption");
  for (i = predconsumption.minCol(); i < predconsumption.maxCol(); i++)
    cons[inarea][i] += (predconsumption[i] * Number[inarea][i].W);
}

//check if more is consumed of prey than was available.  If this is
//the case a flag is set. Changed 22 - May 1997  so that only 95% of a prey
//in an area can be eaten in one timestep.  This is to avoid problems
//with survey indices etc.
void Prey::checkConsumption(int area, int numsubsteps) {
  double maxRatio = pow(MaxRatioConsumed, numsubsteps);
  int i, temp = 0;
  int inarea = this->areaNum(area);
  double rat, biom;

  for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
    rat = 1.0;
    biom = Number[inarea][i].N * Number[inarea][i].W;
    if (biom > verysmall)
      rat = cons[inarea][i] / biom;

    ratio[inarea][i] = rat;
    if (rat > maxRatio) {
      temp = 1;
      overcons[inarea][i] = (rat - maxRatio) * biom;
      overconsumption[inarea][i] += overcons[inarea][i];
      cons[inarea][i] = biom * maxRatio;
    } else
      overcons[inarea][i] = 0.0;

    consumption[inarea][i] += cons[inarea][i];
  }
  tooMuchConsumption[inarea] = temp;
}

void Prey::Reset() {
  int area, l;
  for (area = 0; area < areas.Size(); area++) {
    tooMuchConsumption[area] = 0;
    total[area] = 0.0;
    for (l = 0; l < LgrpDiv->numLengthGroups(); l++) {
      Number[area][l].N = 0.0;
      Number[area][l].W = 0.0;
      numberPriorToEating[area][l].N = 0.0;
      numberPriorToEating[area][l].W = 0.0;
      ratio[area][l] = 0.0;
      biomass[area][l] = 0.0;
      consumption[area][l] = 0.0;
      overconsumption[area][l] = 0.0;
    }
  }
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset consumption data for prey", this->getName());
}

int Prey::isPreyArea(int area) {
  if ((handle.getLogLevel() >= LOGWARN) && (total[this->areaNum(area)] < 0))
    handle.logMessage(LOGWARN, "Warning in prey - negative amount consumed");
  if (isZero(total[this->areaNum(area)]))
    return 0;
  return 1;
}
