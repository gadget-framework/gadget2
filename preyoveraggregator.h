#ifndef preyoveraggregator_h
#define preyoveraggregator_h

#include "agebandmatrix.h"
#include "intmatrix.h"
#include "preyptrvector.h"

/**
 * \class PreyOverAggregator
 * \brief This is the class used to aggregate the calculated overconsumption of preys
 */
class PreyOverAggregator {
public:
  /**
   * \brief This is the PreyOverAggregator constructor
   * \param preys is the PreyPtrVector of the preys that will be aggregated
   * \param Areas is the IntMatrix of areas that the overconsumption can occur on
   * \param LgrpDiv is the LengthGroupDivision for the preys
   */
  PreyOverAggregator(const PreyPtrVector& preys,
    const IntMatrix& Areas, const LengthGroupDivision* const LgrpDiv);
  /**
   * \brief This is the default PreyOverAggregator destructor
   */
  ~PreyOverAggregator() {};
  /**
   * \brief This will reset the prey overconsumption information
   */
  void Reset();
  /**
   * \brief This will aggregate the prey overconsumption that has taken place
   */
  void Sum();
  /**
   * \brief This will return the calculated overconsumption
   * \return total, the DoubleMatrix of overconsumption
   */
  const DoubleMatrix& getSum() const { return total; };
protected:
  /**
   * \brief This is the PreyPtrVector of preys that will be aggregated
   */
  PreyPtrVector preys;
  /**
   * \brief This is the IntMatrix of length groups of the preys
   */
  IntMatrix preyConv;
  /**
   * \brief This will aggregate the predator overconsumption that has taken place
   */
  IntMatrix areas;
  /**
   * \brief This is the DoubleMatrix of the overconsumption that has taken place
   */
  DoubleMatrix total;
};

#endif
