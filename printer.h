#ifndef printer_h
#define printer_h

#include "actionattimes.h"
#include "areatime.h"
#include "lengthgroup.h"
#include "commentstream.h"
#include "predatorptrvector.h"
#include "preyptrvector.h"
#include "stockptrvector.h"
#include "fleetptrvector.h"
#include "charptrvector.h"
#include "intmatrix.h"
#include "gadget.h"

enum PrinterType { STOCKPRINTER = 1, PREDATORPRINTER, PREDATOROVERPRINTER,
  PREYOVERPRINTER, STOCKSTDPRINTER, STOCKPREYFULLPRINTER, PREDPREYSTDPRINTER,
  STOCKFULLPRINTER, FORMATEDSTOCKPRINTER, FORMATEDCHATPRINTER, FORMATEDPREYPRINTER,
  MORTPRINTER, BIOMASSPRINTER, LIKELIHOODPRINTER };

/**
 * \class Printer
 * \brief This is the base class used to print the modelled population during a model simulation
 * \note This will always be overridden by the derived classes that actually print the required information
 */
class Printer {
public:
  /**
   * \brief This is the default Printer constructor
   * \param ptype is the PrinterType of the printer
   */
  Printer(PrinterType ptype) { type = ptype; };
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
   * \param stockvec is the StockPtrVector of all the available stocks
   */
  virtual void setStock(StockPtrVector& stockvec) {};
  /**
   * \brief This will select the fleets required for the printer class to print the requested information
   * \param fleetvec is the FleetPtrVector of all the available fleets
   */
  virtual void setFleet(FleetPtrVector& fleetvec) {};
  /**
   * \brief This will select the predators and preys required for the printer class to print the requested information
   * \param predatorvec is the PredatorPtrVector of all the available predators
   * \param preyvec is the PreyPtrVector of all the available preys
   */
  virtual void setPredAndPrey(PredatorPtrVector& predatorvec, PreyPtrVector& preyvec) {};
  /**
   * \brief This will select the preys required for the printer class to print the requested information
   * \param preyvec is the PreyPtrVector of all the available preys
   */
  virtual void setPrey(PreyPtrVector& preyvec) {};
  /**
   * \brief This will select the stocks, predators and preys required for the printer class to print the requested information
   * \param stockvec is the StockPtrVector of all the available stocks
   * \param predvec is the PredatorPtrVector of all the available predators
   * \param preyvec is the PreyPtrVector of all the available preys
   */
  virtual void setStocksAndPredAndPrey(const StockPtrVector& stockvec,
    const PredatorPtrVector& predvec, const PreyPtrVector& preyvec) {};
  /**
   * \brief This will select the predators required for the printer class to print the requested information
   * \param predatorvec is the PredatorPtrVector of all the available predators
   */
  virtual void setPredator(PredatorPtrVector& predatorvec) {};
  /**
   * \brief This will return the type of printer class
   * \return type
   */
  PrinterType Type() { return type; };
protected:
  /**
   * \brief This ActionAtTimes stores information about when the printer output is required in the model
   */
  ActionAtTimes AAT;
private:
  /**
   * \brief This denotes what type of printer class has been created
   */
  PrinterType type;
};

#endif
