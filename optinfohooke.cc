#include "optinfo.h"
#include "ecosystem.h"
#include "gadget.h"

extern ErrorHandler handle;
extern Ecosystem* EcoSystem;

double h(double* x, int n) {
  return EcoSystem->SimulateAndUpdate(x, n);
}

extern int hooke(double (*h)(double*, int), int n, double startpoint[],
  double endpoint[], double lowerb[], double upperb[], double rho,
  double lambda, double epsilon, int itermax, double init[], double bndcheck);

OptInfoHooke::OptInfoHooke()
  : OptSearch(), hookeiter(1000), rho(0.5), lambda(0.0), hookeeps(1e-4), bndcheck(0.9999) {
  handle.logMessage("Initialising Hooke & Jeeves optimisation algorithm");
}

void OptInfoHooke::read(CommentStream& infile, char* text) {
  while (!infile.eof() && strcasecmp(text, "seed") && strcasecmp(text, "[simann]") && strcasecmp(text, "[bfgs]")) {
    infile >> ws;
    if (strcasecmp(text, "rho") == 0) {
      infile >> rho;

    } else if (strcasecmp(text, "lambda") == 0) {
      infile >> lambda;

    } else if (strcasecmp(text, "hookeeps") == 0) {
      infile >> hookeeps;

    } else if (strcasecmp(text, "hookeiter") == 0) {
      infile >> hookeiter;

    } else if (strcasecmp(text, "bndcheck") == 0) {
      infile >> bndcheck;

    } else {
      handle.logWarning("Warning in optinfofile - unrecognised option", text);
      infile >> text;  //read and ignore the next entry
    }
    infile >> text;
  }

  //check the values specified in the optinfo file ...
  if ((rho < 0) || (rho > 1) || isZero(rho)) {
    handle.logWarning("Warning in optinfofile - value of rho outside bounds", rho);
    rho = 0.5;
  }
  if ((lambda < 0) || (lambda > 1)) {
    handle.logWarning("Warning in optinfofile - value of lambda outside bounds", lambda);
    lambda = rho;
  }
  if ((bndcheck < 0.5) || (bndcheck > 1)) {
    handle.logWarning("Warning in optinfofile - value of bndcheck outside bounds", bndcheck);
    bndcheck = 0.9999;
  }
  if ((isZero(hookeeps)) || (hookeeps < 0)) {
    handle.logWarning("Warning in optinfofile - value of hookeeps outside bounds", hookeeps);
    hookeeps = 1e-4;
  }
}

void OptInfoHooke::OptimiseLikelihood() {
  int i, nopt, opt;
  double tmp;

  handle.logInformation("\nStarting Hooke & Jeeves optimisation algorithm");

  nopt = EcoSystem->numOptVariables();
  DoubleVector val(nopt);
  DoubleVector lbds(nopt);
  DoubleVector ubds(nopt);
  DoubleVector initialval(nopt);
  ParameterVector optswitches(nopt);

  EcoSystem->ScaleVariables();
  EcoSystem->ScaledOptValues(val);
  EcoSystem->LowerOptBds(lbds);
  EcoSystem->UpperOptBds(ubds);
  EcoSystem->InitialOptValues(initialval);
  EcoSystem->OptSwitches(optswitches);

  double* startpoint = new double[nopt];
  double* endpoint = new double[nopt];
  double* upperb = new double[nopt];
  double* lowerb = new double[nopt];
  double* init = new double[nopt];

  for (i = 0; i < nopt; i++) {
    startpoint[i] = val[i];
    lowerb[i] = lbds[i];
    upperb[i] = ubds[i];
    init[i] = initialval[i];
  }

  /* Scaling the bounds, because the parameters are scaled. */
  for (i = 0; i < nopt; i++) {
    lowerb[i] = lowerb[i] / initialval[i];
    upperb[i] = upperb[i] / initialval[i];
    if (lowerb[i] > upperb[i]) {
      /* If bounds smaller than zero */
      tmp = lowerb[i];
      lowerb[i] = upperb[i];
      upperb[i] = tmp;
    }

    /* Warn if the starting point is zero */
    if (isZero(val[i]))
      handle.logWarning("Warning in optinfo - initial value is zero for switch", optswitches[i].getName());
  }

  opt = hooke(&h, nopt, startpoint, endpoint, upperb, lowerb,
    rho, lambda, hookeeps, hookeiter, init, bndcheck);

  cout << "\nHooke & Jeeves finished with a final likelihood score of " << EcoSystem->getLikelihood()
    << "\nafter a total of " << EcoSystem->getFuncEval() << " function evaluations at the point\n";
  EcoSystem->writeOptValues();

  delete[] endpoint;
  delete[] startpoint;
  delete[] upperb;
  delete[] lowerb;
  delete[] init;
}
