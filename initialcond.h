#ifndef initialcond_h
#define initialcond_h

#include "formulamatrix.h"
#include "conversionindex.h"
#include "commentstream.h"
#include "agebandmatrixptrvector.h"
#include "livesonareas.h"
#include "keeper.h"

class Stock;

/**
 * \class InitialCond
 * \brief This is the class used to calculate the initial population of a stock
 *
 * This class calculates the initial population for a model run.  There are 2 ways to do this - either by specifying a normal distribution or by specifying the actual population to be used.
 *
 * For the normal distribution, Gadget will calculate a length distribution for a population of 10,000 fish for each age group, which will then be multiplied by age and area factors to give the initial population.
 *
 * For the numbers, Gadget will read a list of the population in each age-length cell for each area.
 *
 * The initial weight for each age-length cell will be set to the reference weight, multiplied by a conditioning factor.
 */
class InitialCond : protected LivesOnAreas {
public:
  /**
   * \brief This is the InitialCond constructor
   * \param infile is the CommentStream to read the initial conditions data from
   * \param areas is the IntVector of areas that the initial population will be calculated on
   * \param keeper is the Keeper for the current model
   * \param refWeightFile is the name of the reference weight file
   * \param Area is the AreaClass for the current model
   */
  InitialCond(CommentStream& infile, const IntVector& areas, Keeper* const keeper,
     const char* refWeightFile, const AreaClass* const Area);
  /**
   * \brief This is the default InitialCond destructor
   */
  ~InitialCond();
  /**
   * \brief This function will set the initial population of the stock to that specified in the initial conditions file
   * \param Alkeys is the AgeBandMatrixPtrVector that will contain the initial population of the stock
   */
  void Initialise(AgeBandMatrixPtrVector& Alkeys);
  /**
   * \brief This will set the ConversionIndex required for to calculate the initial conditions
   * \param GivenLDiv is the LengthGroupDivision for the stock
   */
  void setCI(const LengthGroupDivision* const GivenLDiv);
  /**
   * \brief This function will print the initial population
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void Print(ofstream& outfile) const;
protected:
  /**
   * \brief This is the function used to read the normal distribution of the initial condtion data
   * \param infile is the CommentStream to read the initial conditions data from
   * \param keeper is the Keeper for the current model
   * \param noagegr is the number of age groups for the initial population
   * \param minage is the minimum age for the initial population
   * \param Area is the AreaClass for the current model
   */
  void readNormalData(CommentStream& infile, Keeper* const keeper,
     int noagegr, int minage, const AreaClass* const Area);
  /**
   * \brief This is the function used to read the numbers of the initial population
   * \param infile is the CommentStream to read the initial conditions data from
   * \param keeper is the Keeper for the current model
   * \param noagegr is the number of age groups for the initial population
   * \param minage is the minimum age for the initial population
   * \param Area is the AreaClass for the current model
   */
  void readNumberData(CommentStream& infile, Keeper* const keeper,
     int noagegr, int minage, const AreaClass* const Area);
  /**
   * \brief This is the LengthGroupDivision of the initial population of the stock
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the ConversionIndex used to convert from the initial population LengthGroupDivision to the stock LengthGroupDivision
   */
  ConversionIndex* CI;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to store the initial population
   */
  AgeBandMatrixPtrVector AreaAgeLength;
  /**
   * \brief This is the area factor used to calculate the initial population
   */
  FormulaMatrix AreaDist;
  /**
   * \brief This is the age factor used to calculate the initial population
   */
  FormulaMatrix AgeDist;
  /**
   * \brief This is the condition factor used to calculate the weight of the initial population
   */
  FormulaMatrix relCond;
  /**
   * \brief This is the mean length at age used to calculate the initial population
   */
  FormulaMatrix meanLength;
  /**
   * \brief This is the standard deviation of the length at age used to calculate the initial population
   */
  FormulaMatrix sdevLength;
  /**
   * \brief This is the optional factor used when calculating the standard deviation of the length
   */
  Formula sdevMult;
  /**
   * \brief This is a flag used to denote whether the numbers for the initial population has been read in from file
   */
  int readNumbers;
};

#endif
