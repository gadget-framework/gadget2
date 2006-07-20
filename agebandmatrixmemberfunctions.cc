#include "agebandmatrix.h"
#include "agebandmatrixptrvector.h"
#include "mathfunc.h"
#include "doublevector.h"
#include "conversionindex.h"
#include "popinfovector.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

void AgeBandMatrix::Add(const AgeBandMatrix& Addition, const ConversionIndex &CI, double ratio) {

  PopInfo pop;
  int minaddage = max(this->minAge(), Addition.minAge());
  int maxaddage = min(this->maxAge(), Addition.maxAge());
  int age, l, minl, maxl;

  if ((maxaddage < minaddage) || (isZero(ratio)))
    return;

  if (CI.isSameDl()) {
    int offset = CI.getOffset();
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
    if (CI.isFiner()) {
      for (age = minaddage; age <= maxaddage; age++) {
        minl = max(this->minLength(age), CI.minPos(Addition.minLength(age)));
        maxl = min(this->maxLength(age), CI.maxPos(Addition.maxLength(age) - 1) + 1);
        for (l = minl; l < maxl; l++) {
          pop = Addition[age][CI.getPos(l)];
          pop *= ratio;
          pop.N /= CI.getNumPos(l);  //JMB CI.getNumPos() should never be zero
          (*v[age - minage])[l] += pop;
        }
      }

    } else {
      for (age = minaddage; age <= maxaddage; age++) {
        minl = max(CI.minPos(this->minLength(age)), Addition.minLength(age));
        maxl = min(CI.maxPos(this->maxLength(age) - 1) + 1, Addition.maxLength(age));
        if (maxl > minl && CI.getPos(maxl - 1) < this->maxLength(age)
            && CI.getPos(minl) >= this->minLength(age)) {
          for (l = minl; l < maxl; l++) {
            pop = Addition[age][l];
            pop *= ratio;
            (*v[age - minage])[CI.getPos(l)] += pop;
          }
        }
      }
    }
  }
}

void AgeBandMatrix::Subtract(const DoubleVector& Ratio, const ConversionIndex& CI) {
  int i, j, j1, j2;
  if (CI.isSameDl()) {
    int offset = CI.getOffset();
    for (i = 0; i < nrow; i++) {
      j1 = max(v[i]->minCol(), CI.minLength());
      j2 = min(v[i]->maxCol(), CI.maxLength());
      for (j = j1; j < j2; j++)
        (*v[i])[j] *= Ratio[j - offset];
    }
  } else {
    for (i = 0; i < nrow; i++) {
      j1 = max(v[i]->minCol(), CI.minLength());
      j2 = min(v[i]->maxCol(), CI.maxLength());
      for (j = j1; j < j2; j++)
        (*v[i])[j] *= Ratio[CI.getPos(j)];
    }
  }
}

void AgeBandMatrix::Multiply(const DoubleVector& Ratio) {
  int i, j;
  for (i = 0; i < nrow; i++)
    for (j = v[i]->minCol(); j < v[i]->maxCol(); j++)
      (*v[i])[j] *= Ratio[i];
}

//summation over all ages for each length.
void AgeBandMatrix::sumColumns(PopInfoVector& Result) const {
  int i, j;
  for (i = 0; i < Result.Size(); i++)
    Result[i].setToZero();
  for (i = 0; i < nrow; i++)
    for (j = v[i]->minCol(); j < v[i]->maxCol(); j++)
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
  j1 = max(v[i]->minCol(), v[i - 1]->minCol());
  j2 = min(v[i]->maxCol(), v[i - 1]->maxCol());
  //For the oldest age group
  for (j = j1; j < j2; j++)
    (*v[i])[j] += (*v[i - 1])[j];
  for (j = v[i - 1]->minCol(); j < v[i - 1]->maxCol(); j++)
    (*v[i - 1])[j].setToZero();
  //Now v[nrow-2] has been added to v[nrow-1] and then set to 0.

  //For the other age groups
  //At the end of each for (i=nrow-2...) loop, the intersection of v[i-1] with
  //v[i] has been copied from v[i-1] to v[i] and v[i-1] has been set to 0.
  for (i = nrow - 2; i > 0; i--) {
    j1 = max(v[i]->minCol(), v[i - 1]->minCol());
    j2 = min(v[i]->maxCol(), v[i - 1]->maxCol());
    for (j = v[i - 1]->minCol(); j < j1; j++)
      (*v[i - 1])[j].setToZero();
    for (j = j1; j < j2; j++) {
      (*v[i])[j] = (*v[i - 1])[j];
      (*v[i - 1])[j].setToZero();
    }
    for (j = j2; j < v[i - 1]->maxCol(); j++)
      (*v[i - 1])[j].setToZero();
  }

  //set number in youngest age group to zero.
  for (j = v[0]->minCol(); j < v[0]->maxCol(); j++)
    (*v[0])[j].setToZero();
}

void AgeBandMatrix::setToZero() {
  int i, j;
  for (i = 0; i < nrow; i++)
    for (j = v[i]->minCol(); j < v[i]->maxCol(); j++)
      (*v[i])[j].setToZero();
}

void AgeBandMatrix::printNumbers(ofstream& outfile) const {
  int i, j;
  int maxcol = 0;
  for (i = minage; i < minage + nrow; i++)
    if (v[i - minage]->maxCol() > maxcol)
      maxcol = v[i - minage]->maxCol();

  for (i = minage; i < minage + nrow; i++) {
    outfile << TAB;
    if (v[i - minage]->minCol() > 0)
      for (j = 0; j < v[i - minage]->minCol(); j++)
        outfile << setw(smallwidth) << setprecision(smallprecision) << 0.0 << sep;

    for (j = v[i - minage]->minCol(); j < v[i - minage]->maxCol(); j++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << (*v[i - minage])[j].N << sep;

    if (v[i - minage]->maxCol() < maxcol)
      for (j = v[i - minage]->maxCol(); j < maxcol; j++)
        outfile << setw(smallwidth) << setprecision(smallprecision) << 0.0 << sep;

    outfile << endl;
  }
}

void AgeBandMatrix::printWeights(ofstream& outfile) const {
  int i, j;
  int maxcol = 0;
  for (i = minage; i < minage + nrow; i++)
    if (v[i - minage]->maxCol() > maxcol)
      maxcol = v[i - minage]->maxCol();

  for (i = minage; i < minage + nrow; i++) {
    outfile << TAB;
    if (v[i - minage]->minCol() > 0)
      for (j = 0; j < v[i - minage]->minCol(); j++)
        outfile << setw(smallwidth) << setprecision(smallprecision) << 0.0 << sep;

    for (j = v[i - minage]->minCol(); j < v[i - minage]->maxCol(); j++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << (*v[i - minage])[j].W << sep;

    if (v[i - minage]->maxCol() < maxcol)
      for (j = v[i - minage]->maxCol(); j < maxcol; j++)
        outfile << setw(smallwidth) << setprecision(smallprecision) << 0.0 << sep;

    outfile << endl;
  }
}

void AgeBandMatrixPtrVector::Migrate(const DoubleMatrix& MI, PopInfoVector& tmp) {
  int i, j, age, length;
  for (age = v[0]->minAge(); age <= v[0]->maxAge(); age++) {
    for (length = v[0]->minLength(age); length < v[0]->maxLength(age); length++) {
      for (j = 0; j < size; j++)
        tmp[j].setToZero();

      //Let tmp[j] keep the population of agelength group on area j after the migration
      for (j = 0; j < size; j++)
        for (i = 0; i < size; i++)
          tmp[j] += (*v[i])[age][length] * MI[j][i];

      for (j = 0; j < size; j++)
        (*v[j])[age][length] = tmp[j];
    }
  }
}
