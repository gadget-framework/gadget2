#include "lengthgroup.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

//Constructor for length division with even increments.
LengthGroupDivision::LengthGroupDivision(double MinL, double MaxL, double DL) : error(0), Dl(DL) {
  if ((MaxL < MinL) || (MinL < 0.0) || (Dl < verysmall)) {
    error = 1;
    return;
  }

  double tmp = (MaxL - MinL) / Dl;
  size = int(tmp + rathersmall);
  if (size == 0) {
    error = 1;
    return;
  }

  minlen = MinL;
  maxlen = MaxL;
  tmp = 0.5 * Dl;

  int i;
  meanlength.resize(size, 0.0);
  minlength.resize(size, 0.0);
  for (i = 0; i < size; i++) {
    minlength[i] = minlen + (Dl * i);
    meanlength[i] = minlength[i] + tmp;
  }
}

//Constructor for length division with uneven increments.
LengthGroupDivision::LengthGroupDivision(const DoubleVector& Breaks) : error(0), Dl(0.0) {
  if ((Breaks.Size() < 2) || (Breaks[0] < 0.0)) {
    error = 1;
    return;
  }

  size = Breaks.Size() - 1;
  minlen = Breaks[0];
  maxlen = Breaks[size];

  int i;
  //JMB should we try to work out if the Dl is the same?
  meanlength.resize(size, 0.0);
  minlength.resize(size, 0.0);
  for (i = 0; i < size; i++) {
    minlength[i] = Breaks[i];
    meanlength[i] = 0.5 * (Breaks[i] + Breaks[i + 1]);
    if ((Breaks[i] > Breaks[i + 1]) || (isEqual(Breaks[i], Breaks[i + 1])))
      error = 1;
  }
}

LengthGroupDivision::LengthGroupDivision(const LengthGroupDivision& l)
  : error(l.error), size(l.size), Dl(l.Dl), minlen(l.minlen), maxlen(l.maxlen),
    meanlength(l.meanlength), minlength(l.minlength) {
}

int LengthGroupDivision::numLengthGroup(double length) const {
  //check if length equals the minimum length.
  if (isEqual(minlen, length))
    return 0;

  //check if length equals the maximum length.
  if (isEqual(maxlen, length))
    return size - 1;

  int i;
  for (i = 0; i < size; i++)
    if ((isEqual(minlength[i], length)) || (minlength[i] < length && length < this->maxLength(i)))
      return i;

  //failed to find length group that matches length
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
      if (!(isEqual(minLength(i + j), addition->minLength(j)))) {
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
  if (!(isEqual(Dl, addition->dl())))
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
  handle.logMessage(LOGWARN, "Minimum length of length group division is", this->minLength());
  handle.logMessage(LOGWARN, "Maximum length of length group division is", this->maxLength());
}

int checkLengthGroupStructure(const LengthGroupDivision* finer,
  const LengthGroupDivision* coarser) {

  int c, f, fmin, fmax;
  double minlength, maxlength;

  //check to see if the intersection is empty
  minlength = max(coarser->minLength(), finer->minLength()) + rathersmall;
  maxlength = min(coarser->maxLength(), finer->maxLength()) - rathersmall;
  if ((minlength > maxlength) || isEqual(minlength, maxlength)) {
    handle.logMessage(LOGWARN, "Error when checking length structure - empty intersection");
    finer->printError();
    coarser->printError();
    return 1;
  }

  fmin = finer->numLengthGroup(minlength);
  fmax = finer->numLengthGroup(maxlength);
  for (f = fmin; f < fmax; f++) {
    c = coarser->numLengthGroup(finer->minLength(f));
    if (c == -1) {
      handle.logMessage(LOGWARN, "Error when checking length structure for the following lengthgroups");
      finer->printError();
      coarser->printError();
      return 1;
    }

    if ((coarser->minLength(c) > (finer->minLength(f) + rathersmall)) ||
         (finer->maxLength(f) > (coarser->maxLength(c) + rathersmall))) {

      handle.logMessage(LOGWARN, "Error when checking length structure for length group", f);
      finer->printError();
      coarser->printError();
      return 1;
    }
  }
  return 0;
}
