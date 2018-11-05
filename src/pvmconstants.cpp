#include "pvmconstants.h"
#include "gadget.h"
#include <Rcpp.h>

PVMConstants::PVMConstants() {
  stopPVM = 1;
  startPVM = 2;
  taskDied = 3;
  masterSendVar = 4;
  masterSendString = 5;
  masterReceiveData = 6;
  masterSendBound = 7;
  dataEncode = 1;
}

void PVMConstants::setDataEncode(int dc) {
  if (dc == 0 || dc == 1 || dc == 2)
    dataEncode = dc;
  else {
    Rcpp::Rcerr << "Error in pvmconstants - invalid value for dataencode\n";
    Rcpp::stop(REXIT_FAILURE);
  }
}
