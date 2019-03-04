#include "regressionline.h"
#include "mathfunc.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

// ********************************************************
// Functions for Regression
// ********************************************************
Regression::Regression() {
  linetype = FREE;
  error = 1;
  useweights = 0;
  sse = a = b = 0.0;
}

Regression::Regression(LineType ltype) {
  linetype = ltype;
  error = 0;
  useweights = 0;
  sse = a = b = 0.0;
}

void Regression::setWeights(const DoubleVector& weights) {
  if (!useweights) {
    handle.logMessage(LOGWARN, "Warning in regression - unexpected use of weights");
    error = 1;
    return;
  }
  w = weights;
}

double Regression::getSSE() {
  //  if (error)
  //    return verybig;
  return sse;
}

void Regression::calcFit() {
  if (error)
    return;

  switch (linetype) {
    case FREE:
      //need to calculate both the slope and the intercept
      this->calcSlopeIntercept();
      break;
    case FIXEDSLOPE:
      //only need to calculate the intercept
      this->calcIntercept();
      break;
    case FIXEDINTERCEPT:
      //only need to calculate the slope
      this->calcSlope();
      break;
    case FIXED:
      //nothing to be done here
      break;
    default:
      handle.logMessage(LOGWARN, "Warning in regression - unrecognised linetype", linetype);
      break;
  }

  //finally calculate the SSE value
  if (useweights)
    this->calcSSEWeights();
  else
    this->calcSSE();
}

void Regression::calcSSE() {
  //if (error)
  //  return;

  int i;
  double tmp;
  sse = 0.0;
  for (i = 0; i < x.Size(); i++) {
    tmp = y[i] - (a + b * x[i]);
    sse += tmp * tmp;
  }
}

void Regression::calcSSEWeights() {
  if (error)
    return;

  int i;
  double tmp;
  sse = 0.0;
  for (i = 0; i < x.Size(); i++) {
    tmp = y[i] - (a + b * x[i]);
    sse += w[i] * tmp * tmp;
  }
}

void Regression::calcSlope() {
  if (error)
    return;

  int i;
  double sumX, sumY;
  sumX = sumY = 0.0;
  for (i = 0; i < x.Size(); i++) {
    sumX += x[i];
    sumY += y[i];
  }
  if (isZero(sumX))
    b = 0.0;
  else
    b = (sumY - (a * x.Size())) / sumX;

  //JMB - if there is a negative slope for the regression then things are going wrong
  if (b < 0.0) {
    handle.logMessage(LOGWARN, "Warning in regression - negative slope for regression line", b);
    //    error = 1;
  }
}

void Regression::calcIntercept() {
  if (error)
    return;

  int i;
  double sumX, sumY;
  sumX = sumY = 0.0;
  for (i = 0; i < x.Size(); i++) {
    sumX += x[i];
    sumY += y[i];
  }
  a = (sumY - (b * sumX)) / x.Size();
}

void Regression::calcSlopeIntercept() {
  if (error)
    return;

  int i;
  double sumX, sumY, nom, denom;
  sumX = sumY = nom = denom = 0.0;
  for (i = 0; i < x.Size(); i++) {
    sumX += x[i];
    sumY += y[i];
  }
  sumX /= x.Size();
  sumY /= y.Size();

  for (i = 0; i < x.Size(); i++) {
    nom += (x[i] - sumX) * (y[i] - sumY);
    denom += (x[i] - sumX) * (x[i] - sumX);
  }

  if (isZero(denom)) {
    b = 0.0;
    a = sumY;
  } else {
    b = nom / denom;
    a = sumY - (b * sumX);
  }

  //JMB - if there is a negative slope for the regression then things are going wrong
  if (b < 0.0) {
    handle.logMessage(LOGWARN, "Warning in regression - negative slope for regression line", b);
    error = 1;
  }
}

// ********************************************************
// Functions for LinearRegression
// ********************************************************
LinearRegression::LinearRegression(LineType ltype) : Regression(ltype) {
}

void LinearRegression::storeVectors(const DoubleVector& modData, const DoubleVector& obsData) {
  error = 0;  //begin by cleaning up error status
  if ((modData.Size() != obsData.Size()) || (modData.Size() < 2)) {
    handle.logMessage(LOGWARN, "Warning in linear regression - invalid vector sizes");
    error = 1;
    return;
  }
  x = modData;
  y = obsData;
}

// ********************************************************
// Functions for LogLinearRegression
// ********************************************************
LogLinearRegression::LogLinearRegression(LineType ltype) : Regression(ltype) {
}

void LogLinearRegression::storeVectors(const DoubleVector& modData, const DoubleVector& obsData) {
  error = 0;  //begin by cleaning up error status
  if ((modData.Size() != obsData.Size()) || (modData.Size() < 2)) {
    handle.logMessage(LOGWARN, "Warning in log linear regression - invalid vector sizes");
    error = 1;
    return;
  }

  x.Reset();
  x.resize(modData.Size(), 0.0);
  y.Reset();
  y.resize(obsData.Size(), 0.0);

  int i, l = 0;
  for (i = 0; i < x.Size(); i++, l++) {
    if (isZero(modData[i]) && isZero(obsData[i])) {
      //omit the point (0.0, 0.0)
      x.Delete(l);
      y.Delete(l);
      l--;  
    } else if ((modData[i] < verysmall) || (obsData[i] < verysmall)) {
      handle.logMessage(LOGWARN, "Warning in log linear regession - received invalid values");
      //error = 1;
      if((modData[i] < verysmall)) {
	x[l] = -20.0;
      } else {
	x[l] = log(modData[i]);
      }

      if((obsData[i] < verysmall)) {
	y[l] = -20.0;
      } else {
	y[l] = log(obsData[i]);
      }

      //return;
    } else {
      x[l] = log(modData[i]);
      y[l] = log(obsData[i]);
    }
  }
}

// ********************************************************
// Functions for WeightRegression
// ********************************************************
WeightRegression::WeightRegression(LineType ltype) : LinearRegression(ltype) {
  useweights = 1;
}

void WeightRegression::storeVectors(const DoubleVector& modData, const DoubleVector& obsData) {
  LinearRegression::storeVectors(modData, obsData);
  if (x.Size() != w.Size()) {
    handle.logMessage(LOGWARN, "Warning in weight regression - invalid vector sizes");
    error = 1;
  }
}

// ********************************************************
// Functions for LogWeightRegression
// ********************************************************
LogWeightRegression::LogWeightRegression(LineType ltype) : LogLinearRegression(ltype) {
  useweights = 1;
}

void LogWeightRegression::storeVectors(const DoubleVector& modData, const DoubleVector& obsData) {
  LogLinearRegression::storeVectors(modData, obsData);
  if (x.Size() != w.Size()) {
    handle.logMessage(LOGWARN, "Warning in log weight regression - invalid vector sizes");
    error = 1;
  }
}
