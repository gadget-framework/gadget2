#ifndef growthimplement_h
#define growthimplement_h

#include "commentstream.h"
#include "doublematrix.h"

class GrowthImplementParameters;

class GrowthImplementParameters {
public:
  GrowthImplementParameters(CommentStream& infile);
  ~GrowthImplementParameters() {};
  int MinLengthgroupGrowth() const { return minlengthgroupgrowth; };
  int MaxLengthgroupGrowth() const { return maxlengthgroupgrowth; };
  double MeanResolution() const { return meanresolution; };
  double VarResolution() const { return varresolution; };
  int NMean() const { return nmean; };
  int NVar() const { return nvar; };
  const DoubleMatrix& Distribution() const { return distribution; };
  const DoubleVector& RealVariance() const { return realvariance; };
protected:
  double meanresolution;
  double varresolution;
  int nmean;
  int nvar;
  int minlengthgroupgrowth;
  int maxlengthgroupgrowth;
  DoubleVector realvariance;
  DoubleMatrix distribution;
};

#endif
