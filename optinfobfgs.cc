#include "optinfo.h"
#include "ecosystem.h"
#include "gadget.h"

extern Ecosystem* EcoSystem;
extern ErrorHandler handle;

double b(double* x, int n) {
  return EcoSystem->SimulateAndUpdate(x, n);
}

extern int bfgs(double (*b)(double*, int), double startpoint[], double endpoint[], double init[],
  int n, int maxiter, double epsilon, double beta, double sigma, double step, double gradacc,
  double gradstep, double errortol, int diffgrad);

OptInfoBFGS::OptInfoBFGS()
  : OptSearch(), bfgsiter(10000), bfgseps(0.01), beta(0.3), sigma(0.01), step(1.0),
    gradacc(0.01), gradstep(0.1), errortol(0.000001), diffgrad(-1), scale(0) {
  handle.logMessage("Initialising BFGS optimisation algorithm");
}

void OptInfoBFGS::OptimiseLikelihood() {
  int i, opt, nvars;

  handle.logInformation("\nStarting BFGS optimisation algorithm");

  nvars = EcoSystem->numOptVariables();
  DoubleVector val(nvars);
  DoubleVector initialval(nvars);
  double* startpoint = new double[nvars];
  double* endpoint = new double[nvars];
  double* init = new double[nvars];

  if (scale == 1)
    EcoSystem->ScaleVariables();
  EcoSystem->ScaledOptValues(val);
  EcoSystem->InitialOptValues(initialval);

  for (i = 0; i < nvars; i++) {
    startpoint[i] = val[i];
    init[i] = initialval[i];
  }

  opt = bfgs(&b, startpoint, endpoint, init, nvars, bfgsiter, bfgseps,
    beta, sigma, step, gradacc, gradstep, errortol, diffgrad);

  cout << "\nBFGS finished with a final likelihood score of " << EcoSystem->getLikelihood()
    << "\nafter a total of " << EcoSystem->getFuncEval() << " function evaluations at the point\n";
  EcoSystem->writeOptValues();

  delete[] init;
  delete[] endpoint;
  delete[] startpoint;
}

void OptInfoBFGS::read(CommentStream& infile, char* text) {
  while (!infile.eof() && strcasecmp(text, "seed") && strcasecmp(text, "[simann]") && strcasecmp(text, "[hooke]")) {
    infile >> ws;
    if (strcasecmp(text,"beta") == 0) {
      infile >> beta;

    } else if (strcasecmp(text, "sigma") == 0) {
      infile >> sigma;

    } else if (strcasecmp(text, "step") == 0) {
      infile >> step;

    } else if (strcasecmp(text, "gradacc") == 0) {
      infile >> gradacc;

    } else if (strcasecmp(text, "gradstep") == 0) {
      infile >> gradstep;

    } else if ((strcasecmp(text, "bfgsiter") == 0) || (strcasecmp(text, "maxiter") == 0)) {
      infile >> bfgsiter;

    } else if (strcasecmp(text, "bfgseps") == 0) {
      infile >> bfgseps;

    } else if (strcasecmp(text, "scale") == 0) {
      infile >> scale;

    } else if (strcasecmp(text, "errortol") == 0) {
      infile >> errortol;

    } else if (strcasecmp(text, "difficultgrad") == 0) {
      infile >> diffgrad;

    } else if (strcasecmp(text, "printing") == 0) {
      handle.logWarning("Warning in optinfofile - bfgsprinting is not implemented in gadget");
      infile >> text;  //read and ignore the next entry

    } else if (strcasecmp(text, "shannonscaling") == 0) {
      handle.logWarning("Warning in optinfofile - shannonscaling is not implemented in gadget");
      infile >> text;  //read and ignore the next entry

    } else if (strcasecmp(text, "bfgspar") == 0) {
      handle.logWarning("Warning in optinfofile - bfgspar is not implemented gadget");
      infile >> text;  //read and ignore the next entry

    } else {
      handle.logWarning("Warning in optinfofile - unrecognised option", text);
      infile >> text;  //read and ignore the next entry
    }
    infile >> text;
  }

  //check the values specified in the optinfo file ...
  if (isZero(beta) || (beta < 0) || (beta > 1)) {
    handle.logWarning("Warning in optinfofile - value of beta outside bounds", beta);
    beta = 0.3;
  }
  if (isZero(sigma) || (sigma < 0)|| (sigma > 1)) {
    handle.logWarning("Warning in optinfofile - value of sigma outside bounds", sigma);
    sigma = 0.01;
  }
  if (step < 1) {
    handle.logWarning("Warning in optinfofile - value of step outside bounds", step);
    step = 1.0;
  }
  if (isZero(bfgseps) || (bfgseps < 0)) {
    handle.logWarning("Warning in optinfofile - value of bfgseps outside bounds", bfgseps);
    bfgseps = 0.01;
  }
  if (isZero(gradacc) || (gradacc < 0) || (gradacc > 1)) {
    handle.logWarning("Warning in optinfofile - value of gradacc outside bounds", gradacc);
    gradacc = 0.0001;
  }
  if (isZero(gradstep) || (gradstep < 0) || (gradstep > 1)) {
    handle.logWarning("Warning in optinfofile - value of gradstep outside bounds", gradstep);
    gradstep = 0.1;
  }
  if (isZero(errortol) || (errortol < 0)) {
    handle.logWarning("Warning in optinfofile - value of errortol outside bounds", errortol);
    errortol = 0.000001;
  }
}
