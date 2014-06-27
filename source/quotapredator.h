#ifndef quotapredator_h
#define quotapredator_h

#include "lengthpredator.h"

/**
 * \class QuotaPredator
 * \brief This is the class used to model the predation of stocks by a fleet that will catch a proportion of the available biomass, according to a specified management catch-quota rule
 */
class QuotaPredator : public LengthPredator {
public:
  /**
   * \brief This is the QuotaPredator constructor
   * \param infile is the CommentStream to read the predation parameters from
   * \param givenname is the name of the predator
   * \param Areas is the IntVector of areas that the predator lives on
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param multscaler is the Formula that can be used to scale the biomass consumed
   */
  QuotaPredator(CommentStream& infile, const char* givenname, const IntVector& Areas,
    const TimeClass* const TimeInfo, Keeper* const keeper, Formula multscaler);
  /**
   * \brief This is the default QuotaPredator destructor
   */
  virtual ~QuotaPredator();
  /**
   * \brief This will calculate the amount the predator consumes on a given area
   * \param area is the area that the prey consumption is being calculated on
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Eat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This will adjust the amount the predator consumes on a given area, to take oversconsumption into consideration
   * \param area is the area that the prey consumption is being calculated on
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void adjustConsumption(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will print the predator information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
protected:
  /**
   * \brief This function will calculate the quota based on the current biomass level of the prey
   * \param biomass is the available biomass of the prey
   * \return calculated quota
   */
  double calcQuota(double biomass);
  /**
   * \brief This is the identifier of the function to be used to calculate the fishing quota
   */
  int functionnumber;
  /**
   * \brief This is the name of the function to be used to calculate the fishing quota
   */
  char* functionname;
  /**
   * \brief This is the DoubleVector used to store the stock biomass levels
   */
  DoubleVector biomasslevel;
  /**
   * \brief This is the FormulaVector used to store the fishing quota levels
   */
  FormulaVector quotalevel;
  /**
   * \brief This is the DoubleVector used to store the calculated fishing quota
   */
  DoubleVector calcquota;
  /**
   * \brief This is the IntVector used to flag which preys are used to calculate the fishing quota levels
   */
  IntVector selectprey;
};

#endif
