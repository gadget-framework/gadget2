#ifndef stomachcontent_h
#define stomachcontent_h

#include "commentstream.h"
#include "lengthgroup.h"
#include "predatorptrvector.h"
#include "preyptrvector.h"
#include "doublematrixptrmatrix.h"
#include "intmatrix.h"
#include "formulamatrix.h"
#include "actionattimes.h"
#include "charptrmatrix.h"

class PredatorAggregator;

class SC {
public:
  SC(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo,
    Keeper* const keeper, const char* datafilename, const char* name);
  virtual ~SC();
  virtual double Likelihood(const TimeClass* const TimeInfo);
  virtual void Reset();
  virtual void Print(ofstream& outfile) const;
  virtual void SetPredatorsAndPreys(PredatorPtrVector&, PreyPtrVector&);
  virtual void Aggregate(int i);
  void CommandLinePrint(ofstream& outfile, const TimeClass& time, const PrintInfo& print);
  virtual void PrintLikelihoodOnStep(ostream& outfile, int time,
    const TimeClass& t, int print_type);
  virtual void PrintLikelihood(ofstream& outfile, const TimeClass& time) {};
  virtual void PrintLikelihoodHeader(ofstream& outfile) {};
protected:
  virtual double CalculateLikelihood(DoubleMatrixPtrVector&, DoubleMatrix&) = 0;
  DoubleMatrixPtrMatrix stomachcontent;   //[timeindex][areas][pred_l][prey_l]
  DoubleMatrixPtrMatrix modelConsumption; //[timeindex][areas][pred_l][prey_l]
  CharPtrVector predatornames;
  CharPtrMatrix preynames;
  CharPtrVector areaindex;
  CharPtrVector predindex;
  CharPtrVector preyindex;
  IntVector Years;
  IntVector Steps;
  DoubleVector predatorlengths;
  IntVector predatorages; //as an alternative to predatorlengths kgf 19/2 99
  DoubleMatrix preylengths;              //[prey_nr][prey_l]
  IntMatrix areas;
  int age_pred; //kgf 22/2 99 to switch between age or length predator
  PredatorAggregator** aggregator;
  LengthGroupDivision** preyLgrpDiv;
  LengthGroupDivision** predLgrpDiv;
  int timeindex;
  ActionAtTimes AAT;
  FormulaMatrix digestioncoeff;
  ofstream printfile;
  double epsilon;
  char* scname;
};

class SCNumbers : public SC {
public:
  SCNumbers(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper,
    const char* datafilename, const char* numfilename, const char* name);
  virtual ~SCNumbers() {};
  virtual void Aggregate(int i);
protected:
  void ReadStomachNumberContent(CommentStream&, const TimeClass* const);
  virtual double CalculateLikelihood(DoubleMatrixPtrVector&, DoubleMatrix&);
};

class SCAmounts : public SC {
public:
  SCAmounts(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper,
    const char* datafilename, const char* numfilename, const char* name);
  virtual ~SCAmounts();
  DoubleMatrixPtrMatrix& Stddev() { return stddev; };
  DoubleMatrixPtrVector& StomachNumbers() { return number; };
  virtual void PrintLikelihood(ofstream&, const TimeClass& time);
  virtual void PrintLikelihoodHeader(ofstream&);
protected:
  void ReadStomachAmountContent(CommentStream&, const TimeClass* const);
  void ReadStomachSampleContent(CommentStream&, const TimeClass* const);
  virtual double CalculateLikelihood(DoubleMatrixPtrVector&, DoubleMatrix&);
  DoubleMatrixPtrMatrix stddev;  //[timeindex][areas][pred_l][prey_l]
  DoubleMatrixPtrVector number;  //[timeindex][areas][pred_l]
};

class SCRatios : public SCAmounts {
public:
  SCRatios(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper,
    const char* datafilename, const char* numfilename, const char* name)
    : SCAmounts(infile, Area, TimeInfo, keeper, datafilename, numfilename, name) {};
  virtual ~SCRatios() {};
  virtual void SetPredatorsAndPreys(PredatorPtrVector&, PreyPtrVector&);
protected:
  virtual double CalculateLikelihood(DoubleMatrixPtrVector&, DoubleMatrix&);
};

class StomachContent : public Likelihood {
public:
  StomachContent(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo,
    Keeper* const keeper, double w, const char* name);
  virtual ~StomachContent();
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  virtual void Reset(const Keeper* const keeper)
    { Likelihood::Reset(keeper); StomCont->Reset(); };
  virtual void Print(ofstream& outfile) const;
  void SetPredatorsAndPreys(PredatorPtrVector& Predators, PreyPtrVector& Preys)
    { StomCont->SetPredatorsAndPreys(Predators, Preys); };
  void CommandLinePrint(ofstream& outfile, const TimeClass& time, const PrintInfo& print)
    { StomCont->CommandLinePrint(outfile, time, print); };
  virtual void PrintLikelihood(ofstream& outfile, const TimeClass& time)
    { StomCont->PrintLikelihood(outfile, time); };
  virtual void PrintLikelihoodHeader(ofstream& outfile)
    { StomCont->PrintLikelihoodHeader(outfile); };
  char* stomachname;
private:
  int functionnumber;
  char* functionname;
  SC* StomCont;
};

#endif

