#ifndef preystdinfo_h
#define preystdinfo_h

#include "areatime.h"
#include "abstrpreystdinfo.h"
#include "preystdinfobylength.h"

class PreyStdInfo;
class Prey;

/**
 * \class PreyStdInfo
 * \brief This is the class used to calculate information about the non-dynamic prey (eg otherfood) from the predation by a predator of the prey
 */
class PreyStdInfo : public AbstrPreyStdInfo {
public:
  /**
   * \brief This is the PreyStdInfo constructor
   * \param prey is the Prey that the predation is consuming
   * \param areas is the IntVector of areas that the predation can take place on
   */
  PreyStdInfo(const Prey* prey, const IntVector& areas);
  /**
   * \brief This is the default PreyStdInfo destructor
   */
  virtual ~PreyStdInfo();
  /**
   * \brief This will return the number consumed by length group for a specified area
   * \param area is the area that the predation is calculated on
   * \return N, a DoubleVector containing the number consumed
   */
  virtual const DoubleVector& NconsumptionByLength(int area) const;
  /**
   * \brief This will return the biomass consumed by length group for a specified area
   * \param area is the area that the predation is calculated on
   * \return B, a DoubleVector containing the number consumed
   */
  virtual const DoubleVector& BconsumptionByLength(int area) const;
  /**
   * \brief This will return the mortality by length group caused by the predation for a specified area
   * \param area is the area that the predation is calculated on
   * \return M, a DoubleVector containing the mortality
   */
  virtual const DoubleVector& MortalityByLength(int area) const;
  /**
   * \brief This is the function that calculates the predation
   * \param TimeInfo is the TimeClass for the current model
   * \param area is the area that the predation is being calculated on
   */
  virtual void Sum(const TimeClass* const TimeInfo, int area);
private:
  /**
   * \brief This is the PreyStdInfoByLength giving information about the predation of the prey by length group
   */
  PreyStdInfoByLength PSIByLength;
  /**
   * \brief This is the Prey that the predation is consuming
   */
  const Prey* prey;
};

#endif
