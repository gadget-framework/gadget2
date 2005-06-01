#ifndef linearregression_h
#define linearregression_h

#include "doublevector.h"

/**
 * \class LinearRegression
 * \brief This is the class that fits a linear regression line to compare 2 vectors
 */
class LinearRegression {
public:
  /**
   * \brief This is the default LinearRegression constructor
   */
  LinearRegression();
  /**
   * \brief This is the default LinearRegression destructor
   */
  ~LinearRegression() {};
  /**
   * \brief This is the function that fits a linear regression line to compare 2 vectors
   * \param x is the DoubleVector containing the modelled data
   * \param y is the DoubleVector containing the input data
   */
  virtual void calcFit(const DoubleVector& x, const DoubleVector& y);
  /**
   * \brief This is the function that fits a linear regression line to compare 2 vectors, specifing the slope
   * \param x is the DoubleVector containing the modelled data
   * \param y is the DoubleVector containing the input data
   * \param slope is the slope of the regression line
   */
  virtual void calcFit(const DoubleVector& x, const DoubleVector& y, double slope);
  /**
   * \brief This is the function that fits a linear regression line to compare 2 vectors, specifing the intercept
   * \param intercept is the intercept of the regression line
   * \param x is the DoubleVector containing the modelled data
   * \param y is the DoubleVector containing the input data
   */
  virtual void calcFit(double intercept, const DoubleVector& x, const DoubleVector& y);
  /**
   * \brief This is the function that fits a linear regression line to compare 2 vectors, specifing the slope and intercept
   * \param x is the DoubleVector containing the modelled data
   * \param y is the DoubleVector containing the input data
   * \param slope is the slope of the regression line
   * \param intercept is the intercept of the regression line
   */
  virtual void calcFit(const DoubleVector& x, const DoubleVector& y, double slope, double intercept);
  /**
   * \brief This will check to see if an error has occured
   * \return error
   */
  int getError() { return error; };
  /**
   * \brief This will return the sum of squares of errors calculated when fitting the regression line
   * \return sse
   */
  double getSSE();
  /**
   * \brief This will return the intercept of the linear regression line
   * \return a
   */
  double getIntersection() { return a; };
  /**
   * \brief This will return the slope of the linear regression line
   * \return b
   */
  double getSlope() { return b; };
protected:
  /**
   * \brief This is a flag to denote whether an error has occured
   */
  int error;
  /**
   * \brief This is the sum of squares of errors
   */
  double sse;
  /**
   * \brief This is the intercept of the linear regression line
   */
  double a;
  /**
   * \brief This is the slope of the linear regression line
   */
  double b;
};

#endif
