#include "suitfuncptrvector.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

SuitFuncPtrVector::~SuitFuncPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void SuitFuncPtrVector::resize(int addsize, SuitFunc* value) {
  int oldsize = size;
  this->resize(addsize);
  int i = 0;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void SuitFuncPtrVector::resize(int addsize) {
  if (v == 0) {
    size = addsize;
    v = new SuitFunc*[size];

  } else if (addsize > 0) {
    SuitFunc** vnew = new SuitFunc*[addsize + size];
    int i = 0;
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void SuitFuncPtrVector::Delete(int pos, Keeper* const keeper) {
  int i;
  if (size > 1) {
    SuitFunc** vnew = new SuitFunc*[size - 1];
    for (i = 0; i < pos; i++)
      vnew[i] = v[i];
    for (i = pos; i < size - 1; i++)
      vnew[i] = v[i + 1];
    for (i = 0; i < v[pos]->numConstants(); i++)
      v[pos]->getConstants()[i].Delete(keeper);
    delete[] v;
    v = vnew;
    size--;
  } else {
    delete[] v;
    v = 0;
    size = 0;
  }
}

int SuitFuncPtrVector::readSuitFunction(CommentStream& infile,
  const char* suitname, const TimeClass* const TimeInfo, Keeper* const keeper) {

  int found = 0;
  SuitFunc* tempFunc = 0;

  if ((strcasecmp(suitname, "expsuitfunca") == 0) || (strcasecmp(suitname, "exponential") == 0)) {
    tempFunc = new ExpSuitFuncA();
    found++;

  } else if ((strcasecmp(suitname, "constsuitfunc") == 0) || (strcasecmp(suitname, "constant") == 0)) {
    tempFunc = new ConstSuitFunc();
    found++;

  } else if (strcasecmp(suitname, "straightline") == 0) {
    tempFunc = new StraightSuitFunc();
    found++;

  } else if (strcasecmp(suitname, "straightlineunbounded") == 0) {
    tempFunc = new StraightUnboundedSuitFunc();
    found++;

  } else if ((strcasecmp(suitname, "expsuitfuncl50") == 0) || (strcasecmp(suitname, "exponentiall50") == 0)) {
    tempFunc = new ExpSuitFuncL50();
    found++;

  } else if ((strcasecmp(suitname, "andersensuitfunc") == 0) || (strcasecmp(suitname, "andersen") == 0)) {
    tempFunc = new AndersenSuitFunc();
    found++;

  } else if (strcasecmp(suitname, "richards") == 0) {
    tempFunc = new RichardsSuitFunc();
    found++;

  } else if (strcasecmp(suitname, "gamma") == 0) {
    tempFunc = new GammaSuitFunc();
    found++;

  } else if (strcasecmp(suitname, "inverse") == 0) {
    handle.logFileMessage(LOGWARN, "\nThe inverse suitability function is not yet implemented");

  } else if (strcasecmp(suitname, "improvedexpsuitfunc") == 0) {
    handle.logFileMessage(LOGWARN, "\nThe improvedexpsuitfunc suitability function is no longer supported");

  } else if (strcasecmp(suitname, "improvedandextendedexpsuitfunc") == 0) {
    handle.logFileMessage(LOGWARN, "\nThe improvedandextendedexpsuitfunc suitability function is no longer supported");

  } else if (strcasecmp(suitname, "extendedexpsuitfuncl50") == 0) {
    handle.logFileMessage(LOGWARN, "\nThe extendedexpsuitfuncl50 suitability function is no longer supported");

  } else if (strcasecmp(suitname, "badexpsuitfuncl50") == 0) {
    handle.logFileMessage(LOGWARN, "\nThe badexpsuitfuncl50 suitability function is no longer supported");

  } else if (strcasecmp(suitname, "surveyselection") == 0) {
    handle.logFileMessage(LOGWARN, "\nThe surveyselection suitability function is no longer supported");

  } else if (strcasecmp(suitname, "cloglog") == 0) {
    handle.logFileMessage(LOGWARN, "\nThe cloglog suitability function is no longer supported");

  } else if (strcasecmp(suitname, "combination") == 0) {
    handle.logFileMessage(LOGWARN, "\nThe combination suitability function is no longer supported");

  } else
    handle.logFileMessage(LOGFAIL, "no valid suitability function found");

  if (found == 1) {
    tempFunc->readConstants(infile, TimeInfo, keeper);
    this->resize(1, tempFunc);
  }
  return found;
}
