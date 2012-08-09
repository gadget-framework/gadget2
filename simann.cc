/* ABSTRACT:                                                                */
/*   Simulated annealing is a global optimization method that distinguishes */
/*   different local optima. Starting from an initial point, the algorithm  */
/*   takes a step and the function is evaluated. When minimizing a function,*/
/*   any downhill step is accepted and the process repeats from this new    */
/*   point. An uphill step may be accepted (thus, it can escape from local  */
/*   optima). This uphill decision is made by the Metropolis criteria. As   */
/*   the optimization process proceeds, the length of the steps decline and */
/*   the algorithm closes in on the global optimum. Since the algorithm     */
/*   makes very few assumptions regarding the function to be optimized, it  */
/*   is quite robust with respect to non-quadratic surfaces. The degree of  */
/*   robustness can be adjusted by the user. In fact, simulated annealing   */
/*   can be used as a local optimizer for difficult functions.              */
/*                                                                          */
/*  The author can be contacted at h2zr1001@vm.cis.smu.edu                  */

/*  This file is a translation of a fortran code, which is an example of the*/
/*  Corana et al. simulated annealing algorithm for multimodal and robust   */
/*  optimization as implemented and modified by by Goffe et al.             */
/*                                                                          */
/*  Use the sample function from Judge with the following suggestions       */
/*  to get a feel for how SA works. When you've done this, you should be    */
/*  ready to use it on most any function with a fair amount of expertise.   */
/*    1. Run the program as is to make sure it runs okay. Take a look at    */
/*       the intermediate output and see how it optimizes as temperature    */
/*       (T) falls. Notice how the optimal point is reached and how         */
/*       falling T reduces VM.                                              */
/*    2. Look through the documentation to SA so the following makes a      */
/*       bit of sense. In line with the paper, it shouldn't be that hard    */
/*       to figure out. The core of the algorithm is described on pp. 4-6   */
/*       and on pp. 28. Also see Corana et al. pp. 264-9.                   */
/*    3. To see the importance of different temperatures, try starting      */
/*       with a very low one (say T = 10E-5). You'll see (i) it never       */
/*       escapes from the local optima (in annealing terminology, it        */
/*       quenches) & (ii) the step length (VM) will be quite small. This    */
/*       is a key part of the algorithm: as temperature (T) falls, step     */
/*       length does too. In a minor point here, note how VM is quickly     */
/*       reset from its initial value. Thus, the input VM is not very       */
/*       important. This is all the more reason to examine VM once the      */
/*       algorithm is underway.                                             */
/*    4. To see the effect of different parameters and their effect on      */
/*       the speed of the algorithm, try RT = .95 & RT = .1. Notice the     */
/*       vastly different speed for optimization. Also try NT = 20. Note    */
/*       that this sample function is quite easy to optimize, so it will    */
/*       tolerate big changes in these parameters. RT and NT are the        */
/*       parameters one should adjust to modify the runtime of the          */
/*       algorithm and its robustness.                                      */
/*    5. Try constraining the algorithm with either LB or UB.               */

/*  Synopsis:                                                               */
/*  This routine implements the continuous simulated annealing global       */
/*  optimization algorithm described in Corana et al.'s article             */
/*  "Minimizing Multimodal Functions of Continuous Variables with the       */
/*  "Simulated Annealing" Algorithm" in the September 1987 (vol. 13,        */
/*  no. 3, pp. 262-280) issue of the ACM Transactions on Mathematical       */
/*  Software.                                                               */

/*  A very quick (perhaps too quick) overview of SA:                        */
/*     SA tries to find the global optimum of an N dimensional function.    */
/*  It moves both up and downhill and as the optimization process           */
/*  proceeds, it focuses on the most promising area.                        */
/*     To start, it randomly chooses a trial point within the step length   */
/*  VM (a vector of length N) of the user selected starting point. The      */
/*  function is evaluated at this trial point and its value is compared     */
/*  to its value at the initial point.                                      */
/*     In a maximization problem, all uphill moves are accepted and the     */
/*  algorithm continues from that trial point. Downhill moves may be        */
/*  accepted; the decision is made by the Metropolis criteria. It uses T    */
/*  (temperature) and the size of the downhill move in a probabilistic      */
/*  manner. The smaller T and the size of the downhill move are, the more   */
/*  likely that move will be accepted. If the trial is accepted, the        */
/*  algorithm moves on from that point. If it is rejected, another point    */
/*  is chosen instead for a trial evaluation.                               */
/*     Each element of VM periodically adjusted so that half of all         */
/*  function evaluations in that direction are accepted.                    */
/*     A fall in T is imposed upon the system with the RT variable by       */
/*  T(i+1) = RT*T(i) where i is the ith iteration. Thus, as T declines,     */
/*  downhill moves are less likely to be accepted and the percentage of     */
/*  rejections rise. Given the scheme for the selection for VM, VM falls.   */
/*  Thus, as T declines, VM falls and SA focuses upon the most promising    */
/*  area for optimization.                                                  */

/*  The importance of the parameter T:                                      */
/*    The parameter T is crucial in using SA successfully. It influences    */
/*  VM, the step length over which the algorithm searches for optima. For   */
/*  a small intial T, the step length may be too small; thus not enough     */
/*  of the function might be evaluated to find the global optima. The user  */
/*  should carefully examine VM in the intermediate output (set IPRINT =    */
/*  1) to make sure that VM is appropriate. The relationship between the    */
/*  initial temperature and the resulting step length is function           */
/*  dependent.                                                              */
/*     To determine the starting temperature that is consistent with        */
/*  optimizing a function, it is worthwhile to run a trial run first. Set   */
/*  RT = 1.5 and T = 1.0. With RT > 1.0, the temperature increases and VM   */
/*  rises as well. Then select the T that produces a large enough VM.       */

/*  For modifications to the algorithm and many details on its use,         */
/*  (particularly for econometric applications) see Goffe, Ferrier          */
/*  and Rogers, "Global Optimization of Statistical Functions with          */
/*  the Simulated Annealing," Journal of Econometrics (forthcoming)         */
/*  For a pre-publication copy, contact                                     */
/*              Bill Goffe                                                  */
/*              Department of Economics                                     */
/*              Southern Methodist University                               */
/*              Dallas, TX  75275                                           */
/*              h2zr1001 @ smuvm1 (Bitnet)                                  */
/*              h2zr1001 @ vm.cis.smu.edu (Internet)                        */

/*  As far as possible, the parameters here have the same name as in        */
/*  the description of the algorithm on pp. 266-8 of Corana et al.          */

/*  Input Parameters:                                                       */
/*    Note: The suggested values generally come from Corana et al. To       */
/*          drastically reduce runtime, see Goffe et al., pp. 17-8 for      */
/*          suggestions on choosing the appropriate RT and NT.              */
/*    n - Number of variables in the function to be optimized. (INT)        */
/*    x - The starting values for the variables of the function to be       */
/*        optimized. (DP(N))                                                */
/*    max - Denotes whether the function should be maximized or             */
/*          minimized. A true value denotes maximization while a false      */
/*          value denotes minimization.                                     */
/*    RT - The temperature reduction factor. The value suggested by         */
/*         Corana et al. is .85. See Goffe et al. for more advice. (DP)     */
/*    EPS - Error tolerance for termination. If the final function          */
/*          values from the last neps temperatures differ from the          */
/*          corresponding value at the current temperature by less than     */
/*          EPS and the final function value at the current temperature     */
/*          differs from the current optimal function value by less than    */
/*          EPS, execution terminates and IER = 0 is returned. (EP)         */
/*    NS - Number of cycles. After NS*N function evaluations, each element  */
/*         of VM is adjusted so that approximately half of all function     */
/*         evaluations are accepted. The suggested value is 20. (INT)       */
/*    nt - Number of iterations before temperature reduction. After         */
/*         NT*NS*N function evaluations, temperature (T) is changed         */
/*         by the factor RT. Value suggested by Corana et al. is            */
/*         MAX(100, 5*N). See Goffe et al. for further advice. (INT)        */
/*    NEPS - Number of final function values used to decide upon termi-     */
/*           nation. See EPS. Suggested value is 4. (INT)                   */
/*    maxevl - The maximum number of function evaluations. If it is         */
/*             exceeded, IER = 1. (INT)                                     */
/*    lb - The lower bound for the allowable solution variables. (DP(N))    */
/*    ub - The upper bound for the allowable solution variables. (DP(N))    */
/*         If the algorithm chooses X(I) .LT. LB(I) or X(I) .GT. UB(I),     */
/*         I = 1, N, a point is from inside is randomly selected. This      */
/*         This focuses the algorithm on the region inside UB and LB.       */
/*         Unless the user wishes to concentrate the search to a par-       */
/*         ticular region, UB and LB should be set to very large positive   */
/*         and negative values, respectively. Note that the starting        */
/*         vector X should be inside this region. Also note that LB and     */
/*         UB are fixed in position, while VM is centered on the last       */
/*         accepted trial set of variables that optimizes the function.     */
/*    c - Vector that controls the step length adjustment. The suggested    */
/*        value for all elements is 2.0. (DP(N))                            */
/*    t - On input, the initial temperature. See Goffe et al. for advice.   */
/*        On output, the final temperature. (DP)                            */
/*    vm - The step length vector. On input it should encompass the         */
/*         region of interest given the starting value X. For point         */
/*         X(I), the next trial point is selected is from X(I) - VM(I)      */
/*         to  X(I) + VM(I). Since VM is adjusted so that about half        */
/*         of all points are accepted, the input value is not very          */
/*         important (i.e. is the value is off, SA adjusts VM to the        */
/*         correct value). (DP(N))                                          */

/*  Output Parameters:                                                      */
/*    xopt - The variables that optimize the function. (DP(N))              */
/*    fopt - The optimal value of the function. (DP)                        */

/* JMB this has been modified to work with the gadget object structure      */
/* This means that the function has been replaced by a call to ecosystem    */
/* object, and we can use the vector objects that have been defined         */

#include "gadget.h"    //All the required standard header files are in here
#include "optinfo.h"
#include "mathfunc.h"
#include "doublevector.h"
#include "intvector.h"
#include "errorhandler.h"
#include "ecosystem.h"
#include "global.h"

extern Ecosystem* EcoSystem;

void OptInfoSimann::OptimiseLikelihood() {

  //set initial values
  int nacc = 0;         //The number of accepted function evaluations
  int nrej = 0;         //The number of rejected function evaluations
  int naccmet = 0;      //The number of metropolis accepted function evaluations

  double tmp, p, pp, ratio, nsdiv;
  double fopt, funcval, trialf;
  int    a, i, j, k, l, offset, quit;
  int    rchange, rcheck, rnumber;  //Used to randomise the order of the parameters

  handle.logMessage(LOGINFO, "\nStarting Simulated Annealing optimisation algorithm\n");
  int nvars = EcoSystem->numOptVariables();
  DoubleVector x(nvars);
  DoubleVector init(nvars);
  DoubleVector trialx(nvars, 0.0);
  DoubleVector bestx(nvars);
  DoubleVector scalex(nvars);
  DoubleVector lowerb(nvars);
  DoubleVector upperb(nvars);
  DoubleVector fstar(tempcheck);
  DoubleVector vm(nvars, vminit);
  IntVector param(nvars, 0);
  IntVector nacp(nvars, 0);

  EcoSystem->resetVariables();  //JMB need to reset variables in case they have been scaled
  if (scale)
    EcoSystem->scaleVariables();
  EcoSystem->getOptScaledValues(x);
  EcoSystem->getOptLowerBounds(lowerb);
  EcoSystem->getOptUpperBounds(upperb);
  EcoSystem->getOptInitialValues(init);

  for (i = 0; i < nvars; i++) {
    bestx[i] = x[i];
    param[i] = i;
  }

  if (scale) {
    for (i = 0; i < nvars; i++) {
      scalex[i] = x[i];
      // Scaling the bounds, because the parameters are scaled
      lowerb[i] = lowerb[i] / init[i];
      upperb[i] = upperb[i] / init[i];
      if (lowerb[i] > upperb[i]) {
        tmp = lowerb[i];
        lowerb[i] = upperb[i];
        upperb[i] = tmp;
      }
    }
  }

  //funcval is the function value at x
  funcval = EcoSystem->SimulateAndUpdate(x);
  if (funcval != funcval) { //check for NaN
    handle.logMessage(LOGINFO, "Error starting Simulated Annealing optimisation with f(x) = infinity");
    converge = -1;
    iters = 1;
    return;
  }

  //the function is to be minimised so switch the sign of funcval (and trialf)
  funcval = -funcval;
  offset = EcoSystem->getFuncEval();  //number of function evaluations done before loop
  nacc++;
  cs /= lratio;  //JMB save processing time
  nsdiv = 1.0 / ns;
  fopt = funcval;
  for (i = 0; i < tempcheck; i++)
    fstar[i] = funcval;

  //Start the main loop.  Note that it terminates if
  //(i) the algorithm succesfully optimises the function or
  //(ii) there are too many function evaluations
  while (1) {
    for (a = 0; a < nt; a++) {
      //Randomize the order of the parameters once in a while, to avoid
      //the order having an influence on which changes are accepted
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

      for (j = 0; j < ns; j++) {
        for (l = 0; l < nvars; l++) {
          //Generate trialx, the trial value of x
          for (i = 0; i < nvars; i++) {
            if (i == param[l]) {
              trialx[i] = x[i] + ((randomNumber() * 2.0) - 1.0) * vm[i];

              //If trialx is out of bounds, try again until we find a point that is OK
              if ((trialx[i] < lowerb[i]) || (trialx[i] > upperb[i])) {
                //JMB - this used to just select a random point between the bounds
                k = 0;
                while ((trialx[i] < lowerb[i]) || (trialx[i] > upperb[i])) {
                  trialx[i] = x[i] + ((randomNumber() * 2.0) - 1.0) * vm[i];
                  k++;
                  if (k > 10)  //we've had 10 tries to find a point neatly, so give up
                    trialx[i] = lowerb[i] + (upperb[i] - lowerb[i]) * randomNumber();
                }
              }

            } else
              trialx[i] = x[i];
          }

          //Evaluate the function with the trial point trialx and return as -trialf
          trialf = EcoSystem->SimulateAndUpdate(trialx);
          trialf = -trialf;

          //If too many function evaluations occur, terminate the algorithm
          iters = EcoSystem->getFuncEval() - offset;
          if (iters > simanniter) {
            handle.logMessage(LOGINFO, "\nStopping Simulated Annealing optimisation algorithm\n");
            handle.logMessage(LOGINFO, "The optimisation stopped after", iters, "function evaluations");
            handle.logMessage(LOGINFO, "The temperature was reduced to", t);
            handle.logMessage(LOGINFO, "The optimisation stopped because the maximum number of function evaluations");
            handle.logMessage(LOGINFO, "was reached and NOT because an optimum was found for this run");
            handle.logMessage(LOGINFO, "Number of directly accepted points", nacc);
            handle.logMessage(LOGINFO, "Number of metropolis accepted points", naccmet);
            handle.logMessage(LOGINFO, "Number of rejected points", nrej);

            score = EcoSystem->SimulateAndUpdate(bestx);
            handle.logMessage(LOGINFO, "\nSimulated Annealing finished with a likelihood score of", score);
            return;
          }

          //Accept the new point if the new function value better
          if ((trialf - funcval) > verysmall) {
            for (i = 0; i < nvars; i++)
              x[i] = trialx[i];
            funcval = trialf;
            nacc++;
            nacp[param[l]]++;  //JMB - not sure about this ...

          } else {
            //Accept according to metropolis condition
            p = expRep((trialf - funcval) / t);
            pp = randomNumber();
            if (pp < p) {
              //Accept point
              for (i = 0; i < nvars; i++)
                x[i] = trialx[i];
              funcval = trialf;
              naccmet++;
              nacp[param[l]]++;
            } else {
              //Reject point
              nrej++;
            }
          }

          // JMB added check for really silly values
          if (isZero(trialf)) {
            handle.logMessage(LOGINFO, "Error in Simulated Annealing optimisation after", iters, "function evaluations, f(x) = 0");
            converge = -1;
            return;
          }

          //If greater than any other point, record as new optimum
          if ((trialf > fopt) && (trialf == trialf)) {
            for (i = 0; i < nvars; i++)
              bestx[i] = trialx[i];
            fopt = trialf;

            if (scale) {
              for (i = 0; i < nvars; i++)
                scalex[i] = bestx[i] * init[i];
              EcoSystem->storeVariables(-fopt, scalex);
            } else
              EcoSystem->storeVariables(-fopt, bestx);

            handle.logMessage(LOGINFO, "\nNew optimum found after", iters, "function evaluations");
            handle.logMessage(LOGINFO, "The likelihood score is", -fopt, "at the point");
            EcoSystem->writeBestValues();
          }
        }
      }

      //Adjust vm so that approximately half of all evaluations are accepted
      for (i = 0; i < nvars; i++) {
        ratio = nsdiv * nacp[i];
        nacp[i] = 0;
        if (ratio > uratio) {
          vm[i] = vm[i] * (1.0 + cs * (ratio - uratio));
        } else if (ratio < lratio) {
          vm[i] = vm[i] / (1.0 + cs * (lratio - ratio));
        }

        if (vm[i] < rathersmall)
          vm[i] = rathersmall;
        if (vm[i] > (upperb[i] - lowerb[i]))
          vm[i] = upperb[i] - lowerb[i];
      }
    }

    //Check termination criteria
    for (i = tempcheck - 1; i > 0; i--)
      fstar[i] = fstar[i - 1];
    fstar[0] = funcval;

    quit = 0;
    if (fabs(fopt - funcval) < simanneps) {
      quit = 1;
      for (i = 0; i < tempcheck - 1; i++)
        if (fabs(fstar[i + 1] - fstar[i]) > simanneps)
          quit = 0;
    }

    handle.logMessage(LOGINFO, "Checking convergence criteria after", iters, "function evaluations ...");

    //Terminate SA if appropriate
    if (quit) {
      handle.logMessage(LOGINFO, "\nStopping Simulated Annealing optimisation algorithm\n");
      handle.logMessage(LOGINFO, "The optimisation stopped after", iters, "function evaluations");
      handle.logMessage(LOGINFO, "The temperature was reduced to", t);
      handle.logMessage(LOGINFO, "The optimisation stopped because an optimum was found for this run");
      handle.logMessage(LOGINFO, "Number of directly accepted points", nacc);
      handle.logMessage(LOGINFO, "Number of metropolis accepted points", naccmet);
      handle.logMessage(LOGINFO, "Number of rejected points", nrej);

      converge = 1;
      score = EcoSystem->SimulateAndUpdate(bestx);
      handle.logMessage(LOGINFO, "\nSimulated Annealing finished with a likelihood score of", score);
      return;
    }

    //If termination criteria is not met, prepare for another loop.
    t *= rt;
    if (t < rathersmall)
      t = rathersmall;  //JMB make sure temperature doesnt get too small

    handle.logMessage(LOGINFO, "Reducing the temperature to", t);
    funcval = fopt;
    for (i = 0; i < nvars; i++)
      x[i] = bestx[i];
  }
}
