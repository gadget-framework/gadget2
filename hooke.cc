/* Nonlinear Optimization using the algorithm of Hooke and Jeeves  */
/*  12 February 1994    author: Mark G. Johnson                    */

/* Find a point X where the nonlinear function f(X) has a local    */
/* minimum.  X is an n-vector and f(X) is a scalar.  In mathe-     */
/* matical notation  f: R^n -> R^1.  The objective function f()    */
/* is not required to be continuous.  Nor does f() need to be      */
/* differentiable.  The program does not use or require            */
/* derivatives of f().                                             */

/* The software user supplies three things: a subroutine that      */
/* computes f(X), an initial "starting guess" of the minimum point */
/* X, and values for the algorithm convergence parameters.  Then   */
/* the program searches for a local minimum, beginning from the    */
/* starting guess, using the Direct Search algorithm of Hooke and  */
/* Jeeves.                                                         */

/* This C program is adapted from the Algol pseudocode found in    */
/* "Algorithm 178: Direct Search" by Arthur F. Kaupe Jr., Commun-  */
/* ications of the ACM, Vol 6. p.313 (June 1963).  It includes the */
/* improvements suggested by Bell and Pike (CACM v.9, p. 684, Sept */
/* 1966) and those of Tomlin and Smith, "Remark on Algorithm 178"  */
/* (CACM v.12).  The original paper, which I don't recommend as    */
/* highly as the one by A. Kaupe, is:  R. Hooke and T. A. Jeeves,  */
/* "Direct Search Solution of Numerical and Statistical Problems", */
/* Journal of the ACM, Vol. 8, April 1961, pp. 212-229.            */

/* Calling sequence:                                               */
/*  int hooke(nvars, startpt, endpt, rho, epsilon, itermax)        */
/*                                                                 */
/*     nvars {an integer}                                          */
/*         This is the number of dimensions in the domain of f().  */
/*         It is the number of coordinates of the starting point   */
/*         (and the minimum point.)                                */
/*     startpt {an array of doubles}                               */
/*         This is the user-supplied guess at the minimum.         */
/*     endpt {an array of doubles}                                 */
/*         This is the calculated location of the local minimum    */
/*     rho {a double}                                              */
/*         This is a user-supplied convergence parameter (more     */
/*         detail below), which should be set to a value between   */
/*         0.0 and 1.0.  Larger values of rho give greater         */
/*         probability of convergence on highly nonlinear          */
/*         functions, at a cost of more function evaluations.      */
/*         Smaller values of rho reduces the number of evaluations */
/*         (and the program running time), but increases the risk  */
/*         of nonconvergence.  See below.                          */
/*     epsilon {a double}                                          */
/*         This is the criterion for halting the search for a      */
/*         minimum.  When the algorithm begins to make less and    */
/*         less progress on each iteration, it checks the halting  */
/*         criterion: if the stepsize is below epsilon, terminate  */
/*         the iteration and return the current best estimate of   */
/*         the minimum.  Larger values of epsilon (such as 1.0e-4) */
/*         give quicker running time, but a less accurate estimate */
/*         of the minimum.  Smaller values of epsilon (such as     */
/*         1.0e-7) give longer running time, but a more accurate   */
/*         estimate of the minimum.                                */
/*     itermax     {an integer}  A second, rarely used, halting    */
/*         criterion.  If the algorithm uses >= itermax            */
/*         iterations, halt.                                       */

/* The user-supplied objective function f(x,n) should return a C   */
/* "double".  Its  arguments are  x -- an array of doubles, and    */
/* n -- an integer.  x is the point at which f(x) should be        */
/* evaluated, and n is the number of coordinates of x.  That is,   */
/* n is the number of coefficients being fitted.                   */

/*             rho, the algorithm convergence control              */

/*  The algorithm works by taking "steps" from one estimate of     */
/*    a minimum, to another (hopefully better) estimate.  Taking   */
/*    big steps gets to the minimum more quickly, at the risk of   */
/*    "stepping right over" an excellent point.  The stepsize is   */
/*    controlled by a user supplied parameter called rho.  At each */
/*    iteration, the stepsize is multiplied by rho  (0 < rho < 1), */
/*    so the stepsize is successively reduced.                     */
/*  Small values of rho correspond to big stepsize changes,        */
/*    which make the algorithm run more quickly.  However, there   */
/*    is a chance (especially with highly nonlinear functions)     */
/*    that these big changes will accidentally overlook a          */
/*    promising search vector, leading to nonconvergence.          */
/*  Large values of rho correspond to small stepsize changes,      */
/*    which force the algorithm to carefully examine nearby points */
/*    instead of optimistically forging ahead.  This improves the  */
/*    probability of convergence.                                  */
/*  The stepsize is reduced until it is equal to (or smaller       */
/*    than) epsilon.  So the number of iterations performed by     */
/*    Hooke-Jeeves is determined by rho and epsilon:               */
/*      rho**(number_of_iterations) = epsilon                      */
/*  In general it is a good idea to set rho to an aggressively     */
/*    small value like 0.5 (hoping for fast convergence).  Then,   */
/*    if the user suspects that the reported minimum is incorrect  */
/*    (or perhaps not accurate enough), the program can be run     */
/*    again with a larger value of rho such as 0.85, using the     */
/*    result of the first minimization as the starting guess to    */
/*    begin the second minimization.                               */

/*                        Normal use:                              */
/*    (1) Code your function f() in the C language                 */
/*    (2) Install your starting guess {or read it in}              */
/*    (3) Run the program                                          */
/*    (4) {for the skeptical}: Use the computed minimum            */
/*        as the starting point for another run                    */

/* Data Fitting:                                                   */
/*  Code your function f() to be the sum of the squares of the     */
/*  errors (differences) between the computed values and the       */
/*  measured values.  Then minimize f() using Hooke-Jeeves.        */
/*  EXAMPLE: you have 20 datapoints (ti, yi) and you want to       */
/*  find A,B,C such that  (A*t*t) + (B*exp(t)) + (C*tan(t))        */
/*  fits the data as closely as possible.  Then f() is just        */
/*  f(x) = SUM (measured_y[i] - ((A*t[i]*t[i]) + (B*exp(t[i]))     */
/*                + (C*tan(t[i]))))^2                              */
/*  where x[] is a 3-vector consisting of {A, B, C}.               */

/*  The author of this software is M.G. Johnson.                   */
/*  Permission to use, copy, modify, and distribute this software  */
/*  for any purpose without fee is hereby granted, provided that   */
/*  this entire notice is included in all copies of any software   */
/*  which is or includes a copy or modification of this software   */
/*  and in all copies of the supporting documentation for such     */
/*  software.  THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT    */
/*  ANY EXPRESS OR IMPLIED WARRANTY.  IN PARTICULAR, NEITHER THE   */
/*  AUTHOR NOR AT&T MAKE ANY REPRESENTATION OR WARRANTY OF ANY     */
/*  KIND CONCERNING THE MERCHANTABILITY OF THIS SOFTWARE OR ITS    */
/*  FITNESS FOR ANY PARTICULAR PURPOSE.                            */

/* JMB this has been modified to work with the gadget object structure   */
/* This means that the function has been replaced by a call to ecosystem */
/* object, and we can use the vector objects that have been defined      */

#include "gadget.h"    //All the required standard header files are in here
#include "optinfo.h"
#include "mathfunc.h"
#include "doublevector.h"
#include "intvector.h"
#include "errorhandler.h"
#include "ecosystem.h"
#include "global.h"

extern Ecosystem* EcoSystem;


/* given a point, look for a better one nearby, one coord at a time */
double OptInfoHooke::bestNearby(DoubleVector& delta, DoubleVector& point, double prevbest, IntVector& param) {

  double minf, ftmp;
  int i;
  DoubleVector z(point);

  minf = prevbest;
  for (i = 0; i < point.Size(); i++) {
    z[param[i]] = point[param[i]] + delta[param[i]];
    ftmp = EcoSystem->SimulateAndUpdate(z);
    if (ftmp < minf) {
      minf = ftmp;
    } else {
      delta[param[i]] = 0.0 - delta[param[i]];
      z[param[i]] = point[param[i]] + delta[param[i]];
      ftmp = EcoSystem->SimulateAndUpdate(z);
      if (ftmp < minf)
        minf = ftmp;
      else
        z[param[i]] = point[param[i]];
    }
  }

  for (i = 0; i < point.Size(); i++)
    point[i] = z[i];
  return minf;
}

void OptInfoHooke::OptimiseLikelihood() {

  double oldf, newf, bestf, steplength, tmp;
  int    i, offset;
  int    rchange, rcheck, rnumber;  //Used to randomise the order of the parameters

  handle.logMessage(LOGINFO, "\nStarting Hooke & Jeeves optimisation algorithm\n");
  int nvars = EcoSystem->numOptVariables();
  DoubleVector x(nvars);
  DoubleVector trialx(nvars);
  DoubleVector bestx(nvars);
  DoubleVector lowerb(nvars);
  DoubleVector upperb(nvars);
  DoubleVector init(nvars);
  DoubleVector initialstep(nvars, rho);
  DoubleVector delta(nvars);
  IntVector param(nvars, 0);
  IntVector lbound(nvars, 0);
  IntVector rbounds(nvars, 0);
  IntVector trapped(nvars, 0);

  EcoSystem->scaleVariables();
  EcoSystem->getOptScaledValues(x);
  EcoSystem->getOptLowerBounds(lowerb);
  EcoSystem->getOptUpperBounds(upperb);
  EcoSystem->getOptInitialValues(init);

  for (i = 0; i < nvars; i++) {
    // Scaling the bounds, because the parameters are scaled
    lowerb[i] = lowerb[i] / init[i];
    upperb[i] = upperb[i] / init[i];
    if (lowerb[i] > upperb[i]) {
      tmp = lowerb[i];
      lowerb[i] = upperb[i];
      upperb[i] = tmp;
    }

    bestx[i] = x[i];
    trialx[i] = x[i];
    param[i] = i;
    delta[i] = ((2 * (rand() % 2)) - 1) * rho;  //JMB - randomise the sign
  }

  bestf = EcoSystem->SimulateAndUpdate(trialx);
  if (bestf != bestf) { //check for NaN
    handle.logMessage(LOGINFO, "Error starting Hooke & Jeeves optimisation with f(x) = infinity");
    converge = -1;
    iters = 1;
    return;
  }

  offset = EcoSystem->getFuncEval();  //number of function evaluations done before loop
  newf = bestf;
  oldf = bestf;
  steplength = lambda;
  if (isZero(steplength))
    steplength = rho;

  while (1) {
    if (isZero(bestf)) {
      iters = EcoSystem->getFuncEval() - offset;
      handle.logMessage(LOGINFO, "Error in Hooke & Jeeves optimisation after", iters, "function evaluations, f(x) = 0");
      converge = -1;
      return;
    }

    /* randomize the order of the parameters once in a while */
    rchange = 0;
    while (rchange < nvars) {
      rnumber = rand() % nvars;
      rcheck = 1;
      for (i = 0; i < rchange; i++)
        if (param[i] == rnumber)
          rcheck = 0;
      if (rcheck) {
        param[rchange] = rnumber;
        rchange++;
      }
    }

    /* find best new point, one coord at a time */
    for (i = 0; i < nvars; i++)
      trialx[i] = x[i];
    newf = this->bestNearby(delta, trialx, bestf, param);

    /* if too many function evaluations occur, terminate the algorithm */
    iters = EcoSystem->getFuncEval() - offset;
    if (iters > hookeiter) {
      handle.logMessage(LOGINFO, "\nStopping Hooke & Jeeves optimisation algorithm\n");
      handle.logMessage(LOGINFO, "The optimisation stopped after", iters, "function evaluations");
      handle.logMessage(LOGINFO, "The steplength was reduced to", steplength);
      handle.logMessage(LOGINFO, "The optimisation stopped because the maximum number of function evaluations");
      handle.logMessage(LOGINFO, "was reached and NOT because an optimum was found for this run");

      score = EcoSystem->SimulateAndUpdate(trialx);
      handle.logMessage(LOGINFO, "\nHooke & Jeeves finished with a likelihood score of", score);
      for (i = 0; i < nvars; i++)
        bestx[i] = trialx[i] * init[i];
      EcoSystem->storeVariables(score, bestx);
      return;
    }

    /* if we made some improvements, pursue that direction */
    while (newf < bestf) {
      for (i = 0; i < nvars; i++) {
        /* if it has been trapped but f has now gotten better (bndcheck) */
        /* we assume that we are out of the trap, reset the counters     */
        /* and go back to the stepsize we had when we got trapped        */
        if ((trapped[i]) && (newf < oldf * bndcheck)) {
          trapped[i] = 0;
          lbound[i] = 0;
          rbounds[i] = 0;
          delta[i] = initialstep[i];

        } else if (trialx[i] < (lowerb[i] + verysmall)) {
          lbound[i]++;
          trialx[i] = lowerb[i];
          if (!trapped[i]) {
            initialstep[i] = delta[i];
            trapped[i] = 1;
          }
          /* if it has hit the bounds 2 times then increase the stepsize */
          if (lbound[i] >= 2)
            delta[i] /= rho;

        } else if (trialx[i] > (upperb[i] - verysmall)) {
          rbounds[i]++;
          trialx[i] = upperb[i];
          if (!trapped[i]) {
            initialstep[i] = delta[i];
            trapped[i] = 1;
          }
          /* if it has hit the bounds 2 times then increase the stepsize */
          if (rbounds[i] >= 2)
            delta[i] /= rho;
        }
      }

      for (i = 0; i < nvars; i++) {
        /* firstly, arrange the sign of delta[] */
        if (trialx[i] < x[i])
          delta[i] = 0.0 - fabs(delta[i]);
        else
          delta[i] = fabs(delta[i]);

        /* now, move further in this direction  */
        tmp = x[i];
        x[i] = trialx[i];
        trialx[i] = trialx[i] + trialx[i] - tmp;
      }

      /* only move forward if this is really an improvement    */
      oldf = newf;
      newf = EcoSystem->SimulateAndUpdate(trialx);
      if ((isEqual(newf, oldf)) || (newf > oldf)) {
        newf = oldf;  //JMB no improvement, so reset the value of newf
        break;
      }

      /* OK, it's better, so update variables and look around  */
      bestf = newf;
      for (i = 0; i < nvars; i++)
        x[i] = trialx[i];
      newf = this->bestNearby(delta, trialx, bestf, param);
      if (isEqual(newf, bestf))
        break;

      /* if too many function evaluations occur, terminate the algorithm */
      iters = EcoSystem->getFuncEval() - offset;
      if (iters > hookeiter) {
        handle.logMessage(LOGINFO, "\nStopping Hooke & Jeeves optimisation algorithm\n");
        handle.logMessage(LOGINFO, "The optimisation stopped after", iters, "function evaluations");
        handle.logMessage(LOGINFO, "The steplength was reduced to", steplength);
        handle.logMessage(LOGINFO, "The optimisation stopped because the maximum number of function evaluations");
        handle.logMessage(LOGINFO, "was reached and NOT because an optimum was found for this run");

        score = EcoSystem->SimulateAndUpdate(trialx);
        handle.logMessage(LOGINFO, "\nHooke & Jeeves finished with a likelihood score of", score);
        for (i = 0; i < nvars; i++)
          bestx[i] = trialx[i] * init[i];
        EcoSystem->storeVariables(score, bestx);
        return;
      }
    }

    iters = EcoSystem->getFuncEval() - offset;
    if (newf < bestf) {
      for (i = 0; i < nvars; i++)
        bestx[i] = x[i] * init[i];
      bestf = newf;
      handle.logMessage(LOGINFO, "\nNew optimum found after", iters, "function evaluations");
      handle.logMessage(LOGINFO, "The likelihood score is", bestf, "at the point");
      EcoSystem->storeVariables(bestf, bestx);
      EcoSystem->writeBestValues();

    } else
      handle.logMessage(LOGINFO, "Checking convergence criteria after", iters, "function evaluations ...");

    /* if the step length is less than hookeeps, terminate the algorithm */
    if (steplength < hookeeps) {
      handle.logMessage(LOGINFO, "\nStopping Hooke & Jeeves optimisation algorithm\n");
      handle.logMessage(LOGINFO, "The optimisation stopped after", iters, "function evaluations");
      handle.logMessage(LOGINFO, "The steplength was reduced to", steplength);
      handle.logMessage(LOGINFO, "The optimisation stopped because an optimum was found for this run");

      converge = 1;
      score = bestf;
      handle.logMessage(LOGINFO, "\nHooke & Jeeves finished with a likelihood score of", score);
      EcoSystem->storeVariables(bestf, bestx);
      return;
    }

    steplength *= rho;
    handle.logMessage(LOGINFO, "Reducing the steplength to", steplength);
    for (i = 0; i < nvars; i++)
      delta[i] *= rho;
  }
}
