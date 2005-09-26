#ifndef poppredator_h
#define poppredator_h

#include "areatime.h"
#include "conversionindex.h"
#include "commentstream.h"
#include "agebandmatrix.h"
#include "doublematrixptrmatrix.h"
#include "predator.h"
#include "keeper.h"

/**
 * \class PopPredator
 * \brief This is the class used to model the predation of a population
 */
class PopPredator : public Predator {
public:
  /**
   * \brief This is the PopPredator constructor to create a predator with a specified length group
   * \param givenname is the name of the predator
   * \param areas is the IntVector of the areas that the predator will live on
   * \param OtherLgrpDiv is the LengthGroupDivision of the predator
   * \param GivenLgrpDiv is the LengthGroupDivision that the predation will be calculated on
   * \note this constructor is used when creating a dynamic predator (ie. StockPredator class)
   */
  PopPredator(const char* givenname, const IntVector& areas,
    const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv);
  /**
   * \brief This is the PopPredator constructor to create a predator without a specified length group
   * \param givenname is the name of the predator
   * \param areas is the IntVector of the areas that the predator will live on
   * \note this constructor is used when creating a non-dynamic predator (ie. LengthPredator class)
   */
  PopPredator(const char* givenname, const IntVector& areas);
  /**
   * \brief This is the default PopPredator destructor
   */
  virtual ~PopPredator();
  /**
   * \brief This function will print the predation data
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This function will return the number of a prey on a specified area before the consumption calculation takes place
   * \param area is the area that the prey consumption is being calculated on
   * \param preyname is the name of the prey
   * \return PopInfoVector containing the number of prey before the consumption takes place
   */
  virtual const PopInfoVector& getNumberPriorToEating(int area, const char* preyname) const;
  /**
   * \brief This will return the amount the predator consumes of a given prey on a given area
   * \param area is the area that the consumption is being calculated on
   * \param preyname is the name of the prey that is being consumed
   * \return DoubleMatrix containing the amount the predator consumes
   */
  virtual const DoubleMatrix& getConsumption(int area, const char* preyname) const;
  /**
   * \brief This will return the amount the predator overconsumes on a given area
   * \param area is the area that the consumption is being calculated on
   * \return overconsumption, a DoubleVector containing the overconsumption by the predator
   */
  virtual const DoubleVector& getOverConsumption(int area) const { return overconsumption[this->areaNum(area)]; };
  /**
   * \brief This will return the total amount the predator overconsumes on a given area
   * \param area is the area that the consumption is being calculated on
   * \return total overconsumption by of the predator
   */
  virtual double getTotalOverConsumption(int area) const;
  /**
   * \brief This will return the length group information for the predator
   * \return LgrpDiv
   */
  virtual const LengthGroupDivision* getLengthGroupDiv() const { return LgrpDiv; };
  /**
   * \brief This function will reset the predation information for the current model run
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Reset(const TimeClass* const TimeInfo);
  /**
   * \brief This will return the biomass the predator consumes of a given prey on a given area
   * \param prey is the index for the prey
   * \param area is the area that the consumption is being calculated on
   * \return amount consumed by the predator of the prey
   */
  virtual const double getConsumptionBiomass(int prey, int area) const;
  /**
   * \brief This will select the preys that will be consumed by the predator
   * \param preyvec is the PreyPtrVector of all the available preys
   * \param keeper is the Keeper for the current model
   */
  void setPrey(PreyPtrVector& preyvec, Keeper* const keeper);
protected:
  /**
   * \brief This is the LengthGroupDivision used to store length information
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the ConversionIndexPtrVector used to convert to the lengths groups for the length-based predator
   */
  ConversionIndex* CI;
  /**
   * \brief This is the PopInfoMatrix used to store information on the number of predators for the current timestep
   * \note the indices for this object are [area][predator length]
   */
  PopInfoMatrix prednumber;
  /**
   * \brief This is the DoubleMatrix used to store information on the overconsumption by the predators on the current timestep
   * \note the indices for this object are [area][predator length]
   */
  DoubleMatrix overconsumption;
  /**
   * \brief This is the DoubleMatrixPtrMatrix used to store information on the consumption by the predators on the current timestep
   * \note the indices for this object are [area][prey][predator length][prey length]
   */
  DoubleMatrixPtrMatrix consumption;
  /**
   * \brief This is the DoubleMatrix used to store information on the total consumption by the predators on the current timestep
   * \note the indices for this object are [area][predator length]
   */
  DoubleMatrix totalconsumption;
  /**
   * \brief This is the DoubleMatrix used to store information on the overconsumption by the predators on the current substep of the current timestep
   * \note the indices for this object are [area][predator length]
   */
  DoubleMatrix overcons;
  /**
   * \brief This is the DoubleMatrixPtrMatrix used to store information on the consumption by the predators on the current substep of the current timestep
   * \note the indices for this object are [area][prey][predator length][prey length]
   */
  DoubleMatrixPtrMatrix cons;
  /**
   * \brief This is the DoubleMatrix used to store information on the total consumption by the predators on the current substep of the current timestep
   * \note the indices for this object are [area][predator length]
   */
  DoubleMatrix totalcons;
};

#endif
