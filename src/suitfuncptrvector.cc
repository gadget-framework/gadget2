#include "suitfuncptrvector.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

SuitFuncPtrVector::~SuitFuncPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void SuitFuncPtrVector::resize(SuitFunc* value) {
  int i;
  if (v == 0) {
    v = new SuitFunc*[1];
  } else {
    SuitFunc** vnew = new SuitFunc*[size + 1];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  v[size] = value;
  size++;
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

void SuitFuncPtrVector::readSuitFunction(CommentStream& infile,
  const char* suitname, const TimeClass* const TimeInfo, Keeper* const keeper) {

  SuitFunc* tempFunc = 0;

  if ((strcasecmp(suitname, "expsuitfunca") == 0) || (strcasecmp(suitname, "exponential") == 0)) {
    tempFunc = new ExpSuitFuncA();

  } else if ((strcasecmp(suitname, "constsuitfunc") == 0) || (strcasecmp(suitname, "constant") == 0)) {
    tempFunc = new ConstSuitFunc();

  } else if (strcasecmp(suitname, "straightline") == 0) {
    tempFunc = new StraightSuitFunc();

  } else if (strcasecmp(suitname, "straightlineunbounded") == 0) {
    tempFunc = new StraightUnboundedSuitFunc();

  } else if ((strcasecmp(suitname, "newexponentiall50") == 0) || (strcasecmp(suitname, "expsuitfuncl50") == 0) || (strcasecmp(suitname, "exponentiall50") == 0)){
    //    handle.logFileMessage(LOGWARN, "\nNote that this exponentiall50 has had a factor of 4 removed from the source code");

    tempFunc = new ExpSuitFuncL50();

  } else if ((strcasecmp(suitname, "andersensuitfunc") == 0) || (strcasecmp(suitname, "andersen") == 0)) {
    tempFunc = new AndersenSuitFunc();

  } else if (strcasecmp(suitname, "richards") == 0) {
    tempFunc = new RichardsSuitFunc();

  } else if (strcasecmp(suitname, "gamma") == 0) {
    tempFunc = new GammaSuitFunc();

  } else if (strcasecmp(suitname, "newinverse") == 0) {
    tempFunc = new InverseSuitFunc();

  } else if (strcasecmp(suitname, "andersenfleet") == 0) {
    tempFunc = new AndersenFleetSuitFunc();

    /*  } else if ((strcasecmp(suitname, "expsuitfuncl50") == 0) || (strcasecmp(suitname, "exponentiall50") == 0)) {
    handle.logFileMessage(LOGFAIL, "\nThe exponentiall50 suitability function is no longer supported\nUse the newexponentiall50 suitability function instead\nNote that this function has had a factor of 4 removed from the source code");
    */
  } else if (strcasecmp(suitname, "inverse") == 0) {
    handle.logFileMessage(LOGFAIL, "\nThe inverse suitability function is no longer supported\nUse the newinverse suitability function instead\nNote that this function has had a factor of 4 removed from the source code");

  } else if (strcasecmp(suitname, "improvedexpsuitfunc") == 0) {
    handle.logFileMessage(LOGFAIL, "\nThe improvedexpsuitfunc suitability function is no longer supported");

  } else if (strcasecmp(suitname, "improvedandextendedexpsuitfunc") == 0) {
    handle.logFileMessage(LOGFAIL, "\nThe improvedandextendedexpsuitfunc suitability function is no longer supported");

  } else if (strcasecmp(suitname, "extendedexpsuitfuncl50") == 0) {
    handle.logFileMessage(LOGFAIL, "\nThe extendedexpsuitfuncl50 suitability function is no longer supported");

  } else if (strcasecmp(suitname, "badexpsuitfuncl50") == 0) {
    handle.logFileMessage(LOGFAIL, "\nThe badexpsuitfuncl50 suitability function is no longer supported");

  } else if (strcasecmp(suitname, "surveyselection") == 0) {
    handle.logFileMessage(LOGFAIL, "\nThe surveyselection suitability function is no longer supported");

  } else if (strcasecmp(suitname, "cloglog") == 0) {
    handle.logFileMessage(LOGFAIL, "\nThe cloglog suitability function is no longer supported");

  } else if (strcasecmp(suitname, "combination") == 0) {
    handle.logFileMessage(LOGFAIL, "\nThe combination suitability function is no longer supported");

  } else
    handle.logFileMessage(LOGFAIL, "no valid suitability function found");

  tempFunc->readConstants(infile, TimeInfo, keeper);
  this->resize(tempFunc);
}
