#include "mortprey.h"
#include "print.h"
#include "intvector.h"
#include "gadget.h"

MortPrey::MortPrey(CommentStream& infile, const intvector& Areas,
  const char* givenname, int minage, int maxage,
  Keeper* const keeper, const LengthGroupDivision* const stock_lgrp)
  : Prey(infile, Areas, givenname, keeper),
    z(Areas.Size(), stock_lgrp->NoLengthGroups(), 0.0),
    mort_fact(Areas.Size(), stock_lgrp->NoLengthGroups(), 0.0),
    prop_surv(Areas.Size(), stock_lgrp->NoLengthGroups(), 0.0),
    cannibalism(Areas.Size(), stock_lgrp->NoLengthGroups(), 0.0) {

  //written by kgf 24/6 98
  //modified by kgf 9/7 98
  prey_lgrp = new LengthGroupDivision(*LgrpDiv);
  delete LgrpDiv; //wrong dimensions set in Prey's constructor
  LgrpDiv = new LengthGroupDivision(*stock_lgrp);

  //Finished reading from input file. Will now resize objects
  //according to the new LgrpDiv, using InitializeObjects.
  this->InitializeObjects();
  intvector size(maxage - minage + 1, LgrpDiv->NoLengthGroups());
  intvector minlength(maxage - minage + 1, 0);
  Alkeys.resize(areas.Size(), minage, minlength, size);
  mean_n.resize(areas.Size(), minage, minlength, size);
  haveCalculatedMeanN.resize(areas.Size(), 0);
  cann_is_true = 0;
}

MortPrey::MortPrey(const doublevector& lengths, const intvector& Areas, int minage,
  int maxage, const char* givenname, const LengthGroupDivision* const stock_lgrp)
  : Prey(lengths, Areas, givenname),
    z(Areas.Size(), stock_lgrp->NoLengthGroups(), 0.0),
    mort_fact(Areas.Size(), stock_lgrp->NoLengthGroups(), 0.0),
    prop_surv(Areas.Size(), stock_lgrp->NoLengthGroups(), 0.0),
    cannibalism(Areas.Size(), stock_lgrp->NoLengthGroups(), 0.0) {

  //written by kgf 24/6 98
  //modifed by kgf 9/7 98
  prey_lgrp = new LengthGroupDivision(*LgrpDiv);
  delete LgrpDiv; //wrong dimensions set in Prey's constructor
  LgrpDiv = new LengthGroupDivision(*stock_lgrp);

  //Will now resize objects
  //according to the new LgrpDiv, using InitializeObjects.
  this->InitializeObjects();
  intvector size(maxage - minage + 1, LgrpDiv->NoLengthGroups());
  intvector minlength(maxage - minage + 1, 0);
  Alkeys.resize(areas.Size(), minage, minlength, size);
  mean_n.resize(areas.Size(), minage, minlength, size);
  haveCalculatedMeanN.resize(areas.Size(), 0);
  cann_is_true = 0;
}

MortPrey::~MortPrey() {
  delete prey_lgrp;
}

void MortPrey::InitializeObjects() {
  popinfo nullpop;
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
  Number.AddRows(areas.Size(), LgrpDiv->NoLengthGroups(), nullpop);
  numberPriortoEating.AddRows(areas.Size(), LgrpDiv->NoLengthGroups(), nullpop);
  biomass.AddRows(areas.Size(), LgrpDiv->NoLengthGroups(), 0.0);
  cons.AddRows(areas.Size(), LgrpDiv->NoLengthGroups(), 0.0);
  consumption.AddRows(areas.Size(), LgrpDiv->NoLengthGroups(), 0.0);
  tooMuchConsumption.resize(areas.Size(), 0);
  total.resize(areas.Size(), 0.0);
  ratio.AddRows(areas.Size(), LgrpDiv->NoLengthGroups(), 0.0);
  overcons.AddRows(areas.Size(), LgrpDiv->NoLengthGroups(), 0.0);
  overconsumption.AddRows(areas.Size(), LgrpDiv->NoLengthGroups(), 0.0);
}

void MortPrey::Sum(const Agebandmatrix& stock, int area, int CurrentSubstep) {
  //written by kgf 22/6 98
  int inarea = AreaNr[area];
  int i, j;
  tooMuchConsumption[inarea] = 0;
  for (i = 0; i < consumption.Ncol(inarea); i++)
    cons[inarea][i] = 0.0;
  for (i = 0; i < Number[inarea].Size(); i++)
    Number[inarea][i].N = 0.0;
  for (i = 0; i < cannibalism[inarea].Size(); i++)
    cannibalism[inarea][i] = 0.0;
  mean_n[inarea].SettoZero();
  Alkeys[inarea].SettoZero(); //The next line copies stock to Alkeys and mean_n
  AgebandmAdd(mean_n[inarea], stock, *CI);
  AgebandmAdd(Alkeys[inarea], stock, *CI);
  Alkeys[inarea].Colsum(Number[inarea]);
  haveCalculatedMeanN[inarea] = 0;

  popinfo sum;
  for (i = 0; i < Number.Ncol(inarea); i++) {
    sum += Number[inarea][i];
    biomass[inarea][i] = Number[inarea][i].N * Number[inarea][i].W;
  }
  total[inarea] = sum.N * sum.W;
  for (i = 0; i < Number[inarea].Size(); i++)
    numberPriortoEating[inarea][i] = Number[inarea][i]; //should be inside if
  if (CurrentSubstep ==1) {
    for (j = 0; j < consumption.Ncol(inarea); j++) {
      consumption[inarea][j] = 0.0;
      overconsumption[inarea][j] = 0.0;
    }
  }
}

const Agebandmatrix& MortPrey::AlkeysPriorToEating(int area) const {
  return Alkeys[AreaNr[area]];
}

const Agebandmatrix& MortPrey::getMeanN(int area) const {
  assert(haveCalculatedMeanN[AreaNr[area]] == 1);
  return mean_n[AreaNr[area]];
}

void MortPrey::Print(ofstream& outfile) const {
  //must be modified! kgf 7/7 98
  outfile << "MortPrey\n";
  int area;
  for (area = 0; area < areas.Size(); area++) {
    outfile << "Alkeys on area " << areas[area] << endl;
    Printagebandm(outfile, Alkeys[area]);
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
  const int inarea = AreaNr[area];
  int l;
  for (l = 0; l < LgrpDiv->NoLengthGroups(); l++) {
    prop_surv[inarea][l] = exp (- z[inarea][l]);
    if (z[inarea][l] != 0.0)
      mort_fact[inarea][l] = (1.0 - prop_surv[inarea][l]) / z[inarea][l];
    else
      mort_fact[inarea][l] = 1.0;
  }
  mean_n[inarea].Multiply(mort_fact[inarea], *CI);
}

void MortPrey::calcZ(int area, const doublevector& natural_m) {
  //written by kgf 25/6 98
  //cannibalism added by kgf 13/7 98
  int i, upp_lim;
  int inarea = AreaNr[area];
  if (z.Ncol(inarea) != natural_m.Size())  //Ncol(i) returns v[i]->Size()
    upp_lim = min(z.Ncol(inarea), natural_m.Size());
  else
    upp_lim = z.Ncol(inarea);
  if (upp_lim < z.Ncol(inarea)) {
    for (i = 0; i < upp_lim; i++) {
      z[inarea][i] = natural_m[i];
      z[inarea][i] += cons[inarea][i];  //add total fishing mortality
      z[inarea][i] += cannibalism[inarea][i];
    }
    for (i = upp_lim; i < z.Ncol(inarea); i++) {
      z[inarea][i] = cons[inarea][i];   //add total fishing mortality
      z[inarea][i] += cannibalism[inarea][i];
    }
  } else {
    for (i = 0; i < z.Ncol(inarea); i++) {
      z[inarea][i] = natural_m[i];
      z[inarea][i] += cons[inarea][i];  //add total fishing mortality
      z[inarea][i] += cannibalism[inarea][i];
    }
  }
}

void MortPrey::SetCannibalism(int area, const doublevector& cann) {
  //written by kgf 13/7 98
  int inarea = AreaNr[area];
  int upp_lim, i;
  assert(cannibalism.Ncol(inarea) == cann.Size());
  if (cannibalism.Ncol(inarea) != cann.Size())  //Ncol(i) returns v[i]->Size()
    upp_lim = min(cannibalism.Ncol(inarea), cann.Size());
  else
    upp_lim = cannibalism.Ncol(inarea);
  for (i = 0; i < upp_lim; i++)
    cannibalism[inarea][i] = cann[i];
}

void MortPrey::addConsMatrix(int pred_no, const bandmatrix& cons_mat) {
  //written by kgf 4/3 99
  //Note that the predator number and the dimensions of the band
  //matrices are suppost to be equal for all areas.
  int a;
  for (a = 0; a < cann_cons.Nrow(); a++)
    cann_cons.ChangeElement(a, pred_no, cons_mat);
}
