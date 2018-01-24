#include "lengthgroup.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"
//Constructor for length division with even increments
LengthGroupDivision::LengthGroupDivision(double MinL, double MaxL, double DL) : error(0), Dl(DL) {
  if ((MaxL < MinL) || (MinL < 0.0) || (Dl < verysmall)) {
    error = 1;
    return;
  }

  int i;
  minlen = MinL;
  maxlen = MaxL;
  double tmp = (maxlen - minlen) / Dl;
  size = int(tmp + rathersmall);
  if (size == 0) {
    error = 1;
    return;
  }

  meanlength.resize(size, 0.0);
  minlength.resize(size, 0.0);
  for (i = 0; i < size; i++) {
    minlength[i] = minlen + (Dl * i);
    meanlength[i] = minlength[i] + (Dl * 0.5);
  }
}

//Constructor for length division with uneven increments
LengthGroupDivision::LengthGroupDivision(const DoubleVector& Breaks) : error(0), Dl(0.0) {
  if ((Breaks.Size() < 2) || (Breaks[0] < 0.0)) {
    error = 1;
    return;
  }

  int i;
  size = Breaks.Size() - 1;
  minlen = Breaks[0];
  maxlen = Breaks[size];
  Dl = Breaks[1] - Breaks[0];
  meanlength.resize(size, 0.0);
  minlength.resize(size, 0.0);

  for (i = 0; i < size; i++) {
    //JMB first some error checks ...
    if ((Breaks[i] > Breaks[i + 1]) || (isEqual(Breaks[i], Breaks[i + 1]))) {
      error = 1;
      return;
    }
    //... and check to see if the step lengths are actually equal
    if (!isZero(Dl) && !isSmall(Breaks[i + 1] - Breaks[i] - Dl))
      Dl = 0.0;
  }

  //finally store the mean and minimum lengths for the length division
  if (isZero(Dl)) {
    for (i = 0; i < size; i++) {
      minlength[i] = Breaks[i];
      meanlength[i] = 0.5 * (Breaks[i] + Breaks[i + 1]);
    }
  } else {
    for (i = 0; i < size; i++) {
      minlength[i] = minlen + (Dl * i);
      meanlength[i] = minlength[i] + (Dl * 0.5);
    }
  }
}

LengthGroupDivision::LengthGroupDivision(const LengthGroupDivision& l)
  : error(l.error), size(l.size), Dl(l.Dl), minlen(l.minlen), maxlen(l.maxlen),
    meanlength(l.meanlength), minlength(l.minlength) {
}

int LengthGroupDivision::numLengthGroup(double len) const {
  //check if len equals the minimum length
  if (isSmall(minlen - len))
    return 0;

  //check if len equals the maximum length
  if (isSmall(maxlen - len))
    return size - 1;

  int i;
  for (i = 0; i < size; i++)
    if ((isSmall(minlength[i] - len)) || ((minlength[i] < len) && (len < this->maxLength(i))))
      return i;

  //failed to find length group that matches len
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
  if ((minlen > addition->minLength(addition->numLengthGroups() - 1))
      || (this->minLength(size - 1) < addition->minLength()))
    return 0;

  int i, j;
  i = j = 0;
  if (((minlen < addition->minLength()) || (isEqual(minlen, addition->minLength()))) &&
      ((this->minLength(size - 1) > addition->minLength(addition->numLengthGroups() - 1)) ||
      (isEqual(this->minLength(size - 1), addition->minLength(addition->numLengthGroups() - 1))))) {

    //only check if the divisions are the same in the overlapping region.
    while (this->minLength(i) < addition->minLength())
      i++;
    for (j = 0; j < addition->numLengthGroups(); j++) {
      if (!(isEqual(this->minLength(i + j), addition->minLength(j)))) {
        error = 1;
        return 0;
      }
    }
    return 1;
  }

  double tempmin = min(minlen, addition->minLength());
  double tempmax = max(maxlen, addition->maxLength());
  DoubleVector lower, middle;

  if ((minlen > addition->minLength()) || (isEqual(minlen, addition->minLength()))) {
    for (; minlen > addition->minLength(i); i++) {
      lower.resize(1, addition->minLength(i));
      middle.resize(1, addition->meanLength(i));
    }
    for (; j - i < size && j < addition->numLengthGroups(); j++) {
      if (!(isEqual(this->minLength(j - i), addition->minLength(j)))) {
        error = 1;
        return 0;
      }
      lower.resize(1, this->minLength(j - i));
      middle.resize(1, this->meanLength(j - i));
    }
    if (j - i >= size) {
      for (; j < addition->numLengthGroups(); j++) {
        lower.resize(1, addition->minLength(j));
        middle.resize(1, addition->meanLength(j));
      }
    } else {
      for (; j - i < size; j++) {
        lower.resize(1, this->minLength(j - i));
        middle.resize(1, this->meanLength(j - i));
      }
    }

  } else {
    for (; this->minLength(i) < addition->minLength(); i++) {
      lower.resize(1, this->minLength(i));
      middle.resize(1, this->meanLength(i));
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

  int c, f;
  double minlength, maxlength;

  //check to see if the intersection is empty
  minlength = max(coarser->minLength(), finer->minLength());
  maxlength = min(coarser->maxLength(), finer->maxLength());
  if ((minlength > maxlength) || isSmall(maxlength - minlength)) {
    handle.logMessage(LOGWARN, "Error when checking length structure - empty intersection");
    finer->printError();
    coarser->printError();
    return 0;
  }

  //if the length groups have the same dl then no need to check any further
  if (isSmall(finer->dl() - coarser->dl()))
    return 1;

  //now we need to check the length grouping for the intersection
  for (f = finer->numLengthGroup(minlength); f < finer->numLengthGroup(maxlength); f++) {
    c = coarser->numLengthGroup(finer->minLength(f));
    if (c == -1) {
      handle.logMessage(LOGWARN, "Error when checking length structure for the following lengthgroups");
      finer->printError();
      coarser->printError();
      return 0;
    }

    if ((coarser->minLength(c) > (finer->minLength(f) + rathersmall)) ||
         (finer->maxLength(f) > (coarser->maxLength(c) + rathersmall))) {

      handle.logMessage(LOGWARN, "Error when checking length structure for length group", f);
      finer->printError();
      coarser->printError();
      return 0;
    }
  }
  return 1;
}
