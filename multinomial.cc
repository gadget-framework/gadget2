#include "multinomial.h"
#include "mathfunc.h"
#include "gadget.h"

double Multinomial::calcLogLikelihood(const DoubleVector& data, const DoubleVector& dist) {

  int i;
  double minp = 1.0 / (dist.Size() * bigvalue);
  double sumdist = 0.0, sumdata = 0.0, sumlog = 0.0;
  double likely = 0.0;
  double tmpsum;

  assert(data.Size() == dist.Size());
  for (i = 0; i < data.Size(); i++) {
    sumdist += dist[i];
    sumdata += data[i];
    sumlog += logFactorial(data[i]);
  }

  if (isZero(sumdist))
    return 0.0;

  tmpsum = 1 / sumdist;
  for (i = 0; i < data.Size(); i++) {
    if (isZero(data[i]))
      likely += 0.0;
    else if (dist[i] * tmpsum >= minp)
      likely -= data[i] * log(dist[i] * tmpsum);
    else
      likely -= data[i] * log(minp);
  }

  sumlog -= logFactorial(sumdata);
  //JMB this is a nasty hack to get round a negative loglikelihood score
  tmpsum = absolute(likely + sumlog);
  loglikelihood += 2.0 * tmpsum;
  return tmpsum;
}
