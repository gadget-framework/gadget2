#ifndef growthimplement_h
#define growthimplement_h

#include "commentstream.h"
#include "doublematrix.h"

class GrowthImplementparameters;

class GrowthImplementparameters {
public:
  GrowthImplementparameters(CommentStream& infile);
  ~GrowthImplementparameters() {};
  int MinLengthgroupGrowth() const { return minlengthgroupgrowth; };
  int MaxLengthgroupGrowth() const { return maxlengthgroupgrowth; };
  double MeanResolution() const { return meanresolution; };
  double VarResolution() const { return varresolution; };
  int NMean() const { return nmean; };
  int NVar() const { return nvar; };
  const doublematrix& Distribution() const { return distribution; };
  const doublevector& RealVariance() const { return realvariance; };
protected:
  double meanresolution;
  double varresolution;
  int nmean;
  int nvar;
  int minlengthgroupgrowth;
  int maxlengthgroupgrowth;
  doublevector realvariance;
  doublematrix distribution;
};

#endif
