#ifndef predatoroverprinter_h
#define predatoroverprinter_h

#include "predatorptrvector.h"
#include "predatoroveraggregator.h"
#include "printer.h"

/**
 * \class PredatorOverPrinter
 * \brief This is the class used to print the overconsumption information for one or more predators
 */
class PredatorOverPrinter : public Printer {
public:
  /**
   * \brief This is the default PredatorOverPrinter constructor
   * \param infile is the CommentStream to read the printer parameters from
   * \param TimeInfo is the TimeClass for the current model
   */
  PredatorOverPrinter(CommentStream& infile, const TimeClass* const TimeInfo);
  /**
   * \brief This is the default PredatorOverPrinter destructor
   */
  virtual ~PredatorOverPrinter();
  /**
   * \brief This will select the predators required for the printer class to print the requested information
   * \param predatorvec is the PredatorPtrVector of all the available predators
   * \param Area is the AreaClass for the current model
   */
  void setPredator(PredatorPtrVector& predatorvec, const AreaClass* const Area);
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
   * \brief This is the CharPtrVector of the names of the aggregated areas to print
   */
  CharPtrVector areaindex;
  /**
   * \brief This is the CharPtrVector of the names of the lengths to print
   */
  CharPtrVector lenindex;
  /**
   * \brief This is the CharPtrVector of the names of the predator names to print
   */
  CharPtrVector predatornames;
  /**
   * \brief This is the LengthGroupDivision of the predator
   */
  LengthGroupDivision* predLgrpDiv;
  /**
   * \brief This is the PredatorOverAggregator used to collect information about the overconsumption
   */
  PredatorOverAggregator* aggregator;
  /**
   * \brief This is the DoubleMatrix used to temporarily store the information returned from the aggregatation function
   */
  const DoubleMatrix* dptr;
};

#endif
