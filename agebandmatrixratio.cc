#include "intvector.h"
#include "agebandmatrixratio.h"
#include "commentstream.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "agebandmatrixratio.icc"
#endif

Agebandmatrixratio::Agebandmatrixratio(const Agebandmatrixratio& initial)
  : minage(initial.Minage()), nrow(initial.Nrow()) {

  int i;
  if (nrow > 0) {
    v = new popratioindexvector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new popratioindexvector(initial[i + minage]);
  } else
      v = 0;
}

Agebandmatrixratio::Agebandmatrixratio(int MinAge, const intvector& minl,
  const intvector& size) : minage(MinAge), nrow(size.Size()) {

  int i;
  if (nrow > 0) {
    v = new popratioindexvector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new popratioindexvector(size[i], minl[i]);
  } else
      v = 0;
}

Agebandmatrixratio::Agebandmatrixratio(int MinAge, const popratioindexvector& initial)
  : minage(MinAge), nrow(1) {

  v = new popratioindexvector*[1];
  v[0] = new popratioindexvector(initial);
}

Agebandmatrixratio::~Agebandmatrixratio() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

int Agebandmatrixratio::NrOfTagExp() const {
  int minlength;
  if (nrow > 0) {
    minlength = this->Minlength(minage);
    return (this->operator[](minage))[minlength].Size();
  } else
    return 0;
}
