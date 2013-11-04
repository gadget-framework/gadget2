#ifndef renewal_h
#define renewal_h

#include "formulamatrixptrvector.h"
#include "conversionindex.h"
#include "commentstream.h"
#include "stock.h"
#include "livesonareas.h"

/**
 * \class RenewalData
 * \brief This is the class used to calculate the renewal to a stock
 *
 * This class adds new fish into a stock, usually into the youngest age group.  The total number of the recruits (in units of 10,000 fish) for each specified timestep is given in the input file.  The lengths of the recruits are calculated from a Normal distribution, from a given mean and standard deviation.  The mean weights of these length groups are then calculated from a specified length-weight relationship.
 */
class RenewalData : public HasName, protected LivesOnAreas {
public:
  /**
   * \brief This is the RenewalData constructor
   * \param infile is the CommentStream to read the renewal data from
   * \param areas is the IntVector of areas that the renewal data will be calculated on
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param refWeightFile is the name of the reference weight file
   * \param givenname is the name of the stock for this RenewalData class
   * \param minage is the minimum age of the stock the renewals will be added to
   * \param maxage is the maximum age of the stock the renewals will be added to
   * \param DL is the step length of the length groups of the stock that the renewals will be added to
   */
  RenewalData(CommentStream& infile, const IntVector& areas, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper, const char* refWeightFile,
    const char* givenname, int minage, int maxage, double DL);
  /**
   * \brief This is the default RenewalData destructor
   */
  ~RenewalData();
  /**
   * \brief This is the function used to add the renewal to the model
   * \param Alkeys is the AgeBandMatrix for the stock to which the renewal will be added
   * \param area is the internal area identifier for the renewal is taking place on
   * \param TimeInfo is the TimeClass for the current model
   */
  void addRenewal(AgeBandMatrix& Alkeys, int area, const TimeClass* const TimeInfo);
  /**
   * \brief This will set the ConversionIndex required for to calculate the renewal
   * \param GivenLDiv is the LengthGroupDivision for the stock
   */
  void setCI(const LengthGroupDivision* const GivenLDiv);
  /**
   * \brief This function will print the renewal data
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void Print(ofstream& outfile) const;
  /**
   * \brief This function will reset the renewal data
   */
  void Reset();
  /**
   * \brief This will check if the renewal process will take place on the current timestep and area
   * \param area is the internal area identifier for the renewal is taking place on
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if renewal data will be added to the model, 0 otherwise
   */
  int isRenewalStepArea(int area, const TimeClass* const TimeInfo);
protected:
  /**
   * \brief This is the function used to read the normal distribution and condition factor for the renewal data
   * \param infile is the CommentStream to read the initial conditions data from
   * \param keeper is the Keeper for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param Area is the AreaClass for the current model
   * \param minage is the minimum age of the renewals
   * \param maxage is the maximum age of the renewals
   */
  void readNormalConditionData(CommentStream& infile, Keeper* const keeper,
     const TimeClass* const TimeInfo, const AreaClass* const Area, int minage, int maxage);
  /**
   * \brief This is the function used to read the normal distribution for the renewal data
   * \param infile is the CommentStream to read the initial conditions data from
   * \param keeper is the Keeper for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param Area is the AreaClass for the current model
   * \param minage is the minimum age of the renewals
   * \param maxage is the maximum age of the renewals
   */
  void readNormalParameterData(CommentStream& infile, Keeper* const keeper,
     const TimeClass* const TimeInfo, const AreaClass* const Area, int minage, int maxage);
  /**
   * \brief This is the function used to read the numbers of the renewal data
   * \param infile is the CommentStream to read the initial conditions data from
   * \param keeper is the Keeper for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param Area is the AreaClass for the current model
   * \param minage is the minimum age of the renewals
   * \param maxage is the maximum age of the renewals
   */
  void readNumberData(CommentStream& infile, Keeper* const keeper,
     const TimeClass* const TimeInfo, const AreaClass* const Area, int minage, int maxage);
  /**
   * \brief This is the identifier of the function to be used to read the renewal data from file
   */
  int readoption;
  /**
   * \brief This is used as the index for the vectors of renewal data
   */
  int index;
  /**
   * \brief This is the IntVector of timesteps that the renewal will take place on
   */
  IntVector renewalTime;
  /**
   * \brief This is the IntVector of areas that the renewal will take place on
   */
  IntVector renewalArea;
  /**
   * \brief This is the IntVector of ages that the renewal will take place on
   */
  IntVector renewalAge;
  /**
   * \brief This is the AgeBandMatrixPtrVector that contains the number and weight of the renewal population
   */
  AgeBandMatrixPtrVector renewalDistribution;
  /**
   * \brief This is the FormulaMatrixPtrVector that contains the number (from the input file) of the renewal population
   */
  FormulaMatrixPtrVector renewalNumber;
  /**
   * \brief This is the FormulaVector of the multiplier used to calculate the renewal population
   */
  FormulaVector renewalMult;
  /**
   * \brief This is the FormulaVector of mean lengths of the stock used to calculate the renewal population
   */
  FormulaVector meanLength;
  /**
   * \brief This is the FormulaVector of the standard deviation of the lengths of the stock used to calculate the renewal population
   */
  FormulaVector sdevLength;
  /**
   * \brief This is the FormulaVector of the condition factor used to calculate the weight of the renewal population
   */
  FormulaVector relCond;
  /**
   * \brief This is the FormulaVector of the multipliers used to calculate the length-weight relationship for the renewal
   */
  FormulaVector alpha;
  /**
   * \brief This is the FormulaVector of the powers used to calculate the length-weight relationship for the renewal
   */
  FormulaVector beta;
  /**
   * \brief This is the DoubleVector of the reference weight values
   */
  DoubleVector refWeight;
  /**
   * \brief This is the ConversionIndex used to convert from the renewal LengthGroupDivision to the stock LengthGroupDivision
   */
  ConversionIndex* CI;
  /**
   * \brief This is the LengthGroupDivision of the renewal of the stock
   */
  LengthGroupDivision* LgrpDiv;
};

#endif
