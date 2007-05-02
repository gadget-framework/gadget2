#ifndef agebandmatrix_h
#define agebandmatrix_h

#include "conversionindex.h"
#include "doublematrix.h"
#include "popinfomatrix.h"
#include "popinfoindexvector.h"

class Maturity;

/**
 * \class AgeBandMatrix
 * \brief This class implements a vector of PopInfoIndexVector values, indexed from minage not 0
 */
class AgeBandMatrix {
public:
  /**
   * \brief This is the default AgeBandMatrix constructor
   */
  AgeBandMatrix() { minage = 0; nrow = 0; v = 0; };
  /**
   * \brief This is the AgeBandMatrix constructor for a specified minimum age and size
   * \param age is the minimum index of the vector to be created
   * \param minl is the IntVector of minimum lengths used when constructing the entries of the vector
   * \param size is the IntVector of sizes used when constructing the entries of the vector
   */
  AgeBandMatrix(int age, const IntVector& minl, const IntVector& size);
  /**
   * \brief This is the AgeBandMatrix constructor for a specified minimum age and initial value
   * \param age is the minimum index of the vector to be created
   * \param initial is the initial value for all the entries of the vector
   * \param minl is the minimum length used when constructing the entries of the vector
   */
  AgeBandMatrix(int age, const PopInfoMatrix& initial, int minl);
  /**
   * \brief This is the AgeBandMatrix constructor for a specified minimum age and initial value
   * \param age is the minimum index of the vector to be created
   * \param initial is the initial value for the entry of the vector
   * \note There will only be one entry on the vector created with this constructor
   */
  AgeBandMatrix(int age, const PopInfoIndexVector& initial);
  /**
   * \brief This is the AgeBandMatrix constructor that creates a copy of an existing AgeBandMatrix
   * \param initial is the AgeBandMatrix to copy
   */
  AgeBandMatrix(const AgeBandMatrix& initial);
  /**
   * \brief This is the AgeBandMatrix destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~AgeBandMatrix();
  /**
   * \brief This will return the minimum age of the population stored in the vector
   * \return minimum age
   */
  int minAge() const { return minage; };
  /**
   * \brief This will return the maximum age of the population stored in the vector
   * \return maximum age
   */
  int maxAge() const { return minage + nrow - 1; };
  /**
   * \brief This will return the number of rows of the vector
   * \return the number of rows of the vector
   */
  int Nrow() const { return nrow; };
  /**
   * \brief This will return the value of an element of the vector
   * \param age is the element of the vector to be returned
   * \return the value of the specified element
   */
  PopInfoIndexVector& operator [] (int age) { return *(v[age - minage]); };
  /**
   * \brief This will return the value of an element of the vector
   * \param age is the element of the vector to be returned
   * \return the value of the specified element
   */
  const PopInfoIndexVector& operator [] (int age) const { return *(v[age - minage]); };
  /**
   * \brief This will return the minimum length of an age group stored in the vector
   * \param age is identifier for the age group
   * \return minimum length
   */
  int minLength(int age) const { return v[age - minage]->minCol(); };
  /**
   * \brief This will return the maximum length of an age group stored in the vector
   * \param age is identifier for the age group
   * \return maximum length
   */
  int maxLength(int age) const { return v[age - minage]->maxCol(); };
  /**
   * \brief This function will sum the columns of each element stored in the vector (ie sum over all ages for each length group of the population)
   * \param Result is the PopInfoVector containing the sum over all ages for each length
   */
  void sumColumns(PopInfoVector& Result) const;
  /**
   * \brief This function will subtract a multiplicative ratio from each element stored in the vector
   * \param Ratio is the DoubleVector of multiplicative constants
   * \param CI is the ConversionIndex that will convert between the length groups of the 2 vectors
   */
  void Subtract(const DoubleVector& Ratio, const ConversionIndex& CI);
  /**
   * \brief This function will multiply each element stored in the vector by a constant
   * \param Ratio is the DoubleVector of multiplicative constants
   */
  void Multiply(const DoubleVector& Ratio);
  /**
   * \brief This function will set the population stored in the vector to zero
   */
  void setToZero();
  /**
   * \brief This function will increase the age of the population stored in the vector to zero
   */
  void IncrementAge();
  /**
   * \brief This function will print the numbers of the population stored in the vector
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void printNumbers(ofstream& outfile) const;
  /**
   * \brief This function will print the mean weights of the population stored in the vector
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void printWeights(ofstream& outfile) const;
  /**
   * \brief This function will increase the length and mean weight of the population stored in the vector, according to values calculated by the GrowthCalc calculations for the population
   * \param Lgrowth is the DoubleMatrix of the calculated change in length due to the growth
   * \param Wgrowth is the DoubleMatrix of the calculated change in mean weight due to the growth
   */
  void Grow(const DoubleMatrix& Lgrowth, const DoubleMatrix& Wgrowth);
  /**
   * \brief This function will increase the length and mean weight of the population stored in the vector, according to values calculated by the GrowthCalc and Maturity calculations for the population
   * \param Lgrowth is the DoubleMatrix of the calculated change in length due to the growth
   * \param Wgrowth is the DoubleMatrix of the calculated change in mean weight due to the growth
   * \param Mat is the Maturity used to calculate (and store) the proportion that population that will mature
   * \param area is the identifier for the are used for the maturation process
   */
  void Grow(const DoubleMatrix& Lgrowth, const DoubleMatrix& Wgrowth, Maturity* const Mat, int area);
  /**
   * \brief This function will increase the length of the population stored in the vector, according to values calculated by the GrowthCalc calculations for the population
   * \param Lgrowth is the DoubleMatrix of the calculated change in length due to the growth
   * \param Weight is the DoubleVector of the specified mean weight of the population
   * \note The mean weight of the population is fixed to values specified in the input file for the population
   */
  void Grow(const DoubleMatrix& Lgrowth, const DoubleVector& Weight);
  /**
   * \brief This function will increase the length and mean weight of the population stored in the vector, according to values calculated by the GrowthCalc and Maturity calculations for the population
   * \param Lgrowth is the DoubleMatrix of the calculated change in length due to the growth
   * \param Weight is the DoubleVector of the specified mean weight of the population
   * \param Mat is the Maturity used to calculate (and store) the proportion that population that will mature
   * \param area is the identifier for the are used for the maturation process
   * \note The mean weight of the population is fixed to values specified in the input file for the population
   */
  void Grow(const DoubleMatrix& Lgrowth, const DoubleVector& Weight, Maturity* const Mat, int area);
  /**
   * \brief This function will add a AgeBandMatrix to the current vector
   * \param Addition is the AgeBandMatrix that will be added to the current vector
   * \param CI is the ConversionIndex that will convert between the length groups of the 2 vectors
   * \param ratio is a multiplicative constant applied to each entry (default value 1.0)
   */
  void Add(const AgeBandMatrix& Addition, const ConversionIndex& CI, double ratio = 1.0);
protected:
  /**
   * \brief This is the index for the vector
   */
  int minage;
  /**
   * \brief This is number of rows of the vector
   */
  int nrow;
  /**
   * \brief This is the indexed vector of PopInfoIndexVector values
   */
  PopInfoIndexVector** v;
};

#endif
