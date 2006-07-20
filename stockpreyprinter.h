#ifndef stockpreyprinter_h
#define stockpreyprinter_h

#include "printer.h"
#include "stockpreyaggregator.h"

/**
 * \class StockPreyPrinter
 * \brief This is the class used to print information about one or more stock preys
 */
class StockPreyPrinter : public Printer {
public:
  /**
   * \brief This is the default StockPreyPrinter constructor
   * \param infile is the CommentStream to read the printer parameters from
   * \param TimeInfo is the TimeClass for the current model
   */
  StockPreyPrinter(CommentStream& infile, const TimeClass* const TimeInfo);
  /**
   * \brief This is the default StockPreyPrinter destructor
   */
  virtual ~StockPreyPrinter();
  /**
   * \brief This will select the preys required for the printer class to print the requested information
   * \param preyvec is the PreyPtrVector of all the available preys
   * \param Area is the AreaClass for the current model
   */
  virtual void setPrey(PreyPtrVector& preyvec, const AreaClass* const Area);
  /**
   * \brief This will print the requested information for the printer class to the ofstream specified
   * \param TimeInfo is the TimeClass for the current model
   * \param printtime is the flag to denote when the printing is taking place
   */
  virtual void Print(const TimeClass* const TimeInfo, int printtime);
protected:
  /**
   * \brief This is the IntMatrix used to store aggregated area information
   */
  IntMatrix areas;
  /**
   * \brief This is the IntMatrix used to store aggregated age information
   */
  IntMatrix ages;
  /**
   * \brief This is the CharPtrVector of the names of the aggregated areas to print
   */
  CharPtrVector areaindex;
  /**
   * \brief This is the CharPtrVector of the names of the aggregated ages to print
   */
  CharPtrVector ageindex;
  /**
   * \brief This is the CharPtrVector of the names of the lengths to print
   */
  CharPtrVector lenindex;
  /**
   * \brief This is the CharPtrVector of the names of the preys to print
   */
  CharPtrVector preynames;
  /**
   * \brief This is the LengthGroupDivision of the prey
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the StockPreyAggregator used to collect information about the stock preys
   */
  StockPreyAggregator* aggregator;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to temporarily store the information returned from the aggregatation function
   */
  const AgeBandMatrixPtrVector* alptr;
};

#endif
