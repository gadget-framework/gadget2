#include "popstatistics.h"
#include "conversion.h"
#include "popinfo.h"
#include "gadget.h"

PopStatistics::PopStatistics(const popinfoindexvector& pop, const LengthGroupDivision* const lgrpdiv)
  : meanlength(0.0), meanweight(0.0), totalnumber(0.0), stddevOflength(0.0) {

  popinfo nullpop;
  popinfovector p(pop.Maxcol(), nullpop);
  int i;
  for (i = pop.Mincol(); i < pop.Maxcol(); i++)
    p[i] = pop[i];
  this->CalcStatistics(p, lgrpdiv);
}

PopStatistics::PopStatistics(const popinfovector& pop, const LengthGroupDivision* const lgrpdiv)
  : meanlength(0.0), meanweight(0.0), totalnumber(0.0), stddevOflength(0.0) {

  this->CalcStatistics(pop, lgrpdiv);
}

PopStatistics::~PopStatistics() {
}

void PopStatistics::CalcStatistics(const popinfovector& pop, const LengthGroupDivision* const lgrpdiv) {
  assert(pop.Size() == lgrpdiv->NoLengthGroups());

  popinfo sum;
  int i;
  double length;

  for (i = 0; i < pop.Size(); i++) {
    if ((iszero(pop[i].W)) && (!(iszero(pop[i].N)))) {
      cerr << "Warning: when calculating statistics on a population, a nonzero\n"
        << "population (" << pop[i].N << ") had zero mean weight\n";
    }

    length = lgrpdiv->Meanlength(i);
    sum += pop[i];
    meanlength += (length * pop[i].N);
  }

  if (sum.N < rathersmall) {
    totalnumber = 0.0;
    meanlength = 0.0;
    meanweight = 0.0;
    stddevOflength = 0.0;
  } else {
    totalnumber = sum.N;
    meanlength /= totalnumber;
    meanweight = sum.W;
    for (i = 0; i < pop.Size(); i++) {
      length = lgrpdiv->Meanlength(i);
      stddevOflength += pop[i].N * (meanlength - length) * (meanlength - length);
    }
    stddevOflength = ((stddevOflength < rathersmall) ? 0.0 : sqrt(stddevOflength / totalnumber));
  }
}
