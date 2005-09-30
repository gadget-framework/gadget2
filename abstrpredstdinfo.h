#ifndef abstrpredstdinfo_h
#define abstrpredstdinfo_h

#include "bandmatrixptrvector.h"
#include "livesonareas.h"
#include "areatime.h"

/**
 * \class AbstrPredStdInfo
 * \brief This is the base class used to calculate information about the predator from the predation by the predator of a prey
 * \note This will always be overridden by the derived classes that actually calculate the predation
 */
class AbstrPredStdInfo : protected LivesOnAreas {
public:
  /**
   * \brief This is the AbstrPredStdInfo constructor
   * \param areas is the IntVector of areas that the predation can take place on
   * \param predminage is the minimum age of the predator (default value 0)
   * \param predmaxage is the maximum age of the predator (default value 0)
   * \param preyminage is the minimum age of the prey (default value 0)
   * \param preymaxage is the maximum age of the prey (default value 0)
   */
  AbstrPredStdInfo(const IntVector& areas, int predminage = 0,
    int predmaxage = 0, int preyminage = 0, int preymaxage = 0);
  /**
   * \brief This is the default AbstrPredStdInfo destructor
   */
  virtual ~AbstrPredStdInfo() {};
  /**
   * \brief This will return the number consumed by length group for a specified area
   * \param area is the area that the predation is calculated on
   * \return 0 (will be overridden in derived classes)
   */
  virtual const BandMatrix& NconsumptionByLength(int area) const = 0;
  /**
   * \brief This will return the biomass consumed by length group for a specified area
   * \param area is the area that the predation is calculated on
   * \return 0 (will be overridden in derived classes)
   */
  virtual const BandMatrix& BconsumptionByLength(int area) const = 0;
  /**
   * \brief This will return the mortality by length group caused by the predation for a specified area
   * \param area is the area that the predation is calculated on
   * \return 0 (will be overridden in derived classes)
   */
  virtual const BandMatrix& MortalityByLength(int area) const = 0;
  /**
   * \brief This will return the number consumed by age group for a specified area
   * \param area is the area that the predation is calculated on
   * \return N, a BandMatrix containing the number consumed
   */
  const BandMatrix& NconsumptionByAge(int area) const;
  /**
   * \brief This will return the biomass consumed by age group for a specified area
   * \param area is the area that the predation is calculated on
   * \return B, a BandMatrix containing the biomass consumed
   */
  const BandMatrix& BconsumptionByAge(int area) const;
  /**
   * \brief This will return the mortality by age group caused by the predation for a specified area
   * \param area is the area that the predation is calculated on
   * \return M, a BandMatrix containing the mortality
   */
  const BandMatrix& MortalityByAge(int area) const;
  /**
   * \brief This is the function that calculates the predation
   * \param TimeInfo is the TimeClass for the current model
   * \param area is the area that the predation is being calculated on
   * \note This will be overridden by the derived classes that actually calculate the predation
   */
  virtual void Sum(const TimeClass* const TimeInfo, int area) = 0;
protected:
  /**
   * \brief This is the BandMatrixPtrVector of the number consumed by age group
   */
  BandMatrixPtrVector NconbyAge;
  /**
   * \brief This is the BandMatrixPtrVector of the biomass consumed by age group
   */
  BandMatrixPtrVector BconbyAge;
  /**
   * \brief This is the BandMatrixPtrVector of the mortality caused by the predation by age group
   */
  BandMatrixPtrVector MortbyAge;
};

#endif
