#ifndef renewal_h
#define renewal_h

#include "formulavector.h"
#include "conversionindex.h"
#include "commentstream.h"
#include "stock.h"
#include "livesonareas.h"

/**
 * \class RenewalData
 * \brief This is the class used to calculate the recruits to a stock
 *
 * This class adds new fish into a stock, usually into the youngest age group.  The total number of the recruits (in units of 10,000 fish) for each specified timestep is given in the input file.  The lengths of the recruits are calculated from a Normal distribution, from a given mean and standard deviation.  The mean weights of these length groups are then calculated from a specified length-weight relationship.
 */
class RenewalData : protected LivesOnAreas {
public:
  /**
   * \brief This is the RenewalData constructor
   * \param infile is the CommentStream to read the recruits data from
   * \param areas is the IntVector of areas that the recruits will be calculated on
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param DL is the step length of the length groups of the stock that the recruits will be added to
   */
  RenewalData(CommentStream& infile, const IntVector& areas,
    const AreaClass* const Area, const TimeClass* const TimeInfo,
    Keeper* const keeper, double DL);
  /**
   * \brief This is the default RenewalData destructor
   */
  ~RenewalData();
  /**
   * \brief This is the function used to add the recruits to the model
   * \param Alkeys is the AgeBandMatrix for the stock to which the recruits will be added
   * \param area is the internal area identifier for the recruitment is taking place on
   * \param TimeInfo is the TimeClass for the current model
   */
  void addRenewal(AgeBandMatrix& Alkeys, int area, const TimeClass* const TimeInfo);
  /**
   * \brief This will set the ConversionIndex required for to calculate the recruits
   * \param GivenLDiv is the LengthGroupDivision for the stock
   */
  void setCI(const LengthGroupDivision* const GivenLDiv);
  /**
   * \brief This function will print the recruits
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void Print(ofstream& outfile) const;
  /**
   * \brief This function will reset the recruitment data
   */
  void Reset();
protected:
  /**
   * \brief This is flag to denote which option was used when reading in the recruits data from file
   */
  int readOption;
  /**
   * \brief This is used as the time index for the vectors of renewal data
   */
  int timeindex;
  /**
   * \brief This is the IntVector of timesteps that the recruitment will take place on
   */
  IntVector RenewalTime;
  /**
   * \brief This is the IntVector of areas that the recruitment will take place on
   */
  IntVector RenewalArea;
  /**
   * \brief This is the AgeBandMatrixPtrVector that contains the number and weight of the recruits
   */
  AgeBandMatrixPtrVector Distribution;
  /**
   * \brief This is the FormulaVector of the number of recruits used to calculate the recruitment population
   */
  FormulaVector Number;
  /**
   * \brief This is the FormulaVector of mean lengths of the recruits used to calculate the recruitment population
   */
  FormulaVector meanLength;
  /**
   * \brief This is the FormulaVector of the standard deviation of the lengths of the recruits used to calculate the recruitment population
   */
  FormulaVector sdevLength;
  /**
   * \brief This is the FormulaVector of the multipliers used to calculate the weight-length relationship for the recruits
   */
  FormulaVector coeff1;
  /**
   * \brief This is the FormulaVector of the powers used to calculate the weight-length relationship for the recruits
   */
  FormulaVector coeff2;
  /**
   * \brief This is the ConversionIndex used to convert from the recruits LengthGroupDivision to the stock LengthGroupDivision
   */
  ConversionIndex* CI;
  /**
   * \brief This is the LengthGroupDivision of the recruits of the stock
   */
  LengthGroupDivision* LgrpDiv;
};

#endif
