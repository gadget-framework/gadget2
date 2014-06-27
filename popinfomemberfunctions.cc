#include "popinfovector.h"
#include "popinfoindexvector.h"
#include "conversionindex.h"
#include "mathfunc.h"
#include "popinfo.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

void PopInfoVector::Sum(const PopInfoVector* const Number, const ConversionIndex& CI) {
  int i;
  for (i = 0; i < size; i++)
    v[i].setToZero();
  for (i = CI.minLength(); i < CI.maxLength(); i++)
    v[CI.getPos(i)] += (*Number)[i];
}

void PopInfoIndexVector::Add(const PopInfoIndexVector& Addition,
  const ConversionIndex& CI, double ratio) {

  if (isZero(ratio))
    return;

  PopInfo pop;
  int l, minl, maxl;

  if (CI.isSameDl()) {
    int offset = CI.getOffset();
    minl = max(this->minCol(), Addition.minCol() + offset);
    maxl = min(this->maxCol(), Addition.maxCol() + offset);
    for (l = minl; l < maxl; l++) {
      pop = Addition[l - offset];
      pop *= ratio;
      v[l] += pop;
    }

  } else {
    if (CI.isFiner()) {
      minl = max(this->minCol(), CI.minPos(Addition.minCol()));
      maxl = min(this->maxCol(), CI.maxPos(Addition.maxCol() - 1) + 1);
      for (l = minl; l < maxl; l++) {
        pop = Addition[CI.getPos(l)];
        pop *= ratio;
        v[l] += pop;
        v[l].N /= CI.getNumPos(l);  //JMB CI.getNumPos() should never be zero
      }

    } else {
      minl = max(CI.minPos(this->minCol()), Addition.minCol());
      maxl = min(CI.maxPos(this->maxCol() - 1) + 1, Addition.maxCol());
      for (l = minl; l < maxl; l++) {
        pop = Addition[l];
        pop *= ratio;
        v[CI.getPos(l)] += pop;
      }
    }
  }
}

void PopInfoIndexVector::Add(const PopInfoIndexVector& Addition,
  const ConversionIndex& CI, const DoubleVector& Ratio, double ratio) {

  if (isZero(ratio))
    return;

  PopInfo pop;
  int l, minl, maxl;

  if (CI.isSameDl()) {
    int offset = CI.getOffset();
    minl = max(this->minCol(), Addition.minCol() + offset);
    maxl = min(this->maxCol(), Addition.maxCol() + offset);
    if (maxl > Ratio.Size() + offset)
      maxl = Ratio.Size() + offset;

    for (l = minl; l < maxl; l++) {
      pop = Addition[l - offset];
      pop *= (ratio * Ratio[l - offset]);
      v[l] += pop;
    }

  } else {
    if (CI.isFiner()) {
      minl = max(this->minCol(), CI.minPos(Addition.minCol()));
      maxl = min(this->maxCol(), CI.maxPos(Addition.maxCol() - 1) + 1);

      for (l = minl; l < maxl; l++) {
        pop = Addition[CI.getPos(l)];
        pop *= (ratio * Ratio[CI.getPos(l)]);
        v[l] += pop;
        v[l].N /= CI.getNumPos(l);  //JMB CI.getNumPos() should never be zero
      }

    } else {
      minl = max(CI.minPos(this->minCol()), Addition.minCol());
      maxl = min(CI.maxPos(this->maxCol() - 1) + 1, Addition.maxCol());
      if (maxl > Ratio.Size())
        maxl = Ratio.Size();

      for (l = minl; l < maxl; l++) {
        pop = Addition[l];
        pop *= (ratio * Ratio[l]);
        v[CI.getPos(l)] += pop;
      }
    }
  }
}
