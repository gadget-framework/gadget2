#include "maturity.h"
#include "agebandmatrixratio.h"
#include "gadget.h"

void Agebandmatrixratio::Grow(const doublematrix& Lgrowth, const Agebandmatrix& Total, const int NrOfTagExp) {
  int i, lgrp, grow, maxlgrp, tag;
  double upfj;
  doublevector number(NrOfTagExp, 0.0);

  //'age' is age,
  //'lgrp' is number of length group,
  //'maxlgrp' is the largest possible number of length group for a given age,
  //'grow' is growth, measured in number of length groups,
  //'i' is shifted age, i.e. age - minage,
  //'number' is just number,
  //And in the loops, 'upfj' is just a dummy variable.

  for (i = 0; i < nrow; i++) {
    //The part that grows to or above the highest length group.
    maxlgrp = v[i]->Maxcol() -1;
    for (tag = 0; tag < NrOfTagExp; tag++)
      number[tag] = 0.0;

    for (lgrp = maxlgrp; lgrp >= v[i]->Maxcol() - Lgrowth.Nrow(); lgrp--) {
      for (grow = v[i]->Maxcol() - lgrp - 1; grow < Lgrowth.Nrow(); grow++) {
        for (tag = 0; tag < NrOfTagExp; tag++) {
          upfj = Lgrowth[grow][lgrp] * (*(*v[i])[lgrp][tag].N);
          number[tag] += upfj;
        }
      }
    }

    for (tag = 0; tag < NrOfTagExp; tag++)
      (*(*v[i])[maxlgrp][tag].N) = number[tag];

    //The center part of the length division
    for (lgrp = v[i]->Maxcol() - 2; lgrp >= v[i]->Mincol() + Lgrowth.Nrow() - 1; lgrp--) {
      for (tag = 0; tag < NrOfTagExp; tag++)
        number[tag] = 0.0;

      for (grow = 0; grow < Lgrowth.Nrow(); grow++) {
        for (tag = 0; tag < NrOfTagExp; tag++) {
          upfj = Lgrowth[grow][lgrp - grow] * (*(*v[i])[lgrp - grow][tag].N);
          number[tag] += upfj;
        }
      }

      for (tag = 0; tag < NrOfTagExp; tag++)
        (*(*v[i])[lgrp][tag].N) = number[tag];
    }

    //The lowest part of the length division.
    for (lgrp = v[i]->Mincol() + Lgrowth.Nrow() - 2; lgrp >= v[i]->Mincol(); lgrp--) {
      for (tag = 0; tag < NrOfTagExp; tag++)
        number[tag] = 0.0;

      for (grow = 0; grow <= lgrp - v[i]->Mincol(); grow++) {
        for (tag = 0; tag < NrOfTagExp; tag++) {
          upfj = Lgrowth[grow][lgrp - grow] * (*(*v[i])[lgrp - grow][tag].N);
          number[tag] += upfj;
        }
      }

      for (tag = 0; tag < NrOfTagExp; tag++)
        (*(*v[i])[lgrp][tag].N) = number[tag];
    }
  }
  this->UpdateRatio(Total, NrOfTagExp);
}

//Same program with certain number of fish made mature.
void Agebandmatrixratio::Grow(const doublematrix& Lgrowth, const Agebandmatrix& Total,
  Maturity* const Mat, const TimeClass* const TimeInfo, const AreaClass* const Area,
  const LengthGroupDivision* const GivenLDiv, int area, const int NrOfTagExp) {

  int i, lgrp, grow, maxlgrp, age, tag;
  double upfj;

  doublevector number(NrOfTagExp, 0.0);
  doublevector matnum(NrOfTagExp, 0.0);

  //'age' is age,
  //'lgrp' is number of length group,
  //'maxlgrp' is the largest possible number of length group for a given age,
  //'grow' is growth, measured in number of length groups,
  //'i' is shifted age, i.e. age - minage,
  //'matnum' is the number that becomes mature,
  //'number' is just number,
  //And in the loops, 'upfj' is just a dummy variable.

  for (i = 0; i < nrow; i++) {
    age = i + minage;
    maxlgrp = v[i]->Maxcol() - 1;
    for (tag = 0; tag < NrOfTagExp; tag++) {
      number[tag] = 0.0;
      matnum[tag] = 0.0;
    }

    //The part that grows to or above the highest length group.
    for (lgrp = maxlgrp; lgrp >= v[i]->Maxcol() - Lgrowth.Nrow(); lgrp--) {
      for (grow = v[i]->Maxcol() - lgrp - 1; grow < Lgrowth.Nrow(); grow++) {
        const double ratio =  Mat->MaturationProbability(age, lgrp, grow, TimeInfo, Area, area);
        for (tag = 0; tag < NrOfTagExp; tag++) {
          upfj = Lgrowth[grow][lgrp] * (*(*v[i])[lgrp][tag].N);
          matnum[tag] += upfj * ratio;
          number[tag] += upfj;
        }
      }
    }

    for (tag = 0; tag < NrOfTagExp; tag++) {
      (*(*v[i])[maxlgrp][tag].N) = number[tag] - matnum[tag];

      if (Total[i + minage][maxlgrp].W > 0 && matnum[tag] > 0)
        Mat->PutInStorage(area, age, maxlgrp, matnum[tag], Total[i + minage][maxlgrp].W, TimeInfo);
      else
        Mat->PutInStorage(area, age, maxlgrp, 0.0, 0.0, TimeInfo);
    }

    //The center part of the length division
    for (lgrp = v[i]->Maxcol() - 2; lgrp >= v[i]->Mincol() + Lgrowth.Nrow() - 1; lgrp--) {
      for (tag = 0; tag < NrOfTagExp; tag++) {
        number[tag] = 0.0;
        matnum[tag] = 0.0;
      }

      for (grow = 0; grow < Lgrowth.Nrow(); grow++) {
        const double ratio = Mat->MaturationProbability(age, lgrp, grow, TimeInfo, Area, area);
        for (tag = 0; tag < NrOfTagExp; tag++) {
          upfj = Lgrowth[grow][lgrp - grow] * (*(*v[i])[lgrp - grow][tag].N);
          matnum[tag] += upfj * ratio;
          number[tag] += upfj;
        }
      }

      for (tag = 0; tag < NrOfTagExp; tag++) {
        (*(*v[i])[lgrp][tag].N) = number[tag] - matnum[tag];

        if (Total[i + minage][lgrp].W > 0 && matnum[tag] > 0)
          Mat->PutInStorage(area, age, lgrp, matnum[tag], Total[i + minage][lgrp].W, TimeInfo);
        else
          Mat->PutInStorage(area, age, lgrp, 0.0, 0.0, TimeInfo);
      }
    }

    //The lowest part of the length division.
    for (lgrp = v[i]->Mincol() + Lgrowth.Nrow() - 2; lgrp >= v[i]->Mincol(); lgrp--) {
      for (tag = 0; tag < NrOfTagExp; tag++) {
        number[tag] = 0.0;
        matnum[tag] = 0.0;
      }

      for (grow = 0; grow <= lgrp - v[i]->Mincol(); grow++) {
        const double ratio = Mat->MaturationProbability(age, lgrp, grow, TimeInfo, Area, area);
        for (tag = 0; tag < NrOfTagExp; tag++) {
          upfj = Lgrowth[grow][lgrp - grow] * (*(*v[i])[lgrp - grow][tag].N);
          matnum[tag] += upfj * ratio;
          number[tag] += upfj;
        }
      }

      for (tag = 0; tag < NrOfTagExp; tag++) {
        (*(*v[i])[lgrp][tag].N) = number[tag] - matnum[tag];
        if (Total[i + minage][lgrp].W > 0 && matnum[tag] > 0)
          Mat->PutInStorage(area, age, lgrp, matnum[tag], Total[i + minage][lgrp].W, TimeInfo);
        else
          Mat->PutInStorage(area, age, lgrp, 0.0, 0.0, TimeInfo);
      }
    }
  }
  this->UpdateRatio(Total, NrOfTagExp);
}

