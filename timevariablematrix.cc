#include "timevariablematrix.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "timevariablematrix.icc"
#endif

//Adds a row to a timevariablematrix. Must be one row at time.
void TimeVariablematrix::AddRows(const TimeVariablevector& tvar, Keeper* const keeper) {
  int add = 1;
  TimeVariablevector** vnew = new TimeVariablevector*[nrow + add];
  int i, j;
  for (i = 0; i < nrow; i++) {
    vnew[i] = v[i];
    for (j = 0; j < Ncol(i); j++)
      (*v[i])[j].Interchange((*vnew[i])[j], keeper);
  }
  delete[] v;
  v = vnew;
  v[nrow] = new TimeVariablevector(tvar.Size());
  for (i = 0; i < tvar.Size(); i++)
    tvar[i].Interchange((*v[nrow])[i], keeper);
  nrow += add;
}

void TimeVariablematrix::DeleteRow(int row, Keeper* const keeper) {
  int i, j;
  assert(nrow > 0);
  assert(0 <= row && row < nrow);
  for (i = 0; i < Ncol(row); i++)
    (*v[row])[i].Delete(keeper);
  delete v[row];
  TimeVariablevector** vnew = new TimeVariablevector*[nrow - 1];
  for (i = 0; i < row; i++) {
    vnew[i] = v[i];
    for (j = 0; j < Ncol(i); j++)
      (*v[i])[j].Interchange((*vnew[i])[j], keeper);
  }
  for (i = row; i < nrow - 1; i++) {
    vnew[i] = v[i + 1];
    for (j = 0; j < Ncol(i); j++)
      (*v[i + 1])[j].Interchange((*vnew[i])[j], keeper);
  }
  delete[] v;
  v = vnew;
  nrow--;
}

TimeVariablematrix::~TimeVariablematrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}
