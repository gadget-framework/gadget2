#ifndef stomachcontent_h
#define stomachcontent_h

#include "commentstream.h"
#include "predatorptrvector.h"
#include "preyptrvector.h"
#include "doublematrixptrmatrix.h"
#include "intmatrix.h"
#include "formulamatrix.h"
#include "actionattimes.h"
#include "charptrmatrix.h"

class PredatorAggregator;
class TimeClass;
class AreaClass;

class SC {
public:
  SC(CommentStream&, const AreaClass* const, const TimeClass* const,
    Keeper* const keeper, const char* name, const char* datafilename);
  virtual ~SC();
  virtual double Likelihood(const TimeClass* const);
  virtual void Reset();
  virtual void Print(ofstream&) const;
  virtual void SetPredatorsAndPreys(Predatorptrvector&, Preyptrvector&);
  virtual void Aggregate(int i);
  void print(ofstream& stomachfile, const TimeClass& time, const PrintInfo& print);
  virtual void printHeader(ofstream& stomachfile, const PrintInfo& print);
  virtual void PrintLikelihoodOnStep(ostream& outfile, int time,
    const TimeClass& t, int print_type);
  virtual void PrintLikelihood(ofstream&, const TimeClass& time) {};
  virtual void PrintLikelihoodHeader(ofstream&) {};
protected:
  virtual double CalculateLikelihood(doublematrixptrvector&, doublematrix&) = 0;
  int minp; //minimum probability
  doublematrixptrmatrix stomachcontent;   //[timeindex][areas][pred_l][prey_l]
  doublematrixptrmatrix modelConsumption; //[timeindex][areas][pred_l][prey_l]
  charptrvector predatornames;
  charptrmatrix preynames;
  charptrvector areaindex;
  charptrvector predindex;
  charptrvector preyindex;
  intvector Years;
  intvector Steps;
  doublevector predatorlengths;
  intvector predatorages; //as an alternative to predatorlengths kgf 19/2 99
  doublematrix preylengths;              //[prey_nr][prey_l]
  intmatrix areas;
  int age_pred; //kgf 22/2 99 to switch between age or length predator
  PredatorAggregator** aggregator;
  int timeindex;
  ActionAtTimes AAT;
  Formulamatrix digestioncoeff;
  ofstream printfile;
  const char* name;
};

class SCNumbers : public SC {
public:
  SCNumbers(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper,
    const char* nam, const char* datafilename, const char* numfilename);
  virtual ~SCNumbers() {};
  virtual void Aggregate(int i);
protected:
  void ReadStomachNumberContent(CommentStream&, const TimeClass* const);
  virtual double CalculateLikelihood(doublematrixptrvector&, doublematrix&);
};

class SCAmounts : public SC {
public:
  SCAmounts(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper,
    const char* nam, const char* datafilename, const char* numfilename);
  virtual ~SCAmounts();
  doublematrixptrmatrix& Stddev() { return stddev; };
  doublematrixptrvector& StomachNumbers() { return number; };
  virtual void PrintLikelihood(ofstream&, const TimeClass& time);
  virtual void PrintLikelihoodHeader(ofstream&);
protected:
  void ReadStomachAmountContent(CommentStream&, const TimeClass* const);
  void ReadStomachSampleContent(CommentStream&, const TimeClass* const);
  virtual double CalculateLikelihood(doublematrixptrvector&, doublematrix&);
  doublematrixptrmatrix stddev;  //[timeindex][areas][pred_l][prey_l]
  doublematrixptrvector number;     //[timeindex][areas][pred_l]
};

class SCRatios : public SCAmounts {
public:
  SCRatios(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper,
    const char* nam, const char* datafilename, const char* numfilename)
    : SCAmounts(infile, Area, TimeInfo, keeper, nam, datafilename, numfilename) {};
  virtual ~SCRatios() {};
  virtual void SetPredatorsAndPreys(Predatorptrvector&, Preyptrvector&);
protected:
  virtual double CalculateLikelihood(doublematrixptrvector&, doublematrix&);
};

class StomachContent : public Likelihood {
public:
  StomachContent(CommentStream&, const AreaClass* const, const TimeClass* const,
    Keeper* const keeper, double likweight, const char* name);
  virtual ~StomachContent() { delete StomCont; };
  virtual void AddToLikelihood(const TimeClass* const);
  virtual void Reset(const Keeper* const keeper)
    { Likelihood::Reset(keeper); StomCont->Reset(); };
  virtual void Print(ofstream&) const;
  virtual void LikelihoodPrint(ofstream&) const {};
  void SetPredatorsAndPreys(Predatorptrvector& Predators, Preyptrvector& Preys)
    { StomCont->SetPredatorsAndPreys(Predators, Preys); };
  void print(ofstream& stomachfile, const TimeClass& time, const PrintInfo& print)
    { StomCont->print(stomachfile, time, print); };
  void printHeader(ofstream& stomachfile, const PrintInfo& print)
    { StomCont->printHeader(stomachfile, print); };
  virtual void PrintLikelihood(ofstream& outfile, const TimeClass& time)
    { StomCont->PrintLikelihood(outfile, time); };
  virtual void PrintLikelihoodHeader(ofstream& outfile)
    { StomCont->PrintLikelihoodHeader(outfile); };
  const char* name;
private:
  int functionnumber;
  SC* StomCont;
};

#endif

