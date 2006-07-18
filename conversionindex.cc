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

  error = 0;
  samedl = 0;
  offset = 0;
  targetisfiner = 0;  //targetisfiner means that L1 is strictly coarser than L2
  interpolate = interp;

  //check to see if the intersection is empty
  if ((tmpmin > tmpmax) || isEqual(tmpmin, tmpmax)) {
    handle.logMessage(LOGWARN, "Error when checking length structure - empty intersection");
    error = 1;
    return;
  }

  if (isZero(L1->dl()) || isZero(L2->dl())) {
    if (checkLengthGroupStructure(L1, L2)) {
      error = 1;
      return;
    }
    Lc = L2;
    Lf = L1;

  } else {
    if (isEqual(L1->dl(), L2->dl())) {
      Lf = L1;
      Lc = L2;
      //JMB - only really the same if they have the same number of length groups
      if (L1->numLengthGroups() == L2->numLengthGroups()) {
        samedl = 1;
        offset = int((Lf->meanLength(0) - Lc->meanLength(0)) / Lf->dl());
      }

    } else if (L1->dl() > L2->dl()) {
      targetisfiner = 1;
      Lf = L2;
      Lc = L1;

    } else {
      Lf = L1;
      Lc = L2;
    }
  }

  nf = Lf->numLengthGroups();
  nc = Lc->numLengthGroups();
  //set minlength and maxlength
  for (i = 0; i < nf; i++) {
    if (Lf->minLength(i) > Lc->minLength() || isEqual(Lf->minLength(i), Lc->minLength())) {
      minlength = i;
      break;
    }
  }

  for (i = nf - 1; i >= 0; i--) {
    if (Lf->maxLength(i) < Lc->maxLength() || isEqual(Lf->maxLength(i), Lc->maxLength())) {
      maxlength = i + 1;
      break;
    }
  }

  k = 0;
  pos.resize(nf, 0);
  for (i = minlength; i < maxlength; i++) {
    for (j = k; j < nc; j++) {
      if (Lf->meanLength(i) > Lc->minLength(j) && Lf->meanLength(i) < Lc->maxLength(j)) {
        pos[i] = j;
        k = j;
        break;
      }
    }
  }

  for (i = maxlength; i < nf; i++)
    pos[i] = nc;

  //if minpos and maxpos are needed
  if (!samedl) {
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

    for (i = 0; i < nc - 1; i++)
      if (maxpos[i + 1] < maxpos[i])
        maxpos[i + 1] = maxpos[i];

    //if number in each length group is needed
    if (targetisfiner) {
      nrof.resize(nf, 1);
      for (i = minlength; i < maxlength; i++)
        nrof[i] = maxpos[pos[i]] - minpos[pos[i]] + 1;
    }
  }

  //if the conversionindex is to be used for interpolation
  if (interpolate) {
    interpratio.resize(nf, -1.0);
    interppos.resize(nf, -1);
    k = 0;
    for (i = minlength; i < maxlength; i++) {
      for (j = k; j < nc - 1; j++) {
        if (Lf->meanLength(i) >= Lc->meanLength(j) && Lf->meanLength(i) < Lc->meanLength(j + 1)) {
          interppos[i] = j;
          k = j;
          break;
        }
      }
    }

    for (i = 0; i < nf; i++) {
      if (interppos[i] != -1) {
        interpratio[i] = (Lf->meanLength(i) - Lc->meanLength(interppos[i])) /
          (Lc->meanLength(interppos[i] + 1) - Lc->meanLength(interppos[i]));
      } else {
        if (Lf->meanLength(i) < Lc->meanLength(0))
          interppos[i] = 0;
        else
          interppos[i] = nc - 1;
      }
    }
  }
}

//The function interpolates values calculated on a coarse length distribution
//Vc to a finer length distribution Vf using the conversionindex CI
void ConversionIndex::interpolateLengths(DoubleVector& Vf, const DoubleVector& Vc) {

  if (!interpolate)
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
