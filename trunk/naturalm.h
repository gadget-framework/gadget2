#ifndef naturalm_h
#define naturalm_h

#include "areatime.h"
#include "commentstream.h"
#include "selectfunc.h"
#include "livesonareas.h"
#include "modelvariablevector.h"
#include "keeper.h"

/**
 * \class NaturalMortality
 * \brief This is the class used to calculate the affect of natural mortality on the stock
 *
 * This class calculates the proportion of the stock that should be removed from the model due to natural mortality (ie. any mortality that is not due to predation, either by another stock or by a fleet).  The number that is stored after this calculation contains the proportion of each area/age group that will survive (ie. is not removed due to the natural mortality).
 *
 * \note This is currently an age-based calculation, and it would be much better to replace this with a length-based calculation, by re-implementing this as a simple predator with a new suitability function to allow for higher levels of 'predation' on both the young/short fish and the old/long fish
 */
class NaturalMortality : public HasName, public LivesOnAreas {
public:
  /**
   * \brief This is the NaturalMortality constructor
   * \param infile is the CommentStream to read the NaturalMortality information from
   * \param minage is the minimum age of the stock
   * \param numage is the number of age groups for the stock
   * \param givenname is the name of the stock for this NaturalMortality class
   * \param Areas is the IntVector of the areas that the stock lives on
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  NaturalMortality(CommentStream& infile, int minage, int numage, const char* givenname,
    const IntVector& Areas, const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the default NaturalMortality destructor
   */
  ~NaturalMortality();
  /**
   * \brief This function will return the DoubleVector of the proportion of each age group that will survive on the current timestep
   * \param area is the area that the natural mortality is being calculated on
   * \return proportion surviving from each age group
   */
  const DoubleVector& getProportion(int area) const { return proportion[this->areaNum(area)]; };
  /**
   * \brief This function will reset the NaturalMortality information
   * \param TimeInfo is the TimeClass for the current model
   */
  void Reset(const TimeClass* const TimeInfo);
  /**
   * \brief This function will print the summary NaturalMortality information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void Print(ofstream& outfile);
protected:
  /**
   * \brief This is the ModelVariableVector used to store the mortality of each age group
   */
  ModelVariableVector mortality;
  /**
   * \brief This is the SelectFunc used to calculate of the proportion of each age group of the stock that will die due to natural mortality
   */
  SelectFunc* fnMortality;
  /**
   * \brief This is the DoubleMatrix used to store the proportion of each age group that will survive (ie. will not die out due to natural mortality)
   */
  DoubleMatrix proportion;
  /**
   * \brief This is the minimum age of the stock, used as an index when calculating proportion
   */
  int minStockAge;
  /**
   * \brief This is the identifier of the function to be used to read the natural mortality data from file
   */
  int readoption;
};

#endif
