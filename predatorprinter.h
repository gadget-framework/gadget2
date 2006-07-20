#ifndef predatorprinter_h
#define predatorprinter_h

#include "printer.h"
#include "predatoraggregator.h"
#include "gadget.h"

/**
 * \class PredatorPrinter
 * \brief This is the class used to print the predation information for one or more predators consuming one or more preys
 */
class PredatorPrinter : public Printer {
public:
  /**
   * \brief This is the default PredatorPrinter constructor
   * \param infile is the CommentStream to read the printer parameters from
   * \param TimeInfo is the TimeClass for the current model
   */
  PredatorPrinter(CommentStream& infile, const TimeClass* const TimeInfo);
  /**
   * \brief This is the default PredatorPrinter destructor
   */
  virtual ~PredatorPrinter();
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
   * \brief This is the IntMatrix used to store aggregated area information
   */
  IntMatrix areas;
  /**
   * \brief This is the CharPtrVector of the names of the aggregated areas to print
   */
  CharPtrVector areaindex;
  /**
   * \brief This is the CharPtrVector of the names of the predator lengths to print
   */
  CharPtrVector predlenindex;
  /**
   * \brief This is the CharPtrVector of the names of the prey lengths to print
   */
  CharPtrVector preylenindex;
  /**
   * \brief This is the CharPtrVector of the names of the predator names to print
   */
  CharPtrVector predatornames;
  /**
   * \brief This is the CharPtrVector of the names of the prey names to print
   */
  CharPtrVector preynames;
  /**
   * \brief This is the LengthGroupDivision of the predator
   */
  LengthGroupDivision* predLgrpDiv;
  /**
   * \brief This is the LengthGroupDivision of the prey
   */
  LengthGroupDivision* preyLgrpDiv;
  /**
   * \brief This is the PredatorAggregator used to collect information about the predation
   */
  PredatorAggregator* aggregator;
  /**
   * \brief This is the flag used to denote whether the biomass consumed or the numbers consumed is printed to the outfile (default is to print biomass)
   */
  int biomass;
  /**
   * \brief This is the DoubleMatrix used to temporarily store the information returned from the aggregatation function
   */
  const DoubleMatrix* dptr;
};

#endif
