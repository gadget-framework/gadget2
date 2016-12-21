#ifndef summaryprinter_h
#define summaryprinter_h

#include "printer.h"

/**
 * \class SummaryPrinter
 * \brief This is the class used to print likelihood summary information
 */
class SummaryPrinter : public Printer {
public:
  /**
   * \brief This is the default SummaryPrinter constructor
   * \param infile is the CommentStream to read the printer parameters from
   */
  SummaryPrinter(CommentStream& infile);
  /**
   * \brief This is the default SummaryPrinter destructor
   */
  virtual ~SummaryPrinter();
  /**
   * \brief This will select the likelihood components required for the printer class to print the requested information
   * \param likevec is the LikelihoodPtrVector of all the available likelihood components
   */
  virtual void setLikelihood(LikelihoodPtrVector& likevec);
  /**
   * \brief This will print the requested information for the printer class to the ofstream specified
   * \param TimeInfo is the TimeClass for the current model
   * \param printtime is the flag to denote when the printing is taking place
   */
  virtual void Print(const TimeClass* const TimeInfo, int printtime);
protected:
  /**
   * \brief This is the LikelihoodPtrVector that is storing the likelihood component to print
   */
  LikelihoodPtrVector like;
};

#endif
