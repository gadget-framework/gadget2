#include "conversionindex.h"
#include "gadget.h"

ConversionIndex::ConversionIndex(const LengthGroupDivision* const L1,
  const LengthGroupDivision* const L2, int interp) {

  if (L1->Maxlength(L1->NoLengthGroups() - 1) <= L2->Minlength(0)
      || L2->Maxlength(L2->NoLengthGroups() - 1) <= L1->Minlength(0)) {
    cerr << "Error: cannot create a mapping between length groups whose "
      << "intersection is empty\nThe length group divisions are:\n";
    printLengthGroupDivisionError(L1);
    cerr << " and\n";
    printLengthGroupDivisionError(L2);
    exit(EXIT_FAILURE);
  }

  int i, j;
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
    checkLengthGroupIsFiner(L1, L2, "CI - finer", "CI - coarser");
    targetisfiner = 0;
    Lc = L2;
    Lf = L1;
  }

  //do the length group divisions have same dl?
  if (Lf->dl() != 0 && Lf->dl() == Lc->dl()) {
    offset = int((Lf->Meanlength(0) - Lc->Meanlength(0)) / Lf->dl());
    samedl = 1;
  } else
    samedl = 0;

  //Set the switches Mpos and NrOf. They determine how much is needed
  //of the conversionindex.
  if (targetisfiner == 1 && samedl == 0)
    NrOf = 1;
  else
    NrOf = 0;

  if (samedl == 0)
    Mpos = 1;
  else
    Mpos = 0;

  nf = Lf->NoLengthGroups();
  nc = Lc->NoLengthGroups();

  //Set minlength and maxlength if coarser LengthGroupDivision does
  //not span all the range of the finer one.
  for (i = 0; i < nf; i++)
    if (Lf->Minlength(i) >= Lc->Minlength(0)) {
      minlength = i;
      break;
    }

  for (i = nf - 1; i >= 0; i--)
    if (Lf->Maxlength(i) <= Lc->Maxlength(nc - 1)) {
      maxlength = i + 1;
      break;
    }

  int k = 0;
  pos.resize(Lf->NoLengthGroups(), 0); //JMB - default value
  for (i = minlength; i < maxlength; i++)
    for (j = k; j < Lc->NoLengthGroups(); j++)
      if (Lf->Meanlength(i) >= Lc->Minlength(j) && Lf->Meanlength(i) <= Lc->Maxlength(j)) {
        pos[i] = j;
        k = j;
        break;
      }

  //If Minpos and Maxpos are needed.
  if (Mpos == 1) {
    minpos.resize(Lc->NoLengthGroups(), Lf->NoLengthGroups() - 1); //initialized to Lf->Size() - 1.
    for (i = minlength; i < maxlength; i++)
      if (i < minpos[pos[i]])
        minpos[pos[i]] = i;

    for (i = Lc->NoLengthGroups() - 1; i > 0; i--)
      if (minpos[i - 1] > minpos[i])
        minpos[i - 1] = minpos[i];

    maxpos.resize(Lc->NoLengthGroups(), 0);
    for (i = minlength; i < maxlength; i++)
      if (i > maxpos[pos[i]])
        maxpos[pos[i]] = i;

    for (i = 0; i < Lc->NoLengthGroups() - 1;i++)
      if (maxpos[i + 1] < maxpos[i])
        maxpos[i + 1] = maxpos[i];
  }

  //If number in each length group is needed.
  if (NrOf) {
    nrof.resize(Lf->NoLengthGroups(), 0);
    for (i = minlength; i < maxlength; i++)
      nrof[i] = maxpos[pos[i]] - minpos[pos[i]] + 1;
  }

  //if the conversionindex is to be used for interpolation.
  if (interp) {
    interpratio.resize(Lf->NoLengthGroups());
    interppos.resize(Lf->NoLengthGroups(), -1);
    k = 0;
    for (i = minlength; i < maxlength; i++)
      for (j = k; j < Lc->NoLengthGroups() - 1; j++)
        if (Lf->Meanlength(i) >= Lc->Meanlength(j) && Lf->Meanlength(i) < Lc->Meanlength(j + 1)) {
          interppos[i] = j;
          k = j;
          break;
        }

    for (i = 0; i < Lf->NoLengthGroups(); i++)
      if (interppos[i] != -1)
        interpratio[i] = (Lf->Meanlength(i) - Lc->Meanlength(interppos[i])) /
          (Lc->Meanlength(interppos[i] + 1) - Lc->Meanlength(interppos[i]));
      else {
        interpratio[i] = -1; //-1 when outside range.
        if (Lf->Meanlength(i) < Lc->Meanlength(0))
          interppos[i] = 0;
        else
          interppos[i] = nc - 1;
      }
  }
}

//The function Interpolates values calculated on a coarse Length distribution
//Vc to a finer length distribution Vf using the conversionindex CI.
void interpolateLengths(DoubleVector& Vf, const DoubleVector& Vc, const ConversionIndex* CI) {
  int i, offset;
  if (CI -> SameDl()) {
    offset = CI->Offset();
    if (CI->Minlength() > 0)
      for (i = 0; i < CI->Minlength(); i++)
        Vf[i] = Vc[0];

    for (i = CI->Minlength(); i < CI->Maxlength(); i++)
      Vf[i] = Vc[i + offset];

    if (CI->Maxlength() < Vf.Size())
      for (i = CI->Maxlength(); i < Vf.Size(); i++)
        Vf[i] = Vc[Vc.Size() - 1];

  } else {
    for (i = 0; i < Vf.Size(); i++) {
      if (CI->InterpRatio(i) != -1)
        Vf[i] = Vc[CI->InterpPos(i)] * (1 - CI->InterpRatio(i)) + Vc[CI->InterpPos(i) + 1] * CI->InterpRatio(i);
      else
        Vf[i] = Vc[CI->InterpPos(i)];
    }
  }
}
