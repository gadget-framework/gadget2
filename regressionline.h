#ifndef regressionline_h
#define regressionline_h

#include "doublevector.h"

enum LineType { FREE = 1, FIXEDSLOPE, FIXEDINTERCEPT, FIXED };

/**
 * \class Regression
 * \brief This is the base class used to fit a regression line to compare 2 vectors
 * \note This will always be overridden by the derived classes that actually fit the regression line
 */
class Regression {
public:
  /**
   * \brief This is the default Regression constructor
   */
  Regression();
  /**
   * \brief This is the Regression constructor for a regression line of a specified LineType
   * \param ltype is the LineType of the regression line
   */
  Regression(LineType ltype);
  /**
   * \brief This is the default Regression destructor
   */
  ~Regression() {};
  /**
   * \brief This is the function that stores 2 vectors that will be compared using a regression line
   * \param modData is the DoubleVector containing the modelled data
   * \param obsData is the DoubleVector containing the observed data
   */
  virtual void storeVectors(const DoubleVector& modData, const DoubleVector& obsData) = 0;
  /**
   * \brief This is the function that fits a regression line to compare the 2 vectors that have been stored, according to the LineType that has been defined
   */
  void calcFit();
  /**
   * \brief This function will set the intercept of the regression line
   * \param intercept is the intercept of the regression line
   */
  void setIntercept(double intercept) { a = intercept; };
  /**
   * \brief This function will set the slope of the regression line
   * \param slope if the slope of the regession line
   */
  void setSlope(double slope) { b = slope; };
  /**
   * \brief This function will check to see if an error has occured
   * \return error
   */
  int getError() { return error; };
  /**
   * \brief This function will return the sum of squares of errors calculated when fitting the regression line
   * \return sse
   */
  double getSSE();
  /**
   * \brief This function will return the intercept of the regression line
   * \return a
   */
  double getIntercept() { return a; };
  /**
   * \brief This function will return the slope of the regression line
   * \return b
   */
  double getSlope() { return b; };
  /**
   * \brief This function will return the fit type for the regression line
   * \return fittype
   */
  const LineType getType() const { return linetype; };
protected:
  /**
   * \brief This is a flag to denote whether an error has occured
   */
  int error;
  /**
   * \brief This is the sum of squares of errors from the regression line
   */
  double sse;
  /**
   * \brief This is the intercept of the regression line
   */
  double a;
  /**
   * \brief This is the slope of the regression line
   */
  double b;
  /**
   * \brief This is the DoubleVector that will contain the the modelled data to be used to fit the regression line
   */
  DoubleVector x;
  /**
   * \brief This is the DoubleVector that will contain the the observed data to be used to fit the regression line
   */
  DoubleVector y;
  /**
   * \brief This denotes what type of line fit is to be used for the regression line
   */
  LineType linetype;
};

/**
 * \class LinearRegression
 * \brief This is the class used to fit a linear regression line to compare 2 vectors
 */
class LinearRegression : public Regression {
public:
  /**
   * \brief This is the default LinearRegression constructor
   * \param ltype is the LineType of the regression line
   */
  LinearRegression(LineType ltype);
  /**
   * \brief This is the default LinearRegression destructor
   */
  ~LinearRegression() {};
  /**
   * \brief This is the function that stores 2 vectors that will be compared using a linear regression line
   * \param modData is the DoubleVector containing the modelled data
   * \param obsData is the DoubleVector containing the observed data
   */
  virtual void storeVectors(const DoubleVector& modData, const DoubleVector& obsData);
};

/**
 * \class LogLinearRegression
 * \brief This is the class used to fit a log linear regression line to compare 2 vectors
 */
class LogLinearRegression : public Regression {
public:
  /**
   * \brief This is the default LogLinearRegression constructor
   * \param ltype is the LineType of the regression line
   */
  LogLinearRegression(LineType ltype);
  /**
   * \brief This is the default LogLinearRegression destructor
   */
  ~LogLinearRegression() {};
  /**
   * \brief This is the function that stores 2 vectors that will be compared using a log linear regression line
   * \param modData is the DoubleVector containing the modelled data
   * \param obsData is the DoubleVector containing the observed data
   */
  virtual void storeVectors(const DoubleVector& modData, const DoubleVector& obsData);
};

#endif
