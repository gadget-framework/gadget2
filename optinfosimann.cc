#include "optinfo.h"
#include "ecosystem.h"
#include "gadget.h"

extern ErrorHandler handle;
extern Ecosystem* EcoSystem;

double s(double* x, int n) {
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

void OptInfoSimann::Read(CommentStream& infile, char* text) {
  while (!infile.eof() && strcasecmp(text, "seed") && strcasecmp(text, "[hooke]") && strcasecmp(text, "[bfgs]")) {
    infile >> ws;
    if (strcasecmp(text, "simanniter") == 0) {
      infile >> simanniter;

    } else if (strcasecmp(text, "t") == 0) {
      infile >> T;

    } else if (strcasecmp(text, "rt") == 0) {
      infile >> rt;

    } else if (strcasecmp(text, "simanneps") == 0) {
      infile >> simanneps;

    } else if (strcasecmp(text, "nt") == 0) {
      infile >> nt;

    } else if (strcasecmp(text, "ns") == 0) {
      infile >> ns;

    } else if (strcasecmp(text, "vm") == 0) {
      infile >> vm;

    } else if (strcasecmp(text, "cstep") == 0) {
      infile >> cs;

    } else if (strcasecmp(text, "check") == 0) {
      infile >> check;

    } else if (strcasecmp(text, "uratio") == 0) {
      infile >> uratio;

    } else if (strcasecmp(text, "lratio") == 0) {
      infile >> lratio;

    } else {
      handle.logWarning("Warning in optinfofile - unknown option", text);
      infile >> text;  //read and ignore the next entry
    }
    infile >> text;
  }

  //check the values specified in the optinfo file ...
  if ((uratio < 0.5) || (uratio > 1)) {
    handle.logWarning("Warning in optinfofile - value of uratio outside bounds", uratio);
    uratio = 0.7;
  }
  if ((lratio < 0) || (lratio > 0.5)) {
    handle.logWarning("Warning in optinfofile - value of lratio outside bounds", lratio);
    lratio = 0.3;
  }
  if ((rt < 0) || (rt > 1)) {
    handle.logWarning("Warning in optinfofile - value of rt outside bounds", rt);
    rt = 0.85;
  }
}

//Considered better to skip scaling of variables here.  Had to change keeper
//so initialvalues start as 1 but scaled values as the same as values
void OptInfoSimann::MaximizeLikelihood() {
  int i, nopt, opt;

  handle.logInformation("\nStarting Simulated Annealing");

  nopt = EcoSystem->numOptVariables();
  DoubleVector val(nopt);
  DoubleVector lbds(nopt);
  DoubleVector ubds(nopt);
  ParameterVector optswitches(nopt);

  EcoSystem->ScaledOptValues(val);
  EcoSystem->LowerBds(lbds);
  EcoSystem->UpperBds(ubds);
  EcoSystem->OptSwitches(optswitches);

  double* startpoint = new double[nopt];
  double* endpoint = new double[nopt];
  double* lowerb = new double[nopt];
  double* upperb = new double[nopt];

  for (i = 0; i < nopt; i++) {
    lowerb[i] = lbds[i];
    upperb[i] = ubds[i];
    startpoint[i] = val[i];
  }

  opt = simann(nopt, startpoint, endpoint, lowerb, upperb, &s, 0,
    simanniter, cs, T, vm, rt, ns, nt, simanneps, uratio, lratio, check);

  cout << "\nSimulated Annealing finished with a final likelihood score of " << EcoSystem->getLikelihood()
    << "\nafter " << EcoSystem->getFuncEval() << " function evaluations at the point\n";
  EcoSystem->writeOptValues();

  delete[] startpoint;
  delete[] endpoint;
  delete[] lowerb;
  delete[] upperb;
}
