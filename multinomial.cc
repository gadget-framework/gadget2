#include "multinomial.h"
#include "mathfunc.h"
#include "gadget.h"

double Multinomial::LogLikelihood(const DoubleVector& data, const DoubleVector& dist) {

  int i;
  double minp = 1.0 / (dist.Size() * bigvalue);
  double sumdist = 0.0, sumdata = 0.0, sumlog = 0.0;
  double likely = 0.0;
  double tmpsum;

  assert(data.Size() == dist.Size());
  for (i = 0; i < data.Size(); i++) {
    sumdist += dist[i];
    sumdata += data[i];
    sumlog += logfactorial(data[i]);
  }

  if (iszero(sumdist)) {
    error = 1;
    return 0.0;
  }

  tmpsum = 1 / sumdist;
  for (i = 0; i < data.Size(); i++) {
    if (iszero(data[i]))
      likely += 0.0;
    else if (dist[i] * tmpsum >= minp)
      likely -= data[i] * log(dist[i] * tmpsum);
    else
      likely -= data[i] * log(minp);
  }

  sumlog -= logfactorial(sumdata);
  loglikelihood += 2.0 * (likely + sumlog);
  return likely;
}
