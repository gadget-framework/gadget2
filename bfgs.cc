#include "optinfo.h"
#include "errorhandler.h"
#include "mathfunc.h"
#include "ecosystem.h"
#include "gadget.h"

/* global ecosystem used to store whether the model converged */
extern Ecosystem* EcoSystem;

/* global variable, defined and initialised in gadget.cc and not modified here */
extern int FuncEval;

/* calculate the smallest eigenvalue of a matrix */
double getSmallestEigenValue(double matrix[NUMVARS][NUMVARS], int nvars) {

  double eigen, temp, phi, norm;
  double L[NUMVARS][NUMVARS], xo[NUMVARS];
  int    i, j, k;

  // calculate the Cholesky factor of the matrix
  for (i = 0; i < nvars; i++)
    xo[i] = 1;
  for (k = 0; k < nvars; k++) {
    L[k][k] = matrix[k][k];
    for (j = 0; j < k - 1; j++)
      L[k][k] -= L[k][j] * L[k][j];
    for (i = k + 1; i < nvars; i++) {
      L[i][k] = matrix[i][k];
      for (j = 0; j < k - 1; j++)
        L[i][k] -= L[i][j] * L[k][j];
      L[i][k] /= L[k][k];
    }
  }

  temp = nvars;
  eigen = 0.0;
  for (k = 0; k < nvars; k++) {
    for (i = 0; i < nvars; i++) {
      for (j = 0; j < i - 1; j++)
        xo[i] -= L[i][j] * xo[j];
      xo[i] /= L[i][i];
    }
    for (i = nvars - 1; i >= 0; i--) {
      for (j = nvars - 1; j > i + 1; j--)
        xo[i] -= L[j][i] * xo[j];
      xo[i] /= L[i][i];
    }

    phi = 0.0;
    norm = 0.0;
    for (i = 0; i < nvars; i++) {
      phi += xo[i];
      norm += xo[i] * xo[i];
    }
    for (i = 0; i < nvars; i++)
      xo[i] /= norm;
    eigen = phi / temp;
    temp = phi;
  }
  return 1.0 / eigen;
}

/* calculate the gradient of a function at a point */
void gradient(double (*f)(double*), double point[], double pointvalue, double diaghess[], double grad[], int nvars, int diffgrad, double gradacc, double gradstep) {

  double tmpacc, f1, f2, mf1, mf2;
  double tmp[NUMVARS], mtmp[NUMVARS], tmp1[NUMVARS], mtmp1[NUMVARS];
  int    i, j;

  if (diffgrad < 1) {
    tmpacc = 1.0 / gradacc;
    for (i = 0; i < nvars; i++) {
      for (j = 0; j < nvars; j++)
        tmp[j] = point[j];
      tmp[i] += gradacc;
      f1 = (*f)(tmp);
      grad[i] = (f1 - pointvalue) * tmpacc;
      diaghess[i] = 1.0;
    }

  } else if (diffgrad == 1) {
    tmpacc = 1.0 / (2.0 * gradacc);
    for (i = 0; i < nvars; i++) {
      for (j = 0; j < nvars; j++) {
        tmp[j] = point[j];
        mtmp[j] = point[j];
      }
      tmp[i] += gradacc;
      mtmp[i] -= gradacc;
      f1 = (*f)(tmp);
      mf1 = (*f)(mtmp);
      grad[i] = (f1 - mf1) * tmpacc;
      if (abs((mf1 - f1) / pointvalue) < rathersmall) {
        gradacc = min(0.01, gradacc / gradstep);
        cout << "\nWarning in BFGS - possible roundoff errors in gradient\n";
      }
      if ((mf1 > pointvalue) && (f1 > pointvalue))
        diffgrad++;
      diaghess[i] = (f1 - 2.0 * pointvalue + mf1) / (gradacc * gradacc);
    }

  } else {
    tmpacc = 1.0 / (12.0 * gradacc);
    for (i = 0; i < nvars; i++) {
      for (j = 0; j < nvars; j++) {
        tmp[j] = point[j];
        mtmp[j] = point[j];
        tmp1[j] = point[j];
        mtmp1[j] = point[j];
      }
      tmp[i] += gradacc;
      tmp1[i] += 2.0 * gradacc;
      mtmp[i] -= gradacc;
      mtmp1[i] -= 2.0 * gradacc;
      f1 = (*f)(tmp);
      f2 = (*f)(tmp1);
      mf1 = (*f)(mtmp);
      mf2 = (*f)(mtmp1);
      grad[i] = (8.0 * f1 - f2 - 8.0 * mf1 + mf2) * tmpacc;
      if ((abs(mf2 - f2) / pointvalue) < rathersmall) {
        gradacc = min(0.01, gradacc / gradstep);
        cout << "\nWarning in BFGS - possible roundoff errors in gradient\n";
      }
      diaghess[i] = (-f2 + 16.0 * f1 - 30.0 * pointvalue + 16.0 * mf1 - mf2 ) * 12.0 * tmpacc * tmpacc;
    }
  }
}

int bfgs(double (*f)(double*), double startpt[], double endpt[], double init[], int nvars,
  int maxevl, double epsilon, double beta, double sigma, double step, double gradacc,
  double gradstep, double errortol, int diffgrad) {

  double h[NUMVARS];
  double y[NUMVARS];
  double By[NUMVARS];
  double newx[NUMVARS];                //current estimation for best point
  double grad[NUMVARS];                //gradient vector
  double oldgrad[NUMVARS];             //old gradient vector
  double diaghess[NUMVARS];            //diagonal of the inverse hessian matrix
  double search[NUMVARS];              //direction for the armijo search
  double invhess[NUMVARS][NUMVARS];    //inverse hessian matrix
  double tmp[NUMVARS];
  double hy, yBy, temphy, tempyby;
  double normgrad, normdeltax;
  double alpha, searchgrad, newf, tmpf, betan;
  int    i, j, check, offset, armijoproblem, armijoquit;

  for (i = 0; i < nvars; i++) {
    grad[i] = 0.0;
    diaghess[i] = 0.0;
    newx[i] = startpt[i];
    tmp[i] = startpt[i];
  }

  newf = (*f)(startpt);
  gradient(f, startpt, newf, diaghess, grad, nvars, diffgrad, gradacc, gradstep);
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

  if (newf != newf) { //check for NaN
    cout << "\nError starting BFGS optimisation with"
      << " f(x) = infinity\nReturning to calling routine ...\n";
    return 0;
  }

  while (1) {
    if (isZero(newf)) {
      cout << "\nError in BFGS optimisation after " << (FuncEval - offset)
       << " function evaluations f(x) = 0\nReturning to calling routine ...\n";
      return 0;
    }

    //If too many function evaluations occur, terminate the algorithm
    if ((FuncEval - offset) > maxevl) {
      cout << "\nStopping BFGS optimisation algorithm\n\n"
        << "The optimisation stopped after " << (FuncEval - offset)
        << " function evaluations (max " << maxevl
        << ")\nThe optimisation stopped because the maximum number of function evalutions"
        << "\nwas reached and NOT because an optimum was found for this run\n";

      newf = (*f)(newx);
      EcoSystem->setFuncEvalBFGS(FuncEval - offset);
      EcoSystem->setLikelihoodBFGS(newf);
      for (i = 0; i < nvars; i++)
        newx[i] *= init[i];
      EcoSystem->StoreVariables(newf, newx);

      cout << "The smallest eigenvalue of the inverse Hessian matrix is " << getSmallestEigenValue(invhess, nvars) << endl;
      return 0;
    }

    if (check == 0 || alpha < 0.0) {
      //JMB - dont print warning on the first loop ...
      if ((FuncEval - offset) > 0)
        cout << "\nWarning in BFGS - resetting search algorithm after "
          << (FuncEval - offset) << " function evaluations" << endl;

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
      tmp[i] = newx[i];

    searchgrad = 0.0;
    alpha = -1.0;
    for (i = 0; i < nvars; i++)
      searchgrad += grad[i] * search[i];
    searchgrad *= sigma;

    if (searchgrad < 0.0) {
      betan = step;
      armijoquit = 0;
      while (!armijoquit && (betan > verysmall)) {
        for (i = 0; i < nvars; i++)
          tmp[i] = newx[i] + betan * search[i];

        tmpf = (*f)(tmp);
        if (((newf - tmpf) >= (-betan * searchgrad)) && (newf > tmpf))
          armijoquit = 1;
        else
          betan *= beta;
      }

      if ((armijoquit) && (tmpf == tmpf)) {
        newf = tmpf;
        alpha = betan;
      } else
        alpha = 0.0;
    }

    if (alpha < 0.0)
      continue;

    gradient(f, tmp, tmpf, diaghess, grad, nvars, diffgrad, gradacc, gradstep);
    if (isZero(alpha)) {
      diffgrad++;
      gradient(f, newx, newf, diaghess, grad, nvars, diffgrad, gradacc, gradstep);
      armijoproblem++;
      if (armijoproblem == 6)
        check = 0;
      continue;
    }

    normgrad = 0.0;
    normdeltax = 0.0;
    hy = 0.0;
    yBy = 0.0;

    for (i = 0; i < nvars; i++) {
      h[i] = alpha * search[i];
      newx[i] += h[i];
      y[i] = grad[i] - oldgrad[i];
      oldgrad[i] = grad[i];
      hy += h[i] * y[i];
      normgrad += grad[i] * grad[i];
      normdeltax += h[i] * h[i];
    }
    normgrad = sqrt(normgrad);
    normdeltax = sqrt(normdeltax);

    if (normdeltax < errortol) {
      cout << "\nWarning in BFGS - search failed because normed deltax value too small\n";
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

    newf = (*f)(newx);  //The value of newf shouldn't change, but just to make sure ...
    cout << "\nNew optimum after " << (FuncEval - offset) << " function evaluations, f(x) = "
      << newf << "\nwith normed gradient value " << normgrad <<  " at\n";
    for (i = 0; i < nvars; i++) {
      endpt[i] = newx[i] * init[i];
      cout << endpt[i] << sep;
    }
    cout << endl;

    //store this point in case the algorithm is interrupted
    EcoSystem->StoreVariables(newf, endpt);

    //If the algorithm has met the convergence criteria, terminate the algorithm
    if (normgrad / (1 + abs(newf)) < epsilon) {
    cout << "\nStopping BFGS optimisation algorithm\n\n"
      << "The optimisation stopped after " << (FuncEval - offset)
      << " function evaluations (max " << maxevl
      << ")\nThe optimisation stopped because an optimum was found for this run\n";

      EcoSystem->setConvergeBFGS(1);
      EcoSystem->setFuncEvalBFGS(FuncEval - offset);
      EcoSystem->setLikelihoodBFGS(newf);
      cout << "The smallest eigenvalue of the inverse Hessian matrix is " << getSmallestEigenValue(invhess, nvars) << endl;
      return 1;
    }
  }
}
