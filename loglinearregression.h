#ifndef loglinearregression_h
#define loglinearregression_h

#include "linearregression.h"

class LogLinearRegression;

/**
 * \class LogLinearRegression
 * \brief This is the class that fits a log linear regression line to compare 2 vectors
 */
class LogLinearRegression {
public:
  /**
   * \brief This is the default LogLinearRegression constructor
   */
  LogLinearRegression() { error = 0; };
  /**
   * \brief This is the default LogLinearRegression destructor
   */
  ~LogLinearRegression() {};
  /**
   * \brief This is the function that fits a log linear regression line to compare 2 vectors
   * \param x is the DoubleVector containing the modelled data
   * \param y is the DoubleVector containing the input data
   */
  void Fit(const DoubleVector& x, const DoubleVector& y);
  /**
   * \brief This is the function that fits a log linear regression line to compare 2 vectors, specifing the slope
   * \param x is the DoubleVector containing the modelled data
   * \param y is the DoubleVector containing the input data
   * \param slope is the slope of the regression line
   */
  void Fit(const DoubleVector& x, const DoubleVector& y, double slope);
  /**
   * \brief This is the function that fits a log linear regression line to compare 2 vectors, specifing the intercept
   * \param intercept is the intercept of the regression line
   * \param x is the DoubleVector containing the modelled data
   * \param y is the DoubleVector containing the input data
   */
  void Fit(double intercept, const DoubleVector& x, const DoubleVector& y);
  /**
   * \brief This is the function that fits a log linear regression line to compare 2 vectors, specifing the slope and intercept
   * \param x is the DoubleVector containing the modelled data
   * \param y is the DoubleVector containing the input data
   * \param slope is the slope of the regression line
   * \param intercept is the intercept of the regression line
   */
  void Fit(const DoubleVector& x, const DoubleVector& y, double slope, double intercept);
  /**
   * \brief This will check to see if an error has occured
   * \return error
   */
  int Error() { return error; };
  /**
   * \brief This will return the sum of squares of errors calculated when fitting the regression line
   * \return sse
   */
  double SSE();
  /**
   * \brief This will return the intercept of the linear regression line
   * \return intercept
   */
  double intersection();
  /**
   * \brief This will return the slope of the linear regression line
   * \return slope
   */
  double slope();
protected:
  /**
   * \brief This is the function that takes the log of the 2 vectors, removing any point at (0,0)
   * \param x is the DoubleVector containing the modelled data
   * \param y is the DoubleVector containing the input data
   * \param Xlog is the DoubleVector that will contain the log of x
   * \param Ylog is the DoubleVector that will contain the log of y
   */
  void CleanAndTakeLog(const DoubleVector& x, const DoubleVector& y,
    DoubleVector& Xlog, DoubleVector& Ylog);
  /**
   * \brief This is a LinearRegression that stores the linear regression line
   */
  LinearRegression LR;
  /**
   * \brief This is a flag to denote whether an error has occured
   */
  int error;
};

#endif
