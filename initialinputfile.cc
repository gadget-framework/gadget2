#include "initialinputfile.h"
#include "gadget.h"

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
  repeatedValues = 0;
  tmpinfile.open(filename, ios::in);
  infile.setStream(tmpinfile);
  if (infile.fail()) {
    cerr << "Error in initial input file - failed to open " << filename << endl;
    exit(EXIT_FAILURE);
  }
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
    if (!infile.eof() && infile.peek() != '\n') {
      cerr << "Error in initial input file - line too long in file\n";
      exit(EXIT_FAILURE);
    }
    istringstream line(textInLine);
    line >> text >> ws;
    if (strcasecmp(text, "switches") == 0) {
      // fileformat with switches and vector value/values.
      repeatedValues = 1;
      infile >> ws;
      if (!readVectorInLine(infile, switches)) {
        cerr << "Error in initial input file - failed to read switches\n";
        exit(EXIT_FAILURE);
      }

    } else {
      // fileformat must be of the form
      // switch value lowerbound upperbound optimise
      repeatedValues = 0;
      if (strcasecmp(text, "switch") != 0) {
        cerr << "Error in initial input file - failed to read switch header information\n";
        exit(EXIT_FAILURE);
      }
      line >> text >> ws;
      if (strcasecmp(text, "value") != 0) {
        cerr << "Error in initial input file - failed to read value header information\n";
        exit(EXIT_FAILURE);
      }
      line >> text >> ws;
      if (strcasecmp(text, "lower") != 0) {
        cerr << "Error in initial input file - failed to read lower bound header information\n";
        exit(EXIT_FAILURE);
      }
      line >> text >> ws;
      if (strcasecmp(text, "upper") != 0) {
        cerr << "Error in initial input file - failed to read upper bound header information\n";
        exit(EXIT_FAILURE);
      }
      line >> text >> ws;
      if ((strcasecmp(text, "optimise") != 0) && (strcasecmp(text, "optimize") != 0)) {
        cerr << "Error in initial input file - failed to read optimise header information\n";
        exit(EXIT_FAILURE);
      }
      if (!line.eof()) {
        cerr << "Error in initial input file - failed to read header information\n";
        exit(EXIT_FAILURE);
      }
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
    if (upperbound.Size() != lowerbound.Size()) {
      cerr << "Error in initial input file - failed to read bounds\n";
      exit(EXIT_FAILURE);

    } else if (upperbound.Size() == 0) {
      cerr << "Error in initial input file - failed to read bounds\n";
      exit(EXIT_FAILURE);

    } else if (values.Size() != switches.Size()) {
      cerr << "Error in initial input file - failed to read switches\n";
      exit(EXIT_FAILURE);

    } else if (values.Size() != lowerbound.Size()) {
      cerr << "Error in initial input file - failed to read bounds\n";
      exit(EXIT_FAILURE);

    } else if (optimise.Size() != values.Size()) {
      cerr << "Error in initial input file - failed to read optimise\n";
      exit(EXIT_FAILURE);
    }

    //check that the names of the switches are unique
    int i, j;
    for (i = 0; i < switches.Size(); i++)
      for (j = 0; j < switches.Size(); j++)
        if ((strcasecmp(switches[i].getName(), switches[j].getName()) == 0) && (i != j)) {
          cerr << "Error in initial input file - repeated switch " << switches[i].getName() << endl;
          exit(EXIT_FAILURE);
        }

  } else {
    this->readNextLine();
    if ((switches.Size() > 0) && (switches.Size() != values.Size())) {
      cerr << "Error in initial input file - failed to read switches\n";
      exit(EXIT_FAILURE);
    }
  }
}

void InitialInputFile::readNextLine() {
  int i;
  double tempX;
  DoubleVector tempValues;

  infile >> ws;
  if (infile.eof()) {
    cerr << "Error in initial input file - failed to read vector\n";
    exit(EXIT_FAILURE);
  }

  char text[LongString];
  strncpy(text, "", LongString);
  infile.get(text, LongString, '\n');
  if (!infile.eof() && infile.peek() != '\n') {
    cerr << "Error in initial input file - line too long in file\n";
    exit(EXIT_FAILURE);
  }

  istringstream line(text);
  while (!line.eof() && !line.fail() && !(line.peek() == ';')) {
    line >> tempX;
    tempValues.resize(1, tempX);
    if (line.good())
      line >> ws;
  }

  if (values.Size() == 0)
    values.resize(tempValues.Size());

  if ((line.fail() && !line.eof()) || tempValues.Size() != values.Size()) {
    cerr << "Error in initial input file - failed to read data\n";
    exit(EXIT_FAILURE);
  }

  for (i = 0; i < tempValues.Size(); i++)
    values[i] = tempValues[i];
}
