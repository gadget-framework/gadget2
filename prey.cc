#include "prey.h"
#include "errorhandler.h"
#include "readword.h"
#include "readaggregation.h"
#include "keeper.h"
#include "mathfunc.h"
#include "gadget.h"

extern ErrorHandler handle;

Prey::Prey(CommentStream& infile, const IntVector& Areas, const char* givenname, Keeper* const keeper)
  : HasName(givenname), LivesOnAreas(Areas), CI(0), LgrpDiv(0) {

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
    handle.Message("Error in prey - failed to create length group");

  keeper->clearLast();
  this->InitialiseObjects();

  //preylenindex is not required - free up memory
  for (i = 0; i < preylenindex.Size(); i++)
    delete[] preylenindex[i];
}

Prey::Prey(const DoubleVector& lengths, const IntVector& Areas, const char* givenname)
  : HasName(givenname), LivesOnAreas(Areas), CI(0), LgrpDiv(0) {

  LgrpDiv = new LengthGroupDivision(lengths);
  if (LgrpDiv->Error())
    handle.Message("Error in prey - failed to create length group");
  this->InitialiseObjects();
}

void Prey::InitialiseObjects() {

  while (Number.Nrow())
    Number.DeleteRow(0);
  while (numberPriorToEating.Nrow())
    numberPriorToEating.DeleteRow(0);
  while (biomass.Nrow())
    biomass.DeleteRow(0);
  while (cons.Nrow())
    cons.DeleteRow(0);
  while (consumption.Nrow())
    consumption.DeleteRow(0);
  while (tooMuchConsumption.Size())
    tooMuchConsumption.Delete(0);
  while (total.Size())
    total.Delete(0);
  while (ratio.Nrow())
    ratio.DeleteRow(0);
  while (overcons.Nrow())
    overcons.DeleteRow(0);
  while (overconsumption.Nrow())
    overconsumption.DeleteRow(0);

  //Now we can resize the objects.
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

  outfile << "\nPrey\n\tName " << this->Name() << "\n\t";
  LgrpDiv->Print(outfile);
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tNumber of prey on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
      outfile.precision(smallprecision);
      outfile.width(smallwidth);
      outfile << sep << Number[area][i].N;
    }
    outfile << endl;
  }
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tWeight of prey on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
      outfile.precision(smallprecision);
      outfile.width(smallwidth);
      outfile << sep << Number[area][i].W;
    }
    outfile << endl;
  }
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tConsumption of prey on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
      outfile.precision(smallprecision);
      outfile.width(smallwidth);
      outfile << sep << consumption[area][i];
    }
    outfile << endl;
  }
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tOverconsumption of prey on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
      outfile.precision(smallprecision);
      outfile.width(smallwidth);
      outfile << sep << overconsumption[area][i];
    }
    outfile << endl;
  }
}

//Reduce the population of the stock by the consumption.
void Prey::Subtract(AgeBandMatrix& Alkeys, int area) {
  int inarea = this->areaNum(area);
  DoubleVector conS(cons[inarea].Size());
  int len;
  for (len = 0; len < conS.Size(); len++)
    conS[len] = (Number[inarea][len].W > verysmall ? cons[inarea][len] / Number[inarea][len].W : 0.0);
  Alkeys.Subtract(conS, *CI, Number[inarea]);
}

//Calculates the consumption. The function gets the consumption in
//mass units, later they are converted to numbers.
void Prey::addConsumption(int area, const DoubleIndexVector& predconsumption) {
  int i, inarea = this->areaNum(area);
  for (i = predconsumption.minCol(); i < predconsumption.maxCol(); i++)
    cons[inarea][i] += predconsumption[i];
}

//check if more is consumed of prey than was available.  If this is
//the case a flag is set. Changed 22 - May 1997  so that only 95% of a prey
//in an area can be eaten in one timestep.  This is to avoid problems
//with survy indices etc.  include maxmortailty.h was added.
void Prey::checkConsumption(int area, int numsubsteps) {
  double maxRatio = pow(MaxRatioConsumed, numsubsteps);
  int i, temp = 0;
  int inarea = this->areaNum(area);
  double rat, biom;

  for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
    rat = 0.0;
    biom = Number[inarea][i].N * Number[inarea][i].W;
    //We must be careful -- it is possible that biomass will equal 0.
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
    total[area] = 0;
    for (l = 0; l < LgrpDiv->numLengthGroups(); l++) {
      Number[area][l].N = 0.0;
      Number[area][l].W = 0.0;
      numberPriorToEating[area][l].N = 0.0;
      numberPriorToEating[area][l].W = 0.0;
      ratio[area][l] = 0.0;
      consumption[area][l] = 0.0;
      overconsumption[area][l] = 0.0;
    }
  }
}
