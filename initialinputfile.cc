#include "initialinputfile.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

int InitialInputFile::isDataLeft() {
  if (!repeatedValues)
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

  infile >> ws;
  if (infile.eof() || infile.fail()) {
    handle.logMessage(LOGFAIL, "Error in initial input file - found no parameters in data file");

  } else if (isdigit(infile.peek())) {
    repeatedValues = 1;

  } else {
    char text[MaxStrLength];
    char textInLine[LongString];
    strncpy(text, "", MaxStrLength);
    strncpy(textInLine, "", LongString);

    infile.getLine(textInLine, LongString);
    if (infile.fail())
      handle.logMessage(LOGFAIL, "Error in initial input file - line too long");

    istringstream line(textInLine);
    line >> text >> ws;
    if (strcasecmp(text, "switches") == 0) {
      // fileformat with switches and vector of values
      repeatedValues = 1;

      Parameter tmpparam;
      while (!line.eof()) {
        line >> tmpparam >> ws;
        if (line.fail() && !line.eof())
          handle.logMessage(LOGFAIL, "Error in initial input file - failed to read switches");
        switches.resize(tmpparam);
      }

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

  if (repeatedValues) {
    this->readNextLine();
    if ((switches.Size() > 0) && (switches.Size() != values.Size()))
      handle.logMessage(LOGFAIL, "Error in initial input file - failed to read switches");

  } else {
    Parameter sw;
    double val, lower, upper;
    int opt, check, rand;
    char c;
    char text[MaxStrLength];
    strncpy(text, "", MaxStrLength);

    rand = check = 0;
    while (check == 0) {
      infile >> sw >> ws;
      switches.resize(sw);
      if (infile.eof())
        check++;

      c = infile.peek();
      if (isdigit(c) || (c == '-')) {
        infile >> val >> ws;
        values.resize(1, val);
      } else if ((c == 'r') || (c == 'R')) {
        infile >> text >> ws;
        if (strcasecmp(text, "random") == 0)
          rand++;
        else
          check++;
      } else
        check++;

      if (infile.eof())
        check++;

      infile >> lower >> upper >> opt >> ws;
      lowerbound.resize(1, lower);
      upperbound.resize(1, upper);
      optimise.resize(1, opt);
      if (infile.eof())
        check++;

      if (rand) {
        // generate a random point somewhere between the bounds
        val = lower + (randomNumber() * (upper - lower));
        handle.logMessage(LOGMESSAGE, "Generating a random starting point", val, sw.getName());
        values.resize(1, val);
        rand = 0;
      }
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

    // check that the names of the switches are unique
    int i, j;
    for (i = 0; i < switches.Size(); i++)
      for (j = 0; j < switches.Size(); j++)
        if ((strcasecmp(switches[i].getName(), switches[j].getName()) == 0) && (i != j))
          handle.logMessage(LOGFAIL, "Error in initial input file - repeated switch", switches[i].getName());
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
  infile.getLine(text, LongString);
  if (infile.fail())
    handle.logMessage(LOGFAIL, "Error in initial input file - line too long");

  istringstream line(text);
  while (!line.eof() && !line.fail() && !(line.peek() == ';')) {
    line >> tempX;
    tempValues.resize(1, tempX);
    if (line.good())
      line >> ws;
  }
  if (line.fail() && !line.eof())
    handle.logMessage(LOGFAIL, "Error in initial input file - failed to read vector");
  values.Reset();
  values = tempValues;
}
