#ifndef ecosystem_h
#define ecosystem_h

#include "stock.h"
#include "baseclassptrvector.h"
#include "otherfood.h"
#include "readfunc.h"
#include "errorhandler.h"
#include "print.h"
#include "tagptrvector.h"
#include "printerptrvector.h"
#include "printer.h"
#include "fleet.h"
#include "printinfo.h" //filenames for printing during optimization
#include "gadget.h"

class Ecosystem {
public:
  Ecosystem();
  //Changed constructor and Readmain, to take a parameter saying if this
  //is a netrun. This is done to prevent EcoSystem from reading the
  //print files when doing a net run. 07.04.00 AJ & mnaa.
  Ecosystem(const char* const filename, int optimize, int netrun,
    int calclikelihood, const char* const inputdir,
    const char* const workingdir, const PrintInfo& pi);
  void Readmain(CommentStream& infile, int optimize, int netrun,
    int calclikelihood, const char* const inputdir,
    const char* const workingdir);
  int ReadLikelihood(CommentStream& infile);
  int ReadPrinters(CommentStream& infile);
  int ReadFleet(CommentStream& infile);
  int ReadTagging(CommentStream& infile);
  int ReadOtherFood(CommentStream& infile);
  int ReadStock(CommentStream& infile, int mortmodel);
  void Initialize(int optimize);
  void PrintStatus(const char* filename) const;
  void PrintLikelihoodInfo(const char* filename) const;
  void PrintInitialInformation(const char* const filename) const;
  void PrintInitialInformationinColumns(const char* const filename) const;
  void PrintValues(const char* const filename, int prec) const;
  void PrintOptValues() const;
  void PrintValuesinColumns(const char* const filename, int prec) const;
  void PrintParamsinColumns(const char* const filename, int prec) const;
  void Update(const StochasticData* const Stochastic) const;
  void Update(const doublevector& values) const;
  int NoVariables() const { return keeper->NoVariables(); };
  void Opt(intvector& opt) const;
  void ValuesOfVariables(doublevector& val) const;
  void InitialOptValues(doublevector &initialval) const;
  void ScaledOptValues(doublevector &initialval) const;
  void OptSwitches(Parametervector& sw) const;
  void OptValues(doublevector &val) const;
  void LowerBds(doublevector& lbds) const;
  void UpperBds(doublevector& ubds) const;
  void InitialValues(doublevector &initialval) const;
  int NoOptVariables() const;
  void ScaleVariables() const;
  void ScaledValues(doublevector& val) const;
  void SimulateOneAreaOneTimeSubstep(int area);
  void GrowthAndSpecialTransactions(int area);
  void UpdateOneTimestepOneArea(int area);
  void SimulateOneTimestep(int print);
  double SimulateAndUpdate(double * x, int n);
  int Simulate(int optimize, int print = 0);
  Stock* findStock(const char* stockname) const;
  Fleet* findFleet(const char* fleetname) const;
  ~Ecosystem();
  double Likelihood() const { return likelihood; };
  int GetFuncEval() const { return funceval; };
  volatile int interrupted;
protected:
  double likelihood;
  int funceval;
  BaseClassptrvector basevec;
  Likelihoodptrvector Likely;
  Printerptrvector printvec;
  Printerptrvector likprintvec; //Seperate vector for likelihood printers,
                                //so they can be called at a different time
                                //than the normal printers. [10.04.00 mnaa]
  TimeClass*  TimeInfo;
  AreaClass*  Area;
  Keeper* keeper;
  charptrvector catchnames;
  charptrvector stocknames;
  Stockptrvector stockvec;
  charptrvector tagnames;
  Tagptrvector tagvec;
  charptrvector otherfoodnames;
  OtherFoodptrvector otherfoodvec;
  charptrvector fleetnames;
  Fleetptrvector fleetvec;
  PrintInfo  printinfo;
  friend class InterruptInterface;
};

#endif
