#ifndef mortprey_h
#define mortprey_h

#include "prey.h"
#include "doublematrixptrvector.h"
#include "areatime.h"
#include "agebandmatrixptrvector.h"
#include "intvector.h"
#include "gadget.h"

class MortPrey : public Prey {
public:
  MortPrey(CommentStream& infile, const IntVector& areas, const char* givenname,
    int minage, int maxage, Keeper* const keeper, const LengthGroupDivision* const stock_lgrp);
  virtual ~MortPrey();
  void InitialiseObjects();
  virtual void Sum(const AgeBandMatrix& Alkeys, int area, int NrofSubstep);
  const AgeBandMatrix& AlkeysPriorToEating(int area) const;
  virtual void Print(ofstream& outfile) const;
  virtual void Reset();
  void calcZ(int area, const DoubleVector& nat_m);
  const AgeBandMatrix& getMeanN(int area) const;
  void setCannibalism(int area, const DoubleVector& cann);
  const DoubleVector& propSurv(int area) { return prop_surv[AreaNr[area]]; };
  friend ostream& printmean_n(ostream& o, const MortPrey& prey, AreaClass area, int indent);
  friend ostream& printz(ostream& o, const MortPrey& prey, AreaClass area, int indent);
  friend ostream& printcannibalism(ostream& o, const MortPrey& prey, AreaClass area, int indent);
  void calcMeanN(int area);
  void calcByAge(int area, TimeClass time);
  int minPredAge() { return minpredage; };
  int maxPredAge() { return maxpredage; };
  void setMinPredAge(int minage) { minpredage = minage; };
  void setMaxPredAge(int maxage) { maxpredage = maxage; };
  void setDimensions(int nrow, int ncol) { cann_cons.AddRows(nrow,ncol); };
  const char* cannPredName(int pred_no) { return cannprednames[pred_no]; };
  void addCannPredName(const char* predname); //kgf 3/3 99
  void addConsMatrix(int ncol, const BandMatrix& cons_mat);
  int getNoCannPreds() { return cannprednames.Size(); };
  const BandMatrix& cannConsum(int area, int pred_no) { return cann_cons[AreaNr[area]][pred_no]; };
  void setConsumption(int area, int pred_no, const BandMatrix& consum);
  int getPredMinAge(int i) { return cann_cons[0][i].minAge(); };
  int getPredMaxAge(int i) { return cann_cons[0][i].maxAge(); };
  int cannIsTrue() { return cann_is_true; };
  void cannIsTrue(int cann_val) { cann_is_true = cann_val; };
  void addAgeGroupMatrix(DoubleMatrix* const agematrix);
  void setAgeMatrix(int pred_no, int area, const DoubleVector& agegroupno);
  DoubleMatrix* ageGroupMatrix(int index) { return agegroupmatrix[index]; };
protected:
  AgeBandMatrixPtrVector Alkeys;
  AgeBandMatrixPtrVector mean_n;
  DoubleMatrix z;
  DoubleMatrix mort_fact;
  DoubleMatrix prop_surv;
  DoubleMatrix cannibalism;
  IntVector haveCalculatedMeanN;
  int minpredage;
  int maxpredage;
  CharPtrVector cannprednames;
  BandMatrixMatrix cann_cons;
  DoubleMatrixPtrVector agegroupmatrix;
  int cann_is_true;
};

#endif
