#include "optinfo.h"
#include "mathfunc.h"
#include "gadget.h"

/* global ecosystem used to store whether the model converged */
extern Ecosystem* EcoSystem;

/* global variable, defined and initialised in gadget.cc and not modified here */
extern int FuncEval;

extern ErrorHandler handle;

double func(double* x, int n) {
  return EcoSystem->SimulateAndUpdate(x, n);
}


OptInfoBfgs::OptInfoBfgs() 
  : OptSearch(), maxiter(100000), eps(0.001), rho(0.01), tau(0.5){
  handle.logMessage("Initialising BFGS");
  numvar = EcoSystem->NoOptVariables();
  int i;
  x = new double[numvar];
  s = new double[numvar];
  gk = new double[numvar];
  g0 = new double[numvar];
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
  delete[] g0;
  for (i = 0; i < numvar; i++)
    delete[] Bk[i];
}

void OptInfoBfgs::MaximizeLikelihood() {
  int i, count;
  DoubleVector val(numvar);
  double* startpoint = new double[numvar];
  EcoSystem->ScaledOptValues(val);
  for (i = 0; i < numvar; i++)
    startpoint[i] = val[i];

  count = iteration(startpoint);
  cout << "\nBFGS finished with final likelihood score of " << EcoSystem->getLikelihood()
    << "\nafter " << EcoSystem->getFuncEval() << " function evaluations at the point\n";
  EcoSystem->writeOptValues();
  delete[] startpoint;
}

void OptInfoBfgs::Read(CommentStream& infile, char* text) {
  while (!infile.eof() && strcasecmp(text, "seed") && strcasecmp(text, "[simann]") && strcasecmp(text, "[hooke]")) {
    if (strcasecmp(text,"rho") == 0) {
      infile >> rho >> ws >> text >> ws;

    } else if (strcasecmp(text, "tau") == 0) {
      infile >> tau >> ws >> text >> ws;

    } else if (strcasecmp(text, "maxiter") == 0) {
      infile >> maxiter >> ws >> text >> ws;

    } else if (strcasecmp(text, "eps") == 0) {
      infile >> eps >> ws >> text >> ws;

    } else {
      handle.logWarning("Warning in optinfofile - unknown option", text);
      infile >> text >> ws >> text >> ws;
    }
  }
}

