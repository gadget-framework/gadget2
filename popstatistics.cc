#include "popstatistics.h"
#include "popinfo.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

PopStatistics::PopStatistics(const PopInfoIndexVector& pop,
  const LengthGroupDivision* const lgrpdiv, int calcweight)
  : meanlength(0.0), meanweight(0.0), totalnumber(0.0), stddevoflength(0.0) {

  PopInfo nullpop;
  PopInfoVector p(pop.Maxcol(), nullpop);
  int i;
  for (i = pop.Mincol(); i < pop.Maxcol(); i++)
    p[i] = pop[i];
  this->CalcStatistics(p, lgrpdiv, calcweight);
}

PopStatistics::PopStatistics(const PopInfoVector& pop,
  const LengthGroupDivision* const lgrpdiv, int calcweight)
  : meanlength(0.0), meanweight(0.0), totalnumber(0.0), stddevoflength(0.0) {

  this->CalcStatistics(pop, lgrpdiv, calcweight);
}

void PopStatistics::CalcStatistics(const PopInfoVector& pop,
  const LengthGroupDivision* const lgrpdiv, int calcweight) {

  PopInfo sum;
  int i;
  double length;

  for (i = 0; i < pop.Size(); i++) {
    if ((isZero(pop[i].W)) && (!(isZero(pop[i].N))) && (calcweight == 0))
      handle.LogWarning("Warning in popstatistics - non-zero population has zero mean weight");

    length = lgrpdiv->Meanlength(i);
    sum += pop[i];
    meanlength += (length * pop[i].N);
  }

  if (sum.N < rathersmall) {
    totalnumber = 0.0;
    meanlength = 0.0;
    meanweight = 0.0;
    stddevoflength = 0.0;
  } else {
    totalnumber = sum.N;
    meanlength /= totalnumber;

    if (calcweight == 0)
      meanweight = sum.W;
    else
      meanweight = 0.0;

    for (i = 0; i < pop.Size(); i++) {
      length = lgrpdiv->Meanlength(i);
      stddevoflength += pop[i].N * (meanlength - length) * (meanlength - length);
    }
    stddevoflength = ((stddevoflength < rathersmall) ? 0.0 : sqrt(stddevoflength / totalnumber));
  }
}
