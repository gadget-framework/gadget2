#include "agebandmatrixratio.h"
#include "agebandmatrixratioptrvector.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

void AgeBandMatrixRatio::updateAndTagLoss(const AgeBandMatrix& Total, const DoubleVector& tagloss) {

  int numTagExperiments = this->numTagExperiments();
  int minlen, maxlen, age, length, tag;
  if (numTagExperiments > 0) {
    for (age = minage; age < minage + nrow; age++) {
      minlen = this->minLength(age);
      maxlen = this->maxLength(age);
      for (length = minlen; length < maxlen; length++) {
        for (tag = 0; tag < numTagExperiments; tag++) {
          (*v[age - minage])[length][tag].R *= tagloss[tag];
          (*(*v[age - minage])[length][tag].N) = (*v[age - minage])[length][tag].R * Total[age][length].N;
        }
      }
    }
  }
}

void AgeBandMatrixRatio::updateNumbers(const AgeBandMatrix& Total) {

  int numTagExperiments = this->numTagExperiments();
  int minlen, maxlen, age, length, tag;
  double number, ratio;

  if (numTagExperiments > 0) {
    for (age = minage; age < minage + nrow; age++) {
      minlen = this->minLength(age);
      maxlen = this->maxLength(age);
      for (length = minlen; length < maxlen; length++) {
        for (tag = 0; tag < numTagExperiments; tag++) {
          number = Total[age][length].N;
          ratio = (*v[age - minage])[length][tag].R;
          if (number < verysmall || ratio < verysmall) {
            (*(*v[age - minage])[length][tag].N) = 0.0;
            (*v[age - minage])[length][tag].R = 0.0;
          } else {
            (*(*v[age - minage])[length][tag].N) = ratio * number;
          }
        }
      }
    }
  }
}

void AgeBandMatrixRatio::updateRatio(const AgeBandMatrix& Total) {

  int numTagExperiments = this->numTagExperiments();
  int minlen, maxlen, age, length, tag;
  double tagnum, totalnum;

  if (numTagExperiments > 0) {
    for (age = minage; age < minage + nrow; age++) {
      minlen = this->minLength(age);
      maxlen = this->maxLength(age);
      for (length = minlen; length < maxlen; length++) {
        for (tag = 0; tag < numTagExperiments; tag++) {
          tagnum = (*(*v[age - minage])[length][tag].N);
          totalnum = Total[age][length].N;
          if (tagnum < verysmall || totalnum < verysmall) {
            (*(*v[age - minage])[length][tag].N) = 0.0;
            (*v[age - minage])[length][tag].R = 0.0;
          } else {
            (*v[age - minage])[length][tag].R = tagnum / totalnum;
          }
        }
      }
    }
  }
}

void AgeBandMatrixRatio::IncrementAge(const AgeBandMatrix& Total) {

  int numTagExperiments = this->numTagExperiments();
  int i, j, j1, j2, tag;

  if (nrow <= 1)
    return;

  if (numTagExperiments > 0) {
    i = nrow - 1;
    j1 = max(v[i]->minCol(), v[i - 1]->minCol());
    j2 = min(v[i]->maxCol(), v[i - 1]->maxCol());
    //For the highest age group
    for (j = j1; j < j2; j++)
      for (tag = 0; tag < numTagExperiments; tag++)
        (*(*v[i])[j][tag].N) += (*(*v[i - 1])[j][tag].N);

    for (j = v[i - 1]->minCol(); j < v[i - 1]->maxCol(); j++)
      for (tag = 0; tag < numTagExperiments; tag++) {
        (*(*v[i - 1])[j][tag].N) = 0.0;
        (*v[i - 1])[j][tag].R = 0.0;
      }

    //For the other age groups.
    //At the end of each for (i=nrow-2...) loop, the intersection of v[i-1] with
    //v[i] has been copied from v[i-1] to v[i] and v[i-1] has been set to 0.
    for (i = nrow - 2; i > 0; i--) {
      j1 = max(v[i]->minCol(), v[i - 1]->minCol());
      j2 = min(v[i]->maxCol(), v[i - 1]->maxCol());
      for (j = v[i - 1]->minCol(); j < j1; j++) {
        for (tag = 0; tag < numTagExperiments; tag++) {
          (*(*v[i - 1])[j][tag].N) = 0.0;
          (*v[i - 1])[j][tag].R = 0.0;
        }
      }

      for (j = j1; j < j2; j++) {
        for (tag = 0; tag < numTagExperiments; tag++) {
          (*(*v[i])[j][tag].N) = (*(*v[i - 1])[j][tag].N);
          (*(*v[i - 1])[j][tag].N) = 0.0;
          (*v[i - 1])[j][tag].R = 0.0;
        }
      }

      for (j = j2; j < v[i - 1]->maxCol(); j++) {
        for (tag = 0; tag < numTagExperiments; tag++) {
          (*(*v[i - 1])[j][tag].N) = 0.0;
          (*v[i - 1])[j][tag].R = 0.0;
        }
      }
    }

    //set number in age zero to zero.
    for (j = v[0]->minCol(); j < v[0]->maxCol(); j++)
      for (tag = 0; tag < numTagExperiments; tag++)
        (*(*v[0])[j][tag].N) = 0;

    this->updateRatio(Total);
  }
}

void AgeBandMatrixRatioPtrVector::Add(const AgeBandMatrixRatioPtrVector& Addition,
  int area, const ConversionIndex &CI, double ratio) {

  //JMB - note area has already been converted to internal area
  int minage =  max(v[area]->minAge(), Addition[area].minAge());
  int maxage =  min(v[area]->maxAge(), Addition[area].maxAge());

  if ((maxage < minage) || isZero(ratio))
    return;

  int age, minl, maxl, i, l, tagid, numtags, offset;
  double numfish;

  numtags = Addition.numTagExperiments();
  if (numtags > tagID.Size())
    handle.logMessage(LOGFAIL, "Error in agebandmatrixratio - wrong number of tagging experiments");

  if (numtags == 0)
    return;

  IntVector tagconversion(numtags, -1);
  for (i = 0; i < numtags; i++) {
    tagconversion[i] = this->getTagID(Addition.getTagName(i));
    if (tagconversion[i] < 0)
      handle.logMessage(LOGFAIL, "Error in agebandmatrixratio - unrecognised tagging experiment", Addition.getTagName(i));
  }

  numfish = 0.0;
  if (CI.isSameDl()) { //Same dl on length distributions
    offset = CI.getOffset();
    for (age = minage; age <= maxage; age++) {
      minl = max(v[area]->minLength(age), Addition[area].minLength(age) + offset);
      maxl = min(v[area]->maxLength(age), Addition[area].maxLength(age) + offset);
      for (l = minl; l < maxl; l++) {
        for (tagid = 0; tagid < numtags; tagid++) {
          numfish = *(Addition[area][age][l - offset][tagid].N) * ratio;
          *((*v[area])[age][l][tagconversion[tagid]].N) += numfish;
        }
      }
    }

  } else { //Not same dl.
    if (CI.isFiner()) {
      //Stock that is added to has finer division than the stock that is added to it.
      for (age = minage; age <= maxage; age++) {
        minl = max(v[area]->minLength(age), CI.minPos(Addition[area].minLength(age)));
        maxl = min(v[area]->maxLength(age), CI.maxPos(Addition[area].maxLength(age) - 1) + 1);
        for (l = minl; l < maxl; l++) {
          for (tagid = 0; tagid < numtags; tagid++) {
            numfish = *(Addition[area][age][CI.getPos(l)][tagid].N) * ratio;
            numfish /= CI.getNumPos(l);  //JMB CI.getNumPos() should never be zero
            *((*v[area])[age][l][tagconversion[tagid]].N) += numfish;
          }
        }
      }

    } else {
      //Stock that is added to has coarser division than the stock that is added to it.
      for (age = minage; age <= maxage; age++) {
        minl = max(CI.minPos(v[area]->minLength(age)), Addition[area].minLength(age));
        maxl = min(CI.maxPos(v[area]->maxLength(age) - 1) + 1, Addition[area].maxLength(age));
        if (maxl > minl && CI.getPos(maxl - 1) < v[area]->maxLength(age)
          && CI.getPos(minl) >= v[area]->minLength(age)) {

          for (l = minl; l < maxl; l++) {
            for (tagid = 0; tagid < numtags; tagid++) {
              numfish = *(Addition[area][age][l][tagid].N) * ratio;
              *((*v[area])[age][CI.getPos(l)][tagconversion[tagid]].N) += numfish;
            }
          }
        }
      }
    }
  }
}

void AgeBandMatrixRatioPtrVector::Migrate(const DoubleMatrix& MI, const AgeBandMatrixPtrVector& Total) {

  DoubleVector tmp(size, 0.0);
  int i, j, age, length, tag;
  int numTagExperiments = tagID.Size();
  if (numTagExperiments > 0) {
    for (age = v[0]->minAge(); age <= v[0]->maxAge(); age++) {
      for (length = v[0]->minLength(age); length < v[0]->maxLength(age); length++) {
        for (tag = 0; tag < numTagExperiments; tag++) {
          tmp.setToZero();
          for (j = 0; j < size; j++)
            for (i = 0; i < size; i++)
              tmp[j] += *((*v[i])[age][length][tag].N) * MI[j][i];

          for (j = 0; j < size; j++)
            *((*v[j])[age][length][tag].N) = tmp[j];
        }
      }
    }
    for (i = 0; i < size; i++)
      v[i]->updateRatio(Total[i]);
  }
}
