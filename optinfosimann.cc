#include "optinfo.h"
#include "gadget.h"

extern ErrorHandler handle;
extern Ecosystem* EcoSystem;

double f(double* x, int n) {
  return EcoSystem->SimulateAndUpdate(x, n);
}

extern int simann(int nvar, double point[], double endpoint[], double lb[],
  double ub[], double (*func)(double*, int), int m, int maxeval, double cstep,
  double tempt, double vmlen, double rt, int ns, int nt, double eps,
  double uratio, double lratio, int check);

OptInfoSimann::OptInfoSimann()
  : OptSearch(), rt(0.85), simanneps(1e-4), ns(5), nt(2), T(100),
    cs(2), vm(1), simanniter(2000), uratio(0.7), lratio(0.3), check(4) {
  handle.logMessage("Initialising Simulated Annealing");
}

OptInfoSimann::~OptInfoSimann() {
}

void OptInfoSimann::Read(CommentStream& infile, char* text) {
  while (!infile.eof() && strcasecmp(text, "seed") && strcasecmp(text, "[hooke]") && strcasecmp(text, "[bfgs]")) {
    if (strcasecmp(text, "simanniter") == 0) {
      infile >> simanniter >> ws >> text >> ws;

    } else if (strcasecmp(text, "t") == 0) {
      infile >> T >> ws >> text >> ws;

    } else if (strcasecmp(text, "rt") == 0) {
      infile >> rt >> ws >> text >> ws;

    } else if (strcasecmp(text, "simanneps") == 0) {
      infile >> simanneps >> ws >> text >> ws;

    } else if (strcasecmp(text, "nt") == 0) {
      infile >> nt >> ws >> text >> ws;

    } else if (strcasecmp(text, "ns") == 0) {
      infile >> ns >> ws >> text >> ws;

    } else if (strcasecmp(text, "vm") == 0) {
      infile >> vm >> ws >> text >> ws;

    } else if (strcasecmp(text, "cstep") == 0) {
      infile >> cs >> ws >> text >> ws;

    } else if (strcasecmp(text, "check") == 0) {
      infile >> check >> ws >> text >> ws;

    } else if (strcasecmp(text, "uratio") == 0) {
      infile >> uratio >> ws >> text >> ws;

    } else if (strcasecmp(text, "lratio") == 0) {
      infile >> lratio >> ws >> text >> ws;

    } else {
      handle.logWarning("Warning in optinfofile - unknown option", text);
      infile >> text >> ws >> text >> ws;
    }
  }
}

//Considered better to skip scaling of variables here.  Had to change keeper
//so initialvalues start as 1 but scaled values as the same as values
void OptInfoSimann::MaximizeLikelihood() {
  int i, nopt, count;
  nopt = EcoSystem->NoOptVariables();
  DoubleVector val(nopt);
  DoubleVector lbds(nopt);
  DoubleVector ubds(nopt);

  EcoSystem->ScaledOptValues(val);
  EcoSystem->LowerBds(lbds);
  EcoSystem->UpperBds(ubds);

  double* startpoint = new double[nopt];
  double* endpoint = new double[nopt];
  double* lowerb = new double[nopt];
  double* upperb = new double[nopt];

  for (i = 0; i < nopt; i++) {
    lowerb[i] = lbds[i];
    upperb[i] = ubds[i];
    startpoint[i] = val[i];
  }

  ParameterVector optswitches(nopt);
  EcoSystem->OptSwitches(optswitches);

  count = simann(nopt, startpoint, endpoint, lowerb, upperb, &f, 0,
    simanniter, cs, T, vm, rt, ns, nt, simanneps, uratio, lratio, check);

  cout << "\nSimulated Annealing finished with a final likelihood score of " << EcoSystem->getLikelihood()
    << "\nafter " << EcoSystem->getFuncEval() << " function evaluations at the point\n";
  EcoSystem->writeOptValues();

  delete[] startpoint;
  delete[] endpoint;
  delete[] lowerb;
  delete[] upperb;
}
