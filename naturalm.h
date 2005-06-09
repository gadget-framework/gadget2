#ifndef naturalm_h
#define naturalm_h

#include "areatime.h"
#include "commentstream.h"
#include "timevariablevector.h"
#include "keeper.h"

/**
 * \class NaturalM
 * \brief This is the class used to calculate the affect of natural mortality on the stock
 *
 * This class calculates the proportion of the stock that should be removed from the model due to natural mortality (ie. any mortality that is not due to predation, either by another stock or by a fleet).  The vector that is stored after this calculation contains the proportion of each age group that will survive (ie. is not removed due to the natural mortality).
 *
 * \note this is currently an age-based calculation, and it would be much better to replace this with a length-based calculation, by re-implementing this as a simple predator with a new suitability function to allow for higher levels of 'predation' on both the young/short fish and the old/long fish
 */
class NaturalM {
public:
  /**
   * \brief This is the NaturalM constructor
   * \param infile is the CommentStream to read the NaturalM information from
   * \param minage is the minimum age of the stock
   * \param maxage is the maximum age of the stock
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  NaturalM(CommentStream& infile, int minage, int maxage,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the default NaturalM destructor
   */
  ~NaturalM() {};
  /**
   * \brief This function will return the DoubleVector of the proportion of each age group that will survive on the current timestep
   * \return proportion surviving from each age group
   */
  const DoubleVector& getProportion() const { return proportion; };
  /**
   * \brief This function will reset the NaturalM information
   * \param TimeInfo is the TimeClass for the current model
   */
  void Reset(const TimeClass* const TimeInfo);
  /**
   * \brief This function will print the summary NaturalM information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void Print(ofstream& outfile);
protected:
  /**
   * \brief This is the TimeVariableVector used to store the mortality of each age group
   */
  TimeVariableVector mortality;
  /**
   * \brief This is the DoubleVector used to store the proportion of each age group that will survive (ie. will not die out due to natural mortality)
   */
  DoubleVector proportion;
};

#endif
