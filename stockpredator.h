#ifndef stockpredator_h
#define stockpredator_h

#include "areatime.h"
#include "lengthgroup.h"
#include "commentstream.h"
#include "agebandmatrixptrvector.h"
#include "bandmatrixptrvector.h"
#include "poppredator.h"

/**
 * \class StockPredator
 * \brief This is the class used to model the predation by stocks
 */
class StockPredator : public PopPredator {
public:
  /**
   * \brief This is the StockPredator constructor
   * \param infile is the CommentStream to read the predator information from
   * \param givenname is the name of the predator
   * \param areas is the IntVector of the areas that the predator will live on
   * \param OtherLgrpDiv is the LengthGroupDivision of the predator
   * \param GivenLgrpDiv is the LengthGroupDivision that the predation will be calculated on
   * \param minage is the minimum age of the predator
   * \param maxage is the maximum age of the predator
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  StockPredator(CommentStream& infile, const char* givenname, const IntVector& areas,
    const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv,
    int minage, int maxage, const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the default StockPredator destructor
   */
  virtual ~StockPredator() {};
  /**
   * \brief This will calculate the amount the predator consumes on a given area
   * \param area is the area that the prey consumption is being calculated on
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Eat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This will calculate the amount of prey that is consumed by the predator
   * \param stock is the AgeBandMatrix giving the amount of prey in the area
   * \param area is the area that the prey consumption is being calculated on
   */
  virtual void Sum(const AgeBandMatrix& stock, int area);
  /**
   * \brief This will adjust the amount the predator consumes on a given area, to take oversconsumption into consideration
   * \param area is the area that the prey consumption is being calculated on
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void adjustConsumption(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will reset the predation information for the current model run
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Reset(const TimeClass* const TimeInfo);
  /**
   * \brief This function will print the predator information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This function will return the proportion of the predation that is due to each age-length cell of the predator
   * \param area is the area that the prey consumption is being calculated on
   * \return Alprop, a BandMatrix containing the age-length proportion of the predation
   */
  const BandMatrix& getALProportion(int area) const { return Alprop[this->areaNum(area)]; };
  /**
   * \brief This function will return the feeding level of the predator on a specified area
   * \param area is the area that the prey consumption is being calculated on
   * \return fphi, a DoubleVector containing the feeding level of the predator
   */
  const DoubleVector& getFPhi(int area) const { return fphi[this->areaNum(area)]; };
  /**
   * \brief This function will return the maximum consumption by length group of the predator on a specified area
   * \param area is the area that the prey consumption is being calculated on
   * \return maxconbylength, a DoubleVector containing the maximum consumption of the predator
   */
  const DoubleVector& getMaxConsumption(int area) const { return maxcons[this->areaNum(area)]; };
  /**
   * \brief This will return the minimum age of the predator
   * \return minimum age
   */
  int minAge() const { return Alkeys[0].minAge(); };
  /**
   * \brief This will return the maximum age of the predator
   * \return maximum age
   */
  int maxAge() const { return Alkeys[0].maxAge(); };
  const AgeBandMatrix& getAgeLengthKeys(int area) const { return Alkeys[this->areaNum(area)]; };
protected:
  /**
   * \brief This is the FormulaVector used to store the consumption parameters
   */
  FormulaVector consParam;
  /**
   * \brief This is the DoubleMatrix used to store the sum of the feeding levels (over all preys) on the current timestep
   * \note The indices for this object are [area][predator length]
   */
  DoubleMatrix Phi;
  /**
   * \brief This is the DoubleMatrix used to store the feeding level on the current timestep
   * \note The indices for this object are [area][predator length]
   */
  DoubleMatrix fphi;
  /**
   * \brief This is the DoubleMatrix used to store the feeding level on the current substep of the current timestep
   * \note The indices for this object are [area][predator length]
   */
  DoubleMatrix subfphi;
  /**
   * \brief This is the DoubleMatrix used to store maximum consumption by predator length group on the current timestep
   * \note The indices for this object are [area][predator length]
   */
  DoubleMatrix maxcons;
  /**
   * \brief This is the BandMatrixPtrVector used to store proportion of the predation that is due to each age-length cell of the predator
   * \note The indices for this object are [area][predator age][predator length]
   */
  BandMatrixPtrVector Alprop;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to store information about the predator population
   * \note The indices for this object are [area][predator age][predator length]
   */
  AgeBandMatrixPtrVector Alkeys;
};

#endif
