#include "optinfo.h"
#include "errorhandler.h"
#include "mathfunc.h"
#include "doublematrix.h"
#include "doublevector.h"
#include "intvector.h"
#include "ecosystem.h"
#include "gadget.h"

/* JMB this has been modified to work with the gadget object structure   */
/* This means that the function has been replaced by a call to ecosystem */
/* object, and we can use the vector objects that have been defined      */

/* global ecosystem used to store whether the model converged */
extern Ecosystem* EcoSystem;

/* global errorhandler used to log messages */
extern ErrorHandler handle;

/* global variable, defined and initialised in gadget.cc and not modified here */
extern int FuncEval;

/* calculate the smallest eigenvalue of a matrix */
double getSmallestEigenValue(DoubleMatrix M) {

  double eigen, temp, phi, norm;
  int    i, j, k;
  int    nvars = M.Nrow();
  DoubleMatrix L(nvars, nvars);
  DoubleVector xo(nvars, 1.0);

  // calculate the Cholesky factor of the matrix
  for (k = 0; k < nvars; k++) {
    L[k][k] = M[k][k];
    for (j = 0; j < k - 1; j++)
      L[k][k] -= L[k][j] * L[k][j];
    for (i = k + 1; i < nvars; i++) {
      L[i][k] = M[i][k];
      for (j = 0; j < k - 1; j++)
        L[i][k] -= L[i][j] * L[k][j];

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
        xo[i] -= L[i][j] * xo[j];

      if (isZero(L[i][i])) {
        handle.logMessage(LOGINFO, "Error in BFGS - divide by zero when calculating smallest eigen value");
        return 0.0;
      } else
        xo[i] /= L[i][i];
    }
    for (i = nvars - 1; i >= 0; i--) {
      for (j = nvars - 1; j > i + 1; j--)
        xo[i] -= L[j][i] * xo[j];

      if (isZero(L[i][i])) {
        handle.logMessage(LOGINFO, "Error in BFGS - divide by zero when calculating smallest eigen value");
        return 0.0;
      } else
        xo[i] /= L[i][i];
    }

    phi = 0.0;
    norm = 0.0;
    for (i = 0; i < nvars; i++) {
      phi += xo[i];
      norm += xo[i] * xo[i];
    }

    if (isZero(norm)) {
      handle.logMessage(LOGINFO, "Error in BFGS - divide by zero when calculating smallest eigen value");
      return 0.0;
    } else
      for (i = 0; i < nvars; i++)
        xo[i] /= norm;

    if (isZero(temp)) {
      handle.logMessage(LOGINFO, "Error in BFGS - divide by zero when calculating smallest eigen value");
      return 0.0;
    } else
      eigen = phi / temp;

    temp = phi;
  }

  if (isZero(eigen)) {
    handle.logMessage(LOGINFO, "Error in BFGS - divide by zero when calculating smallest eigen value");
    return 0.0;
  }
  return 1.0 / eigen;
}

/* calculate the gradient of a function at a point */
void gradient(DoubleVector& point, double pointvalue, DoubleVector& diaghess,
  DoubleVector& grad, int diffgrad, double gradacc, double gradstep) {

  double tmpacc;
  int    i, j;
  int    nvars = point.Size();

  if (diffgrad < 1) {
    DoubleVector gtmp(nvars);
    double ftmp;
    tmpacc = 1.0 / gradacc;
    for (i = 0; i < nvars; i++) {
      for (j = 0; j < nvars; j++)
        gtmp[j] = point[j];
      gtmp[i] += gradacc;
      ftmp = EcoSystem->SimulateAndUpdate(gtmp);
      grad[i] = (ftmp - pointvalue) * tmpacc;
      diaghess[i] = 1.0;
    }

  } else if (diffgrad == 1) {
    DoubleVector gtmpp(nvars);
    DoubleVector gtmpm(nvars);
    double ftmpp, ftmpm;
    tmpacc = 1.0 / (2.0 * gradacc);
    for (i = 0; i < nvars; i++) {
      for (j = 0; j < nvars; j++) {
        gtmpp[j] = point[j];
        gtmpm[j] = point[j];
      }
      gtmpp[i] += gradacc;
      gtmpm[i] -= gradacc;
      ftmpp = EcoSystem->SimulateAndUpdate(gtmpp);
      ftmpm = EcoSystem->SimulateAndUpdate(gtmpm);
      grad[i] = (ftmpp - ftmpm) * tmpacc;
      if (abs((ftmpm - ftmpp) / pointvalue) < rathersmall) {
        gradacc = min(0.01, gradacc / gradstep);
        handle.logMessage(LOGINFO, "Warning in BFGS - possible roundoff errors in gradient");
      }
      if ((ftmpm > pointvalue) && (ftmpp > pointvalue))
        diffgrad++;
      diaghess[i] = (ftmpp - 2.0 * pointvalue + ftmpm) / (gradacc * gradacc);
    }

  } else {
    DoubleVector gtmpp1(nvars);
    DoubleVector gtmpm1(nvars);
    DoubleVector gtmpp2(nvars);
    DoubleVector gtmpm2(nvars);
    double ftmpp1, ftmpm1, ftmpp2, ftmpm2;
    tmpacc = 1.0 / (12.0 * gradacc);
    for (i = 0; i < nvars; i++) {
      for (j = 0; j < nvars; j++) {
        gtmpp1[j] = point[j];
        gtmpp2[j] = point[j];
        gtmpm1[j] = point[j];
        gtmpm2[j] = point[j];
      }
      gtmpp1[i] += gradacc;
      gtmpp2[i] += 2.0 * gradacc;
      gtmpm1[i] -= gradacc;
      gtmpm2[i] -= 2.0 * gradacc;
      ftmpp1 = EcoSystem->SimulateAndUpdate(gtmpp1);
      ftmpp2 = EcoSystem->SimulateAndUpdate(gtmpp2);
      ftmpm1 = EcoSystem->SimulateAndUpdate(gtmpm1);
      ftmpm2 = EcoSystem->SimulateAndUpdate(gtmpm2);
      grad[i] = (8.0 * ftmpp1 - ftmpp2 - 8.0 * ftmpm1 + ftmpm2) * tmpacc;
      if ((abs(ftmpm2 - ftmpp2) / pointvalue) < rathersmall) {
        gradacc = min(0.01, gradacc / gradstep);
        handle.logMessage(LOGINFO, "Warning in BFGS - possible roundoff errors in gradient");
      }
      diaghess[i] = (-ftmpp2 + 16.0 * ftmpp1 - 30.0 * pointvalue + 16.0 * ftmpm1 - ftmpm2 ) * 12.0 * tmpacc * tmpacc;
    }
  }
}

int bfgs(int maxevl, double epsilon, double beta, double sigma, double step,
  double gradacc, double gradstep, double errortol, int diffgrad, int scale) {

  double hy, yBy, temphy, tempyby;
  double normgrad, normdeltax;
  double alpha, searchgrad, newf, tmpf, betan;
  int    i, j, check, offset, armijoproblem, armijoquit;

  int    nvars = EcoSystem->numOptVariables();
  DoubleVector x(nvars);
  DoubleVector trialx(nvars);
  DoubleVector bestx(nvars);
  DoubleVector init(nvars);
  DoubleVector h(nvars, 0.0);
  DoubleVector y(nvars, 0.0);
  DoubleVector By(nvars, 0.0);
  DoubleVector grad(nvars, 0.0);
  DoubleVector oldgrad(nvars, 0.0);
  DoubleVector diaghess(nvars, 0.0);
  DoubleVector search(nvars, 0.0);
  DoubleMatrix invhess(nvars, nvars, 0.0);

  if (scale == 1)
    EcoSystem->scaleVariables();
  EcoSystem->getOptScaledValues(x);
  EcoSystem->getOptInitialValues(init);

  for (i = 0; i < nvars; i++) {
    trialx[i] = x[i];
    bestx[i] = x[i];
  }

  newf = EcoSystem->SimulateAndUpdate(trialx);
  gradient(trialx, newf, diaghess, grad, diffgrad, gradacc, gradstep);
  offset = FuncEval;

  for (i = 0; i < nvars; i++) {
    oldgrad[i] = grad[i];
    for (j = 0; j < nvars; j++)
      invhess[i][j] = 0.0;
    invhess[i][i] = diaghess[i];
  }

  check = 1;
  armijoproblem = 0;
  alpha = 1.0;

  if (newf != newf) //check for NaN
    handle.logMessage(LOGFAIL, "Error starting BFGS optimisation with f(x) = infinity");

  while (1) {
    if (isZero(newf)) {
      handle.logMessage(LOGINFO, "Error in BFGS optimisation after", (FuncEval - offset), "function evaluations, f(x) = 0");
      return 0;
    }

    //If too many function evaluations occur, terminate the algorithm
    if ((FuncEval - offset) > maxevl) {
      handle.logMessage(LOGINFO, "\nStopping BFGS optimisation algorithm\n");
      handle.logMessage(LOGINFO, "The optimisation stopped after", (FuncEval - offset), "function evaluations");
      handle.logMessage(LOGINFO, "The optimisation stopped because the maximum number of function evaluations");
      handle.logMessage(LOGINFO, "was reached and NOT because an optimum was found for this run");

      newf = EcoSystem->SimulateAndUpdate(x);
      EcoSystem->setFuncEvalBFGS(FuncEval - offset);
      EcoSystem->setLikelihoodBFGS(newf);
      for (i = 0; i < nvars; i++)
        x[i] *= init[i];
      EcoSystem->storeVariables(newf, x);
      tmpf = getSmallestEigenValue(invhess);
      if (!isZero(tmpf))
        handle.logMessage(LOGINFO, "The smallest eigenvalue of the inverse Hessian matrix is", tmpf);
      return 0;
    }

    if (check == 0 || alpha < 0.0) {
      //JMB - dont print warning on the first loop ...
      if ((FuncEval - offset) > 0)
        handle.logMessage(LOGINFO, "Warning in BFGS - resetting search algorithm after", (FuncEval - offset), "function evaluations");

      for (i = 0; i < nvars; i++) {
        for (j = 0; j < nvars; j++)
          invhess[i][j] = 0.0;
        invhess[i][i] = diaghess[i];
      }

      check++;
      armijoproblem = 0;
      //JMB - make the step size when calculating the gradient smaller
      gradacc *= gradstep;
      if (gradacc < errortol) {
        gradacc /= gradstep;
        diffgrad++;
      }
    }

    for (i = 0; i < nvars; i++) {
      search[i] = 0.0;
      for (j = 0; j < nvars; j++)
        search[i] -= invhess[i][j] * grad[j];
    }

    // do armijo calculation
    tmpf = newf;
    for (i = 0; i < nvars; i++)
      trialx[i] = x[i];

    searchgrad = 0.0;
    alpha = -1.0;
    for (i = 0; i < nvars; i++)
      searchgrad += grad[i] * search[i];
    searchgrad *= sigma;

    if (searchgrad < 0.0) {
      betan = step;
      armijoquit = 0;
      while ((armijoquit == 0) && (betan > verysmall)) {
        for (i = 0; i < nvars; i++)
          trialx[i] = x[i] + betan * search[i];

        tmpf = EcoSystem->SimulateAndUpdate(trialx);
        if (((newf - tmpf) >= (-betan * searchgrad)) && (newf > tmpf))
          armijoquit = 1;
        else
          betan *= beta;
      }

      if ((armijoquit) && (tmpf == tmpf)) {
        newf = tmpf;
        gradient(trialx, tmpf, diaghess, grad, diffgrad, gradacc, gradstep);
        alpha = betan;
      } else
        alpha = 0.0;
    }

    if ((isZero(alpha)) || (searchgrad > 0.0) || (isZero(searchgrad))) {
      diffgrad++;
      gradient(x, newf, diaghess, grad, diffgrad, gradacc, gradstep);
      armijoproblem++;
      if (armijoproblem == 6)
        check = 0;
      continue;
    }

    if (alpha < 0.0)
      continue;

    normgrad = 0.0;
    normdeltax = 0.0;
    hy = 0.0;
    yBy = 0.0;

    for (i = 0; i < nvars; i++) {
      h[i] = alpha * search[i];
      x[i] += h[i];
      y[i] = grad[i] - oldgrad[i];
      oldgrad[i] = grad[i];
      hy += h[i] * y[i];
      normgrad += grad[i] * grad[i];
      normdeltax += h[i] * h[i];
    }
    normgrad = sqrt(normgrad);
    normdeltax = sqrt(normdeltax);

    if (normdeltax < errortol) {
      handle.logMessage(LOGINFO, "Warning in BFGS - search failed because normed deltax value too small");
      check = 0;
      continue;
    }

    for (i = 0; i < nvars; i++) {
      By[i] = 0.0;
      for (j = 0; j < nvars; j++)
        By[i] += invhess[i][j] * y[j];
      yBy += y[i] * By[i];
    }

    if ((isZero(hy)) || (isZero(yBy))) {
      check = 0;
    } else {
      temphy = 1.0 / hy;
      tempyby = 1.0 / yBy;
      for (i = 0; i < nvars; i++)
        for (j = 0; j < nvars; j++)
          invhess[i][j] += (h[i] * h[j] * temphy) - (By[i] * By[j] * tempyby)
            + yBy * (h[i] * temphy - By[i] * tempyby) * (h[j] * temphy - By[j] * tempyby);
    }

    newf = EcoSystem->SimulateAndUpdate(x);
    for (i = 0; i < nvars; i++)
      bestx[i] = x[i] * init[i];

    EcoSystem->storeVariables(newf, bestx);
    handle.logMessage(LOGINFO, "\nNew optimum found after", (FuncEval - offset), "function evaluations");
    handle.logMessage(LOGINFO, "The likelihood score is", newf, "at the point");
    EcoSystem->writeBestValues();

    //If the algorithm has met the convergence criteria, terminate the algorithm
    if (normgrad / (1.0 + abs(newf)) < epsilon) {
      handle.logMessage(LOGINFO, "\nStopping BFGS optimisation algorithm\n");
      handle.logMessage(LOGINFO, "The optimisation stopped after", (FuncEval - offset), "function evaluations");
      handle.logMessage(LOGINFO, "The optimisation stopped because an optimum was found for this run");

      EcoSystem->setConvergeBFGS(1);
      EcoSystem->setFuncEvalBFGS(FuncEval - offset);
      EcoSystem->setLikelihoodBFGS(newf);
      tmpf = getSmallestEigenValue(invhess);
      if (!isZero(tmpf))
        handle.logMessage(LOGINFO, "The smallest eigenvalue of the inverse Hessian matrix is", tmpf);
      return 1;
    }
  }
}
