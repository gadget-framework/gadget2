#include "popratio.h"
#include "gadget.h"

PopRatio::PopRatio() {
  N = NULL;
  R = -1.0;
}

PopRatio& PopRatio::operator = (const PopRatio& a) {
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

PopRatio& PopRatio::operator += (const PopRatio& a) {
  assert(N != NULL);
  R = -1.0;
  *N = *N + *(a.N);
  return *this;
}

void PopRatio::operator -= (double a) {
  assert(N != NULL);
  *N -= a;
  R = -1.0;
}

void PopRatio::operator *= (double a) {
  assert(N != NULL);
  *N *= a;
  R = -1.0;
}
