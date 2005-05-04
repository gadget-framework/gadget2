#include "conversionindex.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

ConversionIndex::ConversionIndex(const LengthGroupDivision* const L1,
  const LengthGroupDivision* const L2, int interp) {

  if (L1->maxLength() <= L2->minLength() || L2->maxLength() <= L1->minLength()) {
    handle.logMessage(LOGWARN, "Error in conversionindex - intersection between length groups is empty");
    L1->printError();
    L2->printError();
    exit(EXIT_FAILURE);
  }

  int i, j, k, nc, nf;
  int Mpos, NrOf;
  const LengthGroupDivision* Lf;  //Will be the finer length group division.
  const LengthGroupDivision* Lc;  //Will be the coarser length group division.

  //Here after is a check to see which lengthgroupdivision is coarser.
  //targetisfiner means that L1 is strictly coarser than L2.
  if (L1->dl() != 0 && L2->dl() != 0) {
    if (L1->dl() > L2->dl()) {
      targetisfiner = 1;
      Lc = L1;
      Lf = L2;
    } else {
      targetisfiner = 0;
      Lc = L2;
      Lf = L1;
    }

  } else {
    checkLengthGroupIsFiner(L1, L2);
    targetisfiner = 0;
    Lc = L2;
    Lf = L1;
  }

  //do the length group divisions have same dl?
  offset = 0;
  if ((Lf->dl() != 0) && (Lf->dl() == Lc->dl())) {
    offset = int((Lf->meanLength(0) - Lc->meanLength(0)) / Lf->dl());
    samedl = 1;
  } else
    samedl = 0;

  interpolate = interp;
  //set the switches Mpos and NrOf. They determine how much is needed
  //of the conversionindex.
  if (targetisfiner == 1 && samedl == 0)
    NrOf = 1;
  else
    NrOf = 0;

  if (samedl == 0)
    Mpos = 1;
  else
    Mpos = 0;

  nf = Lf->numLengthGroups();
  nc = Lc->numLengthGroups();

  //set minlength and maxlength if coarser LengthGroupDivision does
  //not span all the range of the finer one.
  for (i = 0; i < nf; i++)
    if (Lf->minLength(i) >= Lc->minLength()) {
      minlength = i;
      break;
    }

  for (i = nf - 1; i >= 0; i--)
    if (Lf->maxLength(i) <= Lc->maxLength()) {
      maxlength = i + 1;
      break;
    }

  k = 0;
  pos.resize(nf, 0);
  for (i = minlength; i < maxlength; i++)
    for (j = k; j < nc; j++)
      if (Lf->meanLength(i) >= Lc->minLength(j) && Lf->meanLength(i) <= Lc->maxLength(j)) {
        pos[i] = j;
        k = j;
        break;
      }

  for (i = maxlength; i < nf; i++)
    pos[i] = nc;

  //If minpos and maxpos are needed.
  if (Mpos == 1) {
    minpos.resize(nc, nf - 1); //initialised to Lf->Size() - 1.
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
  }

  //If number in each length group is needed.
  if (NrOf) {
    nrof.resize(nf, 0);
    for (i = minlength; i < maxlength; i++)
      nrof[i] = maxpos[pos[i]] - minpos[pos[i]] + 1;
  }

  //if the conversionindex is to be used for interpolation.
  if (interpolate) {
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
        interpratio[i] = -1; //-1 when outside range.
        if (Lf->meanLength(i) < Lc->meanLength(0))
          interppos[i] = 0;
        else
          interppos[i] = nc - 1;
      }
  }
}

//The function interpolates values calculated on a coarse length distribution
//Vc to a finer length distribution Vf using the conversionindex CI.
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
      if (interpratio[i] != -1)
        Vf[i] = Vc[interppos[i]] * (1.0 - interpratio[i]) + Vc[interppos[i] + 1] * interpratio[i];
      else
        Vf[i] = Vc[interppos[i]];
    }
  }
}
