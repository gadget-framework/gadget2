#include "timevariable.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

TimeVariable::TimeVariable()
  : ValuesReadFromFile(0), usemodelmatrix(0), timestepnr(1), time(1),
  firsttimestepnr(0), lastvalue(-1.0), value(0), description(0) {
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
    Formula* number = new Formula;
    infile.seekg(readPos);
    if (!(infile >> *number))
      handle.Message("Possible error in size of vector - didnt expect to find",text);
    number->Inform(keeper);
    number->Interchange(Value, keeper);
    delete number;
  } else {
    handle.Open(text);
    readFromFile(subcomment, TimeInfo, keeper);
    handle.Close();
  }
  keeper->clearLast();
}

void TimeVariable::readFromFile(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper) {
  int i, j;
  ValuesReadFromFile = 1;
  int nrofcoeff = 0;

  keeper->addString("timevariable");
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  int n = strlen(text) + 1;
  description = new char[n];
  for (i = 0; i < n; i++)
    description[i] = text[i];
  infile >> text;
  if (strcasecmp(text, "nrofcoeff") != 0)
    handle.Unexpected("nrofcoeff", text);
  infile >> nrofcoeff;
  if (nrofcoeff > 0) {
    usemodelmatrix = 1;
    coeff.resize(nrofcoeff, keeper);
    infile >> coeff;
    coeff.Inform(keeper);
  }
  infile >> ws >> text;
  if (strcasecmp(text, "data") != 0)
    handle.Unexpected("data", text);
  const char* errtext = "Something wrong with model matrix, years or steps in timevariable";
  i = 0;
  while (!infile.eof()) {
    years.resize(1);
    steps.resize(1);
    if (nrofcoeff > 0)
      modelmatrix.AddRows(1, nrofcoeff);
    values.resize(1, keeper);
    infile >> ws;
    infile >> years[i];
    if (infile.fail())
      handle.Message(errtext);
    infile >> steps[i];
    if (infile.fail())
      handle.Message(errtext);
    infile >> values[i];
    if (infile.fail())
      handle.Message(errtext);
    values[i].Inform(keeper);
    if (nrofcoeff > 0) {
      for (j = 0; j < nrofcoeff; j++) {
        infile >> modelmatrix[i][j];
        if (infile.fail())
          handle.Message(errtext);
      }
    }
    infile >> ws;
    i++;
  }

  //check if years and steps are not ordered in time.
  for (i = 0; i < years.Size() - 1; i++) {
    if ((years[i + 1] < years[i]) ||
       (years[i + 1] == years[i] && steps[i + 1] <= steps[i]))
      handle.Message("TimeVariable. Years and steps must be in right order");
  }
  firsttimestepnr = -1;
  for (i = 0; i < years.Size(); i++) {
    if (years[i] == TimeInfo->FirstYear() && steps[i] == TimeInfo->FirstStep()) {
      firsttimestepnr = i;
      break;
    }
  }
  if (firsttimestepnr == -1)
    handle.Message("TimeVariable. Something must be specified on first year and step in the simulation");
}

int TimeVariable::DidChange(const TimeClass* const TimeInfo) {
  return ((lastvalue != value && time == TimeInfo->CurrentTime()) || TimeInfo->CurrentTime() == 1);
}

int TimeVariable::AtCurrentTime(const TimeClass* const TimeInfo) const {
  int i;
  if (TimeInfo->CurrentTime() == 1)
    return 1;
  for (i = timestepnr; i < steps.Size(); i++)
    if (steps[i] == TimeInfo->CurrentStep() && years[i] == TimeInfo->CurrentYear())
      return 1;
  return 0;
}

void TimeVariable::Update() {
  assert(!ValuesReadFromFile);
  value = Value;
}

void TimeVariable::Update(const TimeClass* const TimeInfo) {
  int i;
  if (!ValuesReadFromFile)
    value = Value;
  else {
    if (TimeInfo->CurrentTime() == 1)
      timestepnr = firsttimestepnr;
    for (i = timestepnr; i < steps.Size(); i++) {
      if (steps[i] == TimeInfo->CurrentStep() && years[i] == TimeInfo->CurrentYear()) {
        timestepnr = i;
        time = TimeInfo->CurrentTime();
        break;
      }
    }
    lastvalue = value;
    value = 0;
    if (modelmatrix.Nrow() > 0)
      for (i = 0; i < modelmatrix.Ncol(); i++)
        value += coeff[i] * modelmatrix[timestepnr][i];

    value += values[timestepnr];
  }
}

void TimeVariable::Interchange(TimeVariable& Newtvar, Keeper* const keeper) const {
  int i, j;
  Newtvar.lastvalue = lastvalue;
  Newtvar.timestepnr = timestepnr;
  Newtvar.time = time;
  Newtvar.value = value;
  Newtvar.firsttimestepnr = firsttimestepnr;
  Value.Interchange(Newtvar.Value, keeper);
  if (ValuesReadFromFile) {
    Newtvar.description = new char[strlen(description) + 1];
    strcpy(Newtvar.description, description);
    Newtvar.ValuesReadFromFile = ValuesReadFromFile;
    Newtvar.years.resize(years.Size());
    Newtvar.steps.resize(steps.Size());
    for (i = 0; i < steps.Size(); i++) {
      Newtvar.steps[i] = steps[i];
      Newtvar.years[i] = years[i];
    }
    Newtvar.values.resize(values.Size(), keeper);
    for (i = 0; i < values.Size(); i++)
      values[i].Interchange(Newtvar.values[i], keeper);

    if (coeff.Size() > 0) {
      Newtvar.coeff.resize(coeff.Size(), keeper);
      for (i = 0; i < values.Size(); i++)
        coeff[i].Interchange(Newtvar.coeff[i], keeper);
      Newtvar.modelmatrix.AddRows(modelmatrix.Nrow(), modelmatrix.Ncol());
      for (i = 0; i < modelmatrix.Nrow(); i++)
        for (j = 0; j < modelmatrix.Ncol(); j++)
          Newtvar.modelmatrix[i][j] = modelmatrix[i][j];
    }
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

TimeVariable::~TimeVariable() {
  delete[] description;
}
