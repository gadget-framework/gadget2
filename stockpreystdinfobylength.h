#ifndef stockpreystdinfobylength_h
#define stockpreystdinfobylength_h

#include "abstrpreystdinfobylength.h"
#include "lengthgroup.h"

class StockPreyStdInfoByLength;
class StockPrey;

/**
 * \class StockPreyStdInfoByLength
 * \brief This is the class used to calculate information about the dynamic prey (eg stock) by length group from the predation by a predator of the prey
 */
class StockPreyStdInfoByLength : public AbstrPreyStdInfoByLength {
public:
  /**
   * \brief This is the StockPreyStdInfoByLength constructor
   * \param prey is the StockPrey that the predation is consuming
   * \param areas is the IntVector of areas that the predation can take place on
   */
  StockPreyStdInfoByLength(const StockPrey* prey, const IntVector& areas);
  /**
   * \brief This is the default StockPreyStdInfoByLength destructor
   */
  virtual ~StockPreyStdInfoByLength();
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
  const LengthGroupDivision* returnPreyLengthGroupDiv() const { return preyLgrpDiv; };
private:
  /**
   * \brief This is the StockPrey that the predation is consuming
   */
  const StockPrey* prey;
  /**
   * \brief This is the LengthGroupDivision of the prey
   */
  const LengthGroupDivision* preyLgrpDiv;
};

#endif
