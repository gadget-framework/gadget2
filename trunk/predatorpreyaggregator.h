#ifndef predatorpreyaggregator_h
#define predatorpreyaggregator_h

#include "areatime.h"
#include "conversionindexptrvector.h"
#include "agebandmatrixptrvector.h"
#include "doublematrixptrvector.h"
#include "lengthgroup.h"
#include "predatorptrvector.h"
#include "preyptrvector.h"
#include "intmatrix.h"

/**
 * \class PredatorPreyAggregator
 * \brief This is the class used to aggregate predator-prey information
 */
class PredatorPreyAggregator {
public:
  /**
   * \brief This is the PredatorPreyAggregator constructor
   * \param Predators is the PredatorPtrVector of the predators that will be aggregated
   * \param Preys is the PreyPtrVector of the preys that will be aggregated
   * \param Lgrpdiv is the LengthGroupDivision of the preys
   * \param Areas is the IntMatrix of areas that the predation can take place on
   * \param Ages is the IntMatrix of ages of the preys
   */
  PredatorPreyAggregator(const PredatorPtrVector& Predators, const PreyPtrVector& Preys,
   LengthGroupDivision* const Lgrpdiv, const IntMatrix& Areas, const IntMatrix& Ages);
  /**
   * \brief This is the default PredatorPreyAggregator destructor
   */
  ~PredatorPreyAggregator();
  /**
   * \brief This function will reset the predation information
   */
  void Reset();
  /**
   * \brief This function will aggregate the predation data
   * \param TimeInfo is the TimeClass for the current model
   */
  void Sum(const TimeClass* const TimeInfo);
  /**
   * \brief This will return the AgeBandMatrixPtrVector containing the aggregated predation information
   * \return consume, the AgeBandMatrixPtrVector of aggregated predation information
   */
  const AgeBandMatrixPtrVector& getConsumption() const { return consume; };
  /**
   * \brief This will return the DoubleMatrixPtrVector containing the aggregated mortality information
   * \return mortality, the DoubleMatrixPtrVector of mortality information
   */
  const DoubleMatrixPtrVector& getMortality() const { return mortality; };
private:
  /**
   * \brief This is the PredatorPtrVector of the predators that will be aggregated
   */
  PredatorPtrVector predators;
  /**
   * \brief This is the PreyPtrVector of the preys that will be aggregated
   */
  PreyPtrVector preys;
  /**
   * \brief This is the LengthGroupDivision used to store length information
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the IntMatrix used to store area information
   */
  IntMatrix areas;
  /**
   * \brief This is the IntMatrix used to store age information
   */
  IntMatrix ages;
  /**
   * \brief This is the IntMatrix used to store predation information
   * \note doeseat[i][j] is the flag to denote whether predator i eats prey j
   */
  IntMatrix doeseat;
  /**
   * \brief This is the ConversionIndexPtrVector used to convert the length groups of the preys to be aggregated
   */
  ConversionIndexPtrVector CI;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to store information about the prey population before predation
   */
  AgeBandMatrixPtrVector total;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to store information about the population of the prey that is consumed
   */
  AgeBandMatrixPtrVector consume;
  /**
   * \brief This is the DoubleMatrixPtrVector used to store the calculated mortality caused by the predation
   */
  DoubleMatrixPtrVector mortality;
  /**
   * \brief This is the DoubleVector used to temporarily store suitability information when aggregating the predation
   */
  const DoubleVector* suitptr;
  /**
   * \brief This is the AgeBandMatrix used to temporarily store information when aggregating the predation
   */
  const AgeBandMatrix* alptr;
};

#endif
