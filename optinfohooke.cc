#include "optinfo.h"
#include "gadget.h"

extern ErrorHandler handle;
extern Ecosystem* EcoSystem;

double h(double* x, int n) {
  return EcoSystem->SimulateAndUpdate(x, n);
}

extern int hooke(double (*func)(double*, int), int n, double startingpoint[],
  double endpoint[], double lowerb[], double upperb[], double rho,
  double lambda, double epsilon, int itermax, double init[], double bndcheck);

OptInfoHooke::OptInfoHooke()
  : OptSearch(), hookeiter(1000), rho(0.5), lambda(0), hookeeps(1e-4), bndcheck(0.9999) {
  handle.logMessage("Initialising Hooke & Jeeves");
}

OptInfoHooke::~OptInfoHooke() {
}

void OptInfoHooke::Read(CommentStream& infile, char* text) {
  while (!infile.eof() && strcasecmp(text, "seed") && strcasecmp(text, "[simann]") && strcasecmp(text, "[bfgs]")) {
    if (strcasecmp(text, "rho") == 0) {
      infile >> rho >> ws >> text >> ws;

    } else if (strcasecmp(text, "lambda") == 0) {
      infile >> lambda >> ws >> text >> ws;

    } else if (strcasecmp(text, "hookeeps") == 0) {
      infile >> hookeeps >> ws >> text >> ws;

    } else if (strcasecmp(text, "hookeiter") == 0) {
      infile >> hookeiter >> ws >> text >> ws;

    } else if (strcasecmp(text, "bndcheck") == 0) {
      infile >> bndcheck >> ws >> text >> ws;

    } else {
      handle.logWarning("Warning in optinfofile - unknown option", text);
      infile >> text >> ws >> text >> ws;
    } 
  }
}
  
void OptInfoHooke::MaximizeLikelihood() {
  int i, nopt, count;
  double tmp;
  nopt = EcoSystem->NoOptVariables();
  DoubleVector val(nopt);
  DoubleVector lbds(nopt);
  DoubleVector ubds(nopt);
  DoubleVector initialval(nopt);
  
  EcoSystem->ScaleVariables();
  EcoSystem->ScaledOptValues(val);
  EcoSystem->LowerBds(lbds);
  EcoSystem->UpperBds(ubds);
  EcoSystem->InitialOptValues(initialval);
  
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

  ParameterVector optswitches(nopt);
  EcoSystem->OptSwitches(optswitches);

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
      handle.logWarning("Warning in optinfo - initial value is zero for switch", optswitches[i].getValue());
  }

  count = hooke(&h, nopt, startpoint, endpoint, upperb, lowerb,
    rho, lambda, hookeeps, hookeiter, init, bndcheck);

  cout << "\nHooke & Jeeves finished with a final likelihood score of " << EcoSystem->getLikelihood()
    << "\nafter " << EcoSystem->getFuncEval() << " function evaluations at the point\n";
  EcoSystem->writeOptValues();

  delete[] endpoint;
  delete[] startpoint;
  delete[] upperb;
  delete[] lowerb;
  delete[] init;
}
