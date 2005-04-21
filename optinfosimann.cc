#include "optinfo.h"
#include "ecosystem.h"
#include "gadget.h"

extern ErrorHandler handle;
extern Ecosystem* EcoSystem;

double fsa(double* x) {
  return EcoSystem->SimulateAndUpdate(x);
}

extern int simann(int nvar, double point[], double endpoint[], double lb[],
  double ub[], double (*fsa)(double*), int m, int maxeval, double cstep,
  double tempt, double vmlen, double rt, int ns, int nt, double eps,
  double uratio, double lratio, int check);

OptInfoSimann::OptInfoSimann()
  : OptSearch(), rt(0.85), simanneps(1e-4), ns(5), nt(2), t(100.0),
    cs(2.0), vm(1.0), simanniter(2000), uratio(0.7), lratio(0.3), check(4) {
  handle.logMessage("Initialising Simulated Annealing optimisation algorithm");
}

void OptInfoSimann::read(CommentStream& infile, char* text) {
  while (!infile.eof() && strcasecmp(text, "seed") && strcasecmp(text, "[hooke]") && strcasecmp(text, "[bfgs]")) {
    infile >> ws;
    if (strcasecmp(text, "simanniter") == 0) {
      infile >> simanniter;

    } else if (strcasecmp(text, "t") == 0) {
      infile >> t;

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
      handle.logWarning("Warning in optinfofile - unrecognised option", text);
      infile >> text;  //read and ignore the next entry
    }
    infile >> text;
  }

  //check the values specified in the optinfo file ...
  if ((uratio < 0.5) || (uratio > 1)) {
    handle.logWarning("Warning in optinfofile - value of uratio outside bounds", uratio);
    uratio = 0.7;
  }
  if ((lratio < 0) || (lratio > 0.5) || isZero(lratio)) {
    handle.logWarning("Warning in optinfofile - value of lratio outside bounds", lratio);
    lratio = 0.3;
  }
  if ((rt < 0) || (rt > 1) || isZero(rt)) {
    handle.logWarning("Warning in optinfofile - value of rt outside bounds", rt);
    rt = 0.85;
  }
  if ((t < 0) || isZero(t)) {
    handle.logWarning("Warning in optinfofile - value of t outside bounds", t);
    t = 100.0;
  }
  if (nt < 1) {
    handle.logWarning("Warning in optinfofile - value of nt outside bounds", nt);
    nt = 2;
  }
  if (ns < 1) {
    handle.logWarning("Warning in optinfofile - value of ns outside bounds", ns);
    ns = 5;
  }
  if ((isZero(vm)) || (vm < 0)) {
    handle.logWarning("Warning in optinfofile - value of vm outside bounds", vm);
    vm = 1.0;
  }
  if ((isZero(cs)) || (cs < 0)) {
    handle.logWarning("Warning in optinfofile - value of cstep outside bounds", cs);
    cs = 2.0;
  }
  if ((isZero(simanneps)) || (simanneps < 0)) {
    handle.logWarning("Warning in optinfofile - value of simanneps outside bounds", simanneps);
    simanneps = 1e-4;
  }
}

//Considered better to skip scaling of variables here.  Had to change keeper
//so initialvalues start as 1 but scaled values as the same as values
void OptInfoSimann::OptimiseLikelihood() {
  int i, nopt, opt;

  handle.logInformation("\nStarting Simulated Annealing optimisation algorithm");

  nopt = EcoSystem->numOptVariables();
  DoubleVector val(nopt);
  DoubleVector lbds(nopt);
  DoubleVector ubds(nopt);

  EcoSystem->ScaledOptValues(val);
  EcoSystem->LowerOptBds(lbds);
  EcoSystem->UpperOptBds(ubds);

  double* startpoint = new double[nopt];
  double* endpoint = new double[nopt];
  double* lowerb = new double[nopt];
  double* upperb = new double[nopt];

  for (i = 0; i < nopt; i++) {
    lowerb[i] = lbds[i];
    upperb[i] = ubds[i];
    startpoint[i] = val[i];
  }

  opt = simann(nopt, startpoint, endpoint, lowerb, upperb, &fsa, 0,
    simanniter, cs, t, vm, rt, ns, nt, simanneps, uratio, lratio, check);

  cout << "\nSimulated Annealing finished with a final likelihood score of " << EcoSystem->getLikelihood()
    << "\nafter a total of " << EcoSystem->getFuncEval() << " function evaluations at the point\n";
  EcoSystem->writeOptValues();

  delete[] startpoint;
  delete[] endpoint;
  delete[] lowerb;
  delete[] upperb;
}
