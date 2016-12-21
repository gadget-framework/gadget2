#ifndef popratio_h
#define popratio_h

#include "gadget.h"

/**
 * \class PopRatio
 * \brief This is the class used to store information about the number of tagged fish in a population cell of a stock and ratio of the fish from that population cell that are tagged
 */
class PopRatio {
public:
  /**
   * \brief This is the PopRatio constructor
   */
  PopRatio() { N = NULL; R = 0.0; };
  /**
   * \brief This is the PopRatio destructor
   */
  ~PopRatio() {};
  /**
   * \brief This is the number of tagged fish in the population cell
   */
  double* N;
  /**
   * \brief This is the ratio of tagged fish in the population cell
   */
  double R;
  /**
   * \brief This operator will subtract a number from the PopRatio
   * \param a is the number to subtract
   */
  void operator -= (double a) { *N -= a; R = 0.0; };
  /**
   * \brief This operator will increase the PopRatio by a multiplicative constant
   * \param a is the multiplicative constant
   */
  void operator *= (double a) { *N *= a; R = 0.0; };
  /**
   * \brief This operator will set the PopRatio equal to an existing PopRatio
   * \param a is the PopRatio to copy
   */
  PopRatio& operator = (const PopRatio& a);
  /**
   * \brief This operator will add an existing PopRatio to the current PopRatio
   * \param a is the PopRatio to add
   */
  PopRatio& operator += (const PopRatio& a);
};

#endif
