#ifndef predstdinfobylength_h
#define predstdinfobylength_h

#include "areatime.h"
#include "lengthgroup.h"
#include "bandmatrix.h"
#include "abstrpredstdinfo.h"
#include "abstrpreystdinfobylength.h"
#include "poppredator.h"
#include "prey.h"
#include "stockprey.h"

/**
 * \class PredStdInfoByLength
 * \brief This is the class used to calculate information about the predator by length group from the predation by the predator of a prey
 */
class PredStdInfoByLength : protected LivesOnAreas {
public:
  /**
   * \brief This is the PredStdInfo constructor for a non-dynamic prey (eg otherfood)
   * \param predator is the PopPredator of the predator - prey pair
   * \param prey is the Prey of the predator - prey pair
   * \param areas is the IntVector of areas that the predation can take place on
   */
  PredStdInfoByLength(const PopPredator* predator, const Prey* prey,
    const IntVector& areas);
  /**
   * \brief This is the PredStdInfoByLength constructor for a dynamic prey (eg stock)
   * \param predator is the PopPredator of the predator - prey pair
   * \param prey is the StockPrey of the predator - prey pair
   * \param areas is the IntVector of areas that the predation can take place on
   */
  PredStdInfoByLength(const PopPredator* predator, const StockPrey* prey,
    const IntVector& areas);
  /**
   * \brief This is the default PredStdInfoByLength destructor
   */
  virtual ~PredStdInfoByLength();
  /**
   * \brief This will return the number consumed by length group for a specified area
   * \param area is the area that the predation is calculated on
   * \return N, a BandMatrix containing the number consumed
   */
  const BandMatrix& NconsumptionByLength(int area) const;
  /**
   * \brief This will return the biomass consumed by length group for a specified area
   * \param area is the area that the predation is calculated on
   * \return B, a BandMatrix containing the biomass consumed
   */
  const BandMatrix& BconsumptionByLength(int area) const;
  /**
   * \brief This will return the mortality by length group caused by the predation for a specified area
   * \param area is the area that the predation is calculated on
   * \return M, a BandMatrix containing the mortality
   */
  const BandMatrix& MortalityByLength(int area) const;
  /**
   * \brief This is the function that calculates the predation
   * \param TimeInfo is the TimeClass for the current model
   * \param area is the area that the predation is being calculated on
   */
  virtual void Sum(const TimeClass* const TimeInfo, int area);
  /**
   * \brief This will return the length group of the predator
   * \return predLgrpDiv, a LengthGroupDivision for the predator
   */
  const LengthGroupDivision* returnPredLengthGroupDiv() const { return predLgrpDiv; };
  /**
   * \brief This will return the length group of the prey
   * \return preyLgrpDiv, a LengthGroupDivision for the prey
   */
  const LengthGroupDivision* returnPreyLengthGroupDiv() const { return preyLgrpDiv; };
private:
  /**
   * \brief This function will initialise and resize the objects created by the constructor
   */
  void InitialiseObjects();
  /**
   * \brief This is the AbstrPreyStdInfoByLength giving information about the predation of the prey
   */
  AbstrPreyStdInfoByLength* preyinfo;
  /**
   * \brief This is the PopPredator of the predator - prey pair
   */
  const PopPredator* predator;
  /**
   * \brief This is the Prey of the predator - prey pair
   */
  const Prey* prey;
  /**
   * \brief This is the BandMatrixVector of the mortality caused by the predation by length group
   */
  BandMatrixVector MortbyLength;
  /**
   * \brief This is the BandMatrixVector of the number consumed by length group
   */
  BandMatrixVector NconbyLength;
  /**
   * \brief This is the BandMatrixVector of the biomass consumed by length group
   */
  BandMatrixVector BconbyLength;
  /**
   * \brief This is the LengthGroupDivision of the predator
   */
  const LengthGroupDivision* predLgrpDiv;
  /**
   * \brief This is the LengthGroupDivision of the prey
   */
  const LengthGroupDivision* preyLgrpDiv;
};

#endif
