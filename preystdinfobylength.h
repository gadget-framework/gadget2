#ifndef preystdinfobylength_h
#define preystdinfobylength_h

#include "abstrpreystdinfobylength.h"

/**
 * \class PreyStdInfoByLength
 * \brief This is the class used to calculate information about the prey consumed by length group from the predation by a predator of the prey
 */
class PreyStdInfoByLength : public AbstrPreyStdInfoByLength {
public:
  /**
   * \brief This is the PreyStdInfoByLength constructor
   * \param prey is the Prey that the predation is consuming
   * \param areas is the IntVector of areas that the predation can take place on
   */
  PreyStdInfoByLength(const Prey* prey, const IntVector& areas);
  /**
   * \brief This is the default PreyStdInfoByLength destructor
   */
  virtual ~PreyStdInfoByLength();
  /**
   * \brief This is the function that calculates the predation
   * \param TimeInfo is the TimeClass for the current model
   * \param area is the area that the predation is being calculated on
   */
  virtual void Sum(const TimeClass* const TimeInfo, int area);
private:
  /**
   * \brief This is the Prey that the predation is consuming
   */
  const Prey* prey;
};

#endif
