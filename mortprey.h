#ifndef mortprey_h
#define mortprey_h

#include "prey.h"
#include "doublematrixptrvector.h"
#include "areatime.h"
#include "agebandmatrixptrvector.h"
#include "gadget.h"

class MortPrey;
class intvector;

class MortPrey : public Prey {
public:
  MortPrey(CommentStream& infile, const intvector& areas, const char* givenname,
    int minage, int maxage, Keeper* const keeper, const LengthGroupDivision* const stock_lgrp);
  MortPrey(const doublevector& lengths, const intvector& areas, int minage, int maxage,
    const char* givenname, const LengthGroupDivision* const stock_lgrp);
  virtual ~MortPrey();
  virtual prey_type preyType() const { return MORTPREY_TYPE; };
  void InitializeObjects();
  virtual void Sum(const Agebandmatrix& Alkeys, int area, int NrofSubstep);
  const Agebandmatrix& AlkeysPriorToEating(int area) const;
  virtual void Print(ofstream& outfile) const;
  virtual void Reset();
  void calcZ(int area, const doublevector& nat_m);
  const Agebandmatrix& getMeanN(int area) const;
  void setCannibalism(int area, const doublevector& cann);
  const doublevector& propSurv(int area) { return prop_surv[AreaNr[area]]; };
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
  void addConsMatrix(int ncol, const bandmatrix& cons_mat);
  int getNoCannPreds() { return cannprednames.Size(); };
  const bandmatrix& cannConsum(int area, int pred_no) { return cann_cons[AreaNr[area]][pred_no]; };
  void setConsumption(int area, int pred_no, const bandmatrix& consum);
  int getPredMinAge(int i) { return cann_cons[0][i].Minage(); };
  int getPredMaxAge(int i) { return cann_cons[0][i].Maxage(); };
  int cannIsTrue() { return cann_is_true; };
  void cannIsTrue(int cann_val) { cann_is_true = cann_val; };
  void addAgeGroupMatrix(doublematrix* const agematrix);
  const doublematrix* getAgeGroupMatrix(int pred_no) { return agegroupmatrix[pred_no]; };
  void setAgeMatrix(int pred_no, int area, const doublevector& agegroupno);
  doublematrix* ageGroupMatrix(int index) { return agegroupmatrix[index]; };
protected:
  agebandmatrixptrvector Alkeys;
  agebandmatrixptrvector mean_n;
  doublematrix z;
  doublematrix mort_fact;
  doublematrix prop_surv;
  doublematrix cannibalism;
  LengthGroupDivision* prey_lgrp;
  intvector haveCalculatedMeanN;
  int minpredage;
  int maxpredage;
  charptrvector cannprednames;
  bandmatrixmatrix cann_cons;
  doublematrixptrvector agegroupmatrix;
  int cann_is_true;
};

#endif
