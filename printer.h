#ifndef printer_h
#define printer_h

#include "actionattimes.h"
#include "areatime.h"
#include "conversion.h"
#include "commentstream.h"
#include "poppredatorptrvector.h"
#include "predatorptrvector.h"
#include "preyptrvector.h"
#include "ecosystem.h"
#include "gadget.h"

enum PrinterType { STOCKPRINTER = 1, PREDATORPRINTER, PREDATOROVERPRINTER,
  PREYOVERPRINTER, STOCKSTDPRINTER, STOCKPREYFULLPRINTER, PREDPREYSTDPRINTER,
  STOCKFULLPRINTER, FORMATEDSTOCKPRINTER, FORMATEDCHATPRINTER, FORMATEDPREYPRINTER,
  MORTPRINTER, FORMATEDCATCHPRINTER, BIOMASSPRINTER, LIKELIHOODPRINTER };

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
  Printer(PrinterType TYPE) { type = TYPE; };
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
   * \brief This will return the type of printer class
   * \return type
   */
  PrinterType Type() { return type; };
protected:
  /**
   * \brief This stores information about when the printer output is required in the model
   */
  ActionAtTimes aat;
private:
  /**
   * \brief This denotes what type of printer class has been created
   */
  PrinterType type;
};

#endif
