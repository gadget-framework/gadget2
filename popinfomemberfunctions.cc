#include "popinfovector.h"
#include "popinfoindexvector.h"
#include "conversionindex.h"
#include "mathfunc.h"
#include "popinfo.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

//Pre: CI maps from Number to 'this' and Number has finer or even resolution than 'this'.
void PopInfoVector::Sum(const PopInfoVector* const Number, const ConversionIndex& CI) {
  int i;
  for (i = 0; i < size; i++) {
    v[i].N = 0.0;
    v[i].W = 0.0;
  }
  for (i = CI.minLength(); i < CI.maxLength(); i++)
    v[CI.getPos(i)] += (*Number)[i];
}

void PopInfoIndexVector::Add(const PopInfoIndexVector& Addition,
  const ConversionIndex& CI, double ratio) {

  PopInfo pop;
  int l, minl, maxl, offset;

  if (CI.isSameDl()) {   //Same dl on length distributions
    offset = CI.getOffset();
    minl = max(this->minCol(), Addition.minCol() + offset);
    maxl = min(this->maxCol(), Addition.maxCol() + offset);
    for (l = minl; l < maxl; l++) {
      pop = Addition[l - offset];
      pop *= ratio;
      v[l] += pop;
    }
  } else {             //Not same dl on length distributions
    if (CI.isFiner()) {
      //Stock that is added to has finer division than the stock that is added to it.
      minl = max(this->minCol(), CI.minPos(Addition.minCol()));
      maxl = min(this->maxCol(), CI.maxPos(Addition.maxCol() - 1) + 1);
      for (l = minl; l < maxl; l++) {
        pop = Addition[CI.getPos(l)];
        pop *= ratio;
        v[l] += pop;
        if (isZero(CI.Nrof(l)))
          handle.logMessage(LOGWARN, "Warning in popinfoindexvector - divide by zero");
        else
          v[l].N /= CI.Nrof(l);
      }
    } else {
      //Stock that is added to has coarser division than the stock that is added to it.
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

  PopInfo pop;
  int l, minl, maxl, offset;
  if (CI.isSameDl()) {   //Same dl on length distributions
    offset = CI.getOffset();
    minl = max(this->minCol(), Addition.minCol() + offset);
    maxl = min(this->maxCol(), Addition.maxCol() + offset, Ratio.Size());
    for (l = minl; l < maxl; l++) {
      pop = Addition[l - offset];
      pop *= (ratio * Ratio[l]);
      v[l] += pop;
    }
  } else {             //Not same dl on length distributions
    if (CI.isFiner()) {
      //Stock that is added to has finer division than the stock that is added to it.
      minl = max(this->minCol(), CI.minPos(Addition.minCol()));
      maxl = min(this->maxCol(), CI.maxPos(Addition.maxCol() - 1) + 1, Ratio.Size());
      for (l = minl; l < maxl; l++) {
        pop = Addition[CI.getPos(l)];
        pop *= (ratio * Ratio[l]);
        v[l] += pop;
        if (isZero(CI.Nrof(l)))
          handle.logMessage(LOGWARN, "Warning in popinfoindexvector - divide by zero");
        else
          v[l].N /= CI.Nrof(l);
      }
    } else {
      //Stock that is added to has coarser division than the stock that is added to it.
      minl = max(CI.minPos(this->minCol()), Addition.minCol());
      maxl = min(CI.maxPos(this->maxCol() - 1) + 1, Addition.maxCol(), Ratio.Size());
      for (l = minl; l < maxl; l++) {
        pop = Addition[l];
        pop *= (ratio * Ratio[l]);
        v[CI.getPos(l)] += pop;
      }
    }
  }
}
