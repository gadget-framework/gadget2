#include "readfunc.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

void readRefWeights(CommentStream& infile, DoubleMatrix& M) {
  //Check the number of columns in the inputfile
  if (countColumns(infile) != 2)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 2");

  int i = 0;
  M.Reset();
  infile >> ws;
  while (!infile.eof()) {
    //crude check to see if something has gone wrong and avoid infinite loops
    if (!(isdigit(infile.peek())))
      handle.logFileMessage(LOGFAIL, "failed to read data from file");

    M.AddRows(1, 2, 0.0);
    infile >> M[i][0] >> M[i][1];
    if (infile.fail())
      handle.logFileMessage(LOGFAIL, "failed to read reference weights");
    infile >> ws;
    i++;
  }

  //Check the data to make sure that it is continuous and the weights are positive
  if (M[0][0] < 0.0)
    handle.logFileMessage(LOGFAIL, "lengths for reference weights must be positive");
  for (i = 1; i < M.Nrow(); i++)
    if ((M[i][0] - M[i - 1][0]) < verysmall)
      handle.logFileMessage(LOGFAIL, "lengths for reference weights must be strictly increasing");
  for (i = 1; i < M.Nrow(); i++)
    if (M[i][1] < verysmall)
      handle.logFileMessage(LOGFAIL, "weights for reference weights must be positive");

  handle.logMessage(LOGMESSAGE, "Read reference weights OK - number of entries", M.Nrow());
}

int countColumns(CommentStream& infile) {
  if (infile.fail())
    return 0;

  char line[MaxStrLength];
  char temp[MaxStrLength];
  strncpy(line, "", MaxStrLength);
  strncpy(temp, "", MaxStrLength);
  streampos pos = infile.tellg();

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
    c = (char)istr.peek(); //JMB keep GCC 4.3 happy
    if (c == '(') {
      istr.get(c);
      p++;
      while (p > 0) {
        if (istr.eof())
          return 0;
        istr.get(c);
        if (c == '(')
          p++;
        if (c == ')')
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

void readAmounts(CommentStream& infile, const IntVector& tmpareas,
  const TimeClass* const TimeInfo, const AreaClass* const Area,
  FormulaMatrix& amount, const char* givenname) {

  char c;
  char tmpname[MaxStrLength];
  strncpy(tmpname, "", MaxStrLength);
  int i, year, step, area;
  int keepdata, timeid, areaid, tmpareaid, count, reject;

  //Check the number of columns in the inputfile
  infile >> ws;
  if (countColumns(infile) != 5)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 5");

  //Create storage for the data - size of the data is known
  amount.AddRows(TimeInfo->numTotalSteps() + 1, tmpareas.Size(), 0.0);

  year = step = area = count = reject = 0;
  while (!infile.eof()) {
    keepdata = 1;
    infile >> year >> step >> area >> tmpname;

    //crude check to see if something has gone wrong and avoid infinite loops
    if (strlen(tmpname) == 0)
      handle.logFileMessage(LOGFAIL, "failed to read data for", givenname);

    //check if the year and step are in the simulation
    timeid = -1;
    if (TimeInfo->isWithinPeriod(year, step))
      timeid = TimeInfo->calcSteps(year, step);
    else
      keepdata = 0;

    //only keep the data if tmpname matches givenname
    if (strcasecmp(givenname, tmpname) != 0)
      keepdata = 0;

    //only keep the data if area is a required area
    areaid = -1;
    tmpareaid = Area->getInnerArea(area);
    for (i = 0; i < tmpareas.Size(); i++)
      if (tmpareas[i] == tmpareaid)
        areaid = i;

    if (areaid == -1)
      keepdata = 0;

    if (keepdata == 1) {
      //data is required, so store it
      count++;
      infile >> amount[timeid][areaid] >> ws;

    } else { //data not required - skip rest of line
      reject++;
      infile.get(c);
      while (c != '\n' && !infile.eof())
        infile.get(c);
      infile >> ws;
    }
  }

  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in readamounts - found no data in the data file for", givenname);
  if (reject != 0)
    handle.logMessage(LOGMESSAGE, "Discarded invalid amounts data - number of invalid entries", reject);
  handle.logMessage(LOGMESSAGE, "Read amounts data file - number of entries", count);
}

void readGrowthAmounts(CommentStream& infile, const TimeClass* const TimeInfo,
  const AreaClass* const Area, FormulaMatrixPtrVector& amount,
  const CharPtrVector& lenindex, const IntVector& tmpareas) {

  int i, year, step, area;
  char c;
  char tmplength[MaxStrLength];
  strncpy(tmplength, "", MaxStrLength);
  int keepdata, timeid, areaid, lenid, tmpareaid, count, reject;

  //Check the number of columns in the inputfile
  infile >> ws;
  if (countColumns(infile) != 5)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 5");

  year = step = area = count = reject = 0;
  while (!infile.eof()) {
    keepdata = 1;
    infile >> year >> step >> area >> tmplength;

    //crude check to see if something has gone wrong and avoid infinite loops
    if (strlen(tmplength) == 0)
      handle.logFileMessage(LOGFAIL, "failed to read growth data");

    //check if the year and step are in the simulation
    timeid = -1;
    if (TimeInfo->isWithinPeriod(year, step))
      timeid = TimeInfo->calcSteps(year, step);
    else
      keepdata = 0;

    //if tmplength is in lenindex find lengthid, else dont keep the data
    lenid = -1;
    for (i = 0; i < lenindex.Size(); i++)
      if (strcasecmp(lenindex[i], tmplength) == 0)
        lenid = i;

    if (lenid == -1)
      keepdata = 0;

    //only keep the data if area is a required area
    areaid = -1;
    tmpareaid = Area->getInnerArea(area);
    for (i = 0; i < tmpareas.Size(); i++)
      if (tmpareas[i] == tmpareaid)
        areaid = i;

    if (areaid == -1)
      keepdata = 0;

    if (keepdata == 1) {
      count++;
      infile >> (*amount[areaid])[timeid][lenid] >> ws;

    } else { //data not required - skip rest of line
      reject++;
      infile.get(c);
      while (c != '\n' && !infile.eof())
        infile.get(c);
      infile >> ws;
    }
  }

  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in growthamounts - found no data in the data file");
  if (reject != 0)
    handle.logMessage(LOGMESSAGE, "Discarded invalid growth data - number of invalid entries", reject);
  handle.logMessage(LOGMESSAGE, "Read growth data file - number of entries", count);
}
