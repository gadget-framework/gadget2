#include "maturity.h"

/* Update the agebandmatrix to reflect the calculated growth  */
/* Lgrowth contains the ratio of each length group that grows */
/* by a certain number of length groups, and Wgrowth contains */
/* the weight increase for each entry in Lgrowth              */

/* JMB changed to deal with very small weights a bit better   */
void AgeBandMatrix::Grow(const DoubleMatrix& Lgrowth, const DoubleMatrix& Wgrowth) {
  int i, lgrp, grow, maxlgrp;
  double number, wt, upfj;

  number = 0.0;
  wt = 0.0;
  for (i = 0; i < nrow; i++) {
    //The part that grows to or above the highest length group.
    number = 0.0;
    wt = 0.0;
    maxlgrp = v[i]->Maxcol() - 1;
    for (lgrp = maxlgrp; lgrp >= v[i]->Maxcol() - Lgrowth.Nrow(); lgrp--) {
      for (grow = v[i]->Maxcol() - lgrp - 1; grow < Lgrowth.Nrow(); grow++) {
        upfj = Lgrowth[grow][lgrp] * (*v[i])[lgrp].N;
        number += upfj;
        wt += upfj * ((*v[i])[lgrp].W + Wgrowth[grow][lgrp]);
      }
    }

    if ((isZero(number)) || (wt < verysmall)) {
      (*v[i])[maxlgrp].W = 0.0;
      (*v[i])[maxlgrp].N = 0.0;
    } else {
      (*v[i])[maxlgrp].W = wt / number;
      (*v[i])[maxlgrp].N = number;
    }

    //The center part of the length division
    for (lgrp = v[i]->Maxcol() - 2; lgrp >= v[i]->Mincol() + Lgrowth.Nrow() - 1; lgrp--) {
      number = 0.0;
      wt = 0.0;
      for (grow = 0; grow < Lgrowth.Nrow(); grow++) {
        upfj = Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;
        number += upfj;
        wt += upfj * ((*v[i])[lgrp - grow].W + Wgrowth[grow][lgrp - grow]);
      }

      if ((isZero(number)) || (wt < verysmall)) {
        (*v[i])[lgrp].W = 0.0;
        (*v[i])[lgrp].N = 0.0;
      } else {
        (*v[i])[lgrp].W = wt / number;
        (*v[i])[lgrp].N = number;
      }
    }

    //The lowest part of the length division.
    for (lgrp = v[i]->Mincol() + Lgrowth.Nrow() - 2; lgrp >= v[i]->Mincol(); lgrp--) {
      number = 0.0;
      wt = 0.0;
      for (grow = 0; grow <= lgrp - v[i]->Mincol(); grow++) {
        upfj = Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;
        number += upfj;
        wt += upfj * ((*v[i])[lgrp - grow].W + Wgrowth[grow][lgrp - grow]);
      }

      if ((isZero(number)) || (wt < verysmall)) {
        (*v[i])[lgrp].W = 0.0;
        (*v[i])[lgrp].N = 0.0;
      } else {
        (*v[i])[lgrp].W = wt / number;
        (*v[i])[lgrp].N = number;
      }
    }
  }
}

//Same program with certain number of fish made mature.
void AgeBandMatrix::Grow(const DoubleMatrix& Lgrowth, const DoubleMatrix& Wgrowth,
  Maturity* const Mat, const TimeClass* const TimeInfo, const AreaClass* const Area,
  const LengthGroupDivision* const GivenLDiv, int area) {

  int i, lgrp, grow, maxlgrp, age;
  double number, wt, matnum, upfj;

  number = 0.0;
  wt = 0.0;
  matnum = 0.0;
  for (i = 0; i < nrow; i++) {
    age = i + minage;
    maxlgrp = v[i]->Maxcol() - 1;
    number = 0.0;
    wt = 0.0;
    matnum = 0.0;
    for (lgrp = maxlgrp; lgrp >= v[i]->Maxcol() - Lgrowth.Nrow(); lgrp--) {
      for (grow = v[i]->Maxcol() - lgrp - 1; grow < Lgrowth.Nrow(); grow++) {
        upfj = Lgrowth[grow][lgrp] * (*v[i])[lgrp].N;
        const double ratio = Mat->MaturationProbability(age, lgrp, grow, TimeInfo, Area, area);
        matnum += upfj * ratio;
        number += upfj;
        wt += upfj * ((*v[i])[lgrp].W + Wgrowth[grow][lgrp]);
      }
    }

    if ((isZero(number)) || (number - matnum < verysmall) || (wt < verysmall)) {
      (*v[i])[maxlgrp].W = 0.0;
      (*v[i])[maxlgrp].N = 0.0;
      Mat->PutInStorage(area, age, maxlgrp, 0.0, 0.0, TimeInfo);
    } else {
      (*v[i])[maxlgrp].W = wt / number;
      (*v[i])[maxlgrp].N = number - matnum;
      Mat->PutInStorage(area, age, maxlgrp, matnum, wt / number, TimeInfo);
    }

    for (lgrp = v[i]->Maxcol() - 2; lgrp >= v[i]->Mincol() + Lgrowth.Nrow() - 1; lgrp--) {
      number = 0.0;
      wt = 0.0;
      matnum = 0.0;
      for (grow = 0; grow < Lgrowth.Nrow(); grow++) {
        upfj = Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;
        const double ratio = Mat->MaturationProbability(age, lgrp, grow, TimeInfo, Area, area);
        matnum += upfj * ratio;
        number += upfj;
        wt += upfj * ((*v[i])[lgrp - grow].W + Wgrowth[grow][lgrp - grow]);
      }

      if ((isZero(number)) || (number - matnum < verysmall) || (wt < verysmall)) {
        (*v[i])[lgrp].W = 0.0;
        (*v[i])[lgrp].N = 0.0;
        Mat->PutInStorage(area, age, lgrp, 0.0, 0.0, TimeInfo);
      } else {
        (*v[i])[lgrp].W = wt / number;
        (*v[i])[lgrp].N = number - matnum;
        Mat->PutInStorage(area, age, lgrp, matnum, wt / number, TimeInfo);
      }
    }

    for (lgrp = v[i]->Mincol() + Lgrowth.Nrow() - 2; lgrp >= v[i]->Mincol(); lgrp--) {
      number = 0.0;
      wt = 0.0;
      matnum = 0.0;
      for (grow = 0; grow <= lgrp - v[i]->Mincol(); grow++) {
        upfj = Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;
        const double ratio =
          Mat->MaturationProbability(age, lgrp, grow, TimeInfo, Area, area);
        matnum += upfj * ratio;
        number += upfj;
        wt += upfj * ((*v[i])[lgrp - grow].W + Wgrowth[grow][lgrp - grow]);
      }

      if ((isZero(number)) || (number - matnum < verysmall) || (wt < verysmall)) {
        (*v[i])[lgrp].W = 0.0;
        (*v[i])[lgrp].N = 0.0;
        Mat->PutInStorage(area, age, lgrp, 0.0, 0.0, TimeInfo);
      } else {
        (*v[i])[lgrp].W = wt / number;
        (*v[i])[lgrp].N = number - matnum;
        Mat->PutInStorage(area, age, lgrp, matnum, wt / number, TimeInfo);
      }
    }
  }
}

//fleksibest formulation - weight read in from file (should be positive)
void AgeBandMatrix::Grow(const DoubleMatrix& Lgrowth, const DoubleVector& Weight) {
  int i, lgrp, grow, maxlgrp;
  double number, upfj;

  number = 0.0;
  for (i = 0; i < nrow; i++) {
    number = 0.0;
    maxlgrp = v[i]->Maxcol() - 1;
    for (lgrp = maxlgrp; lgrp >= v[i]->Maxcol() - Lgrowth.Nrow(); lgrp--) {
      for (grow = v[i]->Maxcol() - lgrp - 1; grow < Lgrowth.Nrow(); grow++) {
        upfj = Lgrowth[grow][lgrp] * (*v[i])[lgrp].N;
        number += upfj;
      }
    }
    if (number > 0)
      (*v[i])[maxlgrp].W = Weight[maxlgrp];
    else
      (*v[i])[maxlgrp].W = 0.0;

    (*v[i])[maxlgrp].N = number;

    for (lgrp = v[i]->Maxcol() - 2; lgrp >= v[i]->Mincol() + Lgrowth.Nrow() - 1; lgrp--) {
      number = 0;
      for (grow = 0; grow < Lgrowth.Nrow(); grow++) {
        upfj = Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;
        number += upfj;
      }
      if (number > 0)
        (*v[i])[lgrp].W = Weight[lgrp];
      else
        (*v[i])[lgrp].W = 0.0;

      (*v[i])[lgrp].N = number;
    }

    for (lgrp = v[i]->Mincol() + Lgrowth.Nrow() - 2; lgrp >= v[i]->Mincol(); lgrp--) {
      number = 0;
      for (grow = 0; grow <= lgrp - v[i]->Mincol(); grow++) {
        upfj = Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;
        number += upfj;
      }
      if (number > 0)
        (*v[i])[lgrp].W = Weight[lgrp];
      else
        (*v[i])[lgrp].W = 0.0;

      (*v[i])[lgrp].N = number;
    }
  }
}

//fleksibest formulation - weight read in from file (should be positive)
//Same program with certain number of fish made mature.
void AgeBandMatrix::Grow(const DoubleMatrix& Lgrowth, const DoubleVector& Weight,
  Maturity* const Mat, const TimeClass* const TimeInfo, const AreaClass* const Area,
  const LengthGroupDivision* const GivenLDiv, int area) {

  int i, lgrp, grow, maxlgrp, age;
  double number, matnum, upfj;

  number = 0.0;
  matnum = 0.0;
  for (i = 0; i < nrow; i++) {
    age = i + minage;
    maxlgrp = v[i]->Maxcol() - 1;
    number = 0.0;
    matnum = 0.0;
    for (lgrp = maxlgrp; lgrp >= v[i]->Maxcol() - Lgrowth.Nrow(); lgrp--) {
      for (grow = v[i]->Maxcol() - lgrp - 1; grow < Lgrowth.Nrow(); grow++) {
        upfj = Lgrowth[grow][lgrp] * (*v[i])[lgrp].N;
        const double ratio = Mat->MaturationProbability(age, lgrp, grow, TimeInfo, Area, area);
        matnum += upfj * ratio;
        number += upfj;
      }
    }
    if (number > 0)
      (*v[i])[maxlgrp].W = Weight[maxlgrp];

    (*v[i])[maxlgrp].N = number - matnum;

    if (Weight[maxlgrp] > 0 && matnum > 0)
      Mat->PutInStorage(area, age, maxlgrp, matnum, Weight[maxlgrp], TimeInfo);
    else
      Mat->PutInStorage(area, age, maxlgrp, 0.0, 0.0, TimeInfo);

    for (lgrp = v[i]->Maxcol() - 2; lgrp >= v[i]->Mincol() + Lgrowth.Nrow() - 1; lgrp--) {
      number = 0.0;
      matnum = 0.0;
      for (grow = 0; grow < Lgrowth.Nrow(); grow++) {
        upfj = Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;
        const double ratio = Mat->MaturationProbability(age, lgrp, grow, TimeInfo, Area, area);
        matnum += upfj * ratio;
        number += upfj;
      }
      if (number > 0)
        (*v[i])[lgrp].W = Weight[lgrp];

      (*v[i])[lgrp].N = number - matnum;

      if (Weight[lgrp] > 0 && matnum > 0)
        Mat->PutInStorage(area, age, lgrp, matnum, Weight[lgrp], TimeInfo);
      else
        Mat->PutInStorage(area, age, lgrp, 0.0, 0.0, TimeInfo);
    }

    for (lgrp = v[i]->Mincol() + Lgrowth.Nrow() - 2; lgrp >= v[i]->Mincol(); lgrp--) {
      number = 0.0;
      matnum = 0.0;
      for (grow = 0; grow <= lgrp - v[i]->Mincol(); grow++) {
        upfj = Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;
        const double ratio = Mat->MaturationProbability(age, lgrp, grow, TimeInfo, Area, area);
        matnum += upfj * ratio;
        number += upfj;
      }
      if (number > 0)
        (*v[i])[lgrp].W = Weight[lgrp];

      (*v[i])[lgrp].N = number - matnum;

      if (Weight[lgrp] > 0 && matnum > 0)
        Mat->PutInStorage(area, age, lgrp, matnum, Weight[lgrp], TimeInfo);
      else
        Mat->PutInStorage(area, age, lgrp, 0.0, 0.0, TimeInfo);
    }
  }
}
