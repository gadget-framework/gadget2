#include "popratio.h"
#include "gadget.h"

popratio::popratio() {
  N = NULL;
  R = -1.0;
}

popratio& popratio::operator = (const popratio& a) {
  if (a.N == NULL) {
    N = NULL;
    R = -1.0;

  } else {
    if (N == NULL)
      N = a.N;
    else
      *N = *(a.N);

    R = a.R;
  }
  return *this;
}

popratio& popratio::operator += (const popratio& a) {
  assert(N != NULL);
  R = -1.0;
  *N = *N + *(a.N);
  return *this;
}

void popratio::operator -= (double a) {
  assert(N != NULL);
  *N -= a;
  R = -1.0;
}

void popratio::operator *= (double a) {
  assert(N != NULL);
  *N *= a;
  R = -1.0;
}
