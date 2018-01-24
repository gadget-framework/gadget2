#include "errorhandler.h"
#include "optinfo.h"
#include "mathfunc.h"
#include "doublematrix.h"
#include "ecosystem.h"
#include "gadget.h"
#include "global.h"

/* JMB this has been modified to work with the gadget object structure   */
/* This means that the function has been replaced by a call to ecosystem */
/* object, and we can use the vector objects that have been defined      */

extern Ecosystem* EcoSystem;

/* calculate the smallest eigenvalue of a matrix */
double OptInfoBFGS::getSmallestEigenValue(DoubleMatrix M) {

  double eigen, temp, phi, norm;
  int i, j, k;
  int nvars = M.Nrow();
  DoubleMatrix L(nvars, nvars, 0.0);
  DoubleVector xo(nvars, 1.0);

  // calculate the Cholesky factor of the matrix
  for (k = 0; k < nvars; k++) {
    L[k][k] = M[k][k];
    for (j = 0; j < k - 1; j++)
      L[k][k] -= (L[k][j] * L[k][j]);
    for (i = k + 1; i < nvars; i++) {
      L[i][k] = M[i][k];
      for (j = 0; j < k - 1; j++)
        L[i][k] -= (L[i][j] * L[k][j]);

      if (isZero(L[k][k])) {
        handle.logMessage(LOGINFO, "Error in BFGS - divide by zero when calculating smallest eigen value");
        return 0.0;
      } else
        L[i][k] /= L[k][k];
    }
  }

  temp = (double)nvars;
  eigen = 0.0;
  for (k = 0; k < nvars; k++) {
    for (i = 0; i < nvars; i++) {
      for (j = 0; j < i - 1; j++)
        xo[i] -= (L[i][j] * xo[j]);
      xo[i] /= L[i][i];  //JMB we've already checked that L[i][i] is not zero
    }
    for (i = nvars - 1; i >= 0; i--) {
      for (j = nvars - 1; j > i + 1; j--)
        xo[i] -= (L[j][i] * xo[j]);
      xo[i] /= L[i][i];  //JMB we've already checked that L[i][i] is not zero
    }

    phi = 0.0;
    norm = 0.0;
    for (i = 0; i < nvars; i++) {
      phi += xo[i];
      norm += (xo[i] * xo[i]);
    }

    if (isZero(norm) || isZero(temp) || isZero(phi)) {
      handle.logMessage(LOGINFO, "Error in BFGS - divide by zero when calculating smallest eigen value");
      return 0.0;
    }

    for (i = 0; i < nvars; i++)
      xo[i] /= norm;
    eigen = phi / temp;
    temp = phi;
  }

  if (isZero(eigen)) {
    handle.logMessage(LOGINFO, "Error in BFGS - divide by zero when calculating smallest eigen value");
    return 0.0;
  }
  return 1.0 / eigen;
}

/* calculate the gradient of a function at a given point                    */
/* based on the forward difference gradient approximation (A5.6.3 FDGRAD)   */
/* Numerical Methods for Unconstrained Optimization and Nonlinear Equations */
/* by J E Dennis and Robert B Schnabel, published by SIAM, 1996             */
void OptInfoBFGS::gradient(DoubleVector& point, double pointvalue, DoubleVector& newgrad) {

  double ftmp, tmpacc;
  int i, j;
  int nvars = point.Size();
  DoubleVector gtmp(point);

  for (i = 0; i < nvars; i++) {
    for (j = 0; j < nvars; j++)
      gtmp[j] = point[j];

    //JMB the scaled parameter values should aways be positive
    if (point[i] < 0.0)
      handle.logMessage(LOGINFO, "Error in BFGS - negative parameter when calculating the gradient", point[i]);

    tmpacc = gradacc * max(point[i], 1.0);
    gtmp[i] += tmpacc;
    ftmp = EcoSystem->SimulateAndUpdate(gtmp);
    newgrad[i] = (ftmp - pointvalue) / tmpacc;
  }
}

void OptInfoBFGS::OptimiseLikelihood() {

  double hy, yBy, temphy, tempyby, normgrad;
  double searchgrad, newf, tmpf, betan;
  int i, j, resetgrad, offset, armijo;

  handle.logMessage(LOGINFO, "\nStarting BFGS optimisation algorithm\n");
  int nvars = EcoSystem->numOptVariables();
  DoubleVector x(nvars);
  DoubleVector trialx(nvars);
  DoubleVector bestx(nvars);
  DoubleVector init(nvars);
  DoubleVector h(nvars, 0.0);
  DoubleVector y(nvars, 0.0);
  DoubleVector By(nvars, 0.0);
  DoubleVector grad(nvars, 0.0);
  DoubleVector oldgrad(nvars, 0.0);
  DoubleVector search(nvars, 0.0);
  DoubleMatrix invhess(nvars, nvars, 0.0);

  EcoSystem->scaleVariables();  //JMB need to scale variables
  EcoSystem->getOptScaledValues(x);
  EcoSystem->getOptInitialValues(init);

  for (i = 0; i < nvars; i++) {
    trialx[i] = x[i];
    bestx[i] = x[i];
  }

  newf = EcoSystem->SimulateAndUpdate(trialx);
  if (newf != newf) { // check for NaN
    handle.logMessage(LOGINFO, "Error starting BFGS optimisation with f(x) = infinity");
    converge = -1;
    iters = 1;
    return;
  }

  this->gradient(trialx, newf, grad);
  tmpf = newf;
  offset = EcoSystem->getFuncEval();  // number of function evaluations done before loop
  sigma = -sigma; //JMB change sign of sigma (and consequently searchgrad)
  resetgrad = 0;
  for (i = 0; i < nvars; i++) {
    oldgrad[i] = grad[i];
    invhess[i][i] = 1.0;
  }

  while (1) {
    iters = EcoSystem->getFuncEval() - offset;
    if (isZero(newf)) {
      handle.logMessage(LOGINFO, "Error in BFGS optimisation after", iters, "function evaluations, f(x) = 0");
      converge = -1;
      return;
    }

    // terminate the algorithm if too many function evaluations occur
    if (iters > bfgsiter) {
      handle.logMessage(LOGINFO, "\nStopping BFGS optimisation algorithm\n");
      handle.logMessage(LOGINFO, "The optimisation stopped after", iters, "function evaluations");
      handle.logMessage(LOGINFO, "The optimisation stopped because the maximum number of function evaluations");
      handle.logMessage(LOGINFO, "was reached and NOT because an optimum was found for this run");

      score = EcoSystem->SimulateAndUpdate(bestx);
      handle.logMessage(LOGINFO, "\nBFGS finished with a likelihood score of", score);
      return;
    }

    if (resetgrad) {
      // terminate the algorithm if the gradient accuracy required has got too small
      if (gradacc < gradeps) {
        handle.logMessage(LOGINFO, "\nStopping BFGS optimisation algorithm\n");
        handle.logMessage(LOGINFO, "The optimisation stopped after", iters, "function evaluations");
        handle.logMessage(LOGINFO, "The optimisation stopped because the accuracy required for the gradient");
        handle.logMessage(LOGINFO, "calculation is too small and NOT because an optimum was found for this run");

        converge = 2;
        score = EcoSystem->SimulateAndUpdate(bestx);
        handle.logMessage(LOGINFO, "\nBFGS finished with a likelihood score of", score);
        return;
      }

      resetgrad = 0;
      // make the step size when calculating the gradient smaller
      gradacc *= gradstep;
      handle.logMessage(LOGINFO, "Warning in BFGS - resetting search algorithm after", iters, "function evaluations");

      for (i = 0; i < nvars; i++) {
        for (j = 0; j < nvars; j++)
          invhess[i][j] = 0.0;
        invhess[i][i] = 1.0;
      }
    }

    for (i = 0; i < nvars; i++) {
      search[i] = 0.0;
      for (j = 0; j < nvars; j++)
        search[i] -= (invhess[i][j] * grad[j]);
    }

    // do armijo calculation
    searchgrad = 0.0;
    for (i = 0; i < nvars; i++)
      searchgrad += (grad[i] * search[i]);
    searchgrad *= sigma;

    armijo = 0;
    betan = step;
    if (searchgrad > verysmall) {
      while ((armijo == 0) && (betan > rathersmall)) {
        for (i = 0; i < nvars; i++)
          trialx[i] = x[i] + (betan * search[i]);

        tmpf = EcoSystem->SimulateAndUpdate(trialx);
        if ((newf > tmpf) && ((newf - tmpf) > (betan * searchgrad)))
          armijo = 1;
        else
          betan *= beta;
      }
    }

    if (armijo) {
      this->gradient(trialx, tmpf, grad);
    } else {
      resetgrad = 1;
      this->gradient(x, newf, grad);
      continue;
    }

    normgrad = 0.0;
    for (i = 0; i < nvars; i++) {
      h[i] = betan * search[i];
      x[i] += h[i];
      y[i] = grad[i] - oldgrad[i];
      oldgrad[i] = grad[i];
      normgrad += grad[i] * grad[i];
    }
    normgrad = sqrt(normgrad);
    newf = EcoSystem->SimulateAndUpdate(x);
    for (i = 0; i < nvars; i++) {
      bestx[i] = x[i];
      trialx[i] = x[i] * init[i];
    }

    iters = EcoSystem->getFuncEval() - offset;
    EcoSystem->storeVariables(newf, trialx);
    handle.logMessage(LOGINFO, "\nNew optimum found after", iters, "function evaluations");
    handle.logMessage(LOGINFO, "The likelihood score is", newf, "at the point");
    EcoSystem->writeBestValues();

    // terminate the algorithm if the convergence criteria has been met
    if ((normgrad / (1.0 + newf)) < bfgseps) {
      handle.logMessage(LOGINFO, "\nStopping BFGS optimisation algorithm\n");
      handle.logMessage(LOGINFO, "The optimisation stopped after", iters, "function evaluations");
      handle.logMessage(LOGINFO, "The optimisation stopped because an optimum was found for this run");

      converge = 1;
      score = EcoSystem->SimulateAndUpdate(bestx);
      tmpf = this->getSmallestEigenValue(invhess);
      if (!isZero(tmpf))
        handle.logMessage(LOGINFO, "The smallest eigenvalue of the inverse Hessian matrix is", tmpf);
      handle.logMessage(LOGINFO, "\nBFGS finished with a likelihood score of", score);
      return;
    }

    // update the estimate for the inverse hessian matrix
    hy = 0.0;
    yBy = 0.0;
    for (i = 0; i < nvars; i++) {
      hy += h[i] * y[i];
      By[i] = 0.0;
      for (j = 0; j < nvars; j++)
        By[i] += (invhess[i][j] * y[j]);
      yBy += y[i] * By[i];
    }

    if ((isZero(hy)) || (isZero(yBy))) {
      resetgrad = 1;
    } else {
      temphy = 1.0 / hy;
      tempyby = 1.0 / yBy;
      for (i = 0; i < nvars; i++)
        for (j = 0; j < nvars; j++)
          invhess[i][j] += (h[i] * h[j] * temphy) - (By[i] * By[j] * tempyby)
            + yBy * (h[i] * temphy - By[i] * tempyby) * (h[j] * temphy - By[j] * tempyby);
    }
  }
}
