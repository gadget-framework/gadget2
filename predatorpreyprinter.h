#ifndef predatorpreyprinter_h
#define predatorpreyprinter_h

#include "printer.h"
#include "predatorpreyaggregator.h"
#include "gadget.h"

/**
 * \class PredatorPreyPrinter
 * \brief This is the class used to print the predation information for one or more predators consuming one or more preys
 */
class PredatorPreyPrinter : public Printer {
public:
  /**
   * \brief This is the default PredatorPreyPrinter constructor
   * \param infile is the CommentStream to read the printer parameters from
   * \param TimeInfo is the TimeClass for the current model
   */
  PredatorPreyPrinter(CommentStream& infile, const TimeClass* const TimeInfo);
  /**
   * \brief This is the default PredatorPreyPrinter destructor
   */
  virtual ~PredatorPreyPrinter();
  /**
   * \brief This will select the predators and preys required for the printer class to print the requested information
   * \param predatorvec is the PredatorPtrVector of all the available predators
   * \param preyvec is the PreyPtrVector of all the available preys
   * \param Area is the AreaClass for the current model
   */
  virtual void setPredAndPrey(PredatorPtrVector& predatorvec, PreyPtrVector& preyvec, const AreaClass* const Area);
  /**
   * \brief This will print the requested information for the printer class to the ofstream specified
   * \param TimeInfo is the TimeClass for the current model
   * \param printtime is the flag to denote when the printing is taking place
   */
  virtual void Print(const TimeClass* const TimeInfo, int printtime);
protected:
  /**
   * \brief This is the CharPtrVector of the names of the predators to print
   */
  CharPtrVector predatornames;
  /**
   * \brief This is the CharPtrVector of the names of the preys to print
   */
  CharPtrVector preynames;
  /**
   * \brief This is the IntMatrix used to store aggregated area information
   */
  IntMatrix areas;
  /**
   * \brief This is the IntMatrix used to store aggregated age information
   */
  IntMatrix ages;
  /**
   * \brief This is the LengthGroupDivision used to store aggregated length information
   */
  LengthGroupDivision* LgrpDiv;
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
   * \brief This is the PredatorPreyAggregator used to collect information about the predation
   */
  PredatorPreyAggregator* aggregator;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to temporarily store the population information returned from the aggregatation function
   */
  const AgeBandMatrixPtrVector* alptr;
  /**
   * \brief This is the DoubleMatrix used to temporarily store the mortality information returned from the aggregatation function
   */
  const DoubleMatrix* dptr;
};

#endif
