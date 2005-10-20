#include "popstatistics.h"
#include "popinfo.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

PopStatistics::PopStatistics(const PopInfoIndexVector& pop,
  const LengthGroupDivision* const lgrpdiv, int calcweight)
  : meanlength(0.0), meanweight(0.0), totalnumber(0.0), sdevlength(0.0) {

  PopInfo sum;
  int i, offset;
  double length;

  if (pop.Size() != lgrpdiv->numLengthGroups())
    handle.logMessage(LOGFAIL, "Error in popstatistics - length groups dont match population");

  offset = pop.minCol();
  for (i = pop.minCol(); i < pop.maxCol(); i++) {
    if (handle.getLogLevel() >= LOGWARN)
      if ((isZero(pop[i].W)) && (!(isZero(pop[i].N))) && (calcweight == 0))
        handle.logMessage(LOGWARN, "Warning in popstatistics - non-zero population has zero mean weight");

    length = lgrpdiv->meanLength(i - offset);
    sum += pop[i];
    meanlength += (length * pop[i].N);
  }

  if (sum.N > rathersmall) {
    totalnumber = sum.N;
    meanlength /= totalnumber;

    if (calcweight == 0)
      meanweight = sum.W;

    for (i = pop.minCol(); i < pop.maxCol(); i++) {
      length = lgrpdiv->meanLength(i - offset);
      sdevlength += pop[i].N * (meanlength - length) * (meanlength - length);
    }
    sdevlength = ((sdevlength < rathersmall) ? 0.0 : sqrt(sdevlength / totalnumber));
  } else  {
    //JMB reset the meanlength value back to 0
    meanlength = 0.0;
  }
}
