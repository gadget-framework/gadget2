#ifndef abstrpreystdinfobylength_h
#define abstrpreystdinfobylength_h

#include "doublematrix.h"
#include "livesonareas.h"
#include "areatime.h"

class AbstrPreyStdInfoByLength;
class Prey;

/**
 * \class AbstrPreyStdInfoByLength
 * \brief This is the base class used to calculate information about the prey by length group from the predation by a predator of the prey
 * \note This will always be overridden by the derived classes that actually calculate the predation
 */
class AbstrPreyStdInfoByLength : protected LivesOnAreas {
public:
  /**
   * \brief This is the AbstrPreyStdInfoByLength constructor
   * \param p is the Prey that the predation is consuming
   * \param areas is the IntVector of areas that the predation can take place on
   */
  AbstrPreyStdInfoByLength(const Prey* p, const IntVector& areas);
  /**
   * \brief This is the default AbstrPreyStdInfo destructor
   */
  virtual ~AbstrPreyStdInfoByLength();
  /**
   * \brief This will return the number consumed by length group for a specified area
   * \param area is the area that the predation is calculated on
   * \return N, a DoubleVector containing the number consumed
   */
  const DoubleVector& NconsumptionByLength(int area) const;
  /**
   * \brief This will return the biomass consumed by length group for a specified area
   * \param area is the area that the predation is calculated on
   * \return B, a DoubleVector containing the biomass consumed
   */
  const DoubleVector& BconsumptionByLength(int area) const;
  /**
   * \brief This will return the mortality by length group caused by the predation for a specified area
   * \param area is the area that the predation is calculated on
   * \return M, a DoubleVector containing the mortality
   */
  const DoubleVector& MortalityByLength(int area) const;
  /**
   * \brief This is the function that calculates the predation
   * \param TimeInfo is the TimeClass for the current model
   * \param area is the area that the predation is being calculated on
   * \note This will be overridden by the derived classes that actually calculate the predation
   */
  virtual void Sum(const TimeClass* const TimeInfo, int area) = 0;
protected:
  /**
   * \brief This is the DoubleMatrix of the number consumed by length group
   */
  DoubleMatrix MortbyLength;
  /**
   * \brief This is the DoubleMatrix of the biomass consumed by length group
   */
  DoubleMatrix NconbyLength;
  /**
   * \brief This is the DoubleMatrix of the mortality caused by the predation by length group
   */
  DoubleMatrix BconbyLength;
};

#endif
