#include "agebandmatrix.h"
#include "agebandmatrixptrvector.h"
#include "mathfunc.h"
#include "bandmatrix.h"
#include "doublevector.h"
#include "conversionindex.h"
#include "popinfovector.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

void AgeBandMatrix::Add(const AgeBandMatrix& Addition,
  const ConversionIndex &CI, double ratio, int minaddage, int maxaddage) {

  PopInfo pop;
  minaddage = max(this->minAge(), Addition.minAge(), minaddage);
  maxaddage = min(this->maxAge(), Addition.maxAge(), maxaddage);
  int age, l, minl, maxl, offset;

  if (maxaddage < minaddage)
    return;

  if (CI.SameDl()) {
    offset = CI.Offset();
    for (age = minaddage; age <= maxaddage; age++) {
      minl = max(this->minLength(age), Addition.minLength(age) + offset);
      maxl = min(this->maxLength(age), Addition.maxLength(age) + offset);
      for (l = minl; l < maxl; l++) {
        pop = Addition[age][l - offset];
        pop *= ratio;
        (*v[age - minage])[l] += pop;
      }
    }
  } else {
    if (CI.TargetIsFiner()) {
      for (age = minaddage; age <= maxaddage; age++) {
        minl = max(this->minLength(age), CI.Minpos(Addition.minLength(age)));
        maxl = min(this->maxLength(age), CI.Maxpos(Addition.maxLength(age) - 1) + 1);
        for (l = minl; l < maxl; l++) {
          pop = Addition[age][CI.Pos(l)];
          pop *= ratio;
          if (isZero(CI.Nrof(l)))
            handle.logWarning("Error in agebandmatrix - divide by zero");
          else
            pop.N /= CI.Nrof(l);
          (*v[age - minage])[l] += pop;
        }
      }
    } else {
      for (age = minaddage; age <= maxaddage; age++) {
        minl = max(CI.Minpos(this->minLength(age)), Addition.minLength(age));
        maxl = min(CI.Maxpos(this->maxLength(age) - 1) + 1, Addition.maxLength(age));
        if (maxl > minl && CI.Pos(maxl - 1) < this->maxLength(age)
            && CI.Pos(minl) >= this->minLength(age)) {
          for (l = minl; l < maxl; l++) {
            pop = Addition[age][l];
            pop *= ratio;
            (*v[age - minage])[CI.Pos(l)] += pop;
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
      handle.logWarning("Error in agebandmatrix - negative ratio");
      UsedRatio[i] = 0.0;
    }
  }

  if (CI.SameDl()) {
    for (i = 0; i < nrow; i++) {
      j1 = max(v[i]->Mincol(), CI.minLength());
      j2 = min(v[i]->Maxcol(), CI.maxLength());
      for (j = j1; j < j2; j++)
        (*v[i])[j].N *= UsedRatio[j - CI.Offset()];
    }
  } else {
    for (i = 0; i < nrow; i++) {
      j1 = max(v[i]->Mincol(), CI.minLength());
      j2 = min(v[i]->Maxcol(), CI.maxLength());
      for (j = j1; j < j2; j++)
        (*v[i])[j].N *= UsedRatio[CI.Pos(j)];
    }
  }
}

//Pre: similar to AgeBandMatrix::Multiply
void AgeBandMatrix::Subtract(const DoubleVector& Consumption, const ConversionIndex& CI, const PopInfoVector& Nrof) {
  DoubleVector Ratio(CI.Size(), 1.0);
  int i;
  for (i = 0; i < Consumption.Size(); i++) {
    if (Nrof[i].N > verysmall)
      Ratio[i] = 1 - (Consumption[i] / Nrof[i].N);
  }

  this->Multiply(Ratio, CI);
}

//-----------------------------------------------------------------
//Multiply AgeBandMatrix by a Agedependent vector for example
//Natural mortality. Investigate if NaturalM should be allowed to be shorter.
void AgeBandMatrix::Multiply(const DoubleVector& NatM) {
  int i, j;
  int check = min(NatM.Size(), nrow);
  for (i = 0; i < check; i++)
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

  //set number in age zero to zero.
  for (j = v[0]->Mincol(); j < v[0]->Maxcol(); j++) {
    (*v[0])[j].N = 0.0;
    (*v[0])[j].W = 0.0;
  }
}

void AgeBandMatrix::setToZero() {
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

void AgeBandMatrix::printNumbers(ofstream& outfile) const {
  int i, j;
  int maxcol = 0;
  for (i = minage; i < minage + nrow; i++)
    if (v[i - minage]->Maxcol() > maxcol)
      maxcol = v[i - minage]->Maxcol();

  for (i = minage; i < minage + nrow; i++) {
    outfile << TAB;
    if (v[i - minage]->Mincol() > 0) {
      for (j = 0; j < v[i - minage]->Mincol(); j++) {
        outfile.precision(smallprecision);
        outfile.width(smallwidth);
        outfile << 0.0 << sep;
      }
    }
    for (j = v[i - minage]->Mincol(); j < v[i - minage]->Maxcol(); j++) {
      outfile.precision(smallprecision);
      outfile.width(smallwidth);
      outfile << (*v[i - minage])[j].N << sep;
    }
    if (v[i - minage]->Maxcol() < maxcol) {
      for (j = v[i - minage]->Maxcol(); j < maxcol; j++) {
        outfile.precision(smallprecision);
        outfile.width(smallwidth);
        outfile << 0.0 << sep;
      }
    }
    outfile << endl;
  }
}

void AgeBandMatrix::printWeights(ofstream& outfile) const {
  int i, j;
  int maxcol = 0;
  for (i = minage; i < minage + nrow; i++)
    if (v[i - minage]->Maxcol() > maxcol)
      maxcol = v[i - minage]->Maxcol();

  for (i = minage; i < minage + nrow; i++) {
    outfile << TAB;
    if (v[i - minage]->Mincol() > 0) {
      for (j = 0; j < v[i - minage]->Mincol(); j++) {
        outfile.precision(smallprecision);
        outfile.width(smallwidth);
        outfile << 0.0 << sep;
      }
    }
    for (j = v[i - minage]->Mincol(); j < v[i - minage]->Maxcol(); j++) {
      outfile.precision(smallprecision);
      outfile.width(smallwidth);
      outfile << (*v[i - minage])[j].W << sep;
    }
    if (v[i - minage]->Maxcol() < maxcol) {
      for (j = v[i - minage]->Maxcol(); j < maxcol; j++) {
        outfile.precision(smallprecision);
        outfile.width(smallwidth);
        outfile << 0.0 << sep;
      }
    }
    outfile << endl;
  }
}

void AgeBandMatrixPtrVector::Migrate(const DoubleMatrix& MI) {
  assert(MI.Nrow() == size);
  PopInfoVector tmp(size);
  int i, j, age, length;

  for (age = v[0]->minAge(); age <= v[0]->maxAge(); age++) {
    for (length = v[0]->minLength(age); length < v[0]->maxLength(age); length++) {
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
