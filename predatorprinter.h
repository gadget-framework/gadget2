#ifndef predatorprinter_h
#define predatorprinter_h

#include "printer.h"
#include "gadget.h"

class PredatorPrinter;
class PredatorAggregator;

/**
 * \class PredatorPrinter
 * \brief This is the class used to print the predation information for one or more predators consuming one or more preys
 */
class PredatorPrinter : public Printer {
public:
  /**
   * \brief This is the default PredatorPrinter constructor
   * \param infile is the CommentStream to read the printer parameters from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  PredatorPrinter(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This is the default PredatorPrinter destructor
   */
  virtual ~PredatorPrinter();
  /**
   * \brief This will select the predators and preys required for the printer class to print the requested information
   * \param predatorvec is the PredatorPtrVector listing all the available predators
   * \param preyvec is the PreyPtrVector listing all the available preys
   */
  virtual void SetPredAndPrey(PredatorPtrVector& predatorvec, PreyPtrVector& preyvec);
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
   * \brief This is the CharPtrVector of the names for the predator lengths used for printing
   */
  CharPtrVector predlenindex;
  /**
   * \brief This is the CharPtrVector of the names for the prey lengths used for printing
   */
  CharPtrVector preylenindex;
  /**
   * \brief This is the CharPtrVector of the names for the predator names used for printing
   */
  CharPtrVector predatornames;
  /**
   * \brief This is the CharPtrVector of the names for the prey names used for printing
   */
  CharPtrVector preynames;
  /**
   * \brief This is the LengthGroupDivision of the predator
   */
  LengthGroupDivision* predLgrpDiv;
  /**
   * \brief This is the LengthGroupDivision of the prey
   */
  LengthGroupDivision* preyLgrpDiv;
  /**
   * \brief This is the PredatorAggregator used to collect information about the predation
   */
  PredatorAggregator* aggregator;
  /**
   * \brief This ofstream is the file that all the model information gets sent to
   */
  ofstream outfile;
};

#endif
