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

/**
 * \class Printer
 * \brief This is the base class used to print the modelled population during a model simulation
 * \note This will always be overridden by the derived classes that actually print the required information
 */
class Printer {
public:
  /**
   * \brief This is the default Printer constructor
   */
  Printer(PrinterType TYPE) : type(TYPE) {};
  /**
   * \brief This is the default Printer destructor
   */
  virtual ~Printer() {};
  /**
   * \brief This will print the requested information for the printer class to the ofstream specified
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Print(const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This will select the stocks required for the printer class to print the requested information
   * \param stockvec is the StockPtrVector listing all the available stocks
   */
  virtual void SetStock(StockPtrVector& stockvec) {};
  /**
   * \brief This will select the fleets required for the printer class to print the requested information
   * \param fleetvec is the FleetPtrVector listing all the available fleets
   */
  virtual void SetFleet(FleetPtrVector& fleetvec) {};
  /**
   * \brief This will select the predators and preys required for the printer class to print the requested information
   * \param predatorvec is the PredatorPtrVector listing all the available predators
   * \param preyvec is the PreyPtrVector listing all the available preys
   */
  virtual void SetPredAndPrey(PredatorPtrVector& predatorvec, PreyPtrVector& preyvec) {};
  /**
   * \brief This will select the preys required for the printer class to print the requested information
   * \param preyvec is the PreyPtrVector listing all the available preys
   */
  virtual void SetPrey(PreyPtrVector& preyvec) {};
  /**
   * \brief This will select the stocks, predators and preys required for the printer class to print the requested information
   * \param stockvec is the StockPtrVector listing all the available stocks
   * \param poppredvec is the PopPredatorPtrVector listing all the available predators
   * \param preyvec is the PreyPtrVector listing all the available preys
   */
  virtual void SetStocksAndPredAndPrey(const StockPtrVector& stockvec,
    const PopPredatorPtrVector& poppredvec, const PreyPtrVector& preyvec) {};
  /**
   * \brief This will select the predators required for the printer class to print the requested information
   * \param predatorvec is the PredatorPtrVector listing all the available predators
   */
  virtual void SetPredator(PredatorPtrVector& predatorvec) {};
  /**
   * \brief This will return the type of printer clas
   * \return type
   */
  PrinterType Type() const { return type; };
protected:
  /**
   * \brief This stores information about when the output is required
   */
  ActionAtTimes aat;
private:
  /**
   * \brief This denotes what type of printer class has been created
   */
  const PrinterType type;
};

#endif
