#include "multinomial.h"
#include "mathfunc.h"
#include "gadget.h"

double Multinomial::LogLikelihood(const doublevector& data, const doublevector& dist) {

  int i;
  double minp = 1.0 / (dist.Size() * BIGVALUE);
  double sumdist = 0.0, sumdata = 0.0, sumlog = 0.0;
  double likely = 0.0;
  double tmpsum;

  for (i = 0; i < data.Size(); i++) {
    sumdist += dist[i];
    sumdata += data[i];
    sumlog += logfactorial(data[i]);
  }
  sumlog -= logfactorial(sumdata);

  if (iszero(sumdist)) {
    error = 1;
    return 0.0;
  }

  tmpsum = 1 / sumdist;
  for (i = 0; i < data.Size(); i++)
    if (dist[i] * tmpsum >= minp)
      likely -= data[i] * log(dist[i] * tmpsum);
    else if (data[i] != 0.0)
      likely -= data[i] * log(minp);

  loglikelihood += 2.0 * (likely + sumlog);
  return likely;
}
