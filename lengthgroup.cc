#include "lengthgroup.h"
#include "gadget.h"

void printLengthGroupError(double minl, double maxl, double dl, const char* explain) {
  cerr << "Failed to create length group division with\nminimum length " << minl
    << "\nmaximum length " << maxl << "\nand dl " << dl
    << "\nKept string for explanations is " << explain << endl;
  exit(EXIT_FAILURE);
}

void printLengthGroupError(const DoubleVector& breaks, const char* explain) {
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

  if (isZero(Dl))
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

void printLengthGroupDivisionError(const LengthGroupDivision* lgrpdiv) {
  int i;
  if (lgrpdiv->dl() != 0) {
    cerr << "Length group with minimum length " << lgrpdiv->Minlength(0) << ", maximum length "
      << lgrpdiv->Maxlength(lgrpdiv->NoLengthGroups() - 1) << " and stepsize " << lgrpdiv->dl();

  } else {
    cerr << "Length group with lengths ";
    for (i = 0; i < lgrpdiv->NoLengthGroups(); i++)
      cerr << lgrpdiv->Minlength(i) << ", ";

    cerr << lgrpdiv->Maxlength(lgrpdiv->NoLengthGroups() - 1);
  }
  cerr << endl;
}

void printLengthGroupDivisionError(const LengthGroupDivision* finer,
  const LengthGroupDivision* coarser, const char* finername, const char* coarsername) {

  cerr << "Error: Length group divisions for " << finername
    << " is not finer than the length group division for " << coarsername
    << "\nThe length group division for " << finername << " is:\n";
  printLengthGroupDivisionError(finer);
  cerr << "The length group division for " << coarsername << " is:\n";
  printLengthGroupDivisionError(coarser);
}

/* returns -1 if algorithm fails. Should never happen, but Murphys law ...
 * returns 0 if finer is not finer than coarser and -1 and 2 did not happen.
 * returns 1 if finer is indeed finer than coarser.
 * returns 2 if comparison failed -- e.g. the intersection is empty,...
 * If finer is not finer than coarser, BogusLengthGroup will have
 * changed after the function has returned, and keep the number of a
 * length group in coarser that is not wholly contained in a single
 * length group in coarser.*/

int lengthGroupIsFiner(const LengthGroupDivision* finer,
  const LengthGroupDivision* coarser, int& BogusLengthGroup) {

  if (coarser->NoLengthGroups() == 0 || finer->NoLengthGroups() == 0)
    return 2;

  int c = 0;
  int f, fmin;

  int cmax = coarser->NoLengthGroups() - 1;
  int fmax = finer->NoLengthGroups() - 1;
  double allowederror = (coarser->Maxlength(cmax) - coarser->Minlength(0)) *
    rathersmall / coarser->NoLengthGroups();

  double minlength = max(coarser->Minlength(0), finer->Minlength(0));
  double maxlength = min(coarser->Maxlength(cmax), finer->Maxlength(fmax));

  //check to see if the intersection is empty
  if (minlength - allowederror >= maxlength)
    return 2;

  fmin = finer->NoLengthGroup(minlength);
  fmax = finer->NoLengthGroup(maxlength);

  if (minlength - allowederror > finer->Minlength(0))
    if (absolute(minlength - finer->Minlength(fmin)) > allowederror) {
      BogusLengthGroup = fmin;
      return 0;
    }

  if (maxlength + allowederror < finer->Maxlength(fmax))
    if (absolute(maxlength - finer->Minlength(fmax)) > allowederror) {
      BogusLengthGroup = fmax;
      return 0;
    }

  //if fmax is finer is not in the intersection of the length groups
  if (absolute(maxlength - finer->Minlength(fmax)) < allowederror)
    fmax--;

  //fmin is the first length group in finer that is wholly contained
  //in the intersection of finer and coarser.
  //fmax is the last length group in finer in the intersection
  for (f = fmin; f <= fmax; f++) {
    c = coarser->NoLengthGroup(finer->Minlength(f));
    if (c < 0)
      return -1;
    if (!(coarser->Minlength(c) - allowederror <= finer->Minlength(f)
        && finer->Maxlength(f) <= coarser->Maxlength(c) + allowederror)) {
      BogusLengthGroup = f;
      return 0;
    }
  }
  return 1;
}

void checkLengthGroupIsFiner(const LengthGroupDivision* finer,
  const LengthGroupDivision* coarser, const char* finername, const char* coarsername) {

  int BogusLengthGroup = 0;
  int isfiner = lengthGroupIsFiner(finer, coarser, BogusLengthGroup);
  switch(isfiner) {
    case -1:
      cerr << "Error: the algorithm in lengthgroupisfiner does not work.\n"
        << "The length group division for " << finername << " is:\n";
      printLengthGroupDivisionError(finer);
      cerr << "The length group division for " << coarsername << " is:\n";
      printLengthGroupDivisionError(coarser);
      exit(EXIT_FAILURE);
    case 0:
      cerr << "Error in length group " << BogusLengthGroup << " for " << finername << endl;
      printLengthGroupDivisionError(finer, coarser, finername, coarsername);
      exit(EXIT_FAILURE);
    case 1:
      return;
    case 2:
      cerr << "Warning - empty intersection when checking length groups\n";
      return;
    default:
      cerr << "Error when comparing length group divisions.\n"
        << "Unrecognized return code " << isfiner << " from IsLengthGroupFiner.\n"
        << "Please find someone to blame for this.\n";
      printLengthGroupDivisionError(finer, coarser, finername, coarsername);
      exit(EXIT_FAILURE);
    }
}
