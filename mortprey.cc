#include "mortprey.h"
#include "intvector.h"
#include "gadget.h"

MortPrey::MortPrey(CommentStream& infile, const IntVector& Areas,
  const char* givenname, int minage, int maxage,
  Keeper* const keeper, const LengthGroupDivision* const stock_lgrp)
  : Prey(infile, Areas, givenname, keeper) {

  type = MORTPREYTYPE;
  prey_lgrp = new LengthGroupDivision(*LgrpDiv);
  delete LgrpDiv; //wrong dimensions set in Prey's constructor
  LgrpDiv = new LengthGroupDivision(*stock_lgrp);

  this->InitialiseObjects();

  IntVector size(maxage - minage + 1, LgrpDiv->NoLengthGroups());
  IntVector minlength(maxage - minage + 1, 0);
  Alkeys.resize(areas.Size(), minage, minlength, size);
  mean_n.resize(areas.Size(), minage, minlength, size);
}

MortPrey::~MortPrey() {
  int i;
  delete prey_lgrp;
  for (i = 0; i < cannprednames.Size(); i++)
    delete[] cannprednames[i];
}

void MortPrey::InitialiseObjects() {

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
  while (haveCalculatedMeanN.Size())
    haveCalculatedMeanN.Delete(0);
  while (z.Nrow())
    z.DeleteRow(0);
  while (mort_fact.Nrow())
    mort_fact.DeleteRow(0);
  while (prop_surv.Nrow())
    prop_surv.DeleteRow(0);
  while (cannibalism.Nrow())
    cannibalism.DeleteRow(0);

  //Now we can resize the objects.
  int numlength = LgrpDiv->NoLengthGroups();
  int numarea = areas.Size();
  PopInfo nullpop;

  Number.AddRows(numarea, numlength, nullpop);
  numberPriortoEating.AddRows(numarea, numlength, nullpop);
  biomass.AddRows(numarea, numlength, 0.0);
  cons.AddRows(numarea, numlength, 0.0);
  consumption.AddRows(numarea, numlength, 0.0);
  tooMuchConsumption.resize(numarea, 0);
  total.resize(numarea, 0.0);
  ratio.AddRows(numarea, numlength, 0.0);
  overcons.AddRows(numarea, numlength, 0.0);
  overconsumption.AddRows(numarea, numlength, 0.0);
  haveCalculatedMeanN.resize(numarea, 0);
  z.AddRows(numarea, numlength, 0.0);
  mort_fact.AddRows(numarea, numlength, 0.0);
  prop_surv.AddRows(numarea, numlength, 0.0);
  cannibalism.AddRows(numarea, numlength, 0.0);
  cann_is_true = 0;
}

void MortPrey::Sum(const AgeBandMatrix& stock, int area, int CurrentSubstep) {
  //written by kgf 22/6 98
  int inarea = AreaNr[area];
  int i, j;

  tooMuchConsumption[inarea] = 0;
  for (i = 0; i < cons.Ncol(inarea); i++)
    cons[inarea][i] = 0.0;
  for (i = 0; i < Number[inarea].Size(); i++)
    Number[inarea][i].N = 0.0;
  for (i = 0; i < cannibalism[inarea].Size(); i++)
    cannibalism[inarea][i] = 0.0;

  mean_n[inarea].setToZero();
  Alkeys[inarea].setToZero();
  mean_n[inarea].Add(stock, *CI);
  Alkeys[inarea].Add(stock, *CI);
  Alkeys[inarea].Colsum(Number[inarea]);
  haveCalculatedMeanN[inarea] = 0;

  PopInfo sum;
  for (i = 0; i < Number.Ncol(inarea); i++) {
    sum += Number[inarea][i];
    biomass[inarea][i] = Number[inarea][i].N * Number[inarea][i].W;
  }

  total[inarea] = sum.N * sum.W;
  for (i = 0; i < Number[inarea].Size(); i++)
    numberPriortoEating[inarea][i] = Number[inarea][i]; //should be inside if

  if (CurrentSubstep == 1) {
    for (j = 0; j < consumption.Ncol(inarea); j++) {
      consumption[inarea][j] = 0.0;
      overconsumption[inarea][j] = 0.0;
    }
  }
}

const AgeBandMatrix& MortPrey::AlkeysPriorToEating(int area) const {
  return Alkeys[AreaNr[area]];
}

const AgeBandMatrix& MortPrey::getMeanN(int area) const {
  return mean_n[AreaNr[area]];
}

void MortPrey::Print(ofstream& outfile) const {
  //must be modified! kgf 7/7 98
  outfile << "MortPrey\n";
  int area;
  for (area = 0; area < areas.Size(); area++) {
    outfile << "Alkeys on area " << areas[area] << endl;
    Alkeys[area].PrintNumbers(outfile);
  }
  Prey::Print(outfile);
}

void MortPrey::Reset() {
  this->Prey::Reset();
  int area, age, l;
  for (area = 0; area < areas.Size(); area++)   {
    haveCalculatedMeanN[area] = 0;
    for (age = Alkeys[area].Minage(); age <= Alkeys[area].Maxage(); age++) {
      for (l = Alkeys[area].Minlength(age); l < Alkeys[area].Maxlength(age); l++) {
        Alkeys[area][age][l].N = 0.0;
        Alkeys[area][age][l].W = 0.0;
        mean_n[area][age][l].N = 0.0;
        mean_n[area][age][l].W = 0.0;
      }
    }
    for (l = 0; l < z[area].Size(); l++) {
      z[area][l] = 0.0;
      mort_fact[area][l] = 0.0;
      cannibalism[area][l] = 0.0;
    }
  }
}

void MortPrey::calcMeanN(int area) {
  //written by kgf 24/6 98
  assert(haveCalculatedMeanN[area] == 0);
  haveCalculatedMeanN[area] = 1;
  int inarea = AreaNr[area];
  int l;
  for (l = 0; l < LgrpDiv->NoLengthGroups(); l++) {
    prop_surv[inarea][l] = exp(- z[inarea][l]);
    if (isZero(z[inarea][l]))
      mort_fact[inarea][l] = 1.0;
    else
      mort_fact[inarea][l] = (1.0 - prop_surv[inarea][l]) / z[inarea][l];
  }
  mean_n[inarea].Multiply(mort_fact[inarea], *CI);
}

void MortPrey::calcZ(int area, const DoubleVector& natural_m) {
  //written by kgf 25/6 98
  //cannibalism added by kgf 13/7 98
  int i, upp_lim, inarea = AreaNr[area];

  if (z.Ncol(inarea) != natural_m.Size())
    upp_lim = min(z.Ncol(inarea), natural_m.Size());
  else
    upp_lim = z.Ncol(inarea);

  if (upp_lim < z.Ncol(inarea)) {
    for (i = 0; i < upp_lim; i++)
      z[inarea][i] = natural_m[i] + cons[inarea][i] + cannibalism[inarea][i];
    for (i = upp_lim; i < z.Ncol(inarea); i++)
      z[inarea][i] = cons[inarea][i] + cannibalism[inarea][i];
  } else {
    for (i = 0; i < z.Ncol(inarea); i++)
      z[inarea][i] = natural_m[i] + cons[inarea][i] + cannibalism[inarea][i];
  }
}

void MortPrey::setCannibalism(int area, const DoubleVector& cann) {
  //written by kgf 13/7 98
  int i, upp_lim, inarea = AreaNr[area];

  if (cannibalism.Ncol(inarea) != cann.Size())
    upp_lim = min(cannibalism.Ncol(inarea), cann.Size());
  else
    upp_lim = cannibalism.Ncol(inarea);
  for (i = 0; i < upp_lim; i++)
    cannibalism[inarea][i] = cann[i];
}

void MortPrey::addAgeGroupMatrix(DoubleMatrix* const agematrix) {
  agegroupmatrix.resize(1, agematrix);
}

void MortPrey::setAgeMatrix(int pred_no, int area, const DoubleVector& agegroupno) {
  (*agegroupmatrix[pred_no])[area] = agegroupno;
}

void MortPrey::setConsumption(int area, int pred_no, const BandMatrix& consum) {
  cann_cons.ChangeElement(AreaNr[area], pred_no, consum);
}

void MortPrey::addCannPredName(const char* predname) {
  char* tempName = new char[strlen(predname) + 1];
  strcpy(tempName, predname);
  cannprednames.resize(1, tempName);
}

void MortPrey::addConsMatrix(int pred_no, const BandMatrix& cons_mat) {
  //written by kgf 4/3 99
  //Note that the predator number and the dimensions of the band
  //matrices are supposed to be equal for all areas.
  int a;
  for (a = 0; a < cann_cons.Nrow(); a++)
    cann_cons.ChangeElement(a, pred_no, cons_mat);
}
