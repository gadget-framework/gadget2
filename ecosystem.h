#ifndef ecosystem_h
#define ecosystem_h

#include "stock.h"
#include "baseclassptrvector.h"
#include "otherfoodptrvector.h"
#include "readfunc.h"
#include "errorhandler.h"
#include "tagptrvector.h"
#include "printerptrvector.h"
#include "printer.h"
#include "fleet.h"
#include "printinfo.h" //filenames for printing during optimization
#include "gadget.h"

class Ecosystem {
public:
  Ecosystem();
  //Changed constructor and readmain, to take a parameter saying if this
  //is a netrun. This is done to prevent EcoSystem from reading the
  //print files when doing a net run. 07.04.00 AJ & mnaa.
  Ecosystem(const char* const filename, int optimize, int netrun,
    int calclikelihood, const char* const inputdir,
    const char* const workingdir, const PrintInfo& pi);
  void readMain(CommentStream& infile, int optimize, int netrun,
    int calclikelihood, const char* const inputdir,
    const char* const workingdir);
  ~Ecosystem();
  void readLikelihood(CommentStream& infile);
  void readPrinters(CommentStream& infile);
  void readFleet(CommentStream& infile);
  void readTagging(CommentStream& infile);
  void readOtherFood(CommentStream& infile);
  void readStock(CommentStream& infile, int mortmodel);
  void writeStatus(const char* filename) const;
  void writeInitialInformation(const char* const filename) const;
  void writeInitialInformationInColumns(const char* const filename) const;
  void writeValues(const char* const filename, int prec) const;
  void writeValuesInColumns(const char* const filename, int prec) const;
  void writeParamsInColumns(const char* const filename, int prec) const;
  void writeOptValues() const;
  void Initialise(int optimize);
  void PrintLikelihoodInfo(const char* filename) const;
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
  void checkBounds() const;
  void InitialValues(DoubleVector& initialval) const;
  void ScaleVariables() const;
  void ScaledValues(DoubleVector& val) const;
  void SimulateOneAreaOneTimeSubstep(int area);
  void GrowthAndSpecialTransactions(int area);
  void updateOneTimestepOneArea(int area);
  void SimulateOneTimestep();
  int NoVariables() const { return keeper->NoVariables(); };
  int NoOptVariables() const { return keeper->NoOptVariables(); };
  double SimulateAndUpdate(double* x, int n);
  int Simulate(int optimize, int print = 0);
  double getLikelihood() const { return likelihood; };
  int getFuncEval() const { return funceval; };
  int getConverge() const { return converge; };
  void setConverge(int setConverge) { converge = setConverge; };
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
