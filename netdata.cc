#include "netdata.h"

#ifdef GADGET_NETWORK
#include "gadget.h"
#else
#include "paramin.h"
#endif

netDataVariables::netDataVariables(int numVar) {
  tag = -1;
  x_id = -1;
  if (numVar < 1) {
    cout << "Number of variables for netData must be positive\n";
    exit(EXIT_FAILURE);
  }
  x = new double[numVar];
}

netDataVariables::~netDataVariables() {
  delete[] x;
}

netDataResult::netDataResult() {
}

netDataResult::~netDataResult() {
}
