#include "agebandmatrix.h"
#include "agebandmatrixptrvector.h"
#include "mathfunc.h"
#include "bandmatrix.h"
#include "doublevector.h"
#include "conversion.h"
#include "popinfovector.h"
#include "gadget.h"

//Function that Adds one AgeBandMatrix to another.
//Pre: At least that CI contains the mapping from Addition to Alkeys.
void AgebandmAdd(AgeBandMatrix& Alkeys, const AgeBandMatrix& Addition,
  const ConversionIndex &CI, double ratio, int minage, int maxage) {

  PopInfo pop;
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

//Pre: CI converts from 'this' to Ratio and CI.TargetIsFiner() == 1
void AgeBandMatrix::Multiply(const DoubleVector& Ratio, const ConversionIndex& CI) {
  assert(!CI.TargetIsFiner());
  //We use the vector UsedRatio instead of Ratio to reduce the chances
  //of numerical errors and negative stock size.
  DoubleVector UsedRatio(Ratio);
  int i, j, j1, j2;

  for (i = 0; i < UsedRatio.Size(); i++) {
    if (isZero(UsedRatio[i]))
      UsedRatio[i] = 0.0;
    else if (UsedRatio[i] < 0) {
      cerr << "Warning: negative stock population - setting to zero\n";
      UsedRatio[i] = 0.0;
    }
  }

  if (CI.SameDl()) {
    for (i = 0; i < nrow; i++) {
      j1 = max(v[i]->Mincol(), CI.Minlength());
      j2 = min(v[i]->Maxcol(), CI.Maxlength());
      for (j = j1; j < j2; j++)
        (*v[i])[j].N *= UsedRatio[j - CI.Offset()];
    }
  } else {  //Not same dl
    for (i = 0; i < nrow; i++) {
      j1 = max(v[i]->Mincol(), CI.Minlength());
      j2 = min(v[i]->Maxcol(), CI.Maxlength());
      for (j = j1; j < j2; j++)
        (*v[i])[j].N *= UsedRatio[CI.Pos(j)];
    }
  }
}

//Pre: similar to AgeBandMatrix::Multiply
void AgeBandMatrix::Subtract(const DoubleVector& Consumption, const ConversionIndex& CI, const PopInfoVector& Nrof) {
  DoubleVector Ratio(CI.Nf(), 1);
  int i;
  for (i = 0; i < Consumption.Size(); i++) {
    if (Nrof[i].N > 0)
      Ratio[i] = 1 - Consumption[i] / Nrof[i].N;

    if ((Consumption[i] < rathersmall) && (Nrof[i].N < rathersmall))
      Ratio[i] = 1.0;
  }

  this->Multiply(Ratio, CI);
}

//-----------------------------------------------------------------
//Multiply AgeBandMatrix by a Agedependent vector for example
//Natural mortality. Investigate if NaturalM should be allowed to be shorter.
void AgeBandMatrix::Multiply(const DoubleVector& NatM) {
  int i, j;
  for (i = 0; i < NatM.Size() && i < nrow; i++)
    for (j = v[i]->Mincol(); j < v[i]->Maxcol(); j++)
      (*v[i])[j] *= NatM[i];
}

//--------------------------------------------------------------
//Find the Column sum of a BandMatrix.  In AgeBandMatrix it means
//summation over all ages for each length.
void AgeBandMatrix::Colsum(PopInfoVector& Result) const {
  int i, j;
  for (i = 0; i < nrow; i++)
    for (j = v[i]->Mincol(); j < v[i]->Maxcol(); j++)
      Result[j] += (*v[i])[j];
}

//This function increases the age.  When moving from one age class to
//another only the intersection of the agegroups of the length
//classes is moved.  This could possibly be improved later on.
void AgeBandMatrix::IncrementAge() {
  int i, j, j1, j2;

  if (nrow <= 1)
    return;  //No ageing takes place where there is only one age.

  i = nrow - 1;
  j1 = max(v[i]->Mincol(), v[i - 1]->Mincol());
  j2 = min(v[i]->Maxcol(), v[i - 1]->Maxcol());
  //For the highest age group
  for (j = j1; j < j2; j++)
    (*v[i])[j] += (*v[i - 1])[j];
  for (j = v[i - 1]->Mincol(); j < v[i - 1]->Maxcol(); j++) {
    (*v[i - 1])[j].N = 0.0;
    (*v[i - 1])[j].W = 0.0;
  }
  //Now v[nrow-2] has been added to v[nrow-1] and then set to 0.

  //For the other age groups.
  //At the end of each for (i=nrow-2...) loop, the intersection of v[i-1] with
  //v[i] has been copied from v[i-1] to v[i] and v[i-1] has been set to 0.
  for (i = nrow - 2; i > 0; i--) {
    j1 = max(v[i]->Mincol(), v[i - 1]->Mincol());
    j2 = min(v[i]->Maxcol(), v[i - 1]->Maxcol());
    for (j = v[i - 1]->Mincol(); j < j1; j++) {
      (*v[i - 1])[j].N = 0.0;
      (*v[i - 1])[j].W = 0.0;
    }
    for (j = j1; j < j2; j++) {
      (*v[i])[j] = (*v[i - 1])[j];
      (*v[i - 1])[j].N = 0.0;
      (*v[i - 1])[j].W = 0.0;
    }
    for (j = j2; j < v[i - 1]->Maxcol(); j++) {
      (*v[i - 1])[j].N = 0.0;
      (*v[i - 1])[j].W = 0.0;
    }
  }

  //Set number in age zero to zero.
  for (j = v[0]->Mincol(); j < v[0]->Maxcol(); j++) {
    (*v[0])[j].N = 0.0;
    (*v[0])[j].W = 0.0;
  }
}

void AgeBandMatrix::SettoZero() {
  int i, j;
  for (i = 0; i < nrow; i++)
    for (j = v[i]->Mincol(); j < v[i]->Maxcol(); j++) {
      (*v[i])[j].N = 0.0;
      (*v[i])[j].W = 0.0;
    }
}

void AgeBandMatrix::FilterN(double minN) {
  int i, j;
  for (i = 0; i < nrow; i++)
    for (j = v[i]->Mincol(); j < v[i]->Maxcol(); j++)
      if ((*v[i])[j].N < minN) {
        (*v[i])[j].N = 0.0;
        (*v[i])[j].W = 0.0;
      }
}

//Function that implements the migration
void AgeBandMatrixPtrVector::Migrate(const DoubleMatrix& MI) {
  assert(MI.Nrow() == size);
  PopInfoVector tmp(size);
  int i, j, age, length;

  for (age = v[0]->Minage(); age <= v[0]->Maxage(); age++) {
    for (length = v[0]->Minlength(age); length < v[0]->Maxlength(age); length++) {
      for (j = 0; j < size; j++) {
        tmp[j].N = 0.0;
        tmp[j].W = 0.0;
      }
      //Let tmp[j] keep the population of agelength group on area j after the migration
      for (j = 0; j < size; j++)
        for (i = 0; i < size; i++)
          tmp[j] += (*v[i])[age][length] * MI[j][i];

      for (j = 0; j < size; j++)
        (*v[j])[age][length] = tmp[j];
    }
  }
}
