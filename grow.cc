#include "maturity.h"

/* Update the agebandmatrix to reflect the calculated growth  */
/* Lgrowth contains the ratio of each length group that grows */
/* by a certain num of length groups, and Wgrowth contains */
/* the weight increase for each entry in Lgrowth              */

/* JMB changed to deal with very small weights a bit better   */
void AgeBandMatrix::Grow(const DoubleMatrix& Lgrowth, const DoubleMatrix& Wgrowth) {
  int i, lgrp, grow, maxlgrp;
  double num, wt, tmp;

  num = 0.0;
  wt = 0.0;
  for (i = 0; i < nrow; i++) {
    //The part that grows to or above the highest length group.
    num = 0.0;
    wt = 0.0;
    maxlgrp = v[i]->maxCol() - 1;
    for (lgrp = maxlgrp; lgrp >= v[i]->maxCol() - Lgrowth.Nrow(); lgrp--) {
      for (grow = v[i]->maxCol() - lgrp - 1; grow < Lgrowth.Nrow(); grow++) {
        tmp = Lgrowth[grow][lgrp] * (*v[i])[lgrp].N;
        num += tmp;
        wt += tmp * ((*v[i])[lgrp].W + Wgrowth[grow][lgrp]);
      }
    }

    if ((isZero(num)) || (wt < verysmall)) {
      (*v[i])[maxlgrp].W = 0.0;
      (*v[i])[maxlgrp].N = 0.0;
    } else {
      (*v[i])[maxlgrp].W = wt / num;
      (*v[i])[maxlgrp].N = num;
    }

    //The center part of the length division
    for (lgrp = v[i]->maxCol() - 2; lgrp >= v[i]->minCol() + Lgrowth.Nrow() - 1; lgrp--) {
      num = 0.0;
      wt = 0.0;
      for (grow = 0; grow < Lgrowth.Nrow(); grow++) {
        tmp = Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;
        num += tmp;
        wt += tmp * ((*v[i])[lgrp - grow].W + Wgrowth[grow][lgrp - grow]);
      }

      if ((isZero(num)) || (wt < verysmall)) {
        (*v[i])[lgrp].W = 0.0;
        (*v[i])[lgrp].N = 0.0;
      } else {
        (*v[i])[lgrp].W = wt / num;
        (*v[i])[lgrp].N = num;
      }
    }

    //The lowest part of the length division.
    for (lgrp = v[i]->minCol() + Lgrowth.Nrow() - 2; lgrp >= v[i]->minCol(); lgrp--) {
      num = 0.0;
      wt = 0.0;
      for (grow = 0; grow <= lgrp - v[i]->minCol(); grow++) {
        tmp = Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;
        num += tmp;
        wt += tmp * ((*v[i])[lgrp - grow].W + Wgrowth[grow][lgrp - grow]);
      }

      if ((isZero(num)) || (wt < verysmall)) {
        (*v[i])[lgrp].W = 0.0;
        (*v[i])[lgrp].N = 0.0;
      } else {
        (*v[i])[lgrp].W = wt / num;
        (*v[i])[lgrp].N = num;
      }
    }
  }
}

//Same program with certain num of fish made mature.
void AgeBandMatrix::Grow(const DoubleMatrix& Lgrowth, const DoubleMatrix& Wgrowth,
  Maturity* const Mat, const TimeClass* const TimeInfo, const AreaClass* const Area, int area) {

  int i, lgrp, grow, maxlgrp, age;
  double num, wt, matnum, tmp, ratio;

  num = 0.0;
  wt = 0.0;
  matnum = 0.0;
  for (i = 0; i < nrow; i++) {
    age = i + minage;
    maxlgrp = v[i]->maxCol() - 1;
    num = 0.0;
    wt = 0.0;
    matnum = 0.0;
    for (lgrp = maxlgrp; lgrp >= v[i]->maxCol() - Lgrowth.Nrow(); lgrp--) {
      for (grow = v[i]->maxCol() - lgrp - 1; grow < Lgrowth.Nrow(); grow++) {
        tmp = Lgrowth[grow][lgrp] * (*v[i])[lgrp].N;
        ratio = Mat->MaturationProbability(age, lgrp, grow, TimeInfo, area, (*v[i])[lgrp].W);
        matnum += (tmp * ratio);
        num += tmp;
        wt += tmp * ((*v[i])[lgrp].W + Wgrowth[grow][lgrp]);
      }
    }

    if ((isZero(num)) || (wt < verysmall)) {
      //no fish grow to this length cell
      (*v[i])[maxlgrp].W = 0.0;
      (*v[i])[maxlgrp].N = 0.0;
      Mat->PutInStorage(area, age, maxlgrp, 0.0, 0.0, TimeInfo);
    } else if (num - matnum < verysmall) {
      //all the fish that grow to this length cell mature
      (*v[i])[maxlgrp].W = 0.0;
      (*v[i])[maxlgrp].N = 0.0;
      Mat->PutInStorage(area, age, maxlgrp, num, wt / num, TimeInfo);
    } else if (isZero(matnum)) {
      //none of the fish that grow to this length cell mature
      (*v[i])[maxlgrp].W = wt / num;
      (*v[i])[maxlgrp].N = num;
      Mat->PutInStorage(area, age, maxlgrp, 0.0, 0.0, TimeInfo);
    } else {
      (*v[i])[maxlgrp].W = wt / num;
      (*v[i])[maxlgrp].N = num - matnum;
      Mat->PutInStorage(area, age, maxlgrp, matnum, wt / num, TimeInfo);
    }

    for (lgrp = v[i]->maxCol() - 2; lgrp >= v[i]->minCol() + Lgrowth.Nrow() - 1; lgrp--) {
      num = 0.0;
      wt = 0.0;
      matnum = 0.0;
      for (grow = 0; grow < Lgrowth.Nrow(); grow++) {
        tmp = Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;
        ratio = Mat->MaturationProbability(age, lgrp, grow, TimeInfo, area, (*v[i])[lgrp - grow].W);
        matnum += (tmp * ratio);
        num += tmp;
        wt += tmp * ((*v[i])[lgrp - grow].W + Wgrowth[grow][lgrp - grow]);
      }

      if ((isZero(num)) || (wt < verysmall)) {
        //no fish grow to this length cell
        (*v[i])[lgrp].W = 0.0;
        (*v[i])[lgrp].N = 0.0;
        Mat->PutInStorage(area, age, lgrp, 0.0, 0.0, TimeInfo);
      } else if (num - matnum < verysmall) {
        //all the fish that grow to this length cell mature
        (*v[i])[lgrp].W = 0.0;
        (*v[i])[lgrp].N = 0.0;
        Mat->PutInStorage(area, age, lgrp, num, wt / num, TimeInfo);
      } else if (isZero(matnum)) {
        //none of the fish that grow to this length cell mature
        (*v[i])[lgrp].W = wt / num;
        (*v[i])[lgrp].N = num;
        Mat->PutInStorage(area, age, lgrp, 0.0, 0.0, TimeInfo);
      } else {
        (*v[i])[lgrp].W = wt / num;
        (*v[i])[lgrp].N = num - matnum;
        Mat->PutInStorage(area, age, lgrp, matnum, wt / num, TimeInfo);
      }
    }

    for (lgrp = v[i]->minCol() + Lgrowth.Nrow() - 2; lgrp >= v[i]->minCol(); lgrp--) {
      num = 0.0;
      wt = 0.0;
      matnum = 0.0;
      for (grow = 0; grow <= lgrp - v[i]->minCol(); grow++) {
        tmp = Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;
        ratio = Mat->MaturationProbability(age, lgrp, grow, TimeInfo, area, (*v[i])[lgrp - grow].W);
        matnum += (tmp * ratio);
        num += tmp;
        wt += tmp * ((*v[i])[lgrp - grow].W + Wgrowth[grow][lgrp - grow]);
      }

      if ((isZero(num)) || (wt < verysmall)) {
        //no fish grow to this length cell
        (*v[i])[lgrp].W = 0.0;
        (*v[i])[lgrp].N = 0.0;
        Mat->PutInStorage(area, age, lgrp, 0.0, 0.0, TimeInfo);
      } else if (num - matnum < verysmall) {
        //all the fish that grow to this length cell mature
        (*v[i])[lgrp].W = 0.0;
        (*v[i])[lgrp].N = 0.0;
        Mat->PutInStorage(area, age, lgrp, num, wt / num, TimeInfo);
      } else if (isZero(matnum)) {
        //none of the fish that grow to this length cell mature
        (*v[i])[lgrp].W = wt / num;
        (*v[i])[lgrp].N = num;
        Mat->PutInStorage(area, age, lgrp, 0.0, 0.0, TimeInfo);
      } else {
        (*v[i])[lgrp].W = wt / num;
        (*v[i])[lgrp].N = num - matnum;
        Mat->PutInStorage(area, age, lgrp, matnum, wt / num, TimeInfo);
      }
    }
  }
}

//fleksibest formulation - weight read in from file (should be positive)
void AgeBandMatrix::Grow(const DoubleMatrix& Lgrowth, const DoubleVector& Weight) {
  int i, lgrp, grow, maxlgrp;
  double num;

  for (i = 0; i < nrow; i++) {
    num = 0.0;
    maxlgrp = v[i]->maxCol() - 1;
    for (lgrp = maxlgrp; lgrp >= v[i]->maxCol() - Lgrowth.Nrow(); lgrp--)
      for (grow = v[i]->maxCol() - lgrp - 1; grow < Lgrowth.Nrow(); grow++)
        num += Lgrowth[grow][lgrp] * (*v[i])[lgrp].N;

    if (isZero(num)) {
      (*v[i])[maxlgrp].N = 0.0;
      (*v[i])[maxlgrp].W = 0.0;
    } else {
      (*v[i])[maxlgrp].N = num;
      (*v[i])[maxlgrp].W = Weight[maxlgrp];
    }

    for (lgrp = v[i]->maxCol() - 2; lgrp >= v[i]->minCol() + Lgrowth.Nrow() - 1; lgrp--) {
      num = 0.0;
      for (grow = 0; grow < Lgrowth.Nrow(); grow++)
        num += Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;

      if (isZero(num)) {
        (*v[i])[lgrp].N = 0.0;
        (*v[i])[lgrp].W = 0.0;
      } else {
        (*v[i])[lgrp].N = num;
        (*v[i])[lgrp].W = Weight[lgrp];
      }
    }

    for (lgrp = v[i]->minCol() + Lgrowth.Nrow() - 2; lgrp >= v[i]->minCol(); lgrp--) {
      num = 0.0;
      for (grow = 0; grow <= lgrp - v[i]->minCol(); grow++)
        num += Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;

      if (isZero(num)) {
        (*v[i])[lgrp].N = 0.0;
        (*v[i])[lgrp].W = 0.0;
      } else {
        (*v[i])[lgrp].N = num;
        (*v[i])[lgrp].W = Weight[lgrp];
      }
    }
  }
}

//fleksibest formulation - weight read in from file (should be positive)
//Same program with certain num of fish made mature.
void AgeBandMatrix::Grow(const DoubleMatrix& Lgrowth, const DoubleVector& Weight,
  Maturity* const Mat, const TimeClass* const TimeInfo, const AreaClass* const Area, int area) {

  int i, lgrp, grow, maxlgrp, age;
  double num, matnum, tmp, ratio;

  num = 0.0;
  matnum = 0.0;
  for (i = 0; i < nrow; i++) {
    age = i + minage;
    maxlgrp = v[i]->maxCol() - 1;
    num = 0.0;
    matnum = 0.0;
    for (lgrp = maxlgrp; lgrp >= v[i]->maxCol() - Lgrowth.Nrow(); lgrp--) {
      for (grow = v[i]->maxCol() - lgrp - 1; grow < Lgrowth.Nrow(); grow++) {
        tmp = Lgrowth[grow][lgrp] * (*v[i])[lgrp].N;
        ratio = Mat->MaturationProbability(age, lgrp, grow, TimeInfo, area, (*v[i])[lgrp].W);
        matnum += tmp * ratio;
        num += tmp;
      }
    }

    if (isZero(num)) {
      //no fish grow to this length cell
      (*v[i])[maxlgrp].W = 0.0;
      (*v[i])[maxlgrp].N = 0.0;
      Mat->PutInStorage(area, age, maxlgrp, 0.0, 0.0, TimeInfo);
    } else if (num - matnum < verysmall) {
      //all the fish that grow to this length cell mature
      (*v[i])[maxlgrp].W = 0.0;
      (*v[i])[maxlgrp].N = 0.0;
      Mat->PutInStorage(area, age, maxlgrp, num, Weight[maxlgrp], TimeInfo);
    } else if (isZero(matnum)) {
      //none of the fish that grow to this length cell mature
      (*v[i])[maxlgrp].W = Weight[maxlgrp];
      (*v[i])[maxlgrp].N = num;
      Mat->PutInStorage(area, age, maxlgrp, 0.0, 0.0, TimeInfo);
    } else {
      (*v[i])[maxlgrp].W = Weight[maxlgrp];
      (*v[i])[maxlgrp].N = num - matnum;
      Mat->PutInStorage(area, age, maxlgrp, matnum, Weight[maxlgrp], TimeInfo);
    }

    for (lgrp = v[i]->maxCol() - 2; lgrp >= v[i]->minCol() + Lgrowth.Nrow() - 1; lgrp--) {
      num = 0.0;
      matnum = 0.0;
      for (grow = 0; grow < Lgrowth.Nrow(); grow++) {
        tmp = Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;
        ratio = Mat->MaturationProbability(age, lgrp, grow, TimeInfo, area, (*v[i])[lgrp - grow].W);
        matnum += tmp * ratio;
        num += tmp;
      }

      if (isZero(num)) {
        //no fish grow to this length cell
        (*v[i])[lgrp].W = 0.0;
        (*v[i])[lgrp].N = 0.0;
        Mat->PutInStorage(area, age, lgrp, 0.0, 0.0, TimeInfo);
      } else if (num - matnum < verysmall) {
        //all the fish that grow to this length cell mature
        (*v[i])[lgrp].W = 0.0;
        (*v[i])[lgrp].N = 0.0;
        Mat->PutInStorage(area, age, lgrp, num, Weight[lgrp], TimeInfo);
      } else if (isZero(matnum)) {
        //none of the fish that grow to this length cell mature
        (*v[i])[lgrp].W = Weight[lgrp];
        (*v[i])[lgrp].N = num;
        Mat->PutInStorage(area, age, lgrp, 0.0, 0.0, TimeInfo);
      } else {
        (*v[i])[lgrp].W = Weight[lgrp];
        (*v[i])[lgrp].N = num - matnum;
        Mat->PutInStorage(area, age, lgrp, matnum, Weight[lgrp], TimeInfo);
      }
    }

    for (lgrp = v[i]->minCol() + Lgrowth.Nrow() - 2; lgrp >= v[i]->minCol(); lgrp--) {
      num = 0.0;
      matnum = 0.0;
      for (grow = 0; grow <= lgrp - v[i]->minCol(); grow++) {
        tmp = Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;
        ratio = Mat->MaturationProbability(age, lgrp, grow, TimeInfo, area, (*v[i])[lgrp - grow].W);
        matnum += tmp * ratio;
        num += tmp;
      }

      if (isZero(num)) {
        //no fish grow to this length cell
        (*v[i])[lgrp].W = 0.0;
        (*v[i])[lgrp].N = 0.0;
        Mat->PutInStorage(area, age, lgrp, 0.0, 0.0, TimeInfo);
      } else if (num - matnum < verysmall) {
        //all the fish that grow to this length cell mature
        (*v[i])[lgrp].W = 0.0;
        (*v[i])[lgrp].N = 0.0;
        Mat->PutInStorage(area, age, lgrp, num, Weight[lgrp], TimeInfo);
      } else if (isZero(matnum)) {
        //none of the fish that grow to this length cell mature
        (*v[i])[lgrp].W = Weight[lgrp];
        (*v[i])[lgrp].N = num;
        Mat->PutInStorage(area, age, lgrp, 0.0, 0.0, TimeInfo);
      } else {
        (*v[i])[lgrp].W = Weight[lgrp];
        (*v[i])[lgrp].N = num - matnum;
        Mat->PutInStorage(area, age, lgrp, matnum, Weight[lgrp], TimeInfo);
      }
    }
  }
}
