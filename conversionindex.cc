#include "conversionindex.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

ConversionIndex::ConversionIndex(const LengthGroupDivision* const L1,
  const LengthGroupDivision* const L2, int interp) {

  int i, j, k, nc, nf;
  const LengthGroupDivision* Lf;  //will be the finer length group division
  const LengthGroupDivision* Lc;  //will be the coarser length group division
  double tmpmin = max(L1->minLength(), L2->minLength());
  double tmpmax = min(L1->maxLength(), L2->maxLength());

  //check to see if the intersection is empty
  if ((L1->numLengthGroups() == 0) || (L2->numLengthGroups() == 0) ||
       (tmpmin > tmpmax) || isZero(tmpmin - tmpmax)) {

    handle.logMessage(LOGWARN, "Error in conversionindex - intersection between length groups is empty");
    L1->printError();
    L2->printError();
    exit(EXIT_FAILURE);
  }

  samedl = 0;
  offset = 0;
  interpolate = interp;
  //targetisfiner means that L1 is strictly coarser than L2
  if (isZero(L1->dl()) || isZero(L2->dl())) {
    checkLengthGroupIsFiner(L1, L2);
    targetisfiner = 0;
    Lc = L2;
    Lf = L1;

  } else {
    if (L1->dl() > L2->dl()) {
      targetisfiner = 1;
      Lc = L1;
      Lf = L2;
    } else {
      targetisfiner = 0;
      Lc = L2;
      Lf = L1;
    }
    if (isZero(Lf->dl() - Lc->dl())) {
      samedl = 1;
      offset = int((Lf->meanLength(0) - Lc->meanLength(0)) / Lf->dl());
    }
  }

  nf = Lf->numLengthGroups();
  nc = Lc->numLengthGroups();
  //set minlength and maxlength
  for (i = 0; i < nf; i++)
    if (Lf->minLength(i) > Lc->minLength() || isZero(Lf->minLength(i) - Lc->minLength())) {
      minlength = i;
      break;
    }

  for (i = nf - 1; i >= 0; i--)
    if (Lf->maxLength(i) < Lc->maxLength() || isZero(Lf->maxLength(i) - Lc->maxLength())) {
      maxlength = i + 1;
      break;
    }

  k = 0;
  pos.resize(nf, 0);
  for (i = minlength; i < maxlength; i++)
    for (j = k; j < nc; j++)
      if (Lf->meanLength(i) > Lc->minLength(j) && Lf->meanLength(i) < Lc->maxLength(j)) {
        pos[i] = j;
        k = j;
        break;
      }

  for (i = maxlength; i < nf; i++)
    pos[i] = nc;

  //if minpos and maxpos are needed
  if (samedl == 0) {
    minpos.resize(nc, nf - 1); //initialised to Lf->Size() - 1
    for (i = minlength; i < maxlength; i++)
      if (i < minpos[pos[i]])
        minpos[pos[i]] = i;

    for (i = nc - 1; i > 0; i--)
      if (minpos[i - 1] > minpos[i])
        minpos[i - 1] = minpos[i];

    maxpos.resize(nc, 0);
    for (i = minlength; i < maxlength; i++)
      if (i > maxpos[pos[i]])
        maxpos[pos[i]] = i;

    for (i = 0; i < nc - 1;i++)
      if (maxpos[i + 1] < maxpos[i])
        maxpos[i + 1] = maxpos[i];

    //if number in each length group is needed
    if (targetisfiner == 1) {
      nrof.resize(nf, 0);
      for (i = minlength; i < maxlength; i++)
        nrof[i] = maxpos[pos[i]] - minpos[pos[i]] + 1;
    }
  }

  //if the conversionindex is to be used for interpolation
  if (interpolate == 1) {
    interpratio.resize(nf);
    interppos.resize(nf, -1);
    k = 0;
    for (i = minlength; i < maxlength; i++)
      for (j = k; j < nc - 1; j++)
        if (Lf->meanLength(i) >= Lc->meanLength(j) && Lf->meanLength(i) < Lc->meanLength(j + 1)) {
          interppos[i] = j;
          k = j;
          break;
        }

    for (i = 0; i < nf; i++)
      if (interppos[i] != -1)
        interpratio[i] = (Lf->meanLength(i) - Lc->meanLength(interppos[i])) /
          (Lc->meanLength(interppos[i] + 1) - Lc->meanLength(interppos[i]));
      else {
        interpratio[i] = -1.0; //-1 when outside range
        if (Lf->meanLength(i) < Lc->meanLength(0))
          interppos[i] = 0;
        else
          interppos[i] = nc - 1;
      }
  }
}

//The function interpolates values calculated on a coarse length distribution
//Vc to a finer length distribution Vf using the conversionindex CI
void ConversionIndex::interpolateLengths(DoubleVector& Vf, const DoubleVector& Vc) {

  if (!(interpolate))
    handle.logMessage(LOGFAIL, "Error in conversionindex - cannot interpolate between lengthgroups");

  int i;
  if (samedl) {
    if (minlength > 0)
      for (i = 0; i < minlength; i++)
        Vf[i] = Vc[0];

    for (i = minlength; i < maxlength; i++)
      Vf[i] = Vc[i + offset];

    if (maxlength < Vf.Size())
      for (i = maxlength; i < Vf.Size(); i++)
        Vf[i] = Vc[Vc.Size() - 1];

  } else {
    for (i = 0; i < Vf.Size(); i++) {
      if (isZero(interpratio[i] + 1.0))
        Vf[i] = Vc[interppos[i]];
      else
        Vf[i] = Vc[interppos[i]] * (1.0 - interpratio[i]) + Vc[interppos[i] + 1] * interpratio[i];
    }
  }
}
