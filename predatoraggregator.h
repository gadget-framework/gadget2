#ifndef predatoraggregator_h
#define predatoraggregator_h

#include "agebandmatrix.h"
#include "doublematrixptrvector.h"
#include "intmatrix.h"
#include "predatorptrvector.h"
#include "preyptrvector.h"
#include "charptrvector.h"

/**
 * \class PredatorAggregator
 * \brief This is the class used to aggregate predation information
 */
class PredatorAggregator {
public:
  /**
   * \brief This is the PredatorAggregator constructor
   * \param predators is the PredatorPtrVector of the predators that will be aggregated
   * \param preys is the PreyPtrVector of the preys that will be aggregated
   * \param Areas is the IntMatrix of areas that the predation can take place on
   * \param predLgrpDiv is the LengthGroupDivision of the predators
   * \param preyLgrpDiv is the LengthGroupDivision of the preys
   */
  PredatorAggregator(const PredatorPtrVector& predators, const PreyPtrVector& preys,
    const IntMatrix& Areas, const LengthGroupDivision* const predLgrpDiv,
    const LengthGroupDivision* const preyLgrpDiv);
  /**
   * \brief This is the default PredatorAggregator destructor
   */
  ~PredatorAggregator();
  /**
   * \brief This will print the aggregator information to the ofstream specified
   * \param outfile is the ofstream to the file that the aggregator information gets sent to
   */
  void Print(ofstream &outfile) const;
  /**
   * \brief This function will reset the predation  data
   */
  void Reset();
  /**
   * \brief This function will aggregate the predation biomass data
   */
  void Sum();
  /**
   * \brief This function will aggregate the predation number data
   */
  void NumberSum();
  /**
   * \brief This will return the DoubleMatrixPtrVector containing the aggregated predation information
   * \return total, the DoubleMatrixPtrVector of aggregated predation information
   */
  const DoubleMatrixPtrVector& getSum() const { return total; };
protected:
  /**
   * \brief This is the PredatorPtrVector of the predators that will be aggregated
   */
  PredatorPtrVector predators;
  /**
   * \brief This is the PreyPtrVector of the preys that will be aggregated
   */
  PreyPtrVector preys;
  /**
   * \brief This is the IntMatrix used to convert length group information for predators
   * \note predConv[i][j] is the index of the length group in DoubleMatrixPtrVector total to which length group j of predator i belongs.
   */
  IntMatrix predConv;
  /**
   * \brief This is the IntMatrix used to convert length group information for preys
   * \note preyConv[i][j] is the index of the length group in DoubleMatrixPtrVector total to which length group j of prey i belongs.
   */
  IntMatrix preyConv;
  /**
   * \brief This is the IntMatrix used to store area information
   */
  IntMatrix areas;
  /**
   * \brief This is the IntMatrix used to store predation information
   * \note doeseat[i][j] is a flag to denote whether predator i eats prey j
   */
  IntMatrix doeseat;
  /**
   * \brief This is the DoubleMatrixPtrVector used to store the aggregated predation information
   */
  DoubleMatrixPtrVector total;
  /**
   * \brief This is the DoubleMatrix used to temporarily store information when aggregating the predation
   */
  const DoubleMatrix* dptr;
};

#endif
