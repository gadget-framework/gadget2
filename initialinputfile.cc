#include "initialinputfile.h"
#include "gadget.h"

int InitialInputFile::reachedEndOfFile() {
  infile >> ws;
  return infile.eof();
}

void InitialInputFile::getValues(ParameterVector& sw, DoubleVector& val,
  DoubleVector& low, DoubleVector& upp, IntVector& opt) {

  sw = switches;
  val = values;
  low = lowerbound;
  upp = upperbound;

  int i;
  opt.resize(optimise.Size());
  for (i = 0; i < optimise.Size(); i++)
    opt[i] = optimise[i];
}

void InitialInputFile::getVectorValue(DoubleVector& val) {
  val = values;
}

void InitialInputFile::getSwitchValue(ParameterVector& sw) {
  sw = switches;
}

InitialInputFile::InitialInputFile(const char* const filename) : header(0) {
  tmpinfile.open(filename, ios::in);
  infile.setStream(tmpinfile);
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
    char textInLine[LongString];
    strncpy(textInLine, "", LongString);
    infile.get(textInLine, LongString, '\n');
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
      if (!readVectorInLine(infile, switches)) {
        cerr << "Error in initialinput - failed to read switches\n";
        exit(EXIT_FAILURE);
      }
      repeatedValues = 1;
      infile >> ws;

    } else {
      // fileformat must be of the form
      // switch value lowerbound upperbound optimise
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

  this->readHeader();
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

    } else if (optimise.Size() != values.Size()) {
      cerr << "Error in initialinput - failed to read optimise\n";
      exit(EXIT_FAILURE);
    }

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

  char text[LongString];
  strncpy(text, "", LongString);
  infile.get(text, LongString, '\n');
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

int InitialInputFile::readSwitches() const {
  return (switches.Size() > 0);
}

// functions from here are only required when running in gadget from paramin
int InitialInputFile::numVariables() {
  return values.Size();
}

int InitialInputFile::Optimise(int i) const {
  return optimise[i];
}

double InitialInputFile::Values(int i) const {
  return values[i];
}

double InitialInputFile::Lower(int i) const {
  return lowerbound[i];
}

double InitialInputFile::Upper(int i) const {
  return upperbound[i];
}

Parameter InitialInputFile::Switches(int i) const {
  return switches[i];
}
