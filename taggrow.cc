#include "maturity.h"
#include "agebandmatrixratio.h"
#include "gadget.h"

void AgeBandMatrixRatio::Grow(const DoubleMatrix& Lgrowth, const AgeBandMatrix& Total) {

  int numTagExperiments = this->numTagExperiments();
  int i, lgrp, grow, maxlgrp, tag;
  double upfj;
  DoubleVector number(numTagExperiments, 0.0);

  if (numTagExperiments > 0) {
    for (i = 0; i < nrow; i++) {
      //The part that grows to or above the highest length group.
      maxlgrp = v[i]->Maxcol() -1;
      for (tag = 0; tag < numTagExperiments; tag++)
        number[tag] = 0.0;

      for (lgrp = maxlgrp; lgrp >= v[i]->Maxcol() - Lgrowth.Nrow(); lgrp--) {
        for (grow = v[i]->Maxcol() - lgrp - 1; grow < Lgrowth.Nrow(); grow++) {
          for (tag = 0; tag < numTagExperiments; tag++) {
            upfj = Lgrowth[grow][lgrp] * (*(*v[i])[lgrp][tag].N);
            number[tag] += upfj;
          }
        }
      }

      for (tag = 0; tag < numTagExperiments; tag++)
        (*(*v[i])[maxlgrp][tag].N) = number[tag];

      //The center part of the length division
      for (lgrp = v[i]->Maxcol() - 2; lgrp >= v[i]->Mincol() + Lgrowth.Nrow() - 1; lgrp--) {
        for (tag = 0; tag < numTagExperiments; tag++)
          number[tag] = 0.0;

        for (grow = 0; grow < Lgrowth.Nrow(); grow++) {
          for (tag = 0; tag < numTagExperiments; tag++) {
            upfj = Lgrowth[grow][lgrp - grow] * (*(*v[i])[lgrp - grow][tag].N);
            number[tag] += upfj;
          }
        }

        for (tag = 0; tag < numTagExperiments; tag++)
          (*(*v[i])[lgrp][tag].N) = number[tag];
      }

      //The lowest part of the length division.
      for (lgrp = v[i]->Mincol() + Lgrowth.Nrow() - 2; lgrp >= v[i]->Mincol(); lgrp--) {
        for (tag = 0; tag < numTagExperiments; tag++)
          number[tag] = 0.0;

        for (grow = 0; grow <= lgrp - v[i]->Mincol(); grow++) {
          for (tag = 0; tag < numTagExperiments; tag++) {
            upfj = Lgrowth[grow][lgrp - grow] * (*(*v[i])[lgrp - grow][tag].N);
            number[tag] += upfj;
          }
        }

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
  double upfj, ratio;
  DoubleVector number(numTagExperiments, 0.0);
  DoubleVector matnum(numTagExperiments, 0.0);

  if (numTagExperiments > 0) {
    for (i = 0; i < nrow; i++) {
      age = i + minage;
      maxlgrp = v[i]->Maxcol() - 1;
      for (tag = 0; tag < numTagExperiments; tag++) {
        number[tag] = 0.0;
        matnum[tag] = 0.0;
      }

      //The part that grows to or above the highest length group.
      for (lgrp = maxlgrp; lgrp >= v[i]->Maxcol() - Lgrowth.Nrow(); lgrp--) {
        for (grow = v[i]->Maxcol() - lgrp - 1; grow < Lgrowth.Nrow(); grow++) {
          ratio =  Mat->MaturationProbability(age, lgrp, grow, TimeInfo, area, Total[i + minage][lgrp].W);
          for (tag = 0; tag < numTagExperiments; tag++) {
            upfj = Lgrowth[grow][lgrp] * (*(*v[i])[lgrp][tag].N);
            matnum[tag] += upfj * ratio;
            number[tag] += upfj;
          }
        }
      }

      for (tag = 0; tag < numTagExperiments; tag++) {
        (*(*v[i])[maxlgrp][tag].N) = number[tag] - matnum[tag];
        if (Total[i + minage][maxlgrp].W > 0 && matnum[tag] > 0)
          Mat->PutInStorage(area, age, maxlgrp, matnum[tag], TimeInfo, tag);
        else
          Mat->PutInStorage(area, age, maxlgrp, 0.0, TimeInfo, tag);
      }

      //The center part of the length division
      for (lgrp = v[i]->Maxcol() - 2; lgrp >= v[i]->Mincol() + Lgrowth.Nrow() - 1; lgrp--) {
        for (tag = 0; tag < numTagExperiments; tag++) {
          number[tag] = 0.0;
          matnum[tag] = 0.0;
        }

        for (grow = 0; grow < Lgrowth.Nrow(); grow++) {
          ratio = Mat->MaturationProbability(age, lgrp, grow, TimeInfo, area, Total[i + minage][lgrp - grow].W);
          for (tag = 0; tag < numTagExperiments; tag++) {
            upfj = Lgrowth[grow][lgrp - grow] * (*(*v[i])[lgrp - grow][tag].N);
            matnum[tag] += upfj * ratio;
            number[tag] += upfj;
          }
        }

        for (tag = 0; tag < numTagExperiments; tag++) {
          (*(*v[i])[lgrp][tag].N) = number[tag] - matnum[tag];
          if (Total[i + minage][lgrp].W > 0 && matnum[tag] > 0)
            Mat->PutInStorage(area, age, lgrp, matnum[tag], TimeInfo, tag);
          else
            Mat->PutInStorage(area, age, lgrp, 0.0, TimeInfo, tag);
        }
      }

      //The lowest part of the length division.
      for (lgrp = v[i]->Mincol() + Lgrowth.Nrow() - 2; lgrp >= v[i]->Mincol(); lgrp--) {
        for (tag = 0; tag < numTagExperiments; tag++) {
          number[tag] = 0.0;
          matnum[tag] = 0.0;
        }

        for (grow = 0; grow <= lgrp - v[i]->Mincol(); grow++) {
          ratio = Mat->MaturationProbability(age, lgrp, grow, TimeInfo, area, Total[i + minage][lgrp - grow].W);
          for (tag = 0; tag < numTagExperiments; tag++) {
            upfj = Lgrowth[grow][lgrp - grow] * (*(*v[i])[lgrp - grow][tag].N);
            matnum[tag] += upfj * ratio;
            number[tag] += upfj;
          }
        }

        for (tag = 0; tag < numTagExperiments; tag++) {
          (*(*v[i])[lgrp][tag].N) = number[tag] - matnum[tag];
          if (Total[i + minage][lgrp].W > 0 && matnum[tag] > 0)
            Mat->PutInStorage(area, age, lgrp, matnum[tag], TimeInfo, tag);
          else
            Mat->PutInStorage(area, age, lgrp, 0.0, TimeInfo, tag);
        }
      }
    }
    this->updateRatio(Total);
  }
}
