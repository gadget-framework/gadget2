#include "maturity.h"

//Upgrade growth.  Input to program are two rectangular matrices
//Lgrowth describes the ratio of each length group that grows certain
//number of length groups.  Wgrowth describes the weight gain of
//Each element in Lgrowth.

void Agebandmatrix::Grow(const doublematrix& Lgrowth, const doublematrix& Wgrowth) {
  int i, lgrp, grow, maxlgrp;
  double number, wt, upfj;

  //'age' is age,
  //'lgrp' is number of length group,
  //'maxlgrp' is the largest possible number of length group for a given age,
  //'grow' is growth, measured in number of length groups,
  //'i' is shifted age, i.e. age - minage,
  //'number' is just number,
  //'wt' is weight (i.e. mean weight).
  //And in the loops, 'upfj' is just a dummy variable.

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
        /*JMB code removed from here - see RemovedCode.txt for details*/
      }
    }
    if (number > 0)
      (*v[i])[maxlgrp].W = wt / number;

    (*v[i])[maxlgrp].N = number;

    //The center part of the length division
    for (lgrp = v[i]->Maxcol() - 2; lgrp >= v[i]->Mincol() + Lgrowth.Nrow() - 1; lgrp--) {
      number = 0.0;
      wt = 0.0;
      for (grow = 0; grow < Lgrowth.Nrow(); grow++) {
        upfj = Lgrowth[grow][lgrp - grow] * (*v[i])[lgrp - grow].N;
        number += upfj;
        wt += upfj * ((*v[i])[lgrp - grow].W + Wgrowth[grow][lgrp - grow]);
      }
      if (number > 0)
        (*v[i])[lgrp].W = wt / number;
  
      (*v[i])[lgrp].N = number;
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
      if (number > 0)
        (*v[i])[lgrp].W = wt / number;

      (*v[i])[lgrp].N = number;
    }
  }
}

//Same program with certain number of fish made mature.
void Agebandmatrix::Grow(const doublematrix& Lgrowth, const doublematrix& Wgrowth,
  Maturity* const Mat, const TimeClass* const TimeInfo, const AreaClass* const Area,
  const LengthGroupDivision* const GivenLDiv, int area) {

  int i, lgrp, grow, maxlgrp, age;
  double number, wt, matnum, upfj;

  //'age' is age,
  //'lgrp' is number of length group,
  //'maxlgrp' is the largest possible number of length group for a given age,
  //'grow' is growth, measured in number of length groups,
  //'i' is shifted age, i.e. age - minage,
  //'matnum' is the number that becomes mature,
  //'number' is just number,
  //'wt' is weight (i.e. mean weight).
  //And in the loops, 'upfj' is just a dummy variable.

  number = 0.0;
  wt = 0.0;
  matnum = 0.0;
  for (i = 0; i < nrow; i++) {
    age = i + minage;
    maxlgrp = v[i]->Maxcol() - 1;

    //The part that grows to or above the highest length group.
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
    if (number > 0) {
      wt /= number;
      (*v[i])[maxlgrp].W = wt;
    }

    (*v[i])[maxlgrp].N = number - matnum;
    if (wt > 0 && matnum > 0)
      Mat->PutInStorage(area, age, maxlgrp, matnum, wt, TimeInfo);
    else
      Mat->PutInStorage(area, age, maxlgrp, 0.0, 0.0, TimeInfo);

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
      if (number > 0) {
        wt /= number;
        (*v[i])[lgrp].W = wt;
      }

      (*v[i])[lgrp].N = number - matnum;
      if (wt > 0 && matnum > 0)
        Mat->PutInStorage(area, age, lgrp, matnum, wt, TimeInfo);
      else
        Mat->PutInStorage(area, age, lgrp, 0.0, 0.0, TimeInfo);
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
      if (number > 0) {
        wt /= number;
        (*v[i])[lgrp].W = wt;
      }

      (*v[i])[lgrp].N = number - matnum;
      if (wt > 0 && matnum > 0)
        Mat->PutInStorage(area, age, lgrp, matnum, wt, TimeInfo);
      else
        Mat->PutInStorage(area, age, lgrp, 0.0, 0.0, TimeInfo);
    }
  }
}

void Agebandmatrix::Grow(const doublematrix& Lgrowth, const doublevector& Weight) {
  int i, lgrp, grow, maxlgrp;
  double number, upfj;

  //'age' is age,
  //'lgrp' is number of length group,
  //'maxlgrp' is the largest possible number of length group for a given age,
  //'grow' is growth, measured in number of length groups,
  //'i' is shifted age, i.e. age - minage,
  //'number' is just number,
  //And in the loops, 'upfj' is just a dummy variable.

  number = 0.0;
  for (i = 0; i < nrow; i++) {
    //The part that grows to or above the highest length group.
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

    //The center part of the length division
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

    //The lowest part of the length division.
    for (lgrp = v[i]->Mincol()+Lgrowth.Nrow() - 2; lgrp >= v[i]->Mincol(); lgrp--) {
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

//Same program with certain number of fish made mature.
void Agebandmatrix::Grow(const doublematrix& Lgrowth, const doublevector& Weight,
  Maturity* const Mat, const TimeClass* const TimeInfo, const AreaClass* const Area,
  const LengthGroupDivision* const GivenLDiv, int area) {

  int i, lgrp, grow, maxlgrp, age;
  double number, matnum, upfj;

  //'age' is age,
  //'lgrp' is number of length group,
  //'maxlgrp' is the largest possible number of length group for a given age,
  //'grow' is growth, measured in number of length groups,
  //'i' is shifted age, i.e. age - minage,
  //'matnum' is the number that becomes mature,
  //'number' is just number,
  //And in the loops, 'upfj' is just a dummy variable.

  number = 0.0;
  matnum = 0.0;
  for (i = 0; i < nrow; i++) {
    age = i + minage;
    maxlgrp = v[i]->Maxcol() - 1;

    //The part that grows to or above the highest length group.
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
