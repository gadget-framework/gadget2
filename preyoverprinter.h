#ifndef preyoverprinter_h
#define preyoverprinter_h

#include "printer.h"
#include "preyoveraggregator.h"

/**
 * \class PreyOverPrinter
 * \brief This is the class used to print the overconsumption information for one or more preys
 */
class PreyOverPrinter : public Printer {
public:
  /**
   * \brief This is the default PreyOverPrinter constructor
   * \param infile is the CommentStream to read the printer parameters from
   * \param TimeInfo is the TimeClass for the current model
   */
  PreyOverPrinter(CommentStream& infile, const TimeClass* const TimeInfo);
  /**
   * \brief This is the default PreyOverPrinter destructor
   */
  virtual ~PreyOverPrinter();
  /**
   * \brief This will select the preys required for the printer class to print the requested information
   * \param preyvec is the PreyPtrVector of all the available preys
   * \param Area is the AreaClass for the current model
   */
  void setPrey(PreyPtrVector& preyvec, const AreaClass* const Area);
  /**
   * \brief This will print the requested information for the printer class to the ofstream specified
   * \param TimeInfo is the TimeClass for the current model
   * \param printtime is the flag to denote when the printing is taking place
   */
  virtual void Print(const TimeClass* const TimeInfo, int printtime);
protected:
  /**
   * \brief This is the IntMatrix used to store aggregated age information
   */
  IntMatrix areas;
  /**
   * \brief This is the CharPtrVector of the names of the aggregated areas to print
   */
  CharPtrVector areaindex;
  /**
   * \brief This is the CharPtrVector of the names of the lengths to print
   */
  CharPtrVector lenindex;
  /**
   * \brief This is the CharPtrVector of the names of the prey names to print
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
   * \brief This is the DoubleMatrix used to temporarily store the information returned from the aggregatation function
   */
  const DoubleMatrix* dptr;
};

#endif
