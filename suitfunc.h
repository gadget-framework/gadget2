#ifndef suitfunc_h
#define suitfunc_h

#include "commentstream.h"
#include "keeper.h"
#include "timevariablevector.h"
#include "suitfuncptrvector.h"

//JMB - should remove some of the suitability functions that are no longer used

class SuitfuncPtrvector;

class SuitFunc {
protected:
  char* name;
  TimeVariablevector coeff;
public:
  SuitFunc();
  virtual ~SuitFunc();
  void setName(const char* suitfuncName);
  const char* getName();
  const TimeVariablevector& getConstants() const;
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
  friend int readSuitFunction(SuitfuncPtrvector& suitf, CommentStream& infile,
    const char*  suitfname, const TimeClass* const TimeInfo, Keeper* const keeper);
  friend int findSuitFunc(SuitfuncPtrvector& suitf, const char* suitname);
};

class ExpsuitfuncA : public SuitFunc {
private:
  double preyLength;
  double predLength;
public:
  ExpsuitfuncA();
  virtual ~ExpsuitfuncA();
  virtual int usesPredLength();
  virtual int usesPreyLength();
  virtual void setPredLength(double length);
  virtual void setPreyLength(double length);
  virtual double getPredLength();
  virtual double getPreyLength();
  virtual double calculate();
};

class Constsuitfunc : public SuitFunc {
public:
  Constsuitfunc();
  virtual ~Constsuitfunc();
  virtual int usesPredLength();
  virtual int usesPreyLength();
  virtual double calculate();
};

class Andersensuitfunc : public SuitFunc {
private:
  double preyLength;
  double predLength;
public:
  Andersensuitfunc();
  virtual ~Andersensuitfunc();
  virtual int usesPredLength();
  virtual int usesPreyLength();
  virtual void setPredLength(double length);
  virtual void setPreyLength(double length);
  virtual double getPredLength();
  virtual double getPreyLength();
  virtual double calculate();
};

class ImprovedExpsuitfunc : public SuitFunc {
private:
  double preyLength;
  double predLength;
public:
  ImprovedExpsuitfunc();
  virtual ~ImprovedExpsuitfunc();
  virtual int usesPredLength();
  virtual int usesPreyLength();
  virtual void setPredLength(double length);
  virtual void setPreyLength(double length);
  virtual double getPredLength();
  virtual double getPreyLength();
  virtual double calculate();
};

class ImprovedAndExtendedExpsuitfunc : public SuitFunc {
private:
  double preyLength;
  double predLength;
public:
  ImprovedAndExtendedExpsuitfunc();
  virtual ~ImprovedAndExtendedExpsuitfunc();
  virtual int usesPredLength();
  virtual int usesPreyLength();
  virtual void setPredLength(double length);
  virtual void setPreyLength(double length);
  virtual double getPredLength();
  virtual double getPreyLength();
  virtual double calculate();
};

class Expsuitfuncl50 : public SuitFunc {
private:
  double preyLength;
public:
  Expsuitfuncl50();
  virtual ~Expsuitfuncl50();
  virtual int usesPredLength();
  virtual int usesPreyLength();
  virtual void setPreyLength(double length);
  virtual double getPreyLength();
  virtual double calculate();
};

class ExtendedExpsuitfuncl50 : public SuitFunc {
private:
  double preyLength;
public:
  ExtendedExpsuitfuncl50();
  virtual ~ExtendedExpsuitfuncl50();
  virtual int usesPredLength();
  virtual int usesPreyLength();
  virtual void setPreyLength(double length);
  virtual double getPreyLength();
  virtual double calculate();
};

class BadExpsuitfuncl50 : public SuitFunc {
private:
  double preyLength;
public:
  BadExpsuitfuncl50();
  virtual ~BadExpsuitfuncl50();
  virtual int usesPredLength();
  virtual int usesPreyLength();
  virtual void setPreyLength(double length);
  virtual double getPreyLength();
  virtual double calculate();
};

class SurveySelection : public SuitFunc {
private:
  double preyLength;
public:
  SurveySelection();
  virtual ~SurveySelection();
  virtual int usesPredLength();
  virtual int usesPreyLength();
  virtual void setPreyLength(double length);
  virtual double getPreyLength();
  virtual double calculate();
};

class StraightLine : public SuitFunc {
private:
  double preyLength;
public:
  StraightLine();
  virtual ~StraightLine();
  virtual int usesPredLength();
  virtual int usesPreyLength();
  virtual void setPreyLength(double length);
  virtual double getPreyLength();
  virtual double calculate();
};

class  CLogLog: public SuitFunc {
private:
  double preyLength;
public:
  CLogLog();
  virtual ~CLogLog();
  virtual int usesPredLength();
  virtual int usesPreyLength();
  virtual void setPreyLength(double length);
  virtual double getPreyLength();
  virtual double calculate();
};

class Combination : public SuitFunc {
private:
  double preyLength;
public:
  Combination();
  virtual ~Combination();
  virtual int usesPredLength();
  virtual int usesPreyLength();
  virtual void setPreyLength(double length);
  virtual double getPreyLength();
  virtual double calculate();
};

#endif
