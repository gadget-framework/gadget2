#include "popstatistics.h"
#include "popinfo.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

PopStatistics::PopStatistics(const PopInfoIndexVector& pop,
  const LengthGroupDivision* const lgrpdiv, int calcweight)
  : meanlength(0.0), meanweight(0.0), totalnumber(0.0), sdevlength(0.0) {

  PopInfo nullpop;
  PopInfoVector p(pop.maxCol(), nullpop);
  int i;
  for (i = pop.minCol(); i < pop.maxCol(); i++)
    p[i] = pop[i];
  this->calcStatistics(p, lgrpdiv, calcweight);
}

PopStatistics::PopStatistics(const PopInfoVector& pop,
  const LengthGroupDivision* const lgrpdiv, int calcweight)
  : meanlength(0.0), meanweight(0.0), totalnumber(0.0), sdevlength(0.0) {

  this->calcStatistics(pop, lgrpdiv, calcweight);
}

void PopStatistics::calcStatistics(const PopInfoVector& pop,
  const LengthGroupDivision* const lgrpdiv, int calcweight) {

  PopInfo sum;
  int i;
  double length;

  if (pop.Size() != lgrpdiv->numLengthGroups())
    handle.logMessage(LOGFAIL, "Error in popstatistics - length groups dont match population");

  for (i = 0; i < pop.Size(); i++) {
    if (handle.getLogLevel() >= LOGWARN)
      if ((isZero(pop[i].W)) && (!(isZero(pop[i].N))) && (calcweight == 0))
        handle.logMessage(LOGWARN, "Warning in popstatistics - non-zero population has zero mean weight");

    length = lgrpdiv->meanLength(i);
    sum += pop[i];
    meanlength += (length * pop[i].N);
  }

  if (sum.N < rathersmall) {
    totalnumber = 0.0;
    meanlength = 0.0;
    meanweight = 0.0;
    sdevlength = 0.0;
  } else {
    totalnumber = sum.N;
    meanlength /= totalnumber;

    if (calcweight == 0)
      meanweight = sum.W;
    else
      meanweight = 0.0;

    for (i = 0; i < pop.Size(); i++) {
      length = lgrpdiv->meanLength(i);
      sdevlength += pop[i].N * (meanlength - length) * (meanlength - length);
    }
    sdevlength = ((sdevlength < rathersmall) ? 0.0 : sqrt(sdevlength / totalnumber));
  }
}
