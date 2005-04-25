#ifndef abstrpreystdinfo_h
#define abstrpreystdinfo_h

#include "bandmatrixptrvector.h"
#include "livesonareas.h"
#include "areatime.h"
#include "prey.h"

/**
 * \class AbstrPreyStdInfo
 * \brief This is the base class used to calculate information about the prey from the predation by a predator of the prey
 * \note This will always be overridden by the derived classes that actually calculate the predation
 */
class AbstrPreyStdInfo : protected LivesOnAreas {
public:
  /**
   * \brief This is the AbstrPreyStdInfo constructor
   * \param p is the Prey that the predation is consuming
   * \param areas is the IntVector of areas that the predation can take place on
   * \param minage is the minimum age
   * \param maxage is the maximum age
   */
  AbstrPreyStdInfo(const Prey* p, const IntVector& areas,
    int minage = 0, int maxage = 0);
  /**
   * \brief This is the default AbstrPreyStdInfo destructor
   */
  virtual ~AbstrPreyStdInfo() {};
  /**
   * \brief This will return the number consumed by length group for a specified area
   * \param area is the area that the predation is calculated on
   * \return 0 (will be overridden in derived classes)
   */
  virtual const DoubleVector& NconsumptionByLength(int area) const = 0;
  /**
   * \brief This will return the biomass consumed by length group for a specified area
   * \param area is the area that the predation is calculated on
   * \return 0 (will be overridden in derived classes)
   */
  virtual const DoubleVector& BconsumptionByLength(int area) const = 0;
  /**
   * \brief This will return the mortality by length group caused by the predation for a specified area
   * \param area is the area that the predation is calculated on
   * \return 0 (will be overridden in derived classes)
   */
  virtual const DoubleVector& MortalityByLength(int area) const = 0;
  /**
   * \brief This will return the number consumed by age group and length group for a specified area
   * \param area is the area that the predation is calculated on
   * \return N, a BandMatrix containing the number consumed
   */
  const BandMatrix& NconsumptionByAgeAndLength(int area) const;
  /**
   * \brief This will return the number consumed by age group for a specified area
   * \param area is the area that the predation is calculated on
   * \return N, a DoubleIndexVector containing the number consumed
   */
  const DoubleIndexVector& NconsumptionByAge(int area) const;
  /**
   * \brief This will return the biomass consumed by age group and length group for a specified area
   * \param area is the area that the predation is calculated on
   * \return B, a BandMatrix containing the biomass consumed
   */
  const BandMatrix& BconsumptionByAgeAndLength(int area) const;
  /**
   * \brief This will return the biomass consumed by age group for a specified area
   * \param area is the area that the predation is calculated on
   * \return B, a DoubleIndexVector containing the biomass consumed
   */
  const DoubleIndexVector& BconsumptionByAge(int area) const;
  /**
   * \brief This will return the mortality by age group and length group caused by the predation for a specified area
   * \param area is the area that the predation is calculated on
   * \return M, a BandMatrix containing the mortality
   */
  const BandMatrix& MortalityByAgeAndLength(int area) const;
  /**
   * \brief This will return the mortality by age group caused by the predation for a specified area
   * \param area is the area that the predation is calculated on
   * \return M, a DoubleIndexVector containing the mortality
   */
  const DoubleIndexVector& MortalityByAge(int area) const;
  /**
   * \brief This is the function that calculates the predation
   * \param TimeInfo is the TimeClass for the current model
   * \param area is the area that the predation is being calculated on
   * \note This will be overridden by the derived classes that actually calculate the predation
   */
  virtual void Sum(const TimeClass* const TimeInfo, int area) = 0;
protected:
  /**
   * \brief This is the BandMatrix of the number consumed by age group
   */
  BandMatrix NconbyAge;
  /**
   * \brief This is the BandMatrix of the biomass consumed by age group
   */
  BandMatrix BconbyAge;
  /**
   * \brief This is the BandMatrix of the mortality caused by the predation by age group
   */
  BandMatrix MortbyAge;
  /**
   * \brief This is the BandMatrixPtrVector of the number consumed by age group and length group
   */
  BandMatrixPtrVector NconbyAgeAndLength;
  /**
   * \brief This is the BandMatrixPtrVector of the biomass consumed by age group and length group
   */
  BandMatrixPtrVector BconbyAgeAndLength;
  /**
   * \brief This is the BandMatrixPtrVector of the mortality caused by the predation by age group and length group
   */
  BandMatrixPtrVector MortbyAgeAndLength;
};

#endif
