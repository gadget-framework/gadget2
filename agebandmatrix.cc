#include "agebandm.h"

//Function that Reduces the number in given Agelengthkeys by age distributed catch.
//Pre: CI contains the mapping from Addition to Alkeys.
void AgebandmSubtract(Agebandmatrix& Alkeys, const bandmatrix& Catch,
  const ConversionIndex& CI, int allowNegativeResults) {

  const int minage =  max(Alkeys.Minage(), Catch.Minage());
  const int maxage =  min(Alkeys.Maxage(), Catch.Maxage());
  int age, l, minl, maxl, offset;

  if (maxage < minage)
    return;

  if (CI.SameDl()) { //Same dl on length distributions
    offset = CI.Offset();
    for (age = minage; age <= maxage; age++) {
      minl = max(Alkeys.Minlength(age), Catch.Minlength(age) + offset);
      maxl = min(Alkeys.Maxlength(age), Catch.Maxlength(age) + offset);
      for (l = minl; l < maxl; l++)
        Alkeys[age][l].N -= Catch[age][l - offset];
    }

  } else { //Not same dl on length distributions
    if (CI.TargetIsFiner()) {
      doublevector Ratio(CI.Nc());
      for (age = minage; age <= maxage; age++) {
        minl = max(Alkeys.Minlength(age), CI.Minpos(Catch.Minlength(age)));
        maxl = min(Alkeys.Maxlength(age), CI.Maxpos(Catch.Maxlength(age) - 1) + 1);
        if (maxl <= minl)
          return;
        for (l = CI.Pos(minl); l <= CI.Pos(maxl - 1); l++)
          Ratio[l] = 0.0;
        for (l = minl; l < maxl; l++)
          Ratio[CI.Pos(l)] += Alkeys[age][l].N;
        for (l = CI.Pos(minl); l <= CI.Pos(maxl - 1); l++) {
          if (Ratio[l] > 0) {
            if (allowNegativeResults)
              Ratio[l] = 1 - Catch[age][l] / Ratio[l];
            else
              Ratio[l] = max(0.0, 1 - Catch[age][l] / Ratio[l]);
          } else
            Ratio[l] = 1.0;
        }
        for (l = minl; l < maxl; l++)
          Alkeys[age][l].N *= Ratio[CI.Pos(l)];
      }
    } else {
      for (age = minage; age <= maxage; age++) {
        minl = max(CI.Minpos(Alkeys.Minlength(age)), Catch.Minlength(age));
        maxl = min(CI.Maxpos(Alkeys.Maxlength(age) - 1) + 1, Catch.Maxlength(age));
        for (l = minl; l < maxl; l++)
          Alkeys[age][CI.Pos(l)] -= Catch[age][l];
      }
    }
  }
}

//Function that Adds one Agebandmatrix to another.
//Pre: At least that CI contains the mapping from Addition to Alkeys.
void AgebandmAdd(Agebandmatrix& Alkeys, const Agebandmatrix& Addition,
  const ConversionIndex &CI, double ratio, int minage, int maxage) {

  popinfo pop;
  minage = max(Alkeys.Minage(), Addition.Minage(), minage);
  maxage = min(Alkeys.Maxage(), Addition.Maxage(), maxage);
  int age, l, minl, maxl, offset;

  if (maxage < minage)
    return;

  if (CI.SameDl()) { //Same dl on length distributions
    offset = CI.Offset();
    for (age = minage; age <= maxage; age++) {
      minl = max(Alkeys.Minlength(age), Addition.Minlength(age) + offset);
      maxl = min(Alkeys.Maxlength(age), Addition.Maxlength(age) + offset);
      for (l = minl; l < maxl; l++) {
        pop = Addition[age][l - offset];
        pop *= ratio;
        Alkeys[age][l] += pop;
      }
    }
  } else { //Not same dl on length distributions
    if (CI.TargetIsFiner()) {
      for (age = minage; age <= maxage; age++) {
        minl = max(Alkeys.Minlength(age), CI.Minpos(Addition.Minlength(age)));
        maxl = min(Alkeys.Maxlength(age), CI.Maxpos(Addition.Maxlength(age) - 1) + 1);
        for (l = minl; l < maxl; l++) {
          pop = Addition[age][CI.Pos(l)];
          pop *= ratio;
          pop.N /= CI.Nrof(l);
          Alkeys[age][l] += pop;
        }
      }
    } else {
      for (age = minage; age <= maxage; age++) {
        minl = max(CI.Minpos(Alkeys.Minlength(age)), Addition.Minlength(age));
        maxl = min(CI.Maxpos(Alkeys.Maxlength(age) - 1) + 1, Addition.Maxlength(age));
        if (maxl > minl && CI.Pos(maxl - 1) < Alkeys.Maxlength(age)
            && CI.Pos(minl) >= Alkeys.Minlength(age)) {
          for (l = minl; l < maxl; l++) {
            pop = Addition[age][l];
            pop *= ratio;
            Alkeys[age][CI.Pos(l)] += pop;
          }
        }
      }
    }
  }
}
