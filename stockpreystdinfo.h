#ifndef stockpreystdinfo_h
#define stockpreystdinfo_h

#include "areatime.h"
#include "lengthgroup.h"
#include "abstrpreystdinfo.h"
#include "stockpreystdinfobylength.h"

class StockPreyStdInfo;
class StockPrey;

/**
 * \class StockPreyStdInfo
 * \brief This is the class used to calculate information about the dynamic prey (eg stock) from the predation by a predator of the prey
 */
class StockPreyStdInfo : public AbstrPreyStdInfo {
public:
  /**
   * \brief This is the StockPreyStdInfo constructor
   * \param prey is the StockPrey that the predation is consuming
   * \param areas is the IntVector of areas that the predation can take place on
   */
  StockPreyStdInfo(const StockPrey* prey, const IntVector& areas);
  /**
   * \brief This is the default StockPreyStdInfo destructor
   */
  virtual ~StockPreyStdInfo();
  /**
   * \brief This will return the number consumed by length group for a specified area
   * \param area is the area that the predation is calculated on
   * \return N, a DoubleVector containing the number consumed
   */
  virtual const DoubleVector& NconsumptionByLength(int area) const;
  /**
   * \brief This will return the biomass consumed by length group for a specified area
   * \param area is the area that the predation is calculated on
   * \return B, a DoubleVector containing the biomass consumed
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
  /**
   * \brief This will return the length group of the prey
   * \return preyLgrpDiv, a LengthGroupDivision for the prey
   */
  const LengthGroupDivision* returnPreyLengthGroupDiv() const;
private:
  /**
   * \brief This is the StockPreyStdInfoByLength giving information about the predation of the prey by length group
   */
  StockPreyStdInfoByLength SPByLength;
  /**
   * \brief This is the StockPrey that the predation is consuming
   */
  const StockPrey* prey;
};

#endif
