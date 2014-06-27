#ifndef predatoroveraggregator_h
#define predatoroveraggregator_h

#include "agebandmatrix.h"
#include "intmatrix.h"
#include "predatorptrvector.h"

/**
 * \class PredatorOverAggregator
 * \brief This is the class used to aggregate the calculated overconsumption by predators
 */
class PredatorOverAggregator {
public:
  /**
   * \brief This is the PredatorOverAggregator constructor
   * \param predators is the PredatorPtrVector of the predators that will be aggregated
   * \param Areas is the IntMatrix of areas that the overconsumption can occur on
   * \param LgrpDiv is the LengthGroupDivision for the predators
   */
  PredatorOverAggregator(const PredatorPtrVector& predators,
    const IntMatrix& Areas, const LengthGroupDivision* const LgrpDiv);
  /**
   * \brief This is the default PredatorOverAggregator destructor
   */
  ~PredatorOverAggregator() {};
  /**
   * \brief This will reset the predator overconsumption information
   */
  void Reset();
  /**
   * \brief This will aggregate the predator overconsumption that has taken place
   */
  void Sum();
  /**
   * \brief This will return the calculated overconsumption
   * \return total, the DoubleMatrix of overconsumption
   */
  const DoubleMatrix& getSum() const { return total; };
protected:
  /**
   * \brief This is the PredatorPtrVector of predators that will be aggregated
   */
  PredatorPtrVector predators;
  /**
   * \brief This is the IntMatrix of length groups of the predators
   */
  IntMatrix predConv;
  /**
   * \brief This is the IntMatrix used to store area information
   */
  IntMatrix areas;
  /**
   * \brief This is the DoubleMatrix of the overconsumption that has taken place
   */
  DoubleMatrix total;
};

#endif
