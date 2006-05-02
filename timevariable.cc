#include "timevariable.h"
#include "errorhandler.h"
#include "readword.h"
#include "gadget.h"

extern ErrorHandler handle;

TimeVariable::TimeVariable()
  : fromfile(0), usemodelmatrix(0), timeid(1), time(1), lastvalue(-1.0), value(0.0) {
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
    Formula* f = new Formula();
    infile.seekg(readPos);
    if (!(infile >> *f))
      handle.logFileMessage(LOGWARN, "possible error in size of vector - didnt expect to find", text);
    f->Inform(keeper);
    f->Interchange(init, keeper);
    delete f;

  } else {
    handle.Open(text);
    this->readFromFile(subcomment, TimeInfo, keeper);
    handle.Close();
    subfile.close();
    subfile.clear();
  }
  keeper->clearLast();
}

void TimeVariable::readFromFile(CommentStream& infile,
  const TimeClass* const TimeInfo, Keeper* const keeper) {

  int i, j, check, tmpint = 0;
  fromfile = 1;
  int numcoeff = 0;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  //JMB need to initialise init to something
  init.setValue(0.0);
  init.Inform(keeper);

  infile >> text >> ws;  //description of time variable
  keeper->addString(text);

  char c = infile.peek();
  if ((c == 'n') || (c == 'N'))
    readWordAndVariable(infile, "nrofcoeff", numcoeff);

  if (numcoeff > 0) {
    usemodelmatrix = 1;
    coeff.resize(numcoeff, keeper);
    for (i = 0; i < numcoeff; i++)
      if (!(infile >> coeff[i]))
        handle.logFileMessage(LOGFAIL, "failed to read timevariable coefficient");
    coeff.Inform(keeper);
  }

  infile >> ws >> text;
  if (strcasecmp(text, "data") != 0)
    handle.logFileUnexpected(LOGFAIL, "data", text);

  i = 0;
  while (!infile.eof()) {
    infile >> ws >> tmpint;
    years.resize(1, tmpint);
    if (infile.fail())
      handle.logFileMessage(LOGFAIL, "failed to read timevariable year");
    infile >> ws >> tmpint;
    steps.resize(1, tmpint);
    if (infile.fail())
      handle.logFileMessage(LOGFAIL, "failed to read timevariable step");

    values.resize(1, keeper);
    infile >> ws >> values[i];
    if (infile.fail())
      handle.logFileMessage(LOGFAIL, "failed to read timevariable value");
    values[i].Inform(keeper);

    if (usemodelmatrix) {
      modelmatrix.AddRows(1, numcoeff, 0.0);
      for (j = 0; j < numcoeff; j++) {
        infile >> modelmatrix[i][j];
        if (infile.fail())
          handle.logFileMessage(LOGFAIL, "failed to read timevariable matrix");
      }
    }
    infile >> ws;
    i++;
  }
  keeper->clearLast();

  //check if years and steps are ordered in time
  for (i = 0; i < years.Size() - 1; i++)
    if ((years[i + 1] < years[i]) ||
       (years[i + 1] == years[i] && steps[i + 1] <= steps[i]))
      handle.logMessage(LOGFAIL, "Error in timevariable - years and steps are not increasing");

  check = -1;
  for (i = 0; i < years.Size(); i++)
    if (years[i] == TimeInfo->getFirstYear() && steps[i] == TimeInfo->getFirstStep())
      check = i;

  if (check == -1)
    handle.logMessage(LOGFAIL, "Error in timevariable - nothing specified for first timestep of the simulation");
}

int TimeVariable::didChange(const TimeClass* const TimeInfo) {
  if (TimeInfo->getTime() == 1)
    return 1;  //return true for the first timestep
  if (!fromfile)
    return 0;  //return false if the values were not read from file

  return ((!(isEqual(lastvalue, value))) && (time == TimeInfo->getTime()));
}

void TimeVariable::Update(const TimeClass* const TimeInfo) {
  int i;
  if (!fromfile)
    value = init;
  else {
    if (TimeInfo->getTime() == 1)
      timeid = 0;
    for (i = timeid; i < steps.Size(); i++) {
      if (steps[i] == TimeInfo->getStep() && years[i] == TimeInfo->getYear()) {
        timeid = i;
        time = TimeInfo->getTime();
        break;
      }
    }
    lastvalue = value;
    value = 0.0;
    if (usemodelmatrix)
      for (i = 0; i < coeff.Size(); i++)
        value += coeff[i] * modelmatrix[timeid][i];

    value += values[timeid];
  }
}

void TimeVariable::Delete(Keeper* const keeper) const {
  int i;
  for (i = 0; i < coeff.Size(); i++)
    coeff[i].Delete(keeper);
  for (i = 0; i < values.Size(); i++)
    values[i].Delete(keeper);
  init.Delete(keeper);
}

void TimeVariable::Interchange(TimeVariable& newTV, Keeper* const keeper) const {
  int i, j;
  newTV.lastvalue = lastvalue;
  newTV.timeid = timeid;
  newTV.time = time;
  newTV.value = value;
  init.Interchange(newTV.init, keeper);
  if (fromfile) {
    newTV.fromfile = 1;
    newTV.usemodelmatrix = usemodelmatrix;
    newTV.years = years;
    newTV.steps = steps;
    newTV.values.resize(values.Size(), keeper);
    for (i = 0; i < steps.Size(); i++)
      values[i].Interchange(newTV.values[i], keeper);

    if (usemodelmatrix) {
      newTV.coeff.resize(coeff.Size(), keeper);
      for (i = 0; i < coeff.Size(); i++)
        coeff[i].Interchange(newTV.coeff[i], keeper);
      newTV.modelmatrix.AddRows(modelmatrix.Nrow(), modelmatrix.Ncol(), 0.0);
      for (i = 0; i < modelmatrix.Nrow(); i++)
        for (j = 0; j < modelmatrix.Ncol(i); j++)
          newTV.modelmatrix[i][j] = modelmatrix[i][j];
    }
  }
}
