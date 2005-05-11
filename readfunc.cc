#include "readfunc.h"
#include "keeper.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

int readMatrix(CommentStream& infile, DoubleMatrix& M) {
  if (infile.fail())
    return 0;

  char c;
  int i, j;
  double N;
  for (i = 0; i < M.Nrow(); i++) {
    for (j = 0; j < M.Ncol(i); j++) {
      infile >> ws >> N;
      if (!infile.fail())
        M[i][j] = N;
      else {
        handle.logFileMessage(LOGFAIL, "Error in readmatrix - failed to read data");
        return 0;
      }
    }
    infile.get(c);
    while (c == ' ' || c == '\t')
      infile.get(c);
    if (c != '\n') {
      handle.logFileMessage(LOGFAIL, "Error in readmatrix - expected end of line");
      return 0;
    }
  }
  return 1;
}

int readVector(CommentStream& infile, DoubleVector& Vec) {
  if (infile.fail())
    return 0;

  int i;
  double N;
  for (i = 0; i < Vec.Size(); i++) {
    infile >> ws >> N;
    if (!infile.fail())
      Vec[i] = N;
    else {
      handle.logFileMessage(LOGFAIL, "Error in readvector - failed to read data");
      return 0;
    }
  }
  return 1;
}

int readVector(CommentStream& infile, IntVector& Vec) {
  if (infile.fail())
    return 0;

  int i, N;
  for (i = 0; i < Vec.Size(); i++) {
    infile >> ws >> N;
    if (!infile.fail())
      Vec[i] = N;
    else {
      handle.logFileMessage(LOGFAIL, "Error in readvector - failed to read data");
      return 0;
    }
  }
  return 1;
}

int readIndexVector(CommentStream& infile, DoubleIndexVector& Vec) {
  if (infile.fail())
    return 0;

  int i;
  double N;
  for (i = Vec.minCol(); i < Vec.maxCol(); i++) {
    infile >> ws >> N;
    if (!infile.fail())
      Vec[i] = N;
    else {
      handle.logFileMessage(LOGFAIL, "Error in readindexvector - failed to read data");
      return 0;
    }
  }
  return 1;
}

int readVectorInLine(CommentStream& infile, IntVector& Vec) {
  if (infile.fail())
    return 0;

  int i;
  char line[MaxStrLength];
  strncpy(line, "", MaxStrLength);
  infile.getLine(line, MaxStrLength);
  if (infile.fail()) {
    handle.logFileMessage(LOGFAIL, "Error in readvectorinline - failed to read data");
    return 0;
  }

  istringstream istr(line);
  istr >> ws;
  i = 0;
  while (!istr.eof()) {
    if (i == Vec.Size())
      Vec.resize(1);
    istr >> Vec[i];
    if (istr.fail() && !istr.eof()) {
      handle.logFileMessage(LOGFAIL, "Error in readvectorinline - failed to read data");
      return 0;
    }
    istr >> ws;
    i++;
  }
  return 1;
}

int readVectorInLine(CommentStream& infile, DoubleVector& Vec) {
  if (infile.fail())
    return 0;

  int i;
  char line[MaxStrLength];
  strncpy(line, "", MaxStrLength);
  infile.getLine(line, MaxStrLength);
  if (infile.fail()) {
    handle.logFileMessage(LOGFAIL, "Error in readvectorinline - failed to read data");
    return 0;
  }

  istringstream istr(line);
  istr >> ws;
  i = 0;
  while (!istr.eof()) {
    if (i == Vec.Size())
      Vec.resize(1);
    istr >> Vec[i];
    if (istr.fail() && !istr.eof()) {
      handle.logFileMessage(LOGFAIL, "Error in readvectorinline - failed to read data");
      return 0;
    }
    istr >> ws;
    i++;
  }
  return 1;
}

int readRefWeights(CommentStream& infile, DoubleMatrix& M) {
  if (infile.fail())
    return 0;

  int i, j;
  double N;

  //Check the number of columns in the inputfile
  if (countColumns(infile) != 2)
    handle.logFileMessage(LOGFAIL, "Wrong number of columns in inputfile - should be 2");

  i = 0;
  while (!infile.eof()) {
    M.AddRows(1, 2);
    for (j = 0; j < 2; j++) {
      infile >> ws >> N;
      if (!infile.fail())
        M[i][j] = N;
      else {
        handle.logFileMessage(LOGFAIL, "Error in reference weights - failed to read data");
        return 0;
      }
    }
    infile >> ws;
    i++;
  }

  //Check the data to make sure that it is continuous
  for (i = 1; i < M.Nrow(); i++)
    if ((M[i][0] - M[i - 1][0]) < verysmall)
      handle.logFileMessage(LOGFAIL, "Lengths for reference weights must be strictly increasing");

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Read reference weights OK - number of entries", i);
  return 1;
}

int countColumns(CommentStream& infile) {
  if (infile.fail())
    return 0;

  char line[MaxStrLength];
  char temp[MaxStrLength];
  strncpy(line, "", MaxStrLength);
  strncpy(temp, "", MaxStrLength);
  streampos pos;

  pos = infile.tellg();
  infile >> ws;
  infile.getLine(line, MaxStrLength);
  if (infile.fail())
    return 0;

  istringstream istr(line);
  istr >> ws;

  char c;
  int i = 0;
  int p = 0;
  while (!istr.eof()) {
    // with the new formula syntax a column can contain whitespace,
    // so this code was changed to keep track of opening and closing
    // parens when counting columns. [mnaa]
    c = istr.peek();
    if (c=='(') {
      istr.get(c);
      p++;
      while (p > 0) {
        if (istr.eof())
          return 0;
        istr.get(c);
        if (c=='(')
          p++;
        if (c==')')
          p--;
      }
      istr >> ws;
      i++;
    } else {
      istr >> temp;
      if (istr.fail() && !istr.eof())
        return 0;
      istr >> ws;
      i++;
    }
  }

  infile.seekg(pos);
  return i;
}

int readAmounts(CommentStream& infile, const IntVector& tmpareas,
  const TimeClass* const TimeInfo, const AreaClass* const Area,
  FormulaMatrix& amount, Keeper* const keeper, const char* givenname) {

  int i;
  int year, step, area;
  Formula number;  //initialised to 0.0
  IntVector Years, Steps;
  char tmpname[MaxStrLength];
  strncpy(tmpname, "", MaxStrLength);
  int count = 0;
  int keepdata, timeid, areaid, tmpareaid;

  //Find start of distribution data in datafile
  infile >> ws;
  char c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  //Check the number of columns in the inputfile
  if (countColumns(infile) != 5)
    handle.logFileMessage(LOGFAIL, "Wrong number of columns in inputfile - should be 5");

  //Create storage for the data - size of the data is known
  amount.AddRows(TimeInfo->numTotalSteps() + 1, tmpareas.Size(), number);
  Years.resize(TimeInfo->numTotalSteps() + 1, 0);
  Steps.resize(TimeInfo->numTotalSteps() + 1, 0);
  year = TimeInfo->getFirstYear();
  step = TimeInfo->getFirstStep();
  int numsteps = TimeInfo->numSteps();
  for (i = 1; i < Years.Size(); i++) {
    //time is counted from timestep 1
    Years[i] = year;
    Steps[i] = step;
    step++;
    if (step > numsteps) {
      year++;
      step = 1;
    }
  }

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> area >> tmpname;

    //check if the year and step are in the simulation
    timeid = -1;
    if (TimeInfo->isWithinPeriod(year, step)) {
      //calculate the timeid index - time is counted from 1
      for (i = 1; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

    } else
      keepdata = 1;

    //only keep the data if tmpname matches givenname
    if (!(strcasecmp(givenname, tmpname) == 0))
      keepdata = 1;

    //only keep the data if area is a required area
    areaid = -1;
    tmpareaid = Area->InnerArea(area);
    for (i = 0; i < tmpareas.Size(); i++)
      if (tmpareas[i] == tmpareaid)
        areaid = i;

    if (areaid == -1)
      keepdata = 1;

    if (keepdata == 0) {
      //data is required, so store it
      count++;
      infile >> amount[timeid][areaid] >> ws;

    } else { //data not required - skip rest of line
      infile.get(c);
      while (c != '\n' && !infile.eof())
        infile.get(c);
      infile >> ws;
    }
  }

  if ((handle.getLogLevel() >= LOGWARN) && (count == 0))
    handle.logMessage(LOGWARN, "Warning in readamounts - found no data in the data file for", givenname);
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Read amounts data file - number of entries", count);
  return 1;
}

int readGrowthAmounts(CommentStream& infile, const TimeClass* const TimeInfo,
  const AreaClass* const Area, FormulaMatrixPtrVector& amount,
  const CharPtrVector& lenindex, Keeper* const keeper, const IntVector& tmpareas) {

  int i;
  int year, step, area;
  IntVector Years, Steps;
  char tmplength[MaxStrLength];
  strncpy(tmplength, "", MaxStrLength);
  int keepdata, timeid, areaid, lenid, tmpareaid;
  int count = 0;

  //Find start of distribution data in datafile
  infile >> ws;
  char c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  //Check the number of columns in the inputfile
  if (countColumns(infile) != 5)
    handle.logFileMessage(LOGFAIL, "Wrong number of columns in inputfile - should be 5");

  //Create storage for the data - size of the data is known
  Years.resize(TimeInfo->numTotalSteps() + 1, 0);
  Steps.resize(TimeInfo->numTotalSteps() + 1, 0);
  year = TimeInfo->getFirstYear();
  step = TimeInfo->getFirstStep();
  int numsteps = TimeInfo->numSteps();
  for (i = 1; i < Years.Size(); i++) {
    //time is counted from timestep 1
    Years[i] = year;
    Steps[i] = step;
    step++;
    if (step > numsteps) {
      year++;
      step = 1;
    }
  }

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> area >> tmplength;

    //check if the year and step are in the simulation
    timeid = -1;
    if (TimeInfo->isWithinPeriod(year, step)) {
      //calculate the timeid index - time is counted from 1
      for (i = 1; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

    } else {
      //dont keep the data
      keepdata = 1;
    }

    //if tmplength is in lenindex find lengthid, else dont keep the data
    lenid = -1;
    for (i = 0; i < lenindex.Size(); i++)
      if (strcasecmp(lenindex[i], tmplength) == 0)
        lenid = i;

    if (lenid == -1)
      keepdata = 1;

    //only keep the data if area is a required area
    areaid = -1;
    tmpareaid = Area->InnerArea(area);
    for (i = 0; i < tmpareas.Size(); i++)
      if (tmpareas[i] == tmpareaid)
        areaid = i;

    if (areaid == -1)
      keepdata = 1;

    if (keepdata == 0) {
      count++;
      infile >> (*amount[areaid])[timeid][lenid] >> ws;

    } else { //data not required - skip rest of line
      infile.get(c);
      while (c != '\n' && !infile.eof())
        infile.get(c);
      infile >> ws;
    }
  }
  if ((handle.getLogLevel() >= LOGWARN) && (count == 0))
    handle.logMessage(LOGWARN, "Warning in growthamounts - found no data in the data file");
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Read growth data file - number of entries", count);
  return 1;
}

/* This function expects infile to contain rows of data with columns
 * containing years and steps first.  It manipulates infile such that
 * if some year and step is greater than year and step, then infile is
 * positioned at the first occurence of such row and the function
 * returns 1.  If there is no row with year and step greater than the
 * arguments year and step, or if there is a failure in infile, the
 * function returns 0.  */
int FindContinuousYearAndStepWithNoText(CommentStream& infile, int year, int step) {
  if (infile.fail())
    return 0;
  int ryear, rstep;
  streampos pos;
  char c;
  do {
    if (infile.eof())
      return 0;  //No data later than or equal to year and step -- Failure
    pos = infile.tellg();
    infile >> ryear >> rstep;
    if (!infile.good())
      return 0; //Wrong format for file -- Failure
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  } while ((ryear < year || (ryear == year && rstep < step)));
  infile.seekg(pos);
  return 1;
}
