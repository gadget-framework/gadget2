#include "timevariable.h"
#include "errorhandler.h"
#include "readword.h"
#include "gadget.h"

extern ErrorHandler handle;

TimeVariable::TimeVariable()
  : fromfile(0), usemodelmatrix(0), timestepnr(1), time(1),
    firsttimestepnr(0), lastvalue(-1.0), value(0.0) {
}

TimeVariable::~TimeVariable() {
}

void TimeVariable::read(CommentStream& infile,
  const TimeClass* const TimeInfo, Keeper* const keeper) {

  keeper->addString("timevariable");
  ifstream subfile;
  CommentStream subcomment(subfile);
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  streampos readPos;
  readPos = infile.tellg();
  infile >> text;
  subfile.open(text, ios::in);
  if (subfile.fail()) {
    Formula* number = new Formula();
    infile.seekg(readPos);
    if (!(infile >> *number))
      handle.logFileMessage(LOGFAIL, "Possible error in size of vector - didnt expect to find", text);
    number->Inform(keeper);
    number->Interchange(Value, keeper);
    delete number;
  } else {
    handle.Open(text);
    this->readFromFile(subcomment, TimeInfo, keeper);
    handle.Close();
  }
  keeper->clearLast();
}

void TimeVariable::readFromFile(CommentStream& infile,
  const TimeClass* const TimeInfo, Keeper* const keeper) {

  int i, j;
  fromfile = 1;
  int nrofcoeff = 0;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  infile >> text >> ws;  //description of time variable
  keeper->addString(text);

  char c = infile.peek();
  if ((c == 'n') || (c == 'N'))
    readWordAndVariable(infile, "nrofcoeff", nrofcoeff);
  else
    nrofcoeff = 0; //default value for nrofcoeff

  if (nrofcoeff > 0) {
    usemodelmatrix = 1;
    coeff.resize(nrofcoeff, keeper);
    infile >> coeff;
    coeff.Inform(keeper);
  }
  infile >> ws >> text;
  if (strcasecmp(text, "data") != 0)
    handle.logFileUnexpected(LOGFAIL, "data", text);

  i = 0;
  while (!infile.eof()) {
    years.resize(1);
    steps.resize(1);
    values.resize(1, keeper);

    infile >> ws >> years[i];
    if (infile.fail())
      handle.logFileMessage(LOGFAIL, "Error in timevariable - failed to read year");
    infile >> ws >> steps[i];
    if (infile.fail())
      handle.logFileMessage(LOGFAIL, "Error in timevariable - failed to read step");
    infile >> ws >> values[i];
    if (infile.fail())
      handle.logFileMessage(LOGFAIL, "Error in timevariable - failed to read value");
    values[i].Inform(keeper);

    if (usemodelmatrix) {
      modelmatrix.AddRows(1, nrofcoeff);
      for (j = 0; j < nrofcoeff; j++) {
        infile >> modelmatrix[i][j];
        if (infile.fail())
          handle.logFileMessage(LOGFAIL, "Error in timevariable - failed to read matrix");
      }
    }
    infile >> ws;
    i++;
  }
  keeper->clearLast();

  //check if years and steps are not ordered in time.
  for (i = 0; i < years.Size() - 1; i++) {
    if ((years[i + 1] < years[i]) ||
       (years[i + 1] == years[i] && steps[i + 1] <= steps[i]))
      handle.logFileMessage(LOGFAIL, "Error in timevariable - years and steps are not increasing");
  }

  firsttimestepnr = -1;
  for (i = 0; i < years.Size(); i++) {
    if (years[i] == TimeInfo->getFirstYear() && steps[i] == TimeInfo->getFirstStep()) {
      firsttimestepnr = i;
      break;
    }
  }
  if (firsttimestepnr == -1)
    handle.logFileMessage(LOGFAIL, "Error in timevariable - nothing specified for first timestep of the simulation");
}

int TimeVariable::didChange(const TimeClass* const TimeInfo) {
  if (TimeInfo->getTime() == 1)
    return 1;  //return true for the first timestep
  if (!fromfile)
    return 0;  //return false if the values were not read from file

  return ((!(isZero(lastvalue - value))) && (time == TimeInfo->getTime()));
}

void TimeVariable::Update(const TimeClass* const TimeInfo) {
  int i;
  if (!fromfile)
    value = Value;
  else {
    if (TimeInfo->getTime() == 1)
      timestepnr = firsttimestepnr;
    for (i = timestepnr; i < steps.Size(); i++) {
      if (steps[i] == TimeInfo->getStep() && years[i] == TimeInfo->getYear()) {
        timestepnr = i;
        time = TimeInfo->getTime();
        break;
      }
    }
    lastvalue = value;
    value = 0.0;
    if (usemodelmatrix)
      for (i = 0; i < modelmatrix.Ncol(); i++)
        value += coeff[i] * modelmatrix[timestepnr][i];

    value += values[timestepnr];
  }
}

void TimeVariable::Delete(Keeper* const keeper) const {
  int i;
  for (i = 0; i < coeff.Size(); i++)
    coeff[i].Delete(keeper);
  for (i = 0; i < values.Size(); i++)
    values[i].Delete(keeper);
  Value.Delete(keeper);
}

void TimeVariable::Interchange(TimeVariable& newTV, Keeper* const keeper) const {
  int i, j;
  newTV.lastvalue = lastvalue;
  newTV.timestepnr = timestepnr;
  newTV.time = time;
  newTV.value = value;
  newTV.firsttimestepnr = firsttimestepnr;
  Value.Interchange(newTV.Value, keeper);
  if (fromfile) {
    newTV.fromfile = 1;
    newTV.usemodelmatrix = usemodelmatrix;
    newTV.years.resize(years.Size());
    newTV.steps.resize(steps.Size());
    newTV.values.resize(values.Size(), keeper);
    for (i = 0; i < steps.Size(); i++) {
      newTV.steps[i] = steps[i];
      newTV.years[i] = years[i];
      values[i].Interchange(newTV.values[i], keeper);
    }

    if (usemodelmatrix) {
      newTV.coeff.resize(coeff.Size(), keeper);
      for (i = 0; i < values.Size(); i++)
        coeff[i].Interchange(newTV.coeff[i], keeper);
      newTV.modelmatrix.AddRows(modelmatrix.Nrow(), modelmatrix.Ncol());
      for (i = 0; i < modelmatrix.Nrow(); i++)
        for (j = 0; j < modelmatrix.Ncol(); j++)
          newTV.modelmatrix[i][j] = modelmatrix[i][j];
    }
  }
}
