#include "readfunc.h"
#include "keeper.h"
#include "errorhandler.h"
#include "gadget.h"

DoubleMatrix* readMatrix(CommentStream& infile, int x, int y) {
  DoubleMatrix* M =  new DoubleMatrix(x, y);
  if (readMatrix(infile, *M))
    return M;
  else {
    delete M;
    return 0;
  }
}

int readMatrix(CommentStream& infile, DoubleMatrix& M) {
  infile >> ws;
  int x = M.Nrow();
  int y = M.Ncol();
  char c;
  int i, j;
  double N;
  for (i = 0; i < x; i++) {
    for (j = 0; j < y; j++) {
      infile >> ws;
      c = infile.peek();
      infile >> N;
      if (!infile.fail())
        M[i][j] = N;
      else {
        cerr << "Warning - could not read data file in readmatrix\n"
          << "Last character read was " << c << endl;
        return 0;
      }
    }
    c = infile.peek();
    while (c == ' ' || c == '\t')
      infile.get(c);
    if (c != '\n') {
      cerr << "Expected to find end of line after " << N << " in line "
        << i + 1 << " in readmatrix.\nFound instead " << c << endl;
      return 0;
    }
  }
  return 1;
}

DoubleVector* readVector(CommentStream& infile, int length) {
  DoubleVector *Vec = new DoubleVector(length);
  if (readVector(infile, *Vec))
    return Vec;
  else {
    delete Vec;
    return 0;
  }
}

int readVector(CommentStream& infile, IntVector& Vec) {
  if (infile.fail())
    return 0;
  int i;
  for (i = 0; i < Vec.Size(); i++) {
    infile >> Vec[i];
    if (infile.fail()) {
      cerr << "Error occurred when read element no " << i + 1
        << " out of " << Vec.Size() << " in readvector.\n";
      return 0;
    }
  }
  return 1;
}

int readVector(CommentStream& infile, DoubleVector& Vec) {
  if (infile.fail())
    return 0;
  int i;
  for (i = 0; i < Vec.Size(); i++) {
    infile >> Vec[i];
    if (infile.fail()) {
      cerr << "Error occurred when read element no " << i + 1
        << " out of " << Vec.Size() << " in readvector.\n";
      return 0;
    }
  }
  return 1;
}

DoubleIndexVector* readIndexVector(CommentStream& infile, int length, int minlength) {
  DoubleIndexVector *iv = new DoubleIndexVector(length, minlength);
  if (readIndexVector(infile, *iv))
    return iv;
  else {
    delete iv;
    return 0;
  }
}

int readIndexVector(CommentStream& infile, DoubleIndexVector& Vec) {
  if (infile.fail())
    return 0;

  int lower = Vec.Mincol();
  int upper = Vec.Maxcol();
  int i;
  for (i = lower; i < upper; i++) {
    infile >> Vec[i];
    if (infile.fail()) {
      cerr << "Error occurred when read element no " << i + 1 - lower
        << " out of " << upper - lower << " in readindexvector.\n";
      return 0;
    }
  }
  return 1;
}

int readVectorInLine(CommentStream& infile, IntVector& Vec) {
  if (infile.fail())
    return 0;
  char line[MaxStrLength];
  strncpy(line, "", MaxStrLength);
  infile.getline(line, MaxStrLength);
  if (infile.fail()) {
    cerr << "Error occurred when reading a vector in line.\n";
    return 0;
  }
  istringstream istr(line);
  istr >> ws;
  int i = 0;
  while (!istr.eof()) {
    if (i == Vec.Size())
      Vec.resize(1);
    istr >> Vec[i];
    if (istr.fail() && !istr.eof()) {
      cerr << "Error occurred when reading a vector in line.\n";
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
  char line[MaxStrLength];
  strncpy(line, "", MaxStrLength);
  infile.getline(line, MaxStrLength);
  if (infile.fail()) {
    cerr << "Error occurred when reading a vector in line.\n";
    return 0;
  }
  istringstream istr(line);
  istr >> ws;
  int i = 0;
  while (!istr.eof()) {
    if (i == Vec.Size())
      Vec.resize(1);
    istr >> Vec[i];
    if (istr.fail() && !istr.eof()) {
      cerr << "Error occurred when reading a vector in line.\n";
      return 0;
    }
    istr >> ws;
    i++;
  }
  return 1;
}

int read2ColVector(CommentStream& infile, DoubleMatrix& M) {
  int i, j;
  double N;
  infile >> ws;
  i = 0;

  //Check the number of columns in the inputfile
  if (countColumns(infile) != 2)
    cerr << "Wrong number of columns in inputfile - should be 2\n";

  while (!infile.eof()) {
    M.AddRows(1, 2);
    for (j = 0; j < 2; j++) {
      infile >> ws >> N;
      if (!infile.fail())
        M[i][j] = N;
      else {
        cerr << "Warning - could not read data file in read2colvector\n";
        return 0;
      }
    }
    infile >> ws;
    i++;
  }
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
    return 0; //Failure -- wrong format
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
    c = infile.peek();
    while (c != '\n' && !infile.eof())
      infile.get(c);
  } while ((ryear < year || (ryear == year && rstep < step)));
  infile.seekg(pos);
  return 1;  //Success
}

/* This function counts the number of entries on a row in the input file
 * This is done to check the number of columns that are being read.
 * We assume that every row has the same number of entries so we only
 * need to check the file once, but this can be changed */

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
  infile.getline(line, MaxStrLength);
  if (infile.fail())
    return 0;

  istringstream istr(line);
  istr >> ws;
  int i = 0;
  while (!istr.eof()) {
    istr >> temp;
    if (istr.fail() && !istr.eof())
      return 0;
    istr >> ws;
    i++;
  }

  infile.seekg(pos);
  return i;
}

int readAmounts(CommentStream& infile, const IntVector& tmpareas,
  const TimeClass* const TimeInfo, const AreaClass* const Area,
  FormulaMatrix& amount, Keeper* const keeper, const char* givenname) {

  ErrorHandler handle;
  int i;
  int year, step, area;
  Formula number;  //initialised to 0.0
  IntVector Years, Steps;
  char tmpname[MaxStrLength];
  char tmpnumber[MaxStrLength];
  strncpy(tmpname, "", MaxStrLength);
  strncpy(tmpnumber, "", MaxStrLength);
  int count = 0;
  int keepdata, timeid, areaid, tmpareaid;

  //Find start of distribution data in datafile
  infile >> ws;
  char c;
  c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  //Check the number of columns in the inputfile
  if (countColumns(infile) != 5)
    handle.Message("Wrong number of columns in inputfile - should be 5");

  //Create storage for the data - size of the data is known
  amount.AddRows(TimeInfo->TotalNoSteps() + 1, tmpareas.Size(), number);
  Years.resize(TimeInfo->TotalNoSteps() + 1, 0);
  Steps.resize(TimeInfo->TotalNoSteps() + 1, 0);
  year = TimeInfo->FirstYear();
  step = TimeInfo->FirstStep();
  int numsteps = TimeInfo->StepsInYear();
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
    if (TimeInfo->IsWithinPeriod(year, step)) {
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
    if (tmpareaid == -1)
      handle.UndefinedArea(area);
    for (i = 0; i < tmpareas.Size(); i++)
      if (tmpareas[i] == tmpareaid)
        areaid = i;

    if (areaid == -1)
      keepdata = 1;

    if (keepdata == 0) {
      //data is required, so store it
      count++;
      infile >> amount[timeid][areaid] >> ws;

    } else {
      //data is not required, so read but ignore it
      infile >> tmpnumber >> ws;
    }
  }

  if (count == 0)
    cerr << "Warning - found no data in the data file for " << givenname << endl;
  return 1;
}

int readGrowthAmounts(CommentStream& infile, const TimeClass* const TimeInfo,
  const AreaClass* const Area, FormulaMatrixPtrVector& amount,
  const CharPtrVector& lenindex, Keeper* const keeper) {

  ErrorHandler handle;
  int i;
  int year, step, area;
  IntVector Years, Steps;
  char tmplength[MaxStrLength];
  char tmpnumber[MaxStrLength];
  strncpy(tmplength, "", MaxStrLength);
  strncpy(tmpnumber, "", MaxStrLength);
  int keepdata, timeid, areaid, lenid;
  int count = 0;

  //Find start of distribution data in datafile
  infile >> ws;
  char c;
  c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  //Check the number of columns in the inputfile
  if (countColumns(infile) != 5)
    handle.Message("Wrong number of columns in inputfile - should be 5");

  //Create storage for the data - size of the data is known
  Years.resize(TimeInfo->TotalNoSteps() + 1, 0);
  Steps.resize(TimeInfo->TotalNoSteps() + 1, 0);
  year = TimeInfo->FirstYear();
  step = TimeInfo->FirstStep();
  int numsteps = TimeInfo->StepsInYear();
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
    if (TimeInfo->IsWithinPeriod(year, step)) {
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

    if (keepdata == 0) {
      //data is required, so store it
      areaid = Area->InnerArea(area);
      if (areaid == -1)
        handle.UndefinedArea(area);

      count++;
      infile >> (*amount[areaid])[timeid][lenid] >> ws;

    } else {
      //data is not required, so read but ignore it
      infile >> tmpnumber >> ws;
    }
  }
  if (count == 0)
    cerr << "Warning for growthamounts - found no data in the data file\n";
  return 1;
}
