#include "lengthgroup.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

//Constructor for length division with even increments.
LengthGroupDivision::LengthGroupDivision(double MinL, double MaxL, double DL) : error(0), Dl(DL) {
  if ((MaxL < MinL) || (MinL < 0.0) || (DL < verysmall)) {
    error = 1;
    return;
  }

  double tmp = (MaxL - MinL) / Dl;
  size = int(tmp + rathersmall);
  minlen = MinL;
  maxlen = MaxL;

  meanlength.resize(size);
  minlength.resize(size);
  tmp = 0.5 * Dl;
  int i;
  for (i = 0; i < size; i++) {
    minlength[i] = MinL + (Dl * i);
    meanlength[i] = minlength[i] + tmp;
  }
}

//Constructor for length division with uneven increments.
LengthGroupDivision::LengthGroupDivision(const DoubleVector& Breaks) : error(0), Dl(0) {
  if ((Breaks.Size() < 2) || (Breaks[0] < 0)) {
    error = 1;
    return;
  }

  size = Breaks.Size() - 1;
  minlen = Breaks[0];
  maxlen = Breaks[size];

  minlength.resize(size);
  meanlength.resize(size);
  int i;
  for (i = 0; i < size; i++) {
    minlength[i] = Breaks[i];
    meanlength[i] = 0.5 * (Breaks[i] + Breaks[i + 1]);
    if (Breaks[i] >= Breaks[i + 1])
      error = 1;
  }
}

LengthGroupDivision::LengthGroupDivision(const LengthGroupDivision& l)
  : error(l.error), size(l.size), Dl(l.Dl), minlen(l.minlen), maxlen(l.maxlen),
    meanlength(l.meanlength), minlength(l.minlength) {
}

LengthGroupDivision::~LengthGroupDivision() {
}

int LengthGroupDivision::NoLengthGroup(double length) const {
  //Allow some error.
  const double err = (maxlen - minLength(size - 1)) * rathersmall;
  int i;

  //shift the length group down 'err' length units to allow some
  //error in its representation as a double.
  for (i = 0; i < size; i++)
    if (this->minLength(i) - err <= length && length < this->maxLength(i) - err)
      return i;

  //Check if length equals the maximum length.
  if (absolute(maxlen - length) < err)
    return size - 1;
  else
    return -1;
}

double LengthGroupDivision::meanLength(int i) const {
  if (i >= size)
    return meanlength[size - 1];
  else
    return meanlength[i];
}

double LengthGroupDivision::minLength(int i) const {
  if (i >= size)
    return minlength[size - 1];
  else
    return minlength[i];
}

double LengthGroupDivision::maxLength(int i) const {
  if (i >= (size - 1))
    return maxlen;

  return minlength[i + 1];
}

int LengthGroupDivision::Combine(const LengthGroupDivision* const addition) {
  if (minlen > addition->minLength(addition->NoLengthGroups() - 1)
      || minLength(size - 1) < addition->minLength())
    return 0;

  int i = 0;
  int j = 0;

  double tempmin = min(minlen, addition->minLength());
  double tempmax = max(maxlen, addition->maxLength());

  if (minlen <= addition->minLength() &&
      minLength(size - 1) >= addition->minLength(addition->NoLengthGroups() - 1)) {
    //If this is broader, we only have to check if the divisions are the same
    //in the overlapping region.
    while (minLength(i) < addition->minLength())
      i++;
    for (j = 0; j < addition->NoLengthGroups(); j++)
      if (minLength(i + j) != addition->minLength(j)) {
        error = 1;
        return 0;
      }
    return 1;
  }

  DoubleVector lower, middle;  //hold the minlength and meanlength of this
  if (minlen >= addition->minLength()) {
    for (; minlen > addition->minLength(i); i++) {
      lower.resize(1, addition->minLength(i));
      middle.resize(1, addition->meanLength(i));
    }
    for (; j - i < size && j < addition->NoLengthGroups(); j++) {
      if (minLength(j - i) != addition->minLength(j)) {
        error = 1;
        return 0;
      }
      lower.resize(1, minLength(j - i));
      middle.resize(1, meanLength(j - i));
    }
    if (j - i >= size)
      for (; j < addition->NoLengthGroups(); j++) {
        lower.resize(1, addition->minLength(j));
        middle.resize(1, addition->meanLength(j));
      }
    else
      for (; j - i < size; j++) {
        lower.resize(1, minLength(j - i));
        middle.resize(1, meanLength(j - i));
      }
  } else {
    for (; minLength(i) < addition->minLength(); i++) {
      lower.resize(1, minLength(i));
      middle.resize(1, meanLength(i));
    }
    for (j = 0; j < addition->NoLengthGroups(); j++) {
      lower.resize(1, addition->minLength(j));
      middle.resize(1, addition->meanLength(j));
    }
  }

  //set this to the new division
  Dl = 0.0;
  for (i = 0; i < size; i++) {
    minlength[i] = lower[i];
    meanlength[i] = middle[i];
  }
  size = lower.Size();
  for (; i < size; i++) {
    minlength.resize(1, lower[i]);
    meanlength.resize(1, middle[i]);
  }
  minlen = tempmin;
  maxlen = tempmax;
  return 1;
}

void LengthGroupDivision::Print(ofstream& outfile) const {
  int i;
  outfile << "Length group division with " << size << " length groups from " << minlen
    << " up to " << maxlen << endl << TAB;
  for (i = 0; i < size; i++)
    outfile << minlength[i] << sep;
  outfile << maxlen << endl;
}

void LengthGroupDivision::printError() const {
  handle.logWarning("Warning in lengthgroupdivision - failure from length group");
  handle.logWarning("Minimum length", this->minLength());
  handle.logWarning("Maximum length", this->maxLength());
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
  double allowederror = (coarser->maxLength() - coarser->minLength()) *
    rathersmall / coarser->NoLengthGroups();

  double minlength = max(coarser->minLength(), finer->minLength());
  double maxlength = min(coarser->maxLength(), finer->maxLength());

  //check to see if the intersection is empty
  if (minlength - allowederror >= maxlength)
    return 2;

  fmin = finer->NoLengthGroup(minlength);
  fmax = finer->NoLengthGroup(maxlength);

  if (minlength - allowederror > finer->minLength())
    if (absolute(minlength - finer->minLength(fmin)) > allowederror) {
      BogusLengthGroup = fmin;
      return 0;
    }

  if (maxlength + allowederror < finer->maxLength())
    if (absolute(maxlength - finer->minLength(fmax)) > allowederror) {
      BogusLengthGroup = fmax;
      return 0;
    }

  //if fmax is finer is not in the intersection of the length groups
  while (absolute(maxlength - finer->minLength(fmax)) < allowederror)
    fmax--;

  //fmin is the first length group in finer that is wholly contained
  //in the intersection of finer and coarser.
  //fmax is the last length group in finer in the intersection
  for (f = fmin; f <= fmax; f++) {
    c = coarser->NoLengthGroup(finer->minLength(f));
    if (c < 0)
      return -1;
    if (!(coarser->minLength(c) - allowederror <= finer->minLength(f)
        && finer->maxLength(f) <= coarser->maxLength(c) + allowederror)) {
      BogusLengthGroup = f;
      return 0;
    }
  }
  return 1;
}

void checkLengthGroupIsFiner(const LengthGroupDivision* finer,
  const LengthGroupDivision* coarser) {

  int BogusLengthGroup = 0;
  int isfiner = lengthGroupIsFiner(finer, coarser, BogusLengthGroup);
  switch(isfiner) {
    case -1:
      handle.logWarning("Error in lengthgroup - failed to check lengthgroupisfiner");
      finer->printError();
      coarser->printError();
      exit(EXIT_FAILURE);
    case 0:
      handle.logWarning("Error in lengthgroup - failed to check length cell", BogusLengthGroup);
      finer->printError();
      coarser->printError();
      exit(EXIT_FAILURE);
    case 1:
      return;
    case 2:
      handle.logWarning("Warning in lengthgroup - intersection between length groups is empty");
      finer->printError();
      coarser->printError();
      return;
    default:
      handle.logWarning("Error in lengthgroup - unrecognised return from lengthgroupisfiner");
      finer->printError();
      coarser->printError();
      exit(EXIT_FAILURE);
    }
}
