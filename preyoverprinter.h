#ifndef preyoverprinter_h
#define preyoverprinter_h

#include "printer.h"
#include "commentstream.h"
#include "gadget.h"

class PreyOverPrinter;
class TimeClass;
class LengthGroupDivision;
class PreyOverAggregator;
class AreaClass;

/**
 * \class PreyOverPrinter
 * \brief This is the class used to print the overconsumption information for one or more preys
 */
class PreyOverPrinter : public Printer {
public:
  /**
   * \brief This is the default PreyOverPrinter constructor
   * \param infile is the CommentStream to read the printer parameters from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  PreyOverPrinter(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This is the default PreyOverPrinter destructor
   */
  virtual ~PreyOverPrinter();
  /**
   * \brief This will select the preys required for the printer class to print the requested information
   * \param preyvec is the PreyPtrVector listing all the available preys
   */
  void SetPrey(PreyPtrVector& preyvec);
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
   * \brief This is the CharPtrVector of the names for the prey names used for printing
   */
  CharPtrVector preynames;
  /**
   * \brief This is the LengthGroupDivision of the prey
   */
  LengthGroupDivision* preyLgrpDiv;
  /**
   * \brief This is the PreyOverAggregator used to collect information about the overconsumption
   */
  PreyOverAggregator* aggregator;
  /**
   * \brief This ofstream is the file that all the model information gets sent to
   */
  ofstream outfile;
};

#endif
