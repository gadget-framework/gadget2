#ifndef popinfo_h
#define popinfo_h

#include "gadget.h"

/**
 * \class PopInfo
 * \brief This is the class used to store information about the number, and mean weight, of a population cell of a stock
 */
class PopInfo {
public:
  /**
   * \brief This is the PopInfo constructor
   */
  PopInfo() { N = 0.0; W = 0.0; };
  /**
   * \brief This is the PopInfo destructor
   */
  ~PopInfo() {};
  /**
   * \brief This is the number of fish in the population cell
   */
  double N;
  /**
   * \brief This is the mean weight of the fish in the population cell
   */
  double W;
  /**
   * \brief This operator will subtract a number from the PopInfo
   * \param a is the number to subtract
   */
  void operator -= (double a) { N -= a; };
  /**
   * \brief This operator will increase the PopInfo by a multiplicative constant
   * \param a is the multiplicative constant
   */
  void operator *= (double a) { N *= a; };
  /**
   * \brief This function will set the PopInfo to zero
   */
  void setToZero() { N = 0.0; W = 0.0; };
  /**
   * \brief This operator will set the PopInfo equal to an existing PopInfo
   * \param a is the PopInfo to copy
   */
  PopInfo& operator = (const PopInfo& a);
  /**
   * \brief This operator will add an existing PopInfo to the current PopInfo
   * \param a is the PopInfo to add
   */
  PopInfo& operator += (const PopInfo& a);
  /**
   * \brief This operator will multiply the PopInfo by a constant
   * \param a is the constant
   */
  PopInfo operator * (double a);
};

#endif
