#ifndef predatoraggregator_h
#define predatoraggregator_h

#include "agebandmatrix.h"
#include "intmatrix.h"
#include "predatorptrvector.h"
#include "preyptrvector.h"
#include "charptrvector.h"

class PredatorAggregator;

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
   * \brief This is the PredatorAggregator constructor for a fleksibest-style mortality model
   * \param prednames is the CharPtrVector of the names of the predators that will be aggregated
   * \param preys is the PreyPtrVector of the preys that will be aggregated
   * \param Areas is the IntMatrix of areas that the predation can take place on
   * \param ages is the IntMatrix of ages of the predators
   * \param preyLgrpDiv is the LengthGroupDivision of the preys
   */
  PredatorAggregator(const CharPtrVector& prednames, PreyPtrVector& preys,
    const IntMatrix& Areas, const IntVector& ages,
    const LengthGroupDivision* const preyLgrpDiv);
  /**
   * \brief This is the default PredatorAggregator destructor
   */
  ~PredatorAggregator() {};
  /**
   * \brief This function will aggregate the predation biomass data
   */
  void Sum();
  /**
   * \brief This function will aggregate the predation biomass data for a fleksibest-style mortality model
   * \param dummy is a flag to denote that this function is to be used with a fleksibest-style mortality model
   */
  void Sum(int dummy);
  /**
   * \brief This function will aggregate the predation number data
   */
  void NumberSum();
  /**
   * \brief This will return the BandMatrixVector containing the aggregated predation information
   * \return total, the BandMatrixVector of aggregated predation information
   */
  const BandMatrixVector& returnSum() const { return total; };
protected:
  /**
   * \brief This is the PredatorPtrVector of the predators that will be aggregated
   */
  PredatorPtrVector predators;
  /**
   * \brief This is the PreyPtrVector of the preys that will be aggregated
   */
  PreyPtrVector preys;
  IntMatrix predConv;  //[predator][predatorLengthGroup]
  IntMatrix preyConv;  //[prey][preyLengthGroup]
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
   * \brief This is the BandMatrixVector used to store the aggregated predation information
   */
  BandMatrixVector total;
};

#endif
