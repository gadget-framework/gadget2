#include "initialinputfile.h"
#include "gadget.h"

int InitialInputFile::repeatedValuesFileFormat() {
  return repeatedValues;
}

int InitialInputFile::reachedEndOfFile() {
  infile >> ws;
  return infile.eof();
}

void InitialInputFile::getReadValues(ParameterVector& sw,
  DoubleVector& val, DoubleVector& low, DoubleVector& upp, IntVector& opt) {

  sw = switches;
  val = values;
  low = lowerbound;
  upp = upperbound;

  int i;
  opt.resize(optimize.Size());
  for (i = 0; i < optimize.Size(); i++)
    opt[i] = optimize[i];
}

void InitialInputFile::getVectorValue(DoubleVector& val) {
  val = values;
}

void InitialInputFile::getSwitchValue(ParameterVector& sw) {
  sw = switches;
}

InitialInputFile::InitialInputFile(const char* const filename) : header(0) {
  tmpinfile.open(filename);
  infile.SetStream(tmpinfile);
  if (infile.fail()) {
    cerr << "Error in initialinput - failed to open " << filename << endl;
    exit(EXIT_FAILURE);
  }
  repeatedValues = 0;
}

InitialInputFile::~InitialInputFile() {
  tmpinfile.close();
  tmpinfile.clear();
}

void InitialInputFile::correctHeaderText(int index, const char* name) {
  int i, correct;

  switch(index) {
    case 0:
      correct = strcasecmp(name, "switch");
      break;
    case 1:
      correct = strcasecmp(name, "value");
      break;
    case 2:
      correct = strcasecmp(name, "lower");
      break;
    case 3:
      correct = strcasecmp(name, "upper");
      break;
    case 4:
      correct = strcasecmp(name, "optimise");
      if (correct != 0)
        correct = strcasecmp(name, "optimize");
      break;
    default:
      correct = 1;
  }

  if (correct != 0) {
    cerr << "While reading inputfile expected to read header in exactly this order\n"
      << "switch value lower upper optimise\nBut the header just read was";

    for (i = 0; i < header.Size(); i++)
      cerr << sep << header[i];

    cerr << " from " << name << endl;
    exit(EXIT_FAILURE);
  }
}

void InitialInputFile::readHeader() {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, numColumns;

  infile >> ws;
  if (isdigit(infile.peek())) {
    repeatedValues = 1;

  } else {
    char textInLine[VeryLongString];
    strncpy(textInLine, "", VeryLongString);
    infile.get(textInLine, VeryLongString, '\n');
    if (!infile.eof() && infile.peek() != '\n') {
      cerr << "Error in initialinput - line to long in file\n";
      exit(EXIT_FAILURE);
    }
    istringstream line(textInLine);
    line >> text;
    if (line.good())
      line >> ws;

    if (strcasecmp(text, "switches") == 0 && (line.eof() || line.peek() == ';')) {
      // fileformat with switches and vector value/values.
      infile >> ws;
      if (!switches.ReadVectorInLine(infile)) {
        cerr << "Error in initialinput - failed to read switches\n";
        exit(EXIT_FAILURE);
      }
      repeatedValues = 1;
      infile >> ws;

    } else {
      // fileformat must be of the form
      // switch value lowerbound upperbound optimize
      repeatedValues = 0;
      numColumns = 0;
      correctHeaderText(numColumns, text);
      while (!line.eof() && !(line.peek() == ';')) {
        header.resize(1, text);
        line >> text;
        if (line.good())
          line >> ws;

        numColumns++;
        correctHeaderText(numColumns, text);
      }

      if (numColumns != 4) {
        cerr << "While reading inputfile expected to read header in exactly this order\n"
          << "switch value lower upper optimise\nBut the header just read was\n";

        for (i = 0; i < header.Size(); i++)
          cerr << header[i] << sep;

        cerr << "from " << text << endl;
        exit(EXIT_FAILURE);
      }
    }
  }
}

void InitialInputFile::readFromFile() {
  infile >> ws;
  this->readHeader();

  if (repeatedValues == 0) {

    double val;
    double lower;
    double upper;
    int opt;
    Parameter sw;

    infile >> ws;
    while (!infile.eof()) {
      infile >> sw >> ws;
      switches.resize(1, sw);
      infile >> val >> ws;
      values.resize(1, val);
      infile >> lower >> ws;
      lowerbound.resize(1, lower);
      infile >> upper >> ws;
      upperbound.resize(1, upper);
      infile >> opt >> ws;
      optimize.resize(1, opt);
    }
    checkIfFailure();

  } else {
    readVectorFromLine();
    if (this->readSwitches() == 1) {
      if (switches.Size() != values.Size()) {
        cerr << "Error in initialinput - failed to read switches\n";
        exit(EXIT_FAILURE);
      }
    }
  }
}

void InitialInputFile::readVectorFromLine() {
  int i;
  double tempX;
  DoubleVector tempValues;

  infile >> ws;
  if (infile.eof()) {
    cerr << "Error in initialinput - failed to read vector\n";
    exit(EXIT_FAILURE);
  }

  char text[VeryLongString];
  strncpy(text, "", VeryLongString);
  infile.get(text, VeryLongString, '\n');
  if (!infile.eof() && infile.peek() != '\n') {
    cerr << "Error in initialinput - line too long in file\n";
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
    cerr << "Error in initialinput - failed to read data\n";
    exit(EXIT_FAILURE);
  }

  infile >> ws;
  assert(tempValues.Size() == values.Size());
  for (i = 0; i < tempValues.Size(); i++)
    values[i] = tempValues[i];
}

void InitialInputFile::checkIfFailure() {
  if (upperbound.Size() != lowerbound.Size()) {
    cerr << "Error in initialinput - failed to read bounds\n";
    exit(EXIT_FAILURE);

  } else if (upperbound.Size() == 0) {
    cerr << "Error in initialinput - failed to read bounds\n";
    exit(EXIT_FAILURE);

  } else if (values.Size() != switches.Size()) {
    cerr << "Error in initialinput - failed to read switches\n";
    exit(EXIT_FAILURE);

  } else if (values.Size() != lowerbound.Size()) {
    cerr << "Error in initialinput - failed to read bounds\n";
    exit(EXIT_FAILURE);

  } else if (optimize.Size() != values.Size()) {
    cerr << "Error in initialinput - failed to read optimise\n";
    exit(EXIT_FAILURE);
  }
}

int InitialInputFile::NoVariables() const {
  return values.Size();
}

int InitialInputFile::readSwitches() const {
  return (switches.Size() > 0);
}

int InitialInputFile::Optimize(int i) const {
  assert((i < optimize.Size()) && (i >= 0));
  return optimize[i];
}

double InitialInputFile::Values(int i) const {
  assert((i < values.Size()) && (i >= 0));
  return values[i];
}

double InitialInputFile::Lower(int i) const {
  assert((i < lowerbound.Size()) && (i >= 0));
  return lowerbound[i];
}

double InitialInputFile::Upper(int i) const {
  assert((i < upperbound.Size()) && (i >= 0));
  return upperbound[i];
}

Parameter InitialInputFile::Switches(int i) const {
  assert((i < switches.Size()) && (i >= 0));
  return switches[i];
}
