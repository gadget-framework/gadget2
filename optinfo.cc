#include "optinfo.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

/* JMB - removed all the Numerical Recipes BFGS stuff */

extern Ecosystem* EcoSystem;

double f(double* x, int n) {
  return EcoSystem->SimulateAndUpdate(x, n);
}

extern int hooke(double (*f)(double*, int), int n, double startingpoint[],
  double endpoint[], double lowerb[], double upperb[], double rho,
  double lambda, double epsilon, int itermax, double init[], double bndcheck);

extern int simann(int nvar, double point[], double endpoint[], double lb[],
  double ub[], double (*f)(double*, int), int m, int maxeval, double cstep,
  double tempt, double vmlen, double rt, int ns, int nt, double eps,
  double uratio, double lratio, int check);

OptInfo::OptInfo() {
  // Initialise random number generator with system time [morten 02.02.26]
  srand(time(NULL));
}

OptInfo::~OptInfo() {
}

int OptInfo::read(CommentStream &infile, char* text) {
  if (strcasecmp(text, "seed") == 0) {
    // Use seed to initialise random number generator [morten 02.02.26]
    int seed;
    infile >> seed >> ws;
    srand(seed);
    return 1;
  } else if (strcasecmp(text, "") == 0) {
    // JMB - stripping whitespace hasnt worked so passed nothing!
    return 1;
  } else if (strcasecmp(text, "bounds") == 0) {
    // JMB - removed code to read in bounds here
    handle.logWarning("Error in optinfo - bounds should not be specified here");
    return 0;
  } else
    return 0;
}

OptInfoHooke::OptInfoHooke()
  : OptInfo(), hookeiter(1000), rho(0.5), lambda(0), hookeeps(1e-4), bndcheck(0.9999) {
}

OptInfoHooke::~OptInfoHooke() {
}

void OptInfoHooke::read(CommentStream& infile) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  infile >> ws;
  while (!infile.eof()) {
    infile >> text >> ws;

    if (!read(infile, text))
      handle.logWarning("Error in optinfo - unknown option", text);
  }
}

int OptInfoHooke::read(CommentStream& infile, char* text) {
  if (OptInfo::read(infile, text))
    return 1;

  if (strcasecmp(text, "rho") == 0) {
    infile >> rho >> ws;
    return 1;
  } else if (strcasecmp(text, "lambda") == 0) {
    infile >> lambda >> ws;
    return 1;
  } else if (strcasecmp(text, "hookeeps") == 0) {
    infile >> hookeeps >> ws;
    return 1;
  } else if (strcasecmp(text, "hookeiter") == 0) {
    infile >> hookeiter >> ws;
    return 1;
  } else if (strcasecmp(text, "bndcheck") == 0) {
    infile >> bndcheck >> ws;
    return 1;
  } else
    return 0;
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

  count = hooke(&f, nopt, startpoint, endpoint, upperb, lowerb,
    rho, lambda, hookeeps, hookeiter, init, bndcheck);

  cout << "\nOptimisation finished with final likelihood score of " << EcoSystem->getLikelihood()
    << "\nafter " << EcoSystem->getFuncEval() << " function evaluations at the point\n";
  EcoSystem->writeOptValues();

  delete[] endpoint;
  delete[] startpoint;
  delete[] upperb;
  delete[] lowerb;
  delete[] init;
}

OptInfoSimann::OptInfoSimann()
  : OptInfo(), rt(0.85), simanneps(1e-4), ns(15), nt(10), T(100),
    cs(2), vm(1), simanniter(2000), uratio(0.7), lratio(0.3), check(4) {
}

OptInfoSimann::~OptInfoSimann() {
}

void OptInfoSimann::read(CommentStream& infile) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  infile >> ws;
  while (!infile.eof()) {
    infile >> text >> ws;
    if (!read(infile, text))
      handle.logWarning("Error in optinfo - unknown option", text);
  }
}

int OptInfoSimann::read(CommentStream& infile, char* text) {
  if (OptInfo::read(infile, text))
    return 1;

  if (strcasecmp(text, "simanniter") == 0) {
    infile >> simanniter >> ws;
    return 1;
  } else if (strcasecmp(text, "t") == 0) {
    infile >> T >> ws;
    return 1;
  } else if (strcasecmp(text, "rt") == 0) {
    infile >> rt >> ws;
    return 1;
  } else if (strcasecmp(text, "simanneps") == 0) {
    infile >> simanneps >> ws;
    return 1;
  } else if (strcasecmp(text, "nt") == 0) {
    infile >> nt >> ws;
    return 1;
  } else if (strcasecmp(text, "ns") == 0) {
    infile >> ns >> ws;
    return 1;
  } else if (strcasecmp(text, "vm") == 0) {
    infile >> vm >> ws;
    return 1;
  } else if (strcasecmp(text, "cstep") == 0) {
    infile >> cs >> ws;
    return 1;
  } else if (strcasecmp(text, "check") == 0) {
    infile >> check >> ws;
    return 1;
  } else if (strcasecmp(text, "uratio") == 0) {
    infile >> uratio >> ws;
    return 1;
  } else if (strcasecmp(text, "lratio") == 0) {
    infile >> lratio >> ws;
    return 1;
  } else
    return 0;
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

  cout << "\nOptimisation finished with final likelihood score of " << EcoSystem->getLikelihood()
    << "\nafter " << EcoSystem->getFuncEval() << " function evaluations at the point\n";
  EcoSystem->writeOptValues();

  delete[] startpoint;
  delete[] endpoint;
  delete[] lowerb;
  delete[] upperb;
}

OptInfoHookeAndSimann::OptInfoHookeAndSimann()
  : OptInfo(), hookeiter(1000), rho(0.5), lambda(0), hookeeps(1e-4), bndcheck(0.9999),
    simanniter(2000), rt(0.85), simanneps(1e-4), ns(15), nt(10), T(100),
    cs(2), vm(1), uratio(0.7), lratio(0.3), check(4) {
}

OptInfoHookeAndSimann::~OptInfoHookeAndSimann() {
}

void OptInfoHookeAndSimann::read(CommentStream& infile) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  infile >> ws;
  while (!infile.eof()) {
    infile >> text >> ws;
    if (!read(infile, text))
      handle.logWarning("Error in optinfo - unknown option", text);
  }
}

int OptInfoHookeAndSimann::read(CommentStream& infile, char* text) {
  if (OptInfo::read(infile, text))
    return 1;

  if (strcasecmp(text, "rho") == 0) {
    infile >> rho >> ws;
    return 1;
  } else if (strcasecmp(text, "lambda") == 0) {
    infile >> lambda >> ws;
    return 1;
  } else if (strcasecmp(text, "hookeeps") == 0) {
    infile >> hookeeps >> ws;
    return 1;
  } else if (strcasecmp(text, "hookeiter") == 0) {
    infile >> hookeiter >> ws;
    return 1;
  } else if (strcasecmp(text, "bndcheck") == 0) {
    infile >> hookeiter >> ws;
    return 1;
  } else if (strcasecmp(text, "simanniter") == 0) {
    infile >> simanniter >> ws;
    return 1;
  } else if (strcasecmp(text, "t") == 0) {
    infile >> T >> ws;
    return 1;
  } else if (strcasecmp(text, "rt") == 0) {
    infile >> rt >> ws;
    return 1;
  } else if (strcasecmp(text, "simanneps") == 0) {
    infile >> simanneps >> ws;
    return 1;
  } else if (strcasecmp(text, "nt") == 0) {
    infile >> nt >> ws;
    return 1;
  } else if (strcasecmp(text, "ns") == 0) {
    infile >> ns >> ws;
    return 1;
  } else if (strcasecmp(text, "vm") == 0) {
    infile >> vm >> ws;
    return 1;
  } else if (strcasecmp(text, "cstep") == 0) {
    infile >> cs >> ws;
    return 1;
  } else if (strcasecmp(text, "check") == 0) {
    infile >> check >> ws;
    return 1;
  } else if (strcasecmp(text, "uratio") == 0) {
    infile >> uratio >> ws;
    return 1;
  } else if (strcasecmp(text, "lratio") == 0) {
    infile >> lratio >> ws;
    return 1;
  } else
    return 0;
}

void OptInfoHookeAndSimann::MaximizeLikelihood() {
  int i, nopt, count;
  nopt = EcoSystem->NoOptVariables();
  DoubleVector val(nopt);
  DoubleVector initialval(nopt);
  DoubleVector lbds(nopt);
  DoubleVector ubds(nopt);

  EcoSystem->ScaledOptValues(val);
  EcoSystem->LowerBds(lbds);
  EcoSystem->UpperBds(ubds);

  double* startpoint = new double[nopt];
  double* endpoint = new double[nopt];
  double* lowerb = new double[nopt];
  double* upperb = new double[nopt];
  double* init = new double[nopt];

  for (i = 0; i < nopt; i++) {
    lowerb[i] = lbds[i];
    upperb[i] = ubds[i];
    startpoint[i] = val[i];
  }

  ParameterVector optswitches(nopt);
  EcoSystem->OptSwitches(optswitches);

  count = simann(nopt, startpoint, endpoint, lowerb, upperb, &f, 0,
    simanniter, cs, T, vm, rt, ns, nt, simanneps, uratio, lratio, check);

  double tmp;
  EcoSystem->ScaleVariables();
  EcoSystem->ScaledOptValues(val);
  EcoSystem->InitialOptValues(initialval);

  /* Scaling the bounds, because the parameters are now scaled. */
  for (i = 0; i < nopt; i++) {
    init[i] = initialval[i];
    startpoint[i] = val[i];
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

  /* Reset the converge flag for the hooke optimisation */
  EcoSystem->setConverge(0);

  count = hooke(&f, nopt, startpoint, endpoint, upperb, lowerb,
    rho, lambda, hookeeps, hookeiter, init, bndcheck);

  cout << "\nOptimisation finished with final likelihood score of " << EcoSystem->getLikelihood()
    << "\nafter " << EcoSystem->getFuncEval() << " function evaluations at the point\n";
  EcoSystem->writeOptValues();

  delete[] startpoint;
  delete[] endpoint;
  delete[] lowerb;
  delete[] upperb;
  delete[] init;
}
