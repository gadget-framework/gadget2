#ifndef preyoveraggregator_h
#define preyoveraggregator_h

#include "agebandmatrix.h"
#include "intmatrix.h"
#include "preyptrvector.h"

class PreyOverAggregator;

/**
 * \class PreyOverAggregator
 * \brief This is the class used to calculate the overconsumption of preys
 */
class PreyOverAggregator {
public:
  /**
   * \brief This is the PreyOverAggregator constructor
   * \param preys is the PreyPtrVector listing all the available predators
   * \param Areas is the matrix of areas that the overconsumption can occur on
   * \param preyLgrpDiv is the LengthGroupDivision for the preys
   */
  PreyOverAggregator(const PreyPtrVector& preys,
    const IntMatrix& Areas, const LengthGroupDivision* const preyLgrpDiv);
  /**
   * \brief This is the default PreyOverAggregator destructor
   */
  ~PreyOverAggregator() {};
  /**
   * \brief This will calculate the prey overconsumption that has taken place
   */
  void Sum();
  /**
   * \brief This will return the calculated overconsumption
   * \return total
   */
  const DoubleMatrix& ReturnSum() const { return total; };
protected:
  /**
   * \brief This is the PreyPtrVector of preys
   */
  PreyPtrVector preys;
  /**
   * \brief This is the matrix of length groups of the preys
   */
  IntMatrix preyConv;
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
