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
  if ((Breaks.Size() < 2) || (Breaks[0] < 0.0)) {
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
    if ((Breaks[i] > Breaks[i + 1]) || (isZero(Breaks[i] - Breaks[i + 1])))
      error = 1;
  }
}

LengthGroupDivision::LengthGroupDivision(const LengthGroupDivision& l)
  : error(l.error), size(l.size), Dl(l.Dl), minlen(l.minlen), maxlen(l.maxlen),
    meanlength(l.meanlength), minlength(l.minlength) {
}

int LengthGroupDivision::numLengthGroup(double length) const {
  int i;
  for (i = 0; i < size; i++)
    if ((isZero(minlength[i] - length)) || (minlength[i] < length && length < this->maxLength(i)))
      return i;

  //Check if length equals the maximum length.
  if (isZero(maxlen - length))
    return size - 1;
  return -1;
}

double LengthGroupDivision::meanLength(int i) const {
  if (i >= size)
    return meanlength[size - 1];
  return meanlength[i];
}

double LengthGroupDivision::minLength(int i) const {
  if (i >= size)
    return minlength[size - 1];
  return minlength[i];
}

double LengthGroupDivision::maxLength(int i) const {
  if (i >= (size - 1))
    return maxlen;
  return minlength[i + 1];
}

int LengthGroupDivision::Combine(const LengthGroupDivision* const addition) {
  if (minlen > addition->minLength(addition->numLengthGroups() - 1)
      || minLength(size - 1) < addition->minLength())
    return 0;

  int i, j;
  i = j = 0;
  if (minlen <= addition->minLength() &&
      minLength(size - 1) >= addition->minLength(addition->numLengthGroups() - 1)) {
    //only check if the divisions are the same in the overlapping region.
    while (minLength(i) < addition->minLength())
      i++;
    for (j = 0; j < addition->numLengthGroups(); j++) {
      if (!isZero(minLength(i + j) - addition->minLength(j))) {
        error = 1;
        return 0;
      }
    }
    return 1;
  }

  double tempmin = min(minlen, addition->minLength());
  double tempmax = max(maxlen, addition->maxLength());
  DoubleVector lower, middle;

  if (minlen >= addition->minLength()) {
    for (; minlen > addition->minLength(i); i++) {
      lower.resize(1, addition->minLength(i));
      middle.resize(1, addition->meanLength(i));
    }
    for (; j - i < size && j < addition->numLengthGroups(); j++) {
      if (minLength(j - i) != addition->minLength(j)) {
        error = 1;
        return 0;
      }
      lower.resize(1, minLength(j - i));
      middle.resize(1, meanLength(j - i));
    }
    if (j - i >= size) {
      for (; j < addition->numLengthGroups(); j++) {
        lower.resize(1, addition->minLength(j));
        middle.resize(1, addition->meanLength(j));
      }
    } else {
      for (; j - i < size; j++) {
        lower.resize(1, minLength(j - i));
        middle.resize(1, meanLength(j - i));
      }
    }
  } else {
    for (; minLength(i) < addition->minLength(); i++) {
      lower.resize(1, minLength(i));
      middle.resize(1, meanLength(i));
    }
    for (j = 0; j < addition->numLengthGroups(); j++) {
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
  handle.logMessage(LOGWARN, "Warning in lengthgroupdivision - failure in length group");
  handle.logMessage(LOGWARN, "Minimum length of length group", this->minLength());
  handle.logMessage(LOGWARN, "Maximum length of length group", this->maxLength());
}

void checkLengthGroupIsFiner(const LengthGroupDivision* finer,
  const LengthGroupDivision* coarser) {

  double minlength = max(coarser->minLength(), finer->minLength());
  double maxlength = min(coarser->maxLength(), finer->maxLength());

  //check to see if the intersection is empty
  if ((coarser->numLengthGroups() == 0) ||
       (finer->numLengthGroups() == 0) || (minlength > maxlength)) {

    handle.logMessage(LOGWARN, "Error when checking lengthgroupisfiner - empty intersection");
    finer->printError();
    coarser->printError();
    exit(EXIT_FAILURE);
  }

  int c, f, fmin, fmax;
  fmin = finer->numLengthGroup(minlength);
  fmax = finer->numLengthGroup(maxlength);

  //check the minimum length cell of the intersection
  if ((minlength > finer->minLength()) && !isZero(minlength - finer->minLength(fmin))) {
    handle.logMessage(LOGWARN, "Error when checking lengthgroupisfiner for minimum length cell");
    finer->printError();
    coarser->printError();
    exit(EXIT_FAILURE);
  }

  //check the maximum length cell of the intersection
  if ((maxlength < finer->maxLength()) && !isZero(maxlength - finer->minLength(fmax))) {
    handle.logMessage(LOGWARN, "Error when checking lengthgroupisfiner for maximum length cell");
    finer->printError();
    coarser->printError();
    exit(EXIT_FAILURE);
  }

  //if fmax in finer is not in the intersection of the length groups
  while (isZero(maxlength - finer->minLength(fmax)))
    fmax--;

  //fmin is the first length group in finer that is wholly contained
  //in the intersection of finer and coarser.
  //fmax is the last length group in finer in the intersection
  for (f = fmin; f <= fmax; f++) {
    c = coarser->numLengthGroup(finer->minLength(f));
    if (c < 0) {
      handle.logMessage(LOGWARN, "Error when checking lengthgroupisfiner for the following lengthgroups");
      finer->printError();
      coarser->printError();
      exit(EXIT_FAILURE);
    }
    if ((coarser->minLength(c) > (finer->minLength(f) + rathersmall)) ||
         (finer->maxLength(f) > (coarser->maxLength(c) + rathersmall))) {

      handle.logMessage(LOGWARN, "Error when checking lengthgroupisfiner for length cell", f);
      finer->printError();
      coarser->printError();
      exit(EXIT_FAILURE);
    }
  }
}
