#ifndef likelihoodprinter_h
#define likelihoodprinter_h

#include "printer.h"

/**
 * \class LikelihoodPrinter
 * \brief This is the class used to print likelihood information
 */
class LikelihoodPrinter : public Printer {
public:
  /**
   * \brief This is the default LikelihoodPrinter constructor
   * \param infile is the CommentStream to read the printer parameters from
   * \param TimeInfo is the TimeClass for the current model
   */
  LikelihoodPrinter(CommentStream& infile, const TimeClass* const TimeInfo);
  /**
   * \brief This is the default LikelihoodPrinter destructor
   */
  virtual ~LikelihoodPrinter();
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
   * \brief This is the CharPtrVector of the names of the likelihood components to print
   */
  CharPtrVector likenames;
  /**
   * \brief This is the LikelihoodPtrVector that is storing the likelihood component to print
   */
  LikelihoodPtrVector like;
};

#endif
