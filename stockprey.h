#ifndef stockprey_h
#define stockprey_h

#include "prey.h"
#include "agebandmatrixptrvector.h"

class StockPrey;

/**
 * \class StockPrey
 * \brief This is the class used to model a dynamic prey based on a modelled stock
 */
class StockPrey : public Prey {
public:
  /**
   * \brief This is the StockPrey constructor
   * \param infile is the CommentStream to read the prey parameters from
   * \param areas is the list of areas that the prey lives on
   * \param givenname is the name of the prey
   * \param minage is the minimum age of the prey
   * \param maxage is the maximum age of the prey
   * \param keeper is the Keeper for the current model
   */
  StockPrey(CommentStream& infile, const IntVector& areas, const char* givenname,
    int minage, int maxage, Keeper* const keeper);
  /**
   * \brief This is the default StockPrey destructor
   */
  virtual ~StockPrey() {};
  /**
   * \brief This will calculate the amount of prey that is consumed for a given area and timestep
   * \param Alkeys is the AgeBandMatrix giving the amount of prey in the area
   * \param area is the area that the prey consumption is being calculated on
   * \param CurrentSubstep is the substep of the current timestep (so this calculation takes place once per timestep)
   */
  virtual void Sum(const AgeBandMatrix& Alkeys, int area, int CurrentSubstep);
  /**
   * \brief This function will return the amount of prey on the area before consumption
   * \param area is the area that the prey consumption is being calculated on
   * \return Alkeys, the population of the prey for the area before consumption
   */
  const AgeBandMatrix& AlkeysPriorToEating(int area) const { return Alkeys[AreaNr[area]]; };
  /**
   * \brief This function will print the prey information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This function will reset the prey information
   */
  virtual void Reset();
protected:
  /**
   * \brief This is the AgeBandMatrixPtrVector used to store the amount of the prey
   */
  AgeBandMatrixPtrVector Alkeys;
};

#endif
