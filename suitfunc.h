#ifndef suitfunc_h
#define suitfunc_h

#include "commentstream.h"
#include "keeper.h"
#include "timevariablevector.h"
#include "suitfuncptrvector.h"

//JMB - removed some of the suitability functions that are no longer used
//To get the removed suitability functions back, you will need gadget 2.0.03

class SuitFuncPtrVector;

class SuitFunc {
protected:
  char* name;
  TimeVariableVector coeff;
public:
  SuitFunc();
  virtual ~SuitFunc();
  void setName(const char* suitfuncName);
  const char* getName();
  const TimeVariableVector& getConstants() const;
  void readConstants(CommentStream& infile,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  void updateConstants(const TimeClass* const TimeInfo);
  int constantsHaveChanged(const TimeClass* const TimeInfo);
  virtual int usesPredLength() = 0;
  virtual int usesPreyLength() = 0;
  virtual double getPredLength();
  virtual double getPreyLength();
  virtual void setPredLength(double length);
  virtual void setPreyLength(double length);
  virtual double calculate() = 0;
  int noOfConstants();
  friend int readSuitFunction(SuitFuncPtrVector& suitf, CommentStream& infile,
    const char*  suitfname, const TimeClass* const TimeInfo, Keeper* const keeper);
  friend int findSuitFunc(SuitFuncPtrVector& suitf, const char* suitname);
};

class ExpSuitFuncA : public SuitFunc {
private:
  double preyLength;
  double predLength;
public:
  ExpSuitFuncA();
  virtual ~ExpSuitFuncA();
  virtual int usesPredLength();
  virtual int usesPreyLength();
  virtual void setPredLength(double length);
  virtual void setPreyLength(double length);
  virtual double getPredLength();
  virtual double getPreyLength();
  virtual double calculate();
};

class ConstSuitFunc : public SuitFunc {
public:
  ConstSuitFunc();
  virtual ~ConstSuitFunc();
  virtual int usesPredLength();
  virtual int usesPreyLength();
  virtual double calculate();
};

class AndersenSuitFunc : public SuitFunc {
private:
  double preyLength;
  double predLength;
public:
  AndersenSuitFunc();
  virtual ~AndersenSuitFunc();
  virtual int usesPredLength();
  virtual int usesPreyLength();
  virtual void setPredLength(double length);
  virtual void setPreyLength(double length);
  virtual double getPredLength();
  virtual double getPreyLength();
  virtual double calculate();
};

class ExpSuitFuncL50 : public SuitFunc {
private:
  double preyLength;
public:
  ExpSuitFuncL50();
  virtual ~ExpSuitFuncL50();
  virtual int usesPredLength();
  virtual int usesPreyLength();
  virtual void setPreyLength(double length);
  virtual double getPreyLength();
  virtual double calculate();
};

class StraightSuitFunc : public SuitFunc {
private:
  double preyLength;
public:
  StraightSuitFunc();
  virtual ~StraightSuitFunc();
  virtual int usesPredLength();
  virtual int usesPreyLength();
  virtual void setPreyLength(double length);
  virtual double getPreyLength();
  virtual double calculate();
};

#endif
