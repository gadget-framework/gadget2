#include "readfunc.h"
#include "keeper.h"
#include "errorhandler.h"
#include "gadget.h"

doublematrix* ReadMatrix(CommentStream& infile, int x, int y) {
  doublematrix* M =  new doublematrix(x, y);
  if (ReadMatrix(infile, *M))
    return M;
  else {
    delete M;
    return 0;
  }
}

int ReadMatrix(CommentStream& infile, doublematrix& M) {
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
        cerr << "Warning: Could not read data file in ReadMatrix"
          << "\nLast character read was " << c << endl;
        return 0;
      }
    }
    c = infile.peek();
    while (c == ' ' || c == '\t')
      infile.get(c);
    if (c != '\n') {
      cerr << "Expected to find end of line after " << N << " in line "
        << i + 1 << " in ReadMatrix.\nFound instead " << c << endl;
      return 0;
    }
  }
  return 1;
}

doublevector* ReadVector(CommentStream& infile, int length) {
  doublevector *Vec = new doublevector(length);
  if (ReadVector(infile, *Vec))
    return Vec;
  else {
    delete Vec;
    return 0;
  }
}

int ReadVector(CommentStream& infile, intvector& Vec) {
  if (infile.fail())
    return 0;
  int i;
  for (i = 0; i < Vec.Size(); i++) {
    infile >> Vec[i];
    if (infile.fail()) {
      cerr << "Error occurred when read element no " << i + 1
        << " out of " << Vec.Size() << " in ReadVector.\n";
      return 0;
    }
  }
  return 1;
}

int ReadVector(CommentStream& infile, doublevector& Vec) {
  if (infile.fail())
    return 0;
  int i;
  for (i = 0; i < Vec.Size(); i++) {
    infile >> Vec[i];
    if (infile.fail()) {
      cerr << "Error occurred when read element no " << i + 1
        << " out of " << Vec.Size() << " in ReadVector.\n";
      return 0;
    }
  }
  return 1;
}

doubleindexvector* ReadIndexVector(CommentStream& infile, int length, int minlength) {
  doubleindexvector *iv = new doubleindexvector(length, minlength);
  if (ReadIndexVector(infile, *iv))
    return iv;
  else {
    delete iv;
    return 0;
  }
}

int ReadIndexVector(CommentStream& infile, doubleindexvector& Vec) {
  if (infile.fail())
    return 0;

  int lower = Vec.Mincol();
  int upper = Vec.Maxcol();
  int i;
  for (i = lower; i < upper; i++) {
    infile >> Vec[i];
    if (infile.fail()) {
      cerr << "Error occurred when read element no " << i + 1 - lower
        << " out of " << upper - lower << " in ReadIndexVector.\n";
      return 0;
    }
  }
  return 1;
}

int ReadVectorInLine(CommentStream& infile, intvector& Vec) {
  if (infile.fail())
    return 0;
  char line[MaxStrLength];
  strncpy(line, "", MaxStrLength);
  infile.getline(line, MaxStrLength);
  if (infile.fail()) {
    cerr << "Error occurred when reading a vector in line.\n";
    return 0;
  }
  istrstream istr(line);
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

int ReadVectorInLine(CommentStream& infile, doublevector& Vec) {
  if (infile.fail())
    return 0;
  char line[MaxStrLength];
  strncpy(line, "", MaxStrLength);
  infile.getline(line, MaxStrLength);
  if (infile.fail()) {
    cerr << "Error occurred when reading a vector in line.\n";
    return 0;
  }
  istrstream istr(line);
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

int Read2ColVector(CommentStream& infile, doublematrix& M) {
  int i, j;
  double N;
  infile >> ws;
  i = 0;

  //Check the number of columns in the inputfile
  if (!(CheckColumns(infile, 2)))
    cerr << "Wrong number of columns in inputfile - should be 2\n";

  while (!infile.eof()) {
    M.AddRows(1, 2);
    for (j = 0; j < 2; j++) {
      infile >> ws >> N;
      if (!infile.fail())
        M[i][j] = N;
      else {
        cerr << "Warning: Could not read data file in Read2ColVector\n";
        return 0;
      }
    }
    infile >> ws;
    i++;
  }
  return 1;
}

int ReadTextInLine(CommentStream& infile, charptrvector& text) {
  if (infile.fail())
    return 0;
  char line[MaxStrLength];
  strncpy(line, "", MaxStrLength);
  infile.getline(line, MaxStrLength);
  if (infile.fail()){
    cerr << "Error occurred when reading a text in line.\n";
    return 0;
  }
  istrstream istr(line);
  istr >> ws;
  int i = 0;
  while (!istr.eof()) {
    if (i == text.Size()) {
      text.resize(1);
      text[i] = new char[MaxStrLength];
    }
    istr >> text[i];
    if (istr.fail()) {
      cerr << "Error occurred when read element no " << i + 1 << " in ReadTextInLine.\n";
      return 0;
    }
    istr >> ws;
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

/* This function checks the number of entries on a row in the input file
 * This is done to check the number of columns that are being read.
 * We assume that every row has the same number of entries so we only
 * need to check the file once, but this can be changed */

int CheckColumns(CommentStream& infile, int count) {
  if (infile.fail())
    return 0;

  char line[MaxStrLength];
  char temp[MaxStrLength];
  strncpy(line, "", MaxStrLength);
  strncpy(temp, "", MaxStrLength);
  streampos pos;
  int result = 0;

  pos = infile.tellg();
  infile >> ws;
  infile.getline(line, MaxStrLength);
  if (infile.fail())
    return 0;

  istrstream istr(line);
  istr >> ws;
  int i = 0;
  while (!istr.eof()) {
    istr >> temp;
    if (istr.fail() && !istr.eof())
      return 0;
    istr >> ws;
    i++;
  }

  if (i == count)
    result = 1;
  infile.seekg(pos);
  return result;
}
