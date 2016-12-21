#include "conversionindex.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

ConversionIndex::ConversionIndex(const LengthGroupDivision* const L1,
  const LengthGroupDivision* const L2, int interp) {

  int i, j, k, nc, nf;
  const LengthGroupDivision* Lf;  //will be the finer length group division
  const LengthGroupDivision* Lc;  //will be the coarser length group division
  double tmpmin = max(L1->minLength(), L2->minLength());
  double tmpmax = min(L1->maxLength(), L2->maxLength());

  error = samedl = offset = isfiner = 0;
  interpolate = interp;

  //check to see if the intersection is empty
  if ((tmpmin > tmpmax) || (isEqual(tmpmin, tmpmax))) {
    handle.logMessage(LOGWARN, "Error when checking length structure - empty intersection");
    error = 1;
    return;
  }

  if (isZero(L1->dl()) || isZero(L2->dl())) {
    if (!checkLengthGroupStructure(L1, L2)) {
      error = 1;
      return;
    }
    Lf = L1;
    Lc = L2;

  } else if (isSmall(L1->dl() - L2->dl())) {
    Lf = L1;
    Lc = L2;
    //check that the length group divisions are aligned
    double check = (Lf->minLength() - Lc->minLength()) / Lf->dl();
    offset = int(check + verysmall);
    if (isEqual(check, floor(check)))
      samedl = 1;

  } else if (L1->dl() > L2->dl()) {
    isfiner = 1;
    Lf = L2;
    Lc = L1;

  } else {
    Lf = L1;
    Lc = L2;
  }

  nf = Lf->numLengthGroups();
  nc = Lc->numLengthGroups();
  //set minlength and maxlength
  for (i = 0; i < nf; i++) {
    if ((Lf->minLength(i) > Lc->minLength()) || (isSmall(Lf->minLength(i) - Lc->minLength()))) {
      minlength = i;
      break;
    }
  }

  for (i = nf - 1; i >= 0; i--) {
    if ((Lf->maxLength(i) < Lc->maxLength()) || (isSmall(Lf->maxLength(i) - Lc->maxLength()))) {
      maxlength = i + 1;
      break;
    }
  }

  k = 0;
  pos.resize(nf, 0);
  for (i = minlength; i < maxlength; i++) {
    for (j = k; j < nc; j++) {
      if ((Lf->meanLength(i) > Lc->minLength(j)) && (Lf->meanLength(i) < Lc->maxLength(j))) {
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
    if (isfiner) {
      numpos.resize(nf, 1);
      for (i = minlength; i < maxlength; i++)
        numpos[i] = maxpos[pos[i]] - minpos[pos[i]] + 1;
    }

    //if the conversionindex is to be used for interpolation
    if (interpolate) {
      iratio.resize(nf, -1.0);
      ipos.resize(nf, -1);
      k = 0;
      for (i = minlength; i < maxlength; i++) {
        for (j = k; j < nc - 1; j++) {
          if (((Lf->meanLength(i) > Lc->meanLength(j)) || (isSmall(Lf->meanLength(i) - Lc->meanLength(j)))) && (Lf->meanLength(i) < Lc->meanLength(j + 1))) {

            ipos[i] = j;
            k = j;
            break;
          }
        }
      }

      for (i = 0; i < nf; i++) {
        if (ipos[i] == -1) {
          if (Lf->meanLength(i) < Lc->meanLength(0))
            ipos[i] = 0;
          else
            ipos[i] = nc - 1;

        } else {
          iratio[i] = (Lf->meanLength(i) - Lc->meanLength(ipos[i])) /
              (Lc->meanLength(ipos[i] + 1) - Lc->meanLength(ipos[i]));
        }
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
      if (isEqual(iratio[i], -1.0))
        Vf[i] = Vc[ipos[i]];
      else
        Vf[i] = (Vc[ipos[i]] * (1.0 - iratio[i])) + (Vc[ipos[i] + 1] * iratio[i]);
    }
  }
}
