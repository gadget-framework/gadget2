#include "conversion.h"
#include "checkconversion.h"
#include "gadget.h"

void LengthGroupPrintError(double minl, double maxl, double dl, const char* explain) {
  cerr << "Failed to create length group division with\nminimum length " << minl
    << "\nmaximum length " << maxl << "\nand dl " << dl
    << "\nKept string for explanations is " << explain << endl;
  exit(EXIT_FAILURE);
}

void LengthGroupPrintError(const DoubleVector& breaks, const char* explain) {
  cerr << "Failed to create the length group division\n";
  int i;
  for (i = 0; i < breaks.Size(); i++)
    cerr << breaks[i] << sep;
  cerr << "\nKept string for explanations is " << explain << endl;
  exit(EXIT_FAILURE);
}

//Constructor for length division with even increments.
LengthGroupDivision::LengthGroupDivision(double MinL, double MaxL, double DL) : error(0), Dl(DL) {
  assert(MinL >= 0 && Dl > 0 && MaxL >= MinL);
  size = int((MaxL - MinL) / Dl);
  double small = 0.01 * size;  //JMB - changed multiplier from rathersmall

  if (!(-size - small < (MaxL - MinL) / Dl && (MaxL - MinL) / Dl < size + small)) {
    error = 1;
    return;
  }

  meanlength.resize(size);
  minlength.resize(size);
  meanlength[0] = MinL + (0.5 * Dl);
  minlength[0] = MinL;
  int i;
  for (i = 1; i < size; i++) {
    meanlength[i] = meanlength[i - 1] + Dl;
    minlength[i] = minlength[i - 1] + Dl;
  }
}

//Constructor for length division with uneven increments.
LengthGroupDivision::LengthGroupDivision(const DoubleVector& Breaks) : error(0), Dl(0) {
  int i;
  if (Breaks.Size() < 2) {
    error = 1;
    return;
  }
  if (Breaks[0] < 0) {
    error = 1;
    return;
  }

  size = Breaks.Size() - 1;
  minlength.resize(size);
  meanlength.resize(size);
  for (i = 0; i < size; i++) {
    minlength[i] = Breaks[i];
    meanlength[i] = 0.5 * (Breaks[i] + Breaks[i + 1]);
    if (Breaks[i] >= Breaks[i + 1])
      error = 1;
  }
}

LengthGroupDivision::LengthGroupDivision(const LengthGroupDivision& l)
  : error(l.error), size(l.size), Dl(l.Dl), meanlength(l.meanlength), minlength(l.minlength) {
}

LengthGroupDivision::~LengthGroupDivision() {
}

int LengthGroupDivision::NoLengthGroup(double length) const {
  //Allow some error.
  const double err = (Maxlength(size - 1) - Minlength(size - 1)) * rathersmall;
  int i;
  for (i = 0; i < size; i++)
    //shift the length group down 'err' length units to allow some
    //error in its representation as a double.
    if (this->Minlength(i) - err <= length && length < this->Maxlength(i) - err)
      return i;
  //Check if length equals the maximum length.
  if (absolute(this->Maxlength(size - 1) - length) < err)
    return size - 1;
  else
    return -1;
}

double LengthGroupDivision::Meanlength(int i) const {
  if (i >= size)
    return meanlength[size - 1];
  else
    return meanlength[i];
}

double LengthGroupDivision::Minlength(int i) const {
  if (i >= size)
    return minlength[size - 1];
  else
    return minlength[i];
}

double LengthGroupDivision::Maxlength(int i) const {
  int j;
  if (i >= size)
    j = size;
  else
    j = i;

  if (iszero(Dl))
    return minlength[j] + 2 * (meanlength[j] - minlength[j]);
  else
    return minlength[j] + Dl;
}

int LengthGroupDivision::Combine(const LengthGroupDivision* const addition) {
  if (Minlength(0) > addition->Minlength(addition->NoLengthGroups() - 1)
      || Minlength(size - 1) < addition->Minlength(0))
    //Empty intersection
    return 0;

  int i = 0;
  int j = 0;
  if (Minlength(0) <= addition->Minlength(0) &&
      Minlength(size - 1) >= addition->Minlength(addition->NoLengthGroups() - 1)) {
    //If this is broader, we only have to check if the divisions are the same
    //in the overlapping region.
    while (Minlength(i) < addition->Minlength(0))
      i++;
    for (j = 0; j < addition->NoLengthGroups(); j++)
      if (Minlength(i + j) != addition->Minlength(j)) {
        error = 1;
        return 0;
      }
    return 1;
  }

  DoubleVector lower, middle;  //hold the minlength and meanlength of this
  if (Minlength(0) >= addition->Minlength(0)) {
    for (; Minlength(0) > addition->Minlength(i); i++) {
      lower.resize(1, addition->Minlength(i));
      middle.resize(1, addition->Meanlength(i));
    }
    for (; j - i < size && j < addition->NoLengthGroups(); j++) {
      if (Minlength(j - i) != addition->Minlength(j)) {
        error = 1;
        return 0;
      }
      lower.resize(1, Minlength(j - i));
      middle.resize(1, Meanlength(j - i));
    }
    if (j - i >= size)
      for (; j < addition->NoLengthGroups(); j++) {
        lower.resize(1, addition->Minlength(j));
        middle.resize(1, addition->Meanlength(j));
      }
    else
      for (; j - i < size; j++) {
        lower.resize(1, Minlength(j - i));
        middle.resize(1, Meanlength(j - i));
      }
  } else {
    for (; Minlength(i) < addition->Minlength(0); i++) {
      lower.resize(1, Minlength(i));
      middle.resize(1, Meanlength(i));
    }
    for (j = 0; j < addition->NoLengthGroups(); j++) {
      lower.resize(1, addition->Minlength(j));
      middle.resize(1, addition->Meanlength(j));
    }
  }

  //Set this to the new division
  Dl = 0;
  for (i = 0; i < size; i++) {
    minlength[i] = lower[i];
    meanlength[i] = middle[i];
  }
  size = lower.Size();
  for (; i < size; i++) {
    minlength.resize(1, lower[i]);
    meanlength.resize(1, middle[i]);
  }
  return 1;
}

ConversionIndex::ConversionIndex(const LengthGroupDivision* const L1,
  const LengthGroupDivision* const L2, int interp) {

  if (L1->Maxlength(L1->NoLengthGroups() - 1) <= L2->Minlength(0)
      || L2->Maxlength(L2->NoLengthGroups() - 1) <= L1->Minlength(0)) {
    cerr << "Error: cannot create a mapping between length groups whose "
      << "intersection is empty\nThe length group divisions are:\n";
    ErrorPrintLengthGroupDivision(L1);
    cerr << " and\n";
    ErrorPrintLengthGroupDivision(L2);
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
    CheckLengthGroupIsFiner(L1, L2, "CI - finer", "CI - coarser");
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

ConversionIndex::~ConversionIndex() {
}

//The function Interpolates values calculated on a coarse Length distribution
//Vc to a finer length distribution Vf using the conversionindex CI.
void Interp(DoubleVector& Vf, const DoubleVector& Vc, const ConversionIndex* CI) {
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
