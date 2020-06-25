#include "maturity.h"
#include "grower.h"
#include "agebandmatrix.h"
#include "gadget.h"

/* Update the agebandmatrix to reflect the calculated growth  */
/* Lgrowth contains the ratio of each length group that grows */
/* by a certain number of length groups, and Wgrowth contains */
/* the weight increase for each entry in Lgrowth              */

/* JMB changed to deal with very small weights a bit better   */
void AgeBandMatrix::Grow(const DoubleMatrix& Lgrowth, const DoubleMatrix& Wgrowth) {
  int i, lgrp, grow, maxlgrp;
  double num, wt, tmp;

  maxlgrp = Lgrowth.Nrow();
  for (i = 0; i < nrow; i++) {
    //the part that grows to or above the highest length group
    num = 0.0;
    wt = 0.0;
    for (lgrp = v[i]->maxCol() - 1; lgrp >= v[i]->maxCol() - maxlgrp; lgrp--) {
      for (grow = v[i]->maxCol() - lgrp - 1; grow < maxlgrp; grow++) {
        tmp = Lgrowth[grow][lgrp] * (*v[i])[lgrp].N;
        num += tmp;
        wt += tmp * (Wgrowth[grow][lgrp] + (*v[i])[lgrp].W);
      }
    }

    lgrp = v[i]->maxCol() - 1;
    if (isZero(num) || (wt < verysmall)) {
      (*v[i])[lgrp].setToZero();
    } else {
      (*v[i])[lgrp].W = wt / num;
      (*v[i])[lgrp].N = num;
    }

    //the central diagonal part of the length division
    for (lgrp = v[i]->maxCol() - 2; lgrp >= v[i]->minCol() + maxlgrp - 1; lgrp--) {
      num = 0.0;
      wt = 0.0;
      for (grow = 0; grow < maxlgrp; grow++) {
        tmp = Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;
        num += tmp;
        wt += tmp * (Wgrowth[grow][lgrp - grow] + (*v[i])[lgrp - grow].W);
      }

      if (isZero(num) || (wt < verysmall)) {
        (*v[i])[lgrp].setToZero();
      } else {
        (*v[i])[lgrp].W = wt / num;
        (*v[i])[lgrp].N = num;
      }
    }

    //the lowest part of the length division
    for (lgrp = v[i]->minCol() + maxlgrp - 2; lgrp >= v[i]->minCol(); lgrp--) {
      num = 0.0;
      wt = 0.0;
      for (grow = 0; grow <= lgrp - v[i]->minCol(); grow++) {
        tmp = Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;
        num += tmp;
        wt += tmp * (Wgrowth[grow][lgrp - grow] + (*v[i])[lgrp - grow].W);
      }

      if (isZero(num) || (wt < verysmall)) {
        (*v[i])[lgrp].setToZero();
      } else {
        (*v[i])[lgrp].W = wt / num;
        (*v[i])[lgrp].N = num;
      }
    }
  }
}

//Same program with certain num of fish made mature.
void AgeBandMatrix::Grow(const DoubleMatrix& Lgrowth, const DoubleMatrix& Wgrowth, Maturity* const Mat, int area) {

  int i, lgrp, grow, maxlgrp, age;
  double num, wt, matnum, tmp, ratio;

  maxlgrp = Lgrowth.Nrow();  // # of length groups considered for growth
  for (i = 0; i < nrow; i++) {  // For each age index...
    // Gather growths into maximum length group
    age = i + minage;
    num = 0.0; // New total of indivduals in target length group (including individuals that didn't grow)
    wt = 0.0;  // Total weight of target length group
    matnum = 0.0;  // Total maturing individuals in this length group post-growth
    for (lgrp = v[i]->maxCol() - 1; lgrp >= v[i]->maxCol() - maxlgrp; lgrp--) { // lgrp = Count backwards from portential source length groups
      for (grow = v[i]->maxCol() - lgrp - 1; grow < maxlgrp; grow++) {  // grow = # of length groups grown by that will result in ending up here (NB: it's a plus group, and includes 0-growth)
        ratio = Mat->calcMaturation(age, lgrp, grow, (*v[i])[lgrp].W);  // Ratio of lgrp -> grow that will mature
        tmp = Lgrowth[grow][lgrp] * (*v[i])[lgrp].N;  // Individuals that will grow from lgrp by grow into max length group
        matnum += (tmp * ratio);
        num += tmp;
        wt += tmp * (Wgrowth[grow][lgrp] + (*v[i])[lgrp].W);
      }
    }

    // File gathered growth away in maximum length group
    lgrp = v[i]->maxCol() - 1;
    if (isZero(num) || (wt < verysmall)) {
      //no fish grow to this length cell
      (*v[i])[lgrp].setToZero();
      Mat->storeMatureStock(area, age, lgrp, 0.0, 0.0);
    } else if (isZero(matnum)) {
      //none of the fish that grow to this length cell mature
      (*v[i])[lgrp].W = wt / num;
      (*v[i])[lgrp].N = num;
      Mat->storeMatureStock(area, age, lgrp, 0.0, 0.0);
    } else if (isEqual(num, matnum) || (matnum > num)) {
      //all the fish that grow to this length cell mature
      (*v[i])[lgrp].setToZero();
      Mat->storeMatureStock(area, age, lgrp, num, wt / num);
    } else {
      (*v[i])[lgrp].W = wt / num;
      (*v[i])[lgrp].N = num - matnum;
      Mat->storeMatureStock(area, age, lgrp, matnum, wt / num);
    }

    // Consider Growths into smaller length groups, but above maxlgrp (i.e. won't fall off bottom when looking for source groups)
    for (lgrp = v[i]->maxCol() - 2; lgrp >= v[i]->minCol() + maxlgrp - 1; lgrp--) {  // For each target length group...
      num = 0.0; // New total of indivduals in target length group (including individuals that didn't grow)
      wt = 0.0;   // Total weight of target length group
      matnum = 0.0;  // Total maturing individuals in this length group post-growth
      for (grow = 0; grow < maxlgrp; grow++) {  // grow = all possible growth amounts (NB: including 0)
        ratio = Mat->calcMaturation(age, lgrp, grow, (*v[i])[lgrp - grow].W);
        tmp = Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;  // Individuals that will grow from (lgrp - grow) by grow into target lgrp
        matnum += (tmp * ratio);
        num += tmp;
        wt += tmp * (Wgrowth[grow][lgrp - grow] + (*v[i])[lgrp - grow].W);
      }

      if (isZero(num) || (wt < verysmall)) {
        //no fish grow to this length cell
        (*v[i])[lgrp].setToZero();
        Mat->storeMatureStock(area, age, lgrp, 0.0, 0.0);
      } else if (isZero(matnum)) {
        //none of the fish that grow to this length cell mature
        (*v[i])[lgrp].W = wt / num;
        (*v[i])[lgrp].N = num;
        Mat->storeMatureStock(area, age, lgrp, 0.0, 0.0);
      } else if (isEqual(num, matnum) || (matnum > num)) {
        //all the fish that grow to this length cell mature
        (*v[i])[lgrp].setToZero();
        Mat->storeMatureStock(area, age, lgrp, num, wt / num);
      } else {
        (*v[i])[lgrp].W = wt / num;
        (*v[i])[lgrp].N = num - matnum;
        Mat->storeMatureStock(area, age, lgrp, matnum, wt / num);
      }
    }

    // target lengthgroups below maxlgrp, make sure we don't consider growth outside our range
    for (lgrp = v[i]->minCol() + maxlgrp - 2; lgrp >= v[i]->minCol(); lgrp--) {
      num = 0.0;
      wt = 0.0;
      matnum = 0.0;
      for (grow = 0; grow <= lgrp - v[i]->minCol(); grow++) {
        ratio = Mat->calcMaturation(age, lgrp, grow, (*v[i])[lgrp - grow].W);
        tmp = Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;
        matnum += (tmp * ratio);
        num += tmp;
        wt += tmp * (Wgrowth[grow][lgrp - grow] + (*v[i])[lgrp - grow].W);
      }

      if (isZero(num) || (wt < verysmall)) {
        //no fish grow to this length cell
        (*v[i])[lgrp].setToZero();
        Mat->storeMatureStock(area, age, lgrp, 0.0, 0.0);
      } else if (isZero(matnum)) {
        //none of the fish that grow to this length cell mature
        (*v[i])[lgrp].W = wt / num;
        (*v[i])[lgrp].N = num;
        Mat->storeMatureStock(area, age, lgrp, 0.0, 0.0);
      } else if (isEqual(num, matnum) || (matnum > num)) {
        //all the fish that grow to this length cell mature
        (*v[i])[lgrp].setToZero();
        Mat->storeMatureStock(area, age, lgrp, num, wt / num);
      } else {
        (*v[i])[lgrp].W = wt / num;
        (*v[i])[lgrp].N = num - matnum;
        Mat->storeMatureStock(area, age, lgrp, matnum, wt / num);
      }
    }
  }
}

//fleksibest formulation - weight read in from file (should be positive)
void AgeBandMatrix::Grow(const DoubleMatrix& Lgrowth, const DoubleVector& Weight) {
  int i, lgrp, grow, maxlgrp;
  double num;

  maxlgrp = Lgrowth.Nrow();
  for (i = 0; i < nrow; i++) {
    num = 0.0;
    for (lgrp = v[i]->maxCol() - 1; lgrp >= v[i]->maxCol() - maxlgrp; lgrp--)
      for (grow = v[i]->maxCol() - lgrp - 1; grow < maxlgrp; grow++)
        num += (Lgrowth[grow][lgrp] * (*v[i])[lgrp].N);

    lgrp = v[i]->maxCol() - 1;
    if (isZero(num)) {
      (*v[i])[lgrp].setToZero();
    } else {
      (*v[i])[lgrp].N = num;
      (*v[i])[lgrp].W = Weight[lgrp];
    }

    for (lgrp = v[i]->maxCol() - 2; lgrp >= v[i]->minCol() + maxlgrp - 1; lgrp--) {
      num = 0.0;
      for (grow = 0; grow < maxlgrp; grow++)
        num += (Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N);

      if (isZero(num)) {
        (*v[i])[lgrp].setToZero();
      } else {
        (*v[i])[lgrp].N = num;
        (*v[i])[lgrp].W = Weight[lgrp];
      }
    }

    for (lgrp = v[i]->minCol() + maxlgrp - 2; lgrp >= v[i]->minCol(); lgrp--) {
      num = 0.0;
      for (grow = 0; grow <= lgrp - v[i]->minCol(); grow++)
        num += (Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N);

      if (isZero(num)) {
        (*v[i])[lgrp].setToZero();
      } else {
        (*v[i])[lgrp].N = num;
        (*v[i])[lgrp].W = Weight[lgrp];
      }
    }
  }
}

//fleksibest formulation - weight read in from file (should be positive)
//Same program with certain num of fish made mature.
void AgeBandMatrix::Grow(const DoubleMatrix& Lgrowth, const DoubleVector& Weight, Maturity* const Mat, int area) {

  int i, lgrp, grow, maxlgrp, age;
  double num, matnum, tmp, ratio;

  maxlgrp = Lgrowth.Nrow();
  for (i = 0; i < nrow; i++) {
    age = i + minage;
    num = 0.0;
    matnum = 0.0;
    for (lgrp = v[i]->maxCol() - 1; lgrp >= v[i]->maxCol() - maxlgrp; lgrp--) {
      for (grow = v[i]->maxCol() - lgrp - 1; grow < maxlgrp; grow++) {
        ratio = Mat->calcMaturation(age, lgrp, grow, (*v[i])[lgrp].W);
        tmp = Lgrowth[grow][lgrp] * (*v[i])[lgrp].N;
        matnum += (tmp * ratio);
        num += tmp;
      }
    }

    lgrp = v[i]->maxCol() - 1;
    if (isZero(num)) {
      //no fish grow to this length cell
      (*v[i])[lgrp].setToZero();
      Mat->storeMatureStock(area, age, lgrp, 0.0, 0.0);
    } else if (isZero(matnum)) {
      //none of the fish that grow to this length cell mature
      (*v[i])[lgrp].W = Weight[lgrp];
      (*v[i])[lgrp].N = num;
      Mat->storeMatureStock(area, age, lgrp, 0.0, 0.0);
    } else if (isEqual(num, matnum) || (matnum > num)) {
      //all the fish that grow to this length cell mature
      (*v[i])[lgrp].setToZero();
      Mat->storeMatureStock(area, age, lgrp, num, Weight[lgrp]);
    } else {
      (*v[i])[lgrp].W = Weight[lgrp];
      (*v[i])[lgrp].N = num - matnum;
      Mat->storeMatureStock(area, age, lgrp, matnum, Weight[lgrp]);
    }

    for (lgrp = v[i]->maxCol() - 2; lgrp >= v[i]->minCol() + maxlgrp - 1; lgrp--) {
      num = 0.0;
      matnum = 0.0;
      for (grow = 0; grow < maxlgrp; grow++) {
        ratio = Mat->calcMaturation(age, lgrp, grow, (*v[i])[lgrp - grow].W);
        tmp = Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;
        matnum += (tmp * ratio);
        num += tmp;
      }

      if (isZero(num)) {
        //no fish grow to this length cell
        (*v[i])[lgrp].setToZero();
        Mat->storeMatureStock(area, age, lgrp, 0.0, 0.0);
      } else if (isZero(matnum)) {
        //none of the fish that grow to this length cell mature
        (*v[i])[lgrp].W = Weight[lgrp];
        (*v[i])[lgrp].N = num;
        Mat->storeMatureStock(area, age, lgrp, 0.0, 0.0);
      } else if (isEqual(num, matnum) || (matnum > num)) {
        //all the fish that grow to this length cell mature
        (*v[i])[lgrp].setToZero();
        Mat->storeMatureStock(area, age, lgrp, num, Weight[lgrp]);
      } else {
        (*v[i])[lgrp].W = Weight[lgrp];
        (*v[i])[lgrp].N = num - matnum;
        Mat->storeMatureStock(area, age, lgrp, matnum, Weight[lgrp]);
      }
    }

    for (lgrp = v[i]->minCol() + maxlgrp - 2; lgrp >= v[i]->minCol(); lgrp--) {
      num = 0.0;
      matnum = 0.0;
      for (grow = 0; grow <= lgrp - v[i]->minCol(); grow++) {
        ratio = Mat->calcMaturation(age, lgrp, grow, (*v[i])[lgrp - grow].W);
        tmp = Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;
        matnum += (tmp * ratio);
        num += tmp;
      }

      if (isZero(num)) {
        //no fish grow to this length cell
        (*v[i])[lgrp].setToZero();
        Mat->storeMatureStock(area, age, lgrp, 0.0, 0.0);
      } else if (isZero(matnum)) {
        //none of the fish that grow to this length cell mature
        (*v[i])[lgrp].W = Weight[lgrp];
        (*v[i])[lgrp].N = num;
        Mat->storeMatureStock(area, age, lgrp, 0.0, 0.0);
      } else if (isEqual(num, matnum) || (matnum > num)) {
        //all the fish that grow to this length cell mature
        (*v[i])[lgrp].setToZero();
        Mat->storeMatureStock(area, age, lgrp, num, Weight[lgrp]);
      } else {
        (*v[i])[lgrp].W = Weight[lgrp];
        (*v[i])[lgrp].N = num - matnum;
        Mat->storeMatureStock(area, age, lgrp, matnum, Weight[lgrp]);
      }
    }
  }
}
