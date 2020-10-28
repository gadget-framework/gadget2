#include "modelvariable.h"
#include "errorhandler.h"
#include "readword.h"
#include "gadget.h"
#include "global.h"

ModelVariable::ModelVariable() {
  value = 0.0;
  mvtype = MVFORMULA;
}

void ModelVariable::setValue(double newValue) {
  if (mvtype != MVFORMULA)
    handle.logMessage(LOGFAIL, "Error in modelvariable - cannot set value for entries read from file");

  init.setValue(newValue);
  value = newValue;
}

void ModelVariable::read(CommentStream& infile,
  const TimeClass* const TimeInfo, Keeper* const keeper) {

  keeper->addString("modelvariable");
  ifstream subfile;
  CommentStream subcomment(subfile);
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  streampos readPos;
  readPos = infile.tellg();
  infile >> text;
  subfile.open(text, ios::binary);
  if (subfile.fail()) {
    // this is a single value that is read as a formula in case
    infile.seekg(readPos);
    if (!(infile >> init))
      handle.logFileMessage(LOGFAIL, "Error in size of vector - didnt expect to find", text);
    init.Inform(keeper);

  } else {
    // this is either a timevariable or a stockvariable that is read from file
    handle.Open(text);

    subcomment >> text >> ws;  // description of model variable
    keeper->addString(text);

    char c = subcomment.peek();
    int numcoeff = 0;
    if ((c == 'n') || (c == 'N')) {
      readWordAndVariable(subcomment, "nrofcoeff", numcoeff);
      subcomment >> ws;
      c = subcomment.peek();
      if (numcoeff > 0)
        handle.logFileMessage(LOGWARN, "The modelmatrix timevariable formulation is no longer supported");
    }

    // JMB optional multiplier
    if ((c == 'm') || (c == 'M'))
      readWordAndVariable(subcomment, "multiplier", init);
    else
      init.setValue(1.0);
    init.Inform(keeper);

    subcomment >> ws >> text;
    if ((strcasecmp(text, "timedata") == 0) || (strcasecmp(text, "data") == 0)) {
      // JMB this is a timevariable
      mvtype = MVTIME;
      TV.read(subcomment, TimeInfo, keeper);

    } else if (strcasecmp(text, "stockdata") == 0) {
      // JMB this is a stockvariable
      mvtype = MVSTOCK;
      SV.read(subcomment);

    } else
      handle.logFileUnexpected(LOGFAIL, "timedata or stockdata", text);

    keeper->clearLast();
    handle.Close();
    subfile.close();
    subfile.clear();
  }
  keeper->clearLast();
}

int ModelVariable::didChange(const TimeClass* const TimeInfo) {
  if (TimeInfo->getTime() == 1)
    return 1;  //return true for the first timestep

  switch (mvtype) {
    case MVFORMULA:
      return 0;  //return false if the values were not read from file
      break;
    case MVTIME:
      return TV.didChange(TimeInfo);  //return based on time step
      break;
    case MVSTOCK:
      return 1;  //return true if this is based on the stock population
      break;
    default:
      handle.logMessage(LOGWARN, "Warning in modelvariable - unrecognised type", mvtype);
      return 0;
      break;
  }
}

void ModelVariable::Update(const TimeClass* const TimeInfo) {
  switch (mvtype) {
    case MVFORMULA:
      value = init;
      break;
    case MVTIME:
      TV.Update(TimeInfo);
      value = TV.getValue();
      value *= init;  // JMB multiply by scaling factor
      break;
    case MVSTOCK:
      SV.Update();
      value = SV.getValue();
      value *= init;  // JMB multiply by scaling factor
      handle.logMessage(LOGDETAIL, "Value calculated in stock variable", value);
      break;
    default:
      handle.logMessage(LOGWARN, "Warning in modelvariable - unrecognised type", mvtype);
      break;
  }
}

void ModelVariable::Delete(Keeper* const keeper) const {
  init.Delete(keeper);
  if (mvtype == MVTIME)
    TV.Delete(keeper);
  if (mvtype == MVSTOCK)
    SV.Delete();
}

void ModelVariable::Interchange(ModelVariable& newMV, Keeper* const keeper) const {
  newMV.value = value;
  newMV.mvtype = mvtype;
  init.Interchange(newMV.init, keeper);
  if (mvtype == MVTIME)
    TV.Interchange(newMV.TV, keeper);
  if (mvtype == MVSTOCK)
    SV.Interchange(newMV.SV);
}
