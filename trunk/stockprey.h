#ifndef stockprey_h
#define stockprey_h

#include "prey.h"
#include "agebandmatrixptrvector.h"

/**
 * \class StockPrey
 * \brief This is the class used to model a dynamic prey based on a modelled stock
 */
class StockPrey : public Prey {
public:
  /**
   * \brief This is the StockPrey constructor
   * \param infile is the CommentStream to read the prey parameters from
   * \param Areas is the IntVector of areas that the prey lives on
   * \param givenname is the name of the prey
   * \param minage is the minimum age of the prey
   * \param numage is the number of age groups for the prey
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  StockPrey(CommentStream& infile, const IntVector& Areas,
    const char* givenname, int minage, int numage,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the default StockPrey destructor
   */
  virtual ~StockPrey() {};
  /**
   * \brief This will calculate the amount of prey that is consumed for a given area and timestep
   * \param stockAlkeys is the AgeBandMatrix giving the amount of prey in the area
   * \param area is the area that the prey consumption is being calculated on
   */
  virtual void Sum(const AgeBandMatrix& stockAlkeys, int area);
  /**
   * \brief This function will print the prey information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This will return the minimum age of the prey
   * \return minimum age
   */
  int minAge() const { return preyAlkeys[0].minAge(); };
  /**
   * \brief This will return the maximum age of the prey
   * \return maximum age
   */
  int maxAge() const { return preyAlkeys[0].maxAge(); };
  /**
   * \brief This function will return the amount of prey on the area before consumption
   * \param area is the area that the prey consumption is being calculated on
   * \return preyAlkeys, the population of the prey for the area before consumption
   */
  AgeBandMatrix& getConsumptionALK(int area) { return preyAlkeys[this->areaNum(area)]; };
protected:
  /**
   * \brief This is the AgeBandMatrixPtrVector used to store the amount of the prey on each area
   */
  AgeBandMatrixPtrVector preyAlkeys;
};

#endif
