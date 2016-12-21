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
#include "likelihoodptrvector.h"
#include "charptrvector.h"
#include "intmatrix.h"
#include "gadget.h"

enum PrinterType { STOCKPRINTER = 1, PREDATORPRINTER, PREDATOROVERPRINTER,
  PREYOVERPRINTER, STOCKSTDPRINTER, STOCKPREYFULLPRINTER, STOCKFULLPRINTER,
  LIKELIHOODPRINTER, LIKELIHOODSUMMARYPRINTER, STOCKPREYPRINTER, PREDATORPREYPRINTER };

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
  virtual ~Printer() { delete[] filename; };
  /**
   * \brief This will print the requested information for the printer class to the ofstream specified
   * \param TimeInfo is the TimeClass for the current model
   * \param printtime is the flag to denote when the printing is taking place
   */
  virtual void Print(const TimeClass* const TimeInfo, int printtime) = 0;
  /**
   * \brief This will select the stocks required for the printer class to print the requested information
   * \param stockvec is the StockPtrVector of all the available stocks
   * \param Area is the AreaClass for the current model
   */
  virtual void setStock(StockPtrVector& stockvec, const AreaClass* const Area) {};
  /**
   * \brief This will select the predators and preys required for the printer class to print the requested information
   * \param predatorvec is the PredatorPtrVector of all the available predators
   * \param preyvec is the PreyPtrVector of all the available preys
   * \param Area is the AreaClass for the current model
   */
  virtual void setPredAndPrey(PredatorPtrVector& predatorvec, PreyPtrVector& preyvec, const AreaClass* const Area) {};
  /**
   * \brief This will select the preys required for the printer class to print the requested information
   * \param preyvec is the PreyPtrVector of all the available preys
   * \param Area is the AreaClass for the current model
   */
  virtual void setPrey(PreyPtrVector& preyvec, const AreaClass* const Area) {};
  /**
   * \brief This will select the predators required for the printer class to print the requested information
   * \param predatorvec is the PredatorPtrVector of all the available predators
   * \param Area is the AreaClass for the current model
   */
  virtual void setPredator(PredatorPtrVector& predatorvec, const AreaClass* const Area) {};
  /**
   * \brief This will select the likelihood components required for the printer class to print the requested information
   * \param likevec is the LikelihoodPtrVector of all the available likelihood components
   */
  virtual void setLikelihood(LikelihoodPtrVector& likevec) {};
  /**
   * \brief This will return the name of the output file
   * \return name
   */
  const char* getFileName() const { return filename; };
  /**
   * \brief This will return the type of printer class
   * \return type
   */
  PrinterType getType() const { return type; };
protected:
  /**
   * \brief This ActionAtTimes stores information about when the printer output is required in the model
   */
  ActionAtTimes AAT;
  /**
   * \brief This is the name of the output file that all the model information will get sent to
   */
  char* filename;
  /**
   * \brief This ofstream is the file that all the model information gets sent to
   */
  ofstream outfile;
  /**
   * \brief This is the flag used to denote whether the printing takes place at the start or the end of the timestep
   * \note The default value is 0, which corresponds to printing at the end of the timestep
   */
  int printtimeid;
  /**
   * \brief This is precision which can be used to override the default values when printing the information to the output file
   * \note The default value is 0, which corresponds to using values specified in gadget.h
   */
  int precision;
  /**
   * \brief This is width used when printing the information to the output file
   * \note This value is set to precision + 4
   */
  int width;
private:
  /**
   * \brief This denotes what type of printer class has been created
   */
  PrinterType type;
};

#endif
