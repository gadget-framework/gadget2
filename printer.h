#ifndef printer_h
#define printer_h

#include "actionattimes.h"
#include "poppredatorptrvector.h"
#include "predatorptrvector.h"
#include "preyptrvector.h"
#include "ecosystem.h"

class TimeClass;

typedef int PrinterType;
const PrinterType STOCKPRINTER = 1;
const PrinterType PREDATORPRINTER = 2;
const PrinterType PREDATOROVERPRINTER = 3;
const PrinterType PREYOVERPRINTER = 4;
const PrinterType STOCKSTDPRINTER = 5;
const PrinterType STOCKPREYFULLPRINTER = 6;
const PrinterType PREDPREYSTDPRINTER = 7;
const PrinterType STOCKFULLPRINTER = 8;
const PrinterType FORMATEDSTOCKPRINTER = 9;
const PrinterType FORMATEDCHATPRINTER = 10;
const PrinterType FORMATEDPREYPRINTER = 11;
const PrinterType MORTPRINTER = 12;
const PrinterType FORMATEDCATCHPRINTER = 13;
const PrinterType BIOMASSPRINTER = 14;
const PrinterType LIKELIHOODPRINTER = 15;

class Printer {
public:
  Printer(PrinterType TYPE) : type(TYPE) {};
  virtual ~Printer() {};
  virtual void Print(const TimeClass* const TimeInfo) = 0;
  virtual void SetStock(Stockptrvector& stockvec) {};
  virtual void SetFleet(Fleetptrvector& fleetvec) {};
  virtual void Init(const Ecosystem* eco) {};
  virtual void SetPredAndPrey(Predatorptrvector& predatorvec, Preyptrvector& preyvec) {};
  virtual void SetPrey(Preyptrvector& preyvec) {};
  //virtual void SetPred(Predatorptrvector& predatorvec) {};
  virtual void SetStocksAndPredAndPrey(const Stockptrvector& stockvec,
    const PopPredatorptrvector& poppredvec, const Preyptrvector& preyvec) {};
  virtual void SetPredator(Predatorptrvector& predatorvec) {};
  PrinterType Type() const { return type; };
protected:
  ActionAtTimes aat;
private:
  const PrinterType type;
};

#endif
