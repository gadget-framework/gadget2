#include "optinfo.h"
#include "errorhandler.h"
#include "mathfunc.h"
#include "ecosystem.h"
#include "gadget.h"

/* global ecosystem used to store whether the model converged */
extern Ecosystem* EcoSystem;

/* global variable, defined and initialised in gadget.cc and not modified here */
extern int FuncEval;

int OptInfoBfgs::iteration(double* x0) {
  double h[NUMVARS];
  double y[NUMVARS];
  double By[NUMVARS];
  double g0[NUMVARS];  //old gradient
  double hy, yBy, alpha, normgrad, normdeltax, temphy, tempyby;
  int i, j, k, check, offset;

  fk = (*f)(x0, numvar);
  offset = FuncEval;
  gradient(x0, fk);
  for (i = 0; i < numvar; i++) {
    g0[i] = gk[i];
    x[i] = x0[i];
  }

  check = 0;
  k = 0;
  alpha = 1.0;

  if (fk != fk) { //check for NaN
    cout << "\nError starting BFGS optimisation with"
      << " f(x) = infinity\nReturning to calling routine ...\n";
    return 0;
  }

  while (1) {
    if (isZero(fk)) {
      cout << "\nError in BFGS optimisation after " << (FuncEval - offset)
       << " function evaluations f(x) = 0\nReturning to calling routine ...\n";
      return 0;
    }

    //If too many function evaluations occur, terminate the algorithm
    if ((FuncEval - offset) > bfgsiter) {
      cout << "\nStopping BFGS optimisation algorithm\n\n"
        << "The optimisation stopped after " << (FuncEval - offset)
        << " function evaluations (max " << bfgsiter << ")\nThe optimisation stopped because the "
        << "maximum number of function evaluations\nwas reached and NOT because an "
        << "optimum was found for this run\n";

      fk = (*f)(x, numvar);
      EcoSystem->setFuncEvalBFGS(FuncEval - offset);
      EcoSystem->setLikelihoodBFGS(fk);
      return 0;
    }

    if (check == 0 || isZero(alpha)) {
      if (k != 0)
        cerr << "\nWarning in BFGS search - resetting algorithm after "
          << (FuncEval - offset) << " function evaluations" << endl;
      for (i = 0; i < numvar; i++) {
        for (j = 0; j < numvar; j++)
          Bk[i][j] = 0.0;
        Bk[i][i] = 1.0;
      }
      check = 1;
      k++;
      //JMB - make the step size when calculating the gradient smaller
      gradacc *= gradstep;
      if (gradacc < rathersmall)
        gradacc = rathersmall;
    }

    for (i = 0; i < numvar; i++) {
      s[i] = 0.0;
      for (j = 0; j < numvar; j++)
        s[i] -= Bk[i][j] * gk[j];
    }

    alpha = Armijo();
    if (isZero(alpha))
      continue;

    normgrad = 0.0;
    normdeltax = 0.0;
    hy = 0.0;
    yBy = 0.0;

    for (i = 0; i < numvar; i++) {
      h[i] = alpha * s[i];
      x[i] += h[i];
      y[i] = gk[i] - g0[i];
      g0[i] = gk[i];
      hy += h[i] * y[i];
      normgrad += gk[i] * gk[i];
      normdeltax += h[i] * h[i];
    }
    normgrad = sqrt(normgrad);
    normdeltax = sqrt(normdeltax);

    for (i = 0; i < numvar; i++) {
      By[i] = 0.0;
      for (j = 0; j < numvar; j++)
        By[i] += Bk[i][j] * y[j];
      yBy += y[i] * By[i];
    }

    if ((isZero(hy)) || (isZero(yBy))) {
      check = 0;
    } else {
      temphy = 1.0 / hy;
      tempyby = 1.0 / yBy;
      for (i = 0; i < numvar; i++)
        for (j = 0; j < numvar; j++)
          Bk[i][j] += (h[i] * h[j] * temphy) - (By[i] * By[j] * tempyby)
            + yBy * (h[i] * temphy - By[i] * tempyby) * (h[j] * temphy - By[j] * tempyby);
    }

    cout << "\nNew optimum after " << (FuncEval - offset) << " function evaluations, f(x) = " << fk << " at\n";
    for (i = 0; i < numvar; i++)
      cout << x[i] << sep;
    cout << endl;

    //If the algorithm has met the convergence criteria, terminate the algorithm
    if ((normgrad / (1.0 + absolute(fk)) < bfgseps) && (normdeltax < bfgseps)) {
      cout << "\nStopping BFGS optimisation algorithm\n\n"
        << "The optimisation stopped after " << (FuncEval - offset)
        << " function evaluations (max " << bfgsiter << ")\nThe optimisation stopped because "
        << "an optimum was found for this run\n";

      fk = (*f)(x, numvar);
      EcoSystem->setConvergeBFGS(1);
      EcoSystem->setFuncEvalBFGS(FuncEval - offset);
      EcoSystem->setLikelihoodBFGS(fk);
      return 1;
    }
  }
}

void OptInfoBfgs::gradient(double* p, double fp) {
  int i, j;
  double tmpacc;
  double tmp[NUMVARS];

  //h = 0.00001;
  tmpacc = 1.0 / (2.0 * gradacc);
  for (i = 0; i < numvar; i++) {
    for (j = 0; j < numvar; j++)
      tmp[j] = p[j];

    tmp[i] += gradacc;
    gk[i] = ((*f)(tmp, numvar) - fp) * tmpacc;
  }
}

double OptInfoBfgs::Armijo() {
  int i, cond;
  double bn, fn, sg;
  double tmp[NUMVARS];

  cond = 0;
  bn = st;
  fn = fk;
  sg = 0.0;
  for (i = 0; i < numvar; i++)
    sg += gk[i] * s[i];
  sg *= sigma;

  while (!cond && (bn > verysmall)) {
    for (i = 0; i < numvar; i++)
      tmp[i] = x[i] + bn * s[i];

    fn = (*f)(tmp, numvar);
    if (((fk - fn) >= (-bn * sg)) && (fk > fn))
      cond = 1;
    else
      bn *= beta;
  }

  if ((cond) && (fn == fn)) {  //check for NaN
    fk = fn;
    gradient(tmp, fk);
    return bn;
  } else
    return 0.0;
}
