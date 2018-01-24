#include "multinomial.h"
#include "mathfunc.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

double Multinomial::calcLogLikelihood(const DoubleVector& data, const DoubleVector& dist) {

  int i;
  double minp = 1.0 / (dist.Size() * bigvalue);
  double sumdist, sumdata, sumlog, likely, tmp;

  if (data.Size() != dist.Size())
    handle.logMessage(LOGFAIL, "Error in multinomial - vectors not the same size");

  sumdist = sumdata = sumlog = likely = 0.0;
  for (i = 0; i < data.Size(); i++) {
    sumdist += dist[i];
    sumdata += data[i];
    sumlog += logFactorial(data[i]);
  }

  if (isZero(sumdist))
    return 0.0;

  tmp = 1.0 / sumdist;
  for (i = 0; i < data.Size(); i++) {
    if (isZero(data[i]))
      likely += 0.0;
    else if (((dist[i] * tmp) > minp) || (isEqual((dist[i] * tmp), minp)))
      likely -= data[i] * log(dist[i] * tmp);
    else
      likely -= data[i] * log(minp);
  }

  sumlog -= logFactorial(sumdata);
  tmp = 2.0 * (likely + sumlog);
  if (tmp < 0.0)
    handle.logMessage(LOGWARN, "Warning in multinomial - negative total", tmp);

  loglikelihood += tmp;
  return tmp;
}
