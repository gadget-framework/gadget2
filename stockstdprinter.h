#ifndef stockstdprinter_h
#define stockstdprinter_h

#include "commentstream.h"
#include "printer.h"
#include "gadget.h"

class StockStdPrinter;
class TimeClass;
class StockAggregator;
class StockPreyStdInfo;
class AreaClass;
class LengthGroupDivision;

/**
 * \class StockStdPrinter
 * \brief This is the class used to print 'standard' information about a stock
 */
class StockStdPrinter : public Printer {
public:
  /**
   * \brief This is the default StockStdPrinter constructor
   * \param infile is the CommentStream to read the printer parameters from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  StockStdPrinter(CommentStream& infile, const AreaClass* const Area, const  TimeClass* const TimeInfo);
  /**
   * \brief This is the default StockStdPrinter destructor
   */
  virtual ~StockStdPrinter();
  /**
   * \brief This will select the stocks required for the printer class to print the requested information
   * \param stockvec is the StockPtrVector listing all the available stocks
   */
  virtual void SetStock(StockPtrVector& stockvec);
  /**
   * \brief This will print the requested information for the printer class to the ofstream specified
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  /**
   * \brief This is the name of the stock
   */
  char* stockname;
  /**
   * \brief This is the LengthGroupDivision of the stock
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This vector stores information about the internal areas used for printing
   */
  IntVector areas;
  /**
   * \brief This vector stores information about the outer areas used for printing
   */
  IntVector outerareas;
  /**
   * \brief This is the minimum age of the stock
   */
  int minage;
  /**
   * \brief This is the maximum age of the stock
   */
  int maxage;
  /**
   * \brief This is the StockAggregator used to collect information about the stock
   */
  StockAggregator* aggregator;
  /**
   * \brief This is the StockPreyStdInfo used to collect information about the consumption of the stock
   */
  StockPreyStdInfo* preyinfo;
  /**
   * \brief This ofstream is the file that all the model information gets sent to
   */
  ofstream outfile;
  /**
   * \brief This is the scaling factor used to scale the size of the stock
   */
  double Scale;
};

#endif
