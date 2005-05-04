#ifndef stockpreyfullprinter_h
#define stockpreyfullprinter_h

#include "printer.h"
#include "stockpreystdinfo.h"

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
   * \brief This will select the stocks required for the printer class to print the requested information
   * \param stockvec is the StockPtrVector of all the available stocks
   */
  virtual void setStock(StockPtrVector& stockvec);
  /**
   * \brief This will print the requested information for the printer class to the ofstream specified
   * \param TimeInfo is the TimeClass for the current model
   * \param printtime is a flag to denote when the printing is taking place
   */
  virtual void Print(const TimeClass* const TimeInfo, int printtime);
protected:
  /**
   * \brief This is the name of the stock
   */
  char* stockname;
  /**
   * \brief This is the IntVector used to store aggregated internal area information
   */
  IntVector areas;
  /**
   * \brief This is the IntVector used to store aggregated area information
   */
  IntVector outerareas;
  /**
   * \brief This is the StockPreyStdInfo used to collect information about the consumption of the stock
   */
  StockPreyStdInfo* preyinfo;
  /**
   * \brief This is the LengthGroupDivision used to store aggregated area information
   */
  LengthGroupDivision* LgrpDiv;
};

#endif
