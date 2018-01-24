#ifndef prey_h
#define prey_h

#include "popinfovector.h"
#include "conversionindex.h"
#include "commentstream.h"
#include "charptrvector.h"
#include "hasname.h"
#include "livesonareas.h"
#include "doublematrix.h"
#include "areatime.h"
#include "agebandmatrix.h"
#include "keeper.h"
#include "gadget.h"
#include "modelvariable.h"

enum PreyType { STOCKPREY = 1, LENGTHPREY };

/**
 * \class Prey
 * \brief This is the base class used to model the consumption of a prey
 *
 * This class is used to remove fish from the model due to predation, either by a fleet or through consumption by another stock.  The proportion of the fish that are to be removed is calculated, based on the required consumption by all the various predators that will consume the prey.  This ''target consumption'' is then checked to ensure that no more than 95% of the prey species is to be consumed, and the consumption is adjusted if this is the case.  The population of the prey species is then reduced by the total amount that the various predators consume.
 *
 * \note This will be overridden by the derived classes that actually calculate the consumption
 */
class Prey : public HasName, public LivesOnAreas {
public:
  /**
   * \brief This is the Prey constructor to create a prey by reading data from a file
   * \param infile is the CommentStream to read the Prey data from
   * \param areas is the IntVector of areas that the prey lives on
   * \param givenname is the name of the prey
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \note This constructor is used when creating dynamic prey (ie. StockPrey class)
   */
  Prey(CommentStream& infile, const IntVector& areas, const char* givenname,
  const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the Prey constructor for a prey with a given length group
   * \param infile is the CommentStream to read the Prey data from
   * \param givenname is the name of the prey
   * \param areas is the IntVector of areas that the prey lives on
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \note This constructor is used when creating non-dynamic prey (ie. OtherFood class)
   */
  Prey(CommentStream& infile, const char* givenname, const IntVector& areas,
  const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the default Prey destructor
   */
  virtual ~Prey() = 0;
  /**
   * \brief This will calculate the amount of prey that is consumed for a given area and timestep
   * \param Alkeys is the AgeBandMatrix giving the amount of prey in the area
   * \param area is the area that the prey consumption is being calculated on
   * \note This will be overridden by the derived classes that actually calculate the consumption
   */
  virtual void Sum(const AgeBandMatrix& Alkeys, int area) {};
  /**
   * \brief This will calculate the amount of prey that is consumed for a given area and timestep
   * \param NumberInArea is the PopInfoVector giving the amount of prey in the area
   * \param area is the area that the prey consumption is being calculated on
   * \note This will be overridden by the derived classes that actually calculate the consumption
   */
  virtual void Sum(const PopInfoVector& NumberInArea, int area) {};
  /**
   * \brief This will reset the consumption information for the current model run
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Reset(const TimeClass* const TimeInfo);
  /**
   * \brief This function will print the consumption data
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This will set the ConversionIndex required for to calculate the consumption
   * \param GivenLDiv is the LengthGroupDivision for the stock
   */
  virtual void setCI(const LengthGroupDivision* const GivenLDiv);
  /**
   * \brief This function will subtract the prey that is consumed from the stock
   * \param Alkeys is the AgeBandMatrix containing the population of the stock
   * \param area is the area that the consumption is being calculated on
   */
  virtual void Subtract(AgeBandMatrix& Alkeys, int area);
  /**
   * \brief This function will add predation (by biomass) to the amount of the prey that is consumed
   * \param area is the area that the consumption is being calculated on
   * \param predcons is the DoubleVector containing the predation data
   */
  void addBiomassConsumption(int area, const DoubleVector& predcons);
  /**
   * \brief This function will add predation (by numbers) to the amount of the prey that is consumed
   * \param area is the area that the consumption is being calculated on
   * \param predcons is the DoubleVector containing the predation data
   */
  void addNumbersConsumption(int area, const DoubleVector& predcons);
  /**
   * \brief This function will check for overconsumption of the prey
   * \param area is the area that the consumption is being calculated on
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void checkConsumption(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This will return the biomass of prey that is available for consumption by predators
   * \param area is the area that the consumption is being calculated on
   * \param length is the length group of the prey
   * \return biomass
   */
  double getBiomass(int area, int length) const { return biomass[this->areaNum(area)][length]; };
  /**
   * \brief This will return the number of prey that is available for consumption by predators
   * \param area is the area that the consumption is being calculated on
   * \param length is the length group of the prey
   * \return number
   */
  double getNumber(int area, int length) const { return preynumber[this->areaNum(area)][length].N; };
  /**
   * \brief This will return the total biomass of prey that is available for consumption
   * \param area is the area that the consumption is being calculated on
   * \return total biomass
   */
  double getTotalBiomass(int area) const { return total[this->areaNum(area)]; };
  /**
   * \brief This will return the flag that denotes if the prey has been overconsumed on a given area
   * \param area is the area that the consumption is being calculated on
   * \return 1 if the prey has been overconsumed, 0 otherwise
   */
  int isOverConsumption(int area);
  /**
   * \brief This will return the ratio of the prey that has been consumed on a given area
   * \param area is the area that the consumption is being calculated on
   * \return ratio, a DoubleVector containing the ratio of the prey that is consumed
   */
  const DoubleVector& getRatio(int area) const { return ratio[this->areaNum(area)]; };
  /**
   * \brief This will return the ratio of the prey that has been consumed on a given area
   * \param area is the area that the consumption is being calculated on
   * \return ratio, a DoubleVector containing the ratio of the prey that is consumed
   */
  const DoubleVector& getUseRatio(int area) const { return useratio[this->areaNum(area)]; };
  /**
   * \brief This will return the amount of the prey that has been consumed on a given area
   * \param area is the area that the consumption is being calculated on
   * \return consumption, a DoubleVector containing the consumption of the prey
   */
  const DoubleVector& getConsumption(int area) const { return consumption[this->areaNum(area)]; };
  /**
   * \brief This will return the amount of the prey that has been overconsumed on a given area
   * \param area is the area that the consumption is being calculated on
   * \return overconsumption, a DoubleVector containing the overconsumption of the prey
   */
  const DoubleVector& getOverConsumption(int area) const { return overconsumption[this->areaNum(area)]; };
  /**
   * \brief This will return the total amount of the prey that has been overconsumed on a given area
   * \param area is the area that the consumption is being calculated on
   * \return total overconsumption of the prey
   */
  double getTotalOverConsumption(int area) const;
  /**
   * \brief This will return the length group information for the prey
   * \return LgrpDiv
   */
  const LengthGroupDivision* getLengthGroupDiv() const { return LgrpDiv; };
  /**
   * \brief This will return the amount of prey on a given area prior to any consumption by the predators
   * \param area is the area that the consumption is being calculated on
   * \return preynumber, a PopInfoVector containing information about the prey population
   */
  const PopInfoVector& getConsumptionPopInfo(int area) const { return preynumber[this->areaNum(area)]; };
  /**
   * \brief This will return the energy content of the prey
   * \return energy
   */
  double getEnergy() const { return energy; };
  /**
   * \brief This will check if there is any prey to consume on a given area
   * \param area is the area that the prey will be consumed on
   * \return 1 if there is prey to consume, 0 otherwise
   */
  int isPreyArea(int area);
  /**
   * \brief This will return the type of prey class
   * \return type
   */
  PreyType getType() const { return type; };
protected:
  /**
   * \brief This is the ConversionIndex used to convert from the stock LengthGroupDivision to the LengthGroupDivision used for the consumption calculation
   */
  ConversionIndex* CI;
  /**
   * \brief This is the LengthGroupDivision used to store length information
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the PopInfoMatrix used to store information on the number of preys for the current timestep
   * \note The indices for this object are [area][prey length]
   */
  PopInfoMatrix preynumber;
  /**
   * \brief This is the ModelVariable used to store the energy content of the prey (in kilojoules per kilogramme)
   */
  ModelVariable energy;
  /**
   * \brief This is the DoubleMatrix used to store information on the biomass of the prey that is available for the predators to consume on the current timestep
   * \note The indices for this object are [area][prey length]
   */
  DoubleMatrix biomass;
  /**
   * \brief This is the DoubleVector used to store information on the total biomass of the prey that is available for the predators to consume on the current timestep
   */
  DoubleVector total;
  /**
   * \brief This is the DoubleMatrix used to store information on the ratio of the available biomass of the prey that is available for the predators to consume has been consumed on the current timestep
   * \note The indices for this object are [area][prey length]
   */
  DoubleMatrix ratio;
  /**
   * \brief This is the DoubleMatrix used to store information on the ratio of the available biomass of the prey that is available for the predators to consume has been consumed on the current timestep, adjusted to MaxRatio whenever overconsumption has occured
   * \note The indices for this object are [area][prey length]
   */
  DoubleMatrix useratio;
  /**
   * \brief This is the DoubleMatrix used to store information on the consumption of the prey on the current timestep
   * \note The indices for this object are [area][prey length]
   */
  DoubleMatrix consumption;
  /**
   * \brief This is the DoubleMatrix used to store information on the consumption of the prey on the current substep of the current timestep
   * \note The indices for this object are [area][prey length]
   */
  DoubleMatrix cons;
  /**
   * \brief This is the DoubleMatrix used to store information on the overconsumption of the prey on the current timestep
   * \note The indices for this object are [area][prey length]
   */
  DoubleMatrix overconsumption;
  /**
   * \brief This is the IntVector used to store information on whether any overconsumption has occured on the current timestep
   */
  IntVector isoverconsumption;
  /**
   * \brief This is the DoubleMatrix used to store information on the ratio of the prey to subtract on the current timestep
   * \note The indices for this object are [area][prey length]
   */
  DoubleMatrix consratio;
  /**
   * \brief This denotes what type of prey class has been created
   */
  PreyType type;
};

#endif
