#ifndef stockpreyfullprinter_h
#define stockpreyfullprinter_h

#include "commentstream.h"
#include "printer.h"
#include "gadget.h"

class StockPreyFullPrinter;
class TimeClass;
class StockPreyStdInfo;
class AreaClass;
class LengthGroupDivision;

/**
 * \class StockPreyFullPrinter
 * \brief This is the class used to print detailed information about a prey
 */
class StockPreyFullPrinter : public Printer {
public:
  /**
   * \brief This is the default StockPreyFullPrinter constructor
   * \param infile is the CommentStream to read the printer parameters from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  StockPreyFullPrinter(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This is the default StockPreyFullPrinter destructor
   */
  virtual ~StockPreyFullPrinter();
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
   * \brief This vector stores information about the areas used for printing
   */
  IntVector areas;
  /**
   * \brief This vector stores information about the outer areas used for printing
   */
  IntVector outerareas;
  /**
   * \brief This is the StockPreyStdInfo used to collect information about the consumption of the stock
   */
  StockPreyStdInfo* preyinfo;
  /**
   * \brief This ofstream is the file that all the model information gets sent to
   */
  ofstream outfile;
};

#endif
