#include "popratio.h"

PopRatio& PopRatio::operator = (const PopRatio& a) {
  if (a.N == NULL) {
    N = NULL;
    R = 0.0;

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
  *N += *(a.N);
  R = 0.0;
  return *this;
}
