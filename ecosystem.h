#ifndef ecosystem_h
#define ecosystem_h

#include "stock.h"
#include "baseclassptrvector.h"
#include "otherfoodptrvector.h"
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
  ~Ecosystem();
  void ReadLikelihood(CommentStream& infile);
  void ReadPrinters(CommentStream& infile);
  void ReadFleet(CommentStream& infile);
  void ReadTagging(CommentStream& infile);
  void ReadOtherFood(CommentStream& infile);
  void ReadStock(CommentStream& infile, int mortmodel);
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
  void Update(const DoubleVector& values) const;
  void Opt(IntVector& opt) const;
  void ValuesOfVariables(DoubleVector& val) const;
  void InitialOptValues(DoubleVector& initialval) const;
  void ScaledOptValues(DoubleVector& initialval) const;
  void OptSwitches(ParameterVector& sw) const;
  void OptValues(DoubleVector& val) const;
  void LowerBds(DoubleVector& lbds) const;
  void UpperBds(DoubleVector& ubds) const;
  void CheckBounds() const;
  void InitialValues(DoubleVector& initialval) const;
  void ScaleVariables() const;
  void ScaledValues(DoubleVector& val) const;
  void SimulateOneAreaOneTimeSubstep(int area);
  void GrowthAndSpecialTransactions(int area);
  void UpdateOneTimestepOneArea(int area);
  void SimulateOneTimestep(int print);
  int NoVariables() const { return keeper->NoVariables(); };
  int NoOptVariables() const { return keeper->NoOptVariables(); };
  double SimulateAndUpdate(double* x, int n);
  int Simulate(int optimize, int print = 0);
  double Likelihood() const { return likelihood; };
  int GetFuncEval() const { return funceval; };
  int GetConverge() const { return converge; };
  void SetConverge(int setConverge) { converge = setConverge; };
  volatile int interrupted;
protected:
  double likelihood;
  int funceval;
  int converge;
  BaseClassPtrVector basevec;
  LikelihoodPtrVector Likely;
  PrinterPtrVector printvec;
  PrinterPtrVector likprintvec; //Seperate vector for likelihood printers,
                                //so they can be called at a different time
                                //than the normal printers. [10.04.00 mnaa]
  TimeClass* TimeInfo;
  AreaClass* Area;
  Keeper* keeper;
  CharPtrVector catchnames;
  CharPtrVector stocknames;
  StockPtrVector stockvec;
  CharPtrVector tagnames;
  TagPtrVector tagvec;
  CharPtrVector otherfoodnames;
  OtherFoodPtrVector otherfoodvec;
  CharPtrVector fleetnames;
  FleetPtrVector fleetvec;
  PrintInfo  printinfo;
  friend class InterruptInterface;
};

#endif
