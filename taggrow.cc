#include "maturity.h"
#include "agebandmatrixratio.h"
#include "gadget.h"

void AgeBandMatrixRatio::Grow(const DoubleMatrix& Lgrowth, const AgeBandMatrix& Total) {

  int numTagExperiments = this->numTagExperiments();
  int i, lgrp, grow, maxlgrp, tag;

  if (numTagExperiments > 0) {
    DoubleVector number(numTagExperiments, 0.0);

    for (i = 0; i < nrow; i++) {
      //The part that grows to or above the highest length group.
      maxlgrp = v[i]->maxCol() - 1;
      for (tag = 0; tag < numTagExperiments; tag++)
        number[tag] = 0.0;

      for (lgrp = maxlgrp; lgrp >= v[i]->maxCol() - Lgrowth.Nrow(); lgrp--)
        for (grow = v[i]->maxCol() - lgrp - 1; grow < Lgrowth.Nrow(); grow++)
          for (tag = 0; tag < numTagExperiments; tag++)
            number[tag] += Lgrowth[grow][lgrp] * (*(*v[i])[lgrp][tag].N);

      for (tag = 0; tag < numTagExperiments; tag++)
        (*(*v[i])[maxlgrp][tag].N) = number[tag];

      //The center part of the length division
      for (lgrp = v[i]->maxCol() - 2; lgrp >= v[i]->minCol() + Lgrowth.Nrow() - 1; lgrp--) {
        for (tag = 0; tag < numTagExperiments; tag++)
          number[tag] = 0.0;

        for (grow = 0; grow < Lgrowth.Nrow(); grow++)
          for (tag = 0; tag < numTagExperiments; tag++)
            number[tag] += Lgrowth[grow][lgrp - grow] * (*(*v[i])[lgrp - grow][tag].N);

        for (tag = 0; tag < numTagExperiments; tag++)
          (*(*v[i])[lgrp][tag].N) = number[tag];
      }

      //The lowest part of the length division.
      for (lgrp = v[i]->minCol() + Lgrowth.Nrow() - 2; lgrp >= v[i]->minCol(); lgrp--) {
        for (tag = 0; tag < numTagExperiments; tag++)
          number[tag] = 0.0;

        for (grow = 0; grow <= lgrp - v[i]->minCol(); grow++)
          for (tag = 0; tag < numTagExperiments; tag++)
            number[tag] += Lgrowth[grow][lgrp - grow] * (*(*v[i])[lgrp - grow][tag].N);

        for (tag = 0; tag < numTagExperiments; tag++)
          (*(*v[i])[lgrp][tag].N) = number[tag];
      }
    }
    this->updateRatio(Total);
  }
}

//Same program with certain number of fish made mature.
void AgeBandMatrixRatio::Grow(const DoubleMatrix& Lgrowth, const AgeBandMatrix& Total,
  Maturity* const Mat, const TimeClass* const TimeInfo, const AreaClass* const Area, int area) {

  int numTagExperiments = this->numTagExperiments();
  int i, lgrp, grow, maxlgrp, age, tag;
  double tmp, ratio;
  DoubleVector number(numTagExperiments, 0.0);
  DoubleVector matnum(numTagExperiments, 0.0);

  if (numTagExperiments > 0) {
    for (i = 0; i < nrow; i++) {
      age = i + minage;
      maxlgrp = v[i]->maxCol() - 1;
      for (tag = 0; tag < numTagExperiments; tag++) {
        number[tag] = 0.0;
        matnum[tag] = 0.0;
      }

      //The part that grows to or above the highest length group.
      for (lgrp = maxlgrp; lgrp >= v[i]->maxCol() - Lgrowth.Nrow(); lgrp--) {
        for (grow = v[i]->maxCol() - lgrp - 1; grow < Lgrowth.Nrow(); grow++) {
          ratio =  Mat->MaturationProbability(age, lgrp, grow, TimeInfo, area, Total[i + minage][lgrp].W);
          for (tag = 0; tag < numTagExperiments; tag++) {
            tmp = Lgrowth[grow][lgrp] * (*(*v[i])[lgrp][tag].N);
            matnum[tag] += tmp * ratio;
            number[tag] += tmp;
          }
        }
      }

      for (tag = 0; tag < numTagExperiments; tag++) {
        if (isZero(number[tag])) {
          //no fish grow to this length cell
          (*(*v[i])[maxlgrp][tag].N) = 0.0;
          Mat->PutInStorage(area, age, maxlgrp, 0.0, TimeInfo, tag);
        } else if (number[tag] - matnum[tag] < verysmall) {
          //all the fish that grow to this length cell mature
          (*(*v[i])[maxlgrp][tag].N) = 0.0;
          Mat->PutInStorage(area, age, maxlgrp, matnum[tag], TimeInfo, tag);
        } else if (isZero(matnum[tag])) {
          //none of the fish that grow to this length cell mature
          (*(*v[i])[maxlgrp][tag].N) = number[tag];
          Mat->PutInStorage(area, age, maxlgrp, 0.0, TimeInfo, tag);
        } else {
          (*(*v[i])[maxlgrp][tag].N) = number[tag] - matnum[tag];
          Mat->PutInStorage(area, age, maxlgrp, matnum[tag], TimeInfo, tag);
        }
      }

      //The center part of the length division
      for (lgrp = v[i]->maxCol() - 2; lgrp >= v[i]->minCol() + Lgrowth.Nrow() - 1; lgrp--) {
        for (tag = 0; tag < numTagExperiments; tag++) {
          number[tag] = 0.0;
          matnum[tag] = 0.0;
        }

        for (grow = 0; grow < Lgrowth.Nrow(); grow++) {
          ratio = Mat->MaturationProbability(age, lgrp, grow, TimeInfo, area, Total[i + minage][lgrp - grow].W);
          for (tag = 0; tag < numTagExperiments; tag++) {
            tmp = Lgrowth[grow][lgrp - grow] * (*(*v[i])[lgrp - grow][tag].N);
            matnum[tag] += tmp * ratio;
            number[tag] += tmp;
          }
        }

        for (tag = 0; tag < numTagExperiments; tag++) {
          if (isZero(number[tag])) {
            //no fish grow to this length cell
            (*(*v[i])[lgrp][tag].N) = 0.0;
            Mat->PutInStorage(area, age, lgrp, 0.0, TimeInfo, tag);
          } else if (number[tag] - matnum[tag] < verysmall) {
            //all the fish that grow to this length cell mature
            (*(*v[i])[lgrp][tag].N) = 0.0;
            Mat->PutInStorage(area, age, lgrp, matnum[tag], TimeInfo, tag);
          } else if (isZero(matnum[tag])) {
            //none of the fish that grow to this length cell mature
            (*(*v[i])[lgrp][tag].N) = number[tag];
            Mat->PutInStorage(area, age, lgrp, 0.0, TimeInfo, tag);
          } else {
            (*(*v[i])[lgrp][tag].N) = number[tag] - matnum[tag];
            Mat->PutInStorage(area, age, lgrp, matnum[tag], TimeInfo, tag);
          }
        }
      }

      //The lowest part of the length division.
      for (lgrp = v[i]->minCol() + Lgrowth.Nrow() - 2; lgrp >= v[i]->minCol(); lgrp--) {
        for (tag = 0; tag < numTagExperiments; tag++) {
          number[tag] = 0.0;
          matnum[tag] = 0.0;
        }

        for (grow = 0; grow <= lgrp - v[i]->minCol(); grow++) {
          ratio = Mat->MaturationProbability(age, lgrp, grow, TimeInfo, area, Total[i + minage][lgrp - grow].W);
          for (tag = 0; tag < numTagExperiments; tag++) {
            tmp = Lgrowth[grow][lgrp - grow] * (*(*v[i])[lgrp - grow][tag].N);
            matnum[tag] += tmp * ratio;
            number[tag] += tmp;
          }
        }

        for (tag = 0; tag < numTagExperiments; tag++) {
          if (isZero(number[tag])) {
            //no fish grow to this length cell
            (*(*v[i])[lgrp][tag].N) = 0.0;
            Mat->PutInStorage(area, age, lgrp, 0.0, TimeInfo, tag);
          } else if (number[tag] - matnum[tag] < verysmall) {
            //all the fish that grow to this length cell mature
            (*(*v[i])[lgrp][tag].N) = 0.0;
            Mat->PutInStorage(area, age, lgrp, matnum[tag], TimeInfo, tag);
          } else if (isZero(matnum[tag])) {
            //none of the fish that grow to this length cell mature
            (*(*v[i])[lgrp][tag].N) = number[tag];
            Mat->PutInStorage(area, age, lgrp, 0.0, TimeInfo, tag);
          } else {
            (*(*v[i])[lgrp][tag].N) = number[tag] - matnum[tag];
            Mat->PutInStorage(area, age, lgrp, matnum[tag], TimeInfo, tag);
          }
        }
      }
    }
    this->updateRatio(Total);
  }
}
