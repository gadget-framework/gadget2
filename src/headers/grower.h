#ifndef grower_h
#define grower_h

#include "areatime.h"
#include "conversionindex.h"
#include "commentstream.h"
#include "popinfomatrix.h"
#include "doublematrixptrvector.h"
#include "livesonareas.h"
#include "formulavector.h"
#include "keeper.h"
#include "growthcalc.h"

/**
 * \class Grower
 * \brief This is the class used to calculate, and implement, the affect of growth on the stock
 *
 * This class calculates the growth of the stock, according to the different growth functions derived from the GrowthCalcBase class.  Since the growth calculations can take place on a coarser scale than the stock is defined on, the calculated growth is then interpolated to take the length group of the stock into account.  This growth is then implemented by moving the fish up from the old length groups to the new length groups, taking care to ensure that the fish that would move to beyond the largest length group are kept in the plus group.
 */
class Grower : public HasName, protected LivesOnAreas {
public:
  /**
   * \brief This is the Grower constructor
   * \param infile is the CommentStream to read the growth data from
   * \param OtherLgrpDiv is the LengthGroupDivision of the stock
   * \param GivenLgrpDiv is the LengthGroupDivision that the growth will be calculated on
   * \param Areas is the IntVector of areas that the stock lives on
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param refWeight is the name of the file containing the reference weight information for the stock
   * \param givenname is the name of the stock for this Grower class
   * \param Area is the AreaClass for the current model
   * \param lenindex is the CharPtrVector of the lengths used for the growth of the stock
   */
  Grower(CommentStream& infile, const LengthGroupDivision* const OtherLgrpDiv,
    const LengthGroupDivision* const GivenLgrpDiv, const IntVector& Areas,
    const TimeClass* const TimeInfo, Keeper* const keeper, const char* refWeight,
    const char* givenname, const AreaClass* const Area, const CharPtrVector& lenindex);
  /**
   * \brief This is the default Grower destructor
   */
  ~Grower();
  /**
   * \brief This will calculate the growth of the stock for the current timestep on a given area
   * \param area is the area that the growth is being calculated on
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param FeedingLevel is the DoubleVector of the feeding level of the stock
   * \param Consumption is the DoubleVector of the maximum consumption of the stock
   */
  void calcGrowth(int area, const AreaClass* const Area, const TimeClass* const TimeInfo,
    const DoubleVector& FeedingLevel, const DoubleVector& Consumption);
  /**
   * \brief This will calculate the growth of the stock for the current timestep on a given area
   * \param area is the area that the growth is being calculated on
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  void calcGrowth(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This will implement the calculated changes due to growth of the stock
   * \param area is the area that the growth is being calculated on
   * \param NumberInArea is the PopInfoVector giving the current population of the stock
   * \param Lengths is the LengthGroupDivision of the stock
   */
  void implementGrowth(int area, const PopInfoVector& NumberInArea,
    const LengthGroupDivision* const Lengths);
  /**
   * \brief This will implement the calculated changes due to growth of the stock
   * \param area is the area that the growth is being calculated on
   * \param Lengths is the LengthGroupDivision of the stock
   */
  void implementGrowth(int area, const LengthGroupDivision* const Lengths);
  /**
   * \brief This will calculate the amount of the stock that is on a given area and timestep
   * \param NumberInArea is the PopInfoVector giving the amount of the stock in the area
   * \param area is the area that the growth is being calculated on
   */
  void Sum(const PopInfoVector& NumberInArea, int area);
  /**
   * \brief This will reset the growth information for the current model run
   */
  void Reset();
  /**
   * \brief This function will print the growth data
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void Print(ofstream& outfile) const;
  /**
   * \brief This function will return the calculated length increase due to growth on an area
   * \param area is the area that the growth is being calculated on
   * \return lgrowth, a DoubleMatrix containing the length increase
   */
  const DoubleMatrix& getLengthIncrease(int area) const { return *lgrowth[this->areaNum(area)]; };
  /**
   * \brief This function will return the calculated weight increase due to growth on an area
   * \param area is the area that the growth is being calculated on
   * \return wgrowth, a DoubleMatrix containing the weight increase
   */
  const DoubleMatrix& getWeightIncrease(int area) const { return *wgrowth[this->areaNum(area)]; };
  /**
   * \brief This function will return the fixed weight increase due to growth on an area
   * \param area is the area that the growth is being calculated on
   * \return interpWeightGrowth, a DoubleVector containing the weight increase
   */
  const DoubleVector& getWeight(int area) const { return interpWeightGrowth[this->areaNum(area)]; };
  /**
   * \brief This will return the flag used to denote whether the weights have been fixed or not
   * \return fixedweights
   */
  int getFixedWeights() { return fixedweights; };
protected:
  /**
   * \brief This is the PopInfoMatrix used to store information about the current population of the stock that is to grow according to the growth calculations
   */
  PopInfoMatrix numGrow;
  /**
   * \brief This is the LengthGroupDivision used to store length information
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the ConversionIndex used to convert from the stock LengthGroupDivision to the LengthGroupDivision used for the growth calculation
   */
  ConversionIndex* CI;
  /**
   * \brief This is the DoubleMatrix used to store the increase in length on the current timestep
   * \note The indices for this object are [area][stock length]
   */
  DoubleMatrix interpLengthGrowth;
  /**
   * \brief This is the DoubleMatrix used to store the increase in weight on the current timestep
   * \note The indices for this object are [area][stock length]
   */
  DoubleMatrix interpWeightGrowth;
  /**
   * \brief This is the DoubleMatrix used to store the calculated increase in length on the current timestep
   * \note The indices for this object are [area][length]
   */
  DoubleMatrix calcLengthGrowth;
  /**
   * \brief This is the DoubleMatrix used to store the calculated increase in weight on the current timestep
   * \note The indices for this object are [area][length]
   */
  DoubleMatrix calcWeightGrowth;
  /**
   * \brief This is the DoubleMatrixPtrVector used to store the calculated increase in length for each length group on the current timestep
   * \note The indices for this object are [area][change in length][stock length]
   */
  DoubleMatrixPtrVector lgrowth;
  /**
   * \brief This is the DoubleMatrixPtrVector used to store the calculated increase in weight for each length group on the current timestep
   * \note The indices for this object are [area][change in length][stock length]
   */
  DoubleMatrixPtrVector wgrowth;
  /**
   * \brief This is the dummy DoubleVector used during the growth calculation
   * \note The values of this vector are set to zero, and never used, unless the growth function depends on the feeding level of the stock (ie. growth is based on consumption of preys)
   */
  DoubleVector dummyfphi;
  /**
   * \brief This is the GrowthCalcBase used to calculate the growth information
   */
  GrowthCalcBase* growthcalc;
  /**
   * \brief This is the identifier of the function to be used to calculate the growth
   */
  int functionnumber;
  /**
   * \brief This is the flag used to denote whether the change is weight is to be calculated, or fixed to values specified in the input files
   */
  int fixedweights;
  /**
   * \brief This is the maximum number of length groups that an individual fish from the stock can grow on one timestep
   */
  int maxlengthgroupgrowth;
  /**
   * \brief This is the mean growth, as calculated for this length group by the GrowthCalc function
   */
  double growth;
  /**
   * \brief This is the value of alpha in the beta binomial distribution
   */
  double alpha;
  /**
   * \brief This is the value of beta in the beta binomial distribution
   */
  Formula beta;
  /**
   * \brief This is a DoubleVector used when calculating the beta binomial distribution
   * \note This stores the value of n*(n-1)*....(n-x+1)/x!
   */
  DoubleVector part1;
  /**
   * \brief This is a DoubleVector used when calculating the beta binomial distribution
   * \note This stores the value of gamma(n-x+beta)/gamma(beta)
   */
  DoubleVector part2;
  /**
   * \brief This is a double used when calculating the beta binomial distribution
   * \note This stores the value of gamma(alpha+beta)/gamma(n+alpha+beta)
   */
  double part3;
  /**
   * \brief This is a DoubleVector used when calculating the beta binomial distribution
   * \note This stores the value of gamma(x+alpha)/gamma(x)
   */
  DoubleVector part4;
};

#endif
