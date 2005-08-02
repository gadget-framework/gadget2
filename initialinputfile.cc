#include "initialinputfile.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

int InitialInputFile::isDataLeft() {
  if (repeatedValues == 0)
    return 0;
  infile >> ws;
  if (infile.eof())
    return 0;
  return 1;
}

void InitialInputFile::getVectors(ParameterVector& sw, DoubleVector& val,
  DoubleVector& low, DoubleVector& upp, IntVector& opt) {

  sw = switches;
  val = values;
  low = lowerbound;
  upp = upperbound;
  opt = optimise;
}

void InitialInputFile::getValues(DoubleVector& val) {
  val = values;
}

void InitialInputFile::getSwitches(ParameterVector& sw) {
  sw = switches;
}

InitialInputFile::InitialInputFile(const char* const filename) {
  tmpinfile.open(filename, ios::in);
  handle.checkIfFailure(tmpinfile, filename);
  infile.setStream(tmpinfile);
  handle.Open(filename);
}

InitialInputFile::~InitialInputFile() {
  tmpinfile.close();
  tmpinfile.clear();
}

void InitialInputFile::readHeader() {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  infile >> ws;
  if (isdigit(infile.peek())) {
    repeatedValues = 1;

  } else {
    char textInLine[LongString];
    strncpy(textInLine, "", LongString);
    infile.get(textInLine, LongString, '\n');
    if (!infile.eof() && infile.peek() != '\n')
      handle.logMessage(LOGFAIL, "Error in initial input file - line too long");

    istringstream line(textInLine);
    line >> text >> ws;
    if (strcasecmp(text, "switches") == 0) {
      // fileformat with switches and vector value/values.
      repeatedValues = 1;
      infile >> ws;
      if (switches.readline(infile) == 0)
        handle.logMessage(LOGFAIL, "Error in initial input file - failed to read switches");

    } else {
      // fileformat must be of the form
      // switch value lowerbound upperbound optimise
      repeatedValues = 0;
      if (strcasecmp(text, "switch") != 0)
        handle.logFileUnexpected(LOGFAIL, "switch", text);

      line >> text >> ws;
      if (strcasecmp(text, "value") != 0)
        handle.logFileUnexpected(LOGFAIL, "value", text);

      line >> text >> ws;
      if (strcasecmp(text, "lower") != 0)
        handle.logFileUnexpected(LOGFAIL, "lower", text);

      line >> text >> ws;
      if (strcasecmp(text, "upper") != 0)
        handle.logFileUnexpected(LOGFAIL, "upper", text);

      line >> text >> ws;
      if ((strcasecmp(text, "optimise") != 0) && (strcasecmp(text, "optimize") != 0))
        handle.logFileUnexpected(LOGFAIL, "optimise", text);

      if (!line.eof())
        handle.logFileUnexpected(LOGFAIL, "<end of line>", text);
    }
  }
}

void InitialInputFile::readFromFile() {

  this->readHeader();
  infile >> ws;
  if (repeatedValues == 0) {

    Parameter sw;
    double val;
    double lower;
    double upper;
    int opt;
    int check = 0;

    infile >> ws;
    while (check == 0) {
      infile >> sw >> ws;
      switches.resize(1, sw);
      if (infile.eof())
        check++;

      if (isdigit(infile.peek()) || (infile.peek() == '-')) {
        infile >> val >> ws;
        values.resize(1, val);
      } else
        check++;
      if (infile.eof())
        check++;

      if (isdigit(infile.peek()) || (infile.peek() == '-')) {
        infile >> lower >> ws;
        lowerbound.resize(1, lower);
      } else
        check++;
      if (infile.eof())
        check++;

      if (isdigit(infile.peek()) || (infile.peek() == '-')) {
        infile >> upper >> ws;
        upperbound.resize(1, upper);
      } else
        check++;
      if (infile.eof())
        check++;

      if (isdigit(infile.peek())) {
        infile >> opt >> ws;
        optimise.resize(1, opt);
      } else
        check++;
      if (infile.eof())
        check++;
    }

    // check that the parameters have been read correctly
    if (upperbound.Size() != lowerbound.Size())
      handle.logMessage(LOGFAIL, "Error in initial input file - failed to read bounds data");
    else if (upperbound.Size() == 0)
      handle.logMessage(LOGFAIL, "Error in initial input file - failed to read bounds data");
    else if (values.Size() != switches.Size())
      handle.logMessage(LOGFAIL, "Error in initial input file - failed to read switches data");
    else if (values.Size() != lowerbound.Size())
      handle.logMessage(LOGFAIL, "Error in initial input file - failed to read bounds data");
    else if (optimise.Size() != values.Size())
      handle.logMessage(LOGFAIL, "Error in initial input file - failed to read optimise data");

    //check that the names of the switches are unique
    int i, j;
    for (i = 0; i < switches.Size(); i++)
      for (j = 0; j < switches.Size(); j++)
        if ((strcasecmp(switches[i].getName(), switches[j].getName()) == 0) && (i != j))
          handle.logMessage(LOGFAIL, "Error in initial input file - repeated switch", switches[i].getName());

  } else {
    this->readNextLine();
    if ((switches.Size() > 0) && (switches.Size() != values.Size()))
      handle.logMessage(LOGFAIL, "Error in initial input file - failed to read swotches");
  }

  //JMB close the errorhandler to clear the filename from the stack
  handle.Close();
}

void InitialInputFile::readNextLine() {
  int i;
  double tempX;
  DoubleVector tempValues;

  infile >> ws;
  if (infile.eof())
    handle.logMessage(LOGFAIL, "Error in initial input file - failed to read vector");

  char text[LongString];
  strncpy(text, "", LongString);
  infile.get(text, LongString, '\n');
  if (!infile.eof() && infile.peek() != '\n')
    handle.logMessage(LOGFAIL, "Error in initial input file - line too long");

  istringstream line(text);
  while (!line.eof() && !line.fail() && !(line.peek() == ';')) {
    line >> tempX;
    tempValues.resize(1, tempX);
    if (line.good())
      line >> ws;
  }

  if (values.Size() == 0)
    values.resize(tempValues.Size());

  if ((line.fail() && !line.eof()) || tempValues.Size() != values.Size())
    handle.logMessage(LOGFAIL, "Error in initial input file - failed to read vector");

  for (i = 0; i < tempValues.Size(); i++)
    values[i] = tempValues[i];
}
