#ifndef predatoroverprinter_h
#define predatoroverprinter_h

#include "predatorptrvector.h"
#include "printer.h"
#include "gadget.h"

class PredatorOverPrinter;
class TimeClass;
class LengthGroupDivision;
class PredatorOverAggregator;
class AreaClass;

/**
 * \class PredatorOverPrinter
 * \brief This is the class used to print the overconsumption information for one or more predators
 */
class PredatorOverPrinter : public Printer {
public:
  /**
   * \brief This is the default PredatorOverPrinter constructor
   * \param infile is the CommentStream to read the printer parameters from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  PredatorOverPrinter(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This is the default PredatorOverPrinter destructor
   */
  virtual ~PredatorOverPrinter();
  /**
   * \brief This will select the predators required for the printer class to print the requested information
   * \param predatorvec is the PredatorPtrVector listing all the available predators
   */
  void SetPredator(PredatorPtrVector& predatorvec);
  /**
   * \brief This will print the requested information for the printer class to the ofstream specified
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  /**
   * \brief This matrix stores information about the aggregated areas used for printing
   */
  IntMatrix areas;
  /**
   * \brief This is the CharPtrVector of the names for the areas used for printing
   */
  CharPtrVector areaindex;
  /**
   * \brief This is the CharPtrVector of the names for the lengths used for printing
   */
  CharPtrVector lenindex;
  /**
   * \brief This is the CharPtrVector of the names for the predator names used for printing
   */
  CharPtrVector predatornames;
  /**
   * \brief This is the LengthGroupDivision of the predator
   */
  LengthGroupDivision* predLgrpDiv;
  /**
   * \brief This is the PredatorOverAggregator used to collect information about the overconsumption
   */
  PredatorOverAggregator* aggregator;
  /**
   * \brief This ofstream is the file that all the model information gets sent to
   */
  ofstream outfile;
};

#endif
