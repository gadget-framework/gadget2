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
/*       parameters one should adjust to modify the runtime of th           */
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
/*  the Simulated Annealing," Journal of Econometrics (forthcomming)        */
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

/*  Input/Output Parameters:                                                */
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
/*    nacc - The number of accepted function evaluations. (INT)             */
/*    nfcnev - The total number of function evaluations. In a minor point,  */
/*             note that the first evaluation is not used in the core of    */
/*             the algorithm; it simply initializes the algorithm (INT)     */
/*    nobds - The total number of trial function evaluations that           */
/*            would have been out of bounds of LB and UB. Note that         */
/*            a trial point is randomly selected between LB and UB. (INT)   */

#include "gadget.h"    //All the required standard header files are in here
#include "mathfunc.h"

#define NEPS   (6)     //Number of final func. values to decide upon termination.
#define VARS   (350)   //Maximum number of variables.

//This function replaces exp to avoid under- and overflow.
double expRep(double inn) {
  double rdum, exprep;
  rdum = inn;
  if (rdum > 174.)
    exprep = 3.69e75;
  else if (rdum < -180.)
    exprep = 0.0;
  else
    exprep = exp(rdum);
  return exprep;
}

//Returns uniformly-distributed doubles in range 0.0 to 1.0
double randomNumber() {
  int r = rand();
  double k = r % 32767;
  return k / 32767.;
}

int simann(int nvar, double point[], double endpoint[], double lb[], double ub[],
  double (*f)(double*, int), int m, int maxeval, double step[],
  double tempt, double stepl[], double rt, int ns, int nt, double eps) {

  int n = nvar;         //Number of variables in the function to be optimized
  double x[VARS];       //The starting values for the variables
  int max = m;          //Denotes whether the func. should be maximized or
                        //minimized: 1 = maximization 0 = minimization
  int maxevl = maxeval; //Maximum number of func. evaluations
  double c[VARS];       //Vector that controls the step length adjustment
                        //Suggested to be 2.0 for all elements
  double t = tempt;     //On input, the initial temperature
                        //On output, the final temperature
  double vm[VARS];      //The step length vector
  double fstar[NEPS];
  double fopt;          //The optimal value of the function
  double funcval, fp;
  double ratio;         //Dummy variable for updating vm
  int nacp[VARS];       //Number of accepted tries for each parameter
  int quit = 0;         //Used to check the exit criteria

  //Set initial values
  int nacc = 0;         //The number of accepted function evaluations
  int nrej = 0;         //The number of rejected function evaluations
  int nnew = 0;         //Number of optimum found
  int nobds = 0;        //Total number of trial functions that were out of bounds
  int nfcnev = 0;       //Total number of function evaluations
  int i;
  double xp[VARS];
  int param[VARS];  //Vector containing the order of the parameters at each time

  for (i = 0; i < n; i++) {
    x[i] = point[i];
    endpoint[i] = point[i];
    c[i] = step[i];
    vm[i] = stepl[i];
    nacp[i] = 0;
  }
  for (i = 0; i < NEPS; i++)
    fstar[i] = 1.0e20;

  cout << "\nStarting Simulated Annealing\n";
  //funcval is the function value at x
  funcval = (*f)(x, n);
  nfcnev++;

  //If the function is to be minmized, switch the sign of the function
  if (!max)
    funcval = -funcval;
  fopt = funcval;
  fstar[0] = funcval;

  for (i = 0; i < n; i++)
    param[i] = i;

  int a, j, h, k, change, l;
  double p, pp;

  //Start the main loop.  Note that it terminates if
  //(i) the algorithm succesfully optimizes the function or
  //(ii) there are too many function evaluations
  while (1) {
    for (a = 0; a < nt; a++) {
      for (j = 0; j < ns; j++) {
        for (l = 0; l < n; l++) {
          if (nfcnev % (15 * n) == 0) {
            //Randomize the order of the parameters once in a while, to avoid
            //the order having an influence on which changes are accepted
            change = 0;
            while (change < n) {
              h = rand() % n;
              k = 1;
              for (i = 0; i < change; i++)
                if (param[i] == h)
                  k = 0;

              if (k) {
                param[change] = h;
                change++;
              }
            }
          }

          //Generate xp, the trial value of x
          for (i = 0; i < n; i++) {
            if (i == param[l])
              xp[i] = x[i] + (randomNumber() * 2.0 - 1.0) * vm[i];
            else
              xp[i] = x[i];

            //If xp is out of bounds, select a point in bounds for the trial
            if ((xp[i] < lb[i]) || (xp[i] > ub[i])) {
              xp[i] = lb[i] + (ub[i] - lb[i]) * randomNumber();
              nobds++;
            }
          }

          //Evaluate the function with the trial point xp and return as fp
          fp = (*f)(xp, n);
          if (!max)
            fp = -fp;

          nfcnev++;
          //If too many function evaluations occur, terminate the algorithm
          if (nfcnev >= maxevl) {
            cout << "\nStopping Simulated Annealing\n\nThe optimisation stopped after " << nfcnev
              << " function evaluations (max " << maxevl << ")\nThe optimisation stopped "
              << "because the maximum number of function evaluations\nwas reached and "
              << "NOT because an optimum was found for this run\n";

            fp = (*f)(endpoint, n);
            //if (!max)
            //  fopt = -fopt;

            return nfcnev;
          }

          //Accept the new point if the new function value better
          if (fp >= funcval) {
            //Accept point.
            for (i = 0; i < n; i++)
              x[i] = xp[i];
            funcval = fp;
            nacc++;
            nacp[l]++;

          } else {
            //Accept according to metropolis condition
            p = expRep((fp - funcval) / t);
            pp = randomNumber();
            if (pp < p) {
              //Accept point
              for (i = 0; i < n; i++)
                x[i] = xp[i];
              funcval = fp;
              nacc++;
              nacp[l]++;
            } else
              //Reject point
              nrej++;
          }

          // JMB added check for really silly values
          if (isZero(fp)) {
            cout << "\nError in Simulated Annealing optimisation after " << nfcnev
              << " function evaluations f(x) = 0\nReturning to calling routine ...\n";
            return 0;
          }

          //If greater than any other point, record as new optimum
          if (fp > fopt) {
            cout << "\nNew optimum after " << nfcnev << " function evaluations, f(x) = " << -fp << " at\n";
            for (i = 0; i < n; i++) {
              endpoint[i] = xp[i];
              cout << endpoint[i] << sep;
            }
            cout << endl;
            fopt = fp;
            nnew++;
          }
        }
      }

      //Adjust vm so that approximately half of all evaluations are accepted
      for (i = 0; i < n; i++) {
        ratio =  nacp[i] / ns;
        if (ratio > 0.8) {
          vm[i] = vm[i] * (1.0 + c[i] * (ratio - 0.6) * 2.5);
        } else if (ratio < 0.5) {
          vm[i] = vm[i] / (1.0 + c[i] * (0.4 - ratio) * 2.5);
        }

        if (vm[i] > (ub[i] - lb[i])) {
          vm[i] = ub[i] - lb[i];
        } else if (vm[i] < 1e-6) {
          vm[i] = stepl[i];
        }
      }
      for (i = 0; i < n; i++)
        nacp[i] = 0;
    }

    //Check termination criteria
    quit = 0;
    fstar[0] = funcval;
    if ((fopt - fstar[0]) <= eps)
      quit = 1;
    for (i = 0; i < NEPS; i++)
      if (absolute(funcval - fstar[i]) > eps)
        quit = 0;

    //Terminate SA if appropriate
    if (quit) {
      cout << "\nStopping Simulated Annealing\n\nThe optimisation stopped after " << nfcnev
        << " function evaluations (max " << maxevl << ")\nThe optimisation stopped "
        << "because an optimum was found for this run\n";

      fp = (*f)(endpoint, n);
      //if (!max)
      //  fopt = -fopt;

      return nfcnev;
    }

    //If termination criteria is not met, prepare for another loop.
    t = rt * t;
    for (i = NEPS - 1; i > 0; i--)
      fstar[i] = fstar[i - 1];
    funcval = fopt;
    for (i = 0; i < n; i++)
      x[i] = endpoint[i];

  }
}
