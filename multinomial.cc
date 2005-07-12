#include "multinomial.h"
#include "mathfunc.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

double Multinomial::calcLogLikelihood(const DoubleVector& data, const DoubleVector& dist) {

  int i;
  double minp = 1.0 / (dist.Size() * bigvalue);
  double sumdist = 0.0, sumdata = 0.0, sumlog = 0.0;
  double likely = 0.0;
  double tmpsum;

  if (data.Size() != dist.Size())
    handle.logMessage(LOGFAIL, "Error in multinomial - vectors not the same size");

  for (i = 0; i < data.Size(); i++) {
    sumdist += dist[i];
    sumdata += data[i];
    sumlog += logFactorial(data[i]);
  }

  if (isZero(sumdist))
    return 0.0;

  tmpsum = 1.0 / sumdist;
  for (i = 0; i < data.Size(); i++) {
    if (isZero(data[i]))
      likely += 0.0;
    else if (dist[i] * tmpsum >= minp)
      likely -= data[i] * log(dist[i] * tmpsum);
    else
      likely -= data[i] * log(minp);
  }

  sumlog -= logFactorial(sumdata);
  tmpsum = 2.0 * (likely + sumlog);
  if (tmpsum < 0)
    handle.logMessage(LOGWARN, "Warning in multinomial - negative total", tmpsum);

  loglikelihood += tmpsum;
  return tmpsum;
}
