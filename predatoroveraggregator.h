#ifndef predatoroveraggregator_h
#define predatoroveraggregator_h

#include "agebandmatrix.h"
#include "intmatrix.h"
#include "predatorptrvector.h"

class PredatorOverAggregator;

/**
 * \class PredatorOverAggregator
 * \brief This is the class used to calculate the overconsumption by predators
 */
class PredatorOverAggregator {
public:
  /**
   * \brief This is the PredatorOverAggregator constructor
   * \param predators is the PredatorPtrVector listing all the available predators
   * \param Areas is the matrix of areas that the overconsumption can occur on
   * \param predLgrpDiv is the LengthGroupDivision for the predators
   */
  PredatorOverAggregator(const PredatorPtrVector& predators,
    const IntMatrix& Areas, const LengthGroupDivision* const predLgrpDiv);
  /**
   * \brief This is the default PredatorOverAggregator destructor
   */
  ~PredatorOverAggregator() {};
  /**
   * \brief This will calculate the predator overconsumption that has taken place
   */
  void Sum();
  /**
   * \brief This will return the calculated overconsumption
   * \return total
   */
  const DoubleMatrix& ReturnSum() const { return total; };
protected:
  /**
   * \brief This is the PredatorPtrVector of predators
   */
  PredatorPtrVector predators;
  /**
   * \brief This is the matrix of length groups of the predators
   */
  IntMatrix predConv;
  /**
   * \brief This is the matrix of the areas on which the overconsumption will be calculated
   */
  IntMatrix areas;
  /**
   * \brief This is the matrix of the overconsumption that has taken place
   */
  DoubleMatrix total;
};

#endif
