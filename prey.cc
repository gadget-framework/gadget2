#include "prey.h"
#include "errorhandler.h"
#include "conversion.h"
#include "readword.h"
#include "readaggregation.h"
#include "keeper.h"
#include "mathfunc.h"
#include "gadget.h"

Prey::Prey(CommentStream& infile, const IntVector& Areas, const char* givenname, Keeper* const keeper)
  : HasName(givenname), LivesOnAreas(Areas), CI(0), LgrpDiv(0) {

  ErrorHandler handle;
  keeper->AddString("prey");
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;

  //Read the prey length group data
  DoubleVector preylengths;
  CharPtrVector preylenindex;
  char aggfilename[MaxStrLength];
  strncpy(aggfilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  readWordAndValue(infile, "preylengths", aggfilename);
  datafile.open(aggfilename);
  checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  i = readLengthAggregation(subdata, preylengths, preylenindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  LgrpDiv = new LengthGroupDivision(preylengths);
  if (LgrpDiv->Error())
    printLengthGroupError(preylengths, "length groups for prey");

  keeper->ClearLast();
  this->InitializeObjects();

  //preylenindex is not required - free up memory
  for (i = 0; i < preylenindex.Size(); i++)
    delete[] preylenindex[i];
}

Prey::Prey(const DoubleVector& lengths, const IntVector& Areas, const char* givenname)
  : HasName(givenname), LivesOnAreas(Areas), CI(0), LgrpDiv(0) {

  LgrpDiv = new LengthGroupDivision(lengths);
  if (LgrpDiv->Error())
    printLengthGroupError(lengths, givenname);
  this->InitializeObjects();
}

void Prey::InitializeObjects() {
  PopInfo nullpop;

  while (Number.Nrow())
    Number.DeleteRow(0);
  while (numberPriortoEating.Nrow())
    numberPriortoEating.DeleteRow(0);
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
  int numlen = LgrpDiv->NoLengthGroups();
  int numarea = areas.Size();

  Number.AddRows(numarea, numlen, nullpop);
  numberPriortoEating.AddRows(numarea, numlen, nullpop);
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

void Prey::SetCI(const LengthGroupDivision* const GivenLDiv) {
  CI = new ConversionIndex(GivenLDiv, LgrpDiv);
}

void Prey::Print(ofstream& outfile) const {
  int i, area;

  outfile << "\nPrey\n\tname " << this->Name();
  outfile << "\n\tPrey lengths";
  for (i = 0; i < LgrpDiv->NoLengthGroups(); i++)
    outfile << sep << LgrpDiv->Minlength(i);
  outfile << sep << LgrpDiv->Maxlength(i-1) << endl;
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tNumber of prey on area " << areas[area];
    for (i = 0; i < LgrpDiv->NoLengthGroups(); i++) {
      outfile.precision(smallprecision);
      outfile.width(printwidth);
      outfile << sep << Number[area][i].N;
    }
    outfile << endl;
  }
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tWeight of prey on area " << areas[area];
    for (i = 0; i < LgrpDiv->NoLengthGroups(); i++) {
      outfile.precision(smallprecision);
      outfile.width(printwidth);
      outfile << sep << Number[area][i].W;
    }
    outfile << endl;
  }
  outfile << "\tConsumption of prey:\n";
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tArea " << areas[area];
    for (i = 0; i < LgrpDiv->NoLengthGroups(); i++) {
      outfile.precision(smallprecision);
      outfile.width(printwidth);
      outfile << sep << consumption[area][i];
    }
    outfile << endl;
  }
  outfile << "\tOverconsumption of prey:\n";
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tArea " << areas[area];
    for (i = 0; i < LgrpDiv->NoLengthGroups(); i++) {
      outfile.precision(smallprecision);
      outfile.width(printwidth);
      outfile << sep << overconsumption[area][i];
    }
    outfile << endl;
  }
  outfile << endl;
}

//Reduce the population of the stock by the consumption.
void Prey::Subtract(AgeBandMatrix& Alkeys, int area) {
  const int inarea = AreaNr[area];
  DoubleVector conS(cons[inarea].Size());
  int len;
  for (len = 0; len < conS.Size(); len++)
    conS[len] = (Number[inarea][len].W > 0 ? cons[inarea][len] / Number[inarea][len].W : 0);
  Alkeys.Subtract(conS, *CI, Number[inarea]);
}

//Calculates the consumption. The function gets the consumption in
//mass units, later they are converted to numbers.
void Prey::AddConsumption(int area, const DoubleIndexVector& predconsumption) {
  int i;
  for (i = predconsumption.Mincol(); i < predconsumption.Maxcol(); i++)
    cons[AreaNr[area]][i] += predconsumption[i];
}

//Check if more is consumed of prey than was available.  If this is
//the case a flag is set. Changed 22 - May 1997  so that only 95% of a prey
//in an area can be eaten in one timestep.  This is to avoid problems
//with survy indices etc.  include maxmortailty.h was added.
void Prey::CheckConsumption(int area, int NrOfSubsteps) {
  double MaxRatioConsumed = pow(MAX_RATIO_CONSUMED, NrOfSubsteps);
  int i, temp = 0;
  int inarea = AreaNr[area];
  double rat, biom;

  for (i = 0; i < LgrpDiv->NoLengthGroups(); i++) {
    rat = 0.0;
    biom = Number[inarea][i].N * Number[inarea][i].W;
    //We must be careful -- it is possible that biomass will equal 0.
    if (!isZero(biom))
      rat = cons[inarea][i] / biom;

    ratio[inarea][i] = rat;
    if (rat > MaxRatioConsumed) {
      temp = 1;
      overcons[inarea][i] = (rat - MaxRatioConsumed) * biom;
      overconsumption[inarea][i] += overcons[inarea][i];
      cons[inarea][i] = biom * MaxRatioConsumed;
    } else
      overcons[inarea][i] = 0.0;

    consumption[inarea][i] += cons[inarea][i];
  }
  tooMuchConsumption[inarea] = temp;
}

const DoubleVector& Prey::Bconsumption(int area) const {
  return consumption[AreaNr[area]];
}

//CheckConsumption has to be called before this one is.
const DoubleVector& Prey::OverConsumption(int area) const {
  return overconsumption[AreaNr[area]];
}

void Prey::Reset() {
  int area, l;
  for (area = 0; area < areas.Size(); area++) {
    tooMuchConsumption[area] = 0;
    total[area] = 0;
    for (l = 0; l < LgrpDiv->NoLengthGroups(); l++) {
      Number[area][l].N = 0.0;
      Number[area][l].W = 0.0;
      numberPriortoEating[area][l].N = 0.0;
      numberPriortoEating[area][l].W = 0.0;
      ratio[area][l] = 0.0;
      consumption[area][l] = 0.0;
      overconsumption[area][l] = 0.0;
    }
  }
}

//Return the original number of prey in an area.
const PopInfoVector& Prey::NumberPriortoEating(int area) const {
  return numberPriortoEating[AreaNr[area]];
}

void Prey::Multiply(AgeBandMatrix& stock_alkeys, const DoubleVector& rat) {
  //written by kgf 31/7 98
  //Note! ratio is supposed to have equal dimensions to Prey.
  stock_alkeys.Multiply(rat, *CI);
}
