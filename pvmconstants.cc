#include "pvmconstants.h"

#ifdef GADGET_NETWORK
#include "gadget.h"
#else
#include "paramin.h"
#endif

pvmconstants::pvmconstants() {
  endType = 1;
  startType = 2;
  taskDied = 3;
  masterSendNetDataVar = 4;
  masterSendString = 5;
  masterReceiveNetDataResult = 6;
  masterSendBound = 7;
  dataencode = 1;
}

pvmconstants::~pvmconstants() {
}

const void pvmconstants::setDataEncode(int dc) {
  if (dc == 0 || dc == 1 || dc == 2)
    dataencode = dc;
  else {
    cerr << "Illegal value for DataEncode\n";
    exit(EXIT_FAILURE);
  }
}

const int pvmconstants::getDiedTag() const {
  return taskDied;
}

const int pvmconstants::getStopTag() const {
  return endType;
}

const int pvmconstants::getStartTag() const {
  return startType;
}

const int pvmconstants::getMasterSendVarTag() const {
  return masterSendNetDataVar;
}

const int pvmconstants::getMasterSendStringTag() const {
  return masterSendString;
}

const int pvmconstants::getMasterSendBoundTag() const {
  return masterSendBound;
}

const int pvmconstants::getMasterReceiveDataTag() const {
  return masterReceiveNetDataResult;
}

const int pvmconstants::getDataEncode() const {
  return dataencode;
}
