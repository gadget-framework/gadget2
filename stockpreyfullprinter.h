#ifndef stockpreyfullprinter_h
#define stockpreyfullprinter_h

#include "printer.h"
#include "stockpreyaggregator.h"

/**
 * \class StockPreyFullPrinter
 * \brief This is the class used to print detailed information about a prey
 */
class StockPreyFullPrinter : public Printer {
public:
  /**
   * \brief This is the default StockPreyFullPrinter constructor
   * \param infile is the CommentStream to read the printer parameters from
   * \param TimeInfo is the TimeClass for the current model
   */
  StockPreyFullPrinter(CommentStream& infile, const TimeClass* const TimeInfo);
  /**
   * \brief This is the default StockPreyFullPrinter destructor
   */
  virtual ~StockPreyFullPrinter();
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
   * \brief This is the name of the stock prey
   */
  char* preyname;
  /**
   * \brief This is the minimum age of the stock prey
   */
  int minage;
  /**
   * \brief This is the IntVector used to store aggregated area information
   */
  IntVector outerareas;
  /**
   * \brief This is the LengthGroupDivision of the prey
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the StockPreyAggregator used to collect information about the stock prey
   */
  StockPreyAggregator* aggregator;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to temporarily store the information returned from the aggregatation function
   */
  const AgeBandMatrixPtrVector* alptr;
};

#endif
