#include "optinfo.h"
#include "ecosystem.h"
#include "gadget.h"

extern Ecosystem* EcoSystem;
extern ErrorHandler handle;

double func(double* x, int n) {
  return EcoSystem->SimulateAndUpdate(x, n);
}

OptInfoBfgs::OptInfoBfgs()
  : OptSearch(), bfgsiter(10000), bfgseps(0.01), beta(0.3), sigma(0.01), step(1.0),
    gradacc(0.0001), gradstep(0.1) {
    
  handle.logMessage("Initialising BFGS optimisation algorithm");
  numvar = EcoSystem->numOptVariables();
  int i;
  x = new double[numvar];
  s = new double[numvar];
  gk = new double[numvar];
  Bk = new double*[numvar];
  f = &func;
  for (i = 0; i < numvar; i++) {
    Bk[i] = new double[numvar];
  }
}

OptInfoBfgs::~OptInfoBfgs() {
  int i;
  delete[] x;
  delete[] s;
  delete[] gk;
  for (i = 0; i < numvar; i++)
    delete[] Bk[i];
}

void OptInfoBfgs::OptimiseLikelihood() {
  int i, opt;

  handle.logInformation("\nStarting BFGS optimisation algorithm");

  DoubleVector val(numvar);
  double* startpoint = new double[numvar];
  EcoSystem->ScaledOptValues(val);
  for (i = 0; i < numvar; i++)
    startpoint[i] = val[i];

  opt = iteration(startpoint);
  cout << "\nBFGS finished with a final likelihood score of " << EcoSystem->getLikelihood()
    << "\nafter a total of " << EcoSystem->getFuncEval() << " function evaluations at the point\n";
  EcoSystem->writeOptValues();

  delete[] startpoint;
}

void OptInfoBfgs::read(CommentStream& infile, char* text) {
  while (!infile.eof() && strcasecmp(text, "seed") && strcasecmp(text, "[simann]") && strcasecmp(text, "[hooke]")) {
    infile >> ws;
    if (strcasecmp(text,"beta") == 0) {
      infile >> beta;

    } else if (strcasecmp(text, "sigma") == 0) {
      infile >> sigma;

    } else if ((strcasecmp(text, "st") == 0) || (strcasecmp(text, "step") == 0)) {
      infile >> step;

    } else if (strcasecmp(text, "gradacc") == 0) {
      infile >> gradacc;

    } else if (strcasecmp(text, "gradstep") == 0) {
      infile >> gradstep;

    } else if ((strcasecmp(text, "maxiterations") == 0) || (strcasecmp(text, "maxiter") == 0) || (strcasecmp(text, "bfgsiter") == 0)) {
      infile >> bfgsiter;

    } else if ((strcasecmp(text, "eps") == 0) || (strcasecmp(text, "bfgseps") == 0)) {
      infile >> bfgseps;

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
}
