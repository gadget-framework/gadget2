#include "optinfo.h"
#include "gadget.h"

extern Ecosystem* EcoSystem;

double f(double* x, int n) {
  return EcoSystem->SimulateAndUpdate(x, n);
}

extern int hooke(double (*f)(double*, int), int n, double startingpoint[],
  double endpoint[], double lowerb[], double upperb[],
  double rho, double lambda, double epsilon, int itermax);

extern int simann(int nvar, double point[], double endpoint[], double lb[],
  double ub[], double (*f)(double*, int), int m, int maxeval, double step[],
  double tempt, double stepl[], double rt, int ns, int nt, double eps);

OptInfo::OptInfo() {
  nopt = EcoSystem->NoOptVariables();
  // Initialize random number generator with system time [morten 02.02.26]
  srand(time(NULL));
}

OptInfo::~OptInfo() {
}

int OptInfo::Read(CommentStream &infile, char* text) {
  if (strcasecmp(text, "seed") == 0) {
    // Use seed to initialize random number generator [morten 02.02.26]
    int seed;
    infile >> seed >> ws;
    srand(seed); // Initialize random number generator with seed
    return 1;
  } else if (strcasecmp(text, "") == 0) {
    // JMB - stripping whitespace hasnt worked so passed nothing!
    return 1;
  } else if (strcasecmp(text, "bounds") == 0) {
    // JMB - removed code to read in bounds here
    cerr << "Error in optinfofile - bounds are no longer read here\n"
      << "Specify the bounds with the -i inputfile switch instead\n";
    return 0;
  } else
    return 0;
}

OptInfoHooke::OptInfoHooke()
  : OptInfo(), MaximumIterations(10), Rho_begin(0.5), lambda(0), epsmin(1e-6) {
}

OptInfoHooke::~OptInfoHooke() {
}

void OptInfoHooke::Read(CommentStream& infile) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  infile >> ws;
  while (!infile.eof()) {
    infile >> text >> ws;

    if (!Read(infile, text))
      cerr << "Warning in optinfo hooke - unknown option " << text << endl;
  }
}

int OptInfoHooke::Read(CommentStream& infile, char* text) {
  if (OptInfo::Read(infile, text))
    return 1;

  if (strcasecmp(text, "rho") == 0) {
    infile >> Rho_begin >> ws;
    return 1;
  } else if (strcasecmp(text, "lambda") == 0) {
    infile >> lambda >> ws;
    return 1;
  } else if (strcasecmp(text, "hookeeps") == 0) {
    infile >> epsmin >> ws;
    return 1;
  } else if (strcasecmp(text, "hookeiter") == 0) {
    infile >> MaximumIterations >> ws;
    return 1;
  } else
    return 0;
}

void OptInfoHooke::MaximizeLikelihood() {
  int i, count;
  double tmp;
  nopt = EcoSystem->NoOptVariables();
  doublevector val(nopt);
  doublevector lbds(nopt);
  doublevector ubds(nopt);
  doublevector initialval(nopt);

  EcoSystem->ScaleVariables();
  EcoSystem->ScaledOptValues(val);
  EcoSystem->LowerBds(lbds);
  EcoSystem->UpperBds(ubds);
  EcoSystem->InitialOptValues(initialval);

  double* startpoint = new double[nopt];
  double* endpoint = new double[nopt];
  double* upperb = new double[nopt];
  double* lowerb = new double[nopt];

  for (i = 0; i < nopt; i++) {
    startpoint[i] = val[i];
    lowerb[i] = lbds[i];
    upperb[i] = ubds[i];
  }

  Parametervector optswitches(nopt);
  EcoSystem->OptSwitches(optswitches);

  /* Scaling the bounds, because tha parameters are scaled. */
  for (i = 0; i < nopt; i++) {
    lowerb[i] = lowerb[i] / initialval[i];
    upperb[i] = upperb[i] / initialval[i];
    if (lowerb[i] > upperb[i]) {
      /* If bounds smaller than 0 */
      tmp = lowerb[i];
      lowerb[i] = upperb[i];
      upperb[i] = tmp;
    }
  }

  count = 0;
  for (i = 0; i < nopt; i++) {
    if (iszero(val[i])) {
      count++;
      cerr << "Error: for switch " << optswitches[i] << " starting value is zero\n"
        << "which will give poor convergence for Hooke and Jeeves optimisation\n";
    }
    if ((lowerb[i] > 1) || (upperb[i] < 1)) {
      count++;
      cerr << "Error: for switch " << optswitches[i] << " the initial value is outside the bounds\n";
    }
    if (upperb[i] < lowerb[i]) {
      count++;
      cerr << "Error: for switch " << optswitches[i] << " the upperbound is less than the lowerbound\n";
    }
  }

  if (count > 0) {
    cerr << "Exiting with " << count << " errors in the initial parameter values\n";
    exit(EXIT_FAILURE);
  }

  int FinalValue;
  //JMB - swapped the order of upperb and lowerb to match entries for hooke()
  FinalValue = hooke(&f, nopt, startpoint, endpoint, upperb, lowerb,
    Rho_begin, lambda, epsmin, MaximumIterations);

  for (i = 0; i < nopt; i++)
    val[i] = initialval[i] * endpoint[i];

  cout << "\nOptimization finished with final likelihood score of " << EcoSystem->Likelihood()
    << "\nafter " << EcoSystem->GetFuncEval() << " function evaluations at the point\n";
  EcoSystem->PrintOptValues();

  delete[] endpoint;
  delete[] startpoint;
  delete[] upperb;
  delete[] lowerb;
}

OptInfoSimann::OptInfoSimann()
  : OptInfo(), rt(0.85), eps(1e-4), ns(15), nt(10), T(100),
    cs(2), vm(1), maxim(0), maxevl(2000) {
}

OptInfoSimann::~OptInfoSimann() {
}

void OptInfoSimann::Read(CommentStream& infile) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  infile >> ws;
  while (!infile.eof()) {
    infile >> text >> ws;
    if (!Read(infile, text))
      cerr << "Warning in optinfo simann - unknown option " << text << endl;
  }
}

int OptInfoSimann::Read(CommentStream& infile, char* text) {
  if (OptInfo::Read(infile, text))
    return 1;

  if (strcasecmp(text, "simanniter") == 0) {
    infile >> maxevl >> ws;
    return 1;
  } else if (strcasecmp(text, "T") == 0) {
    infile >> T >> ws;
    return 1;
  } else if (strcasecmp(text, "rt") == 0) {
    infile >> rt >> ws;
    return 1;
  } else if (strcasecmp(text, "simanneps") == 0) {
    infile >> eps >> ws;
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
  } else
    return 0;
}

//Considered better to skip scaling of variables here.  Had to change keeper
//so initialvalues start as 1 but scaled values as the same as values
void OptInfoSimann::MaximizeLikelihood() {
  int i, count;
  nopt = EcoSystem->NoOptVariables();
  doublevector val(nopt);
  doublevector lbds(nopt);
  doublevector ubds(nopt);

  EcoSystem->ScaledOptValues(val);
  EcoSystem->LowerBds(lbds);
  EcoSystem->UpperBds(ubds);

  double* startpoint = new double[nopt];
  double* endpoint = new double[nopt];
  double* lowerb = new double[nopt];
  double* upperb = new double[nopt];
  double* cstep = new double[nopt];
  double* vmstep = new double[nopt];

  for (i = 0; i < nopt; i++) {
    cstep[i] = cs;
    vmstep[i] = vm;
    lowerb[i] = lbds[i];
    upperb[i] = ubds[i];
    startpoint[i] = val[i];
  }

  Parametervector optswitches(nopt);
  EcoSystem->OptSwitches(optswitches);
  count = 0;
  for (i = 0; i < nopt; i++) {
    if (lowerb[i] > val[i]) {
      count++;
      cerr << "Error: for switch " << optswitches[i] << " lowerbound " << lowerb[i]
        << " is greater than the starting value " << val[i] << endl;
    }
    if (upperb[i] < val[i]) {
      count++;
      cerr << "Error: for switch " << optswitches[i] << " upperbound " << upperb[i]
        << " is less that the starting value " << val[i] << endl;
    }
    if (upperb[i] < lowerb[i]) {
      count++;
      cerr << "Error: for switch " << optswitches[i] << " upperbound " << upperb[i]
        << " is less than the lowerbound " << lowerb[i] << endl;
    }
  }

  if (count > 0) {
    cerr << "Exiting with " << count << " errors in the initial parameter values\n";
    exit(EXIT_FAILURE);
  }

  int Finalvalue;
  Finalvalue = simann(nopt, startpoint, endpoint, lowerb, upperb, &f, maxim,
    maxevl, cstep, T, vmstep, rt, ns, nt, eps);

  cout << "\nOptimization finished with final likelihood score of " << EcoSystem->Likelihood()
    << "\nafter " << EcoSystem->GetFuncEval() << " function evaluations at the point\n";
  EcoSystem->PrintOptValues();

  delete[] startpoint;
  delete[] endpoint;
  delete[] vmstep;
  delete[] cstep;
  delete[] lowerb;
  delete[] upperb;
}

OptInfoHookeAndSimann::OptInfoHookeAndSimann()
  : OptInfo(), HookeMaxIter(10), Rho_begin(0.5), lambda(0), epsmin(1e-6),
  SimannMaxIter(2000), rt(0.85), eps(1e-4),
  ns(15), nt(10), T(100), cs(2), vm(1), maxim(0) {
}

OptInfoHookeAndSimann::~OptInfoHookeAndSimann() {
}

void OptInfoHookeAndSimann::Read(CommentStream& infile) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  infile >> ws;
  while (!infile.eof()) {
    infile >> text >> ws;
    if (!Read(infile, text))
      cerr << "Warning in optinfo simannandhooke - unknown option " << text << endl;
  }
}

int OptInfoHookeAndSimann::Read(CommentStream& infile, char* text) {
  if (OptInfo::Read(infile, text))
    return 1;

  if (strcasecmp(text, "rho") == 0) {
    infile >> Rho_begin >> ws;
    return 1;
  } else if (strcasecmp(text, "lambda") == 0) {
    infile >> lambda >> ws;
    return 1;
  } else if (strcasecmp(text, "hookeeps") == 0) {
    infile >> epsmin >> ws;
    return 1;
  } else if (strcasecmp(text, "hookeiter") == 0) {
    infile >> HookeMaxIter >> ws;
    return 1;
  } else if (strcasecmp(text, "simanniter") == 0) {
    infile >> SimannMaxIter >> ws;
    return 1;
  } else if (strcasecmp(text, "T") == 0) {
    infile >> T >> ws;
    return 1;
  } else if (strcasecmp(text, "rt") == 0) {
    infile >> rt >> ws;
    return 1;
  } else if (strcasecmp(text, "simanneps") == 0) {
    infile >> eps >> ws;
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
  } else
    return 0;
}

void OptInfoHookeAndSimann::MaximizeLikelihood() {
  int i, count;
  nopt = EcoSystem->NoOptVariables();
  doublevector val(nopt);
  doublevector initialval(nopt);
  doublevector lbds(nopt);
  doublevector ubds(nopt);

  EcoSystem->ScaledOptValues(val);
  EcoSystem->LowerBds(lbds);
  EcoSystem->UpperBds(ubds);

  double* startpoint = new double[nopt];
  double* endpoint = new double[nopt];
  double* lowerb = new double[nopt];
  double* upperb = new double[nopt];
  double* cstep = new double[nopt];
  double* vmstep = new double[nopt];

  for (i = 0; i < nopt; i++) {
    cstep[i] = cs;
    vmstep[i] = vm;
    lowerb[i] = lbds[i];
    upperb[i] = ubds[i];
    startpoint[i] = val[i];
  }

  Parametervector optswitches(nopt);
  EcoSystem->OptSwitches(optswitches);
  count = 0;
  for (i = 0; i < nopt; i++) {
    if (lowerb[i] > val[i]) {
      count++;
      cerr << "Error: for switch " << optswitches[i] << " lowerbound " << lowerb[i]
        << " is greater than the starting value " << val[i] << endl;
    }
    if (upperb[i] < val[i]) {
      count++;
      cerr << "Error: for switch " << optswitches[i] << " upperbound " << upperb[i]
        << " is less that the starting value " << val[i] << endl;
    }
    if (upperb[i] < lowerb[i]) {
      count++;
      cerr << "Error: for switch " << optswitches[i] << " upperbound " << upperb[i]
        << " is less than the lowerbound " << lowerb[i] << endl;
    }
  }

  if (count > 0) {
    cerr << "Exiting with " << count << " errors in the initial parameter values\n";
    exit(EXIT_FAILURE);
  }

  int Finalvalue;
  Finalvalue = simann(nopt, startpoint, endpoint, lowerb, upperb, &f, maxim,
    SimannMaxIter, cstep, T, vmstep, rt, ns, nt, eps);

  for (i = 0; i < nopt; i++)
    val[i] = endpoint[i];

  double tmp;
  EcoSystem->ScaleVariables();
  EcoSystem->ScaledOptValues(val);
  EcoSystem->InitialOptValues(initialval);
  for (i = 0; i < nopt; i++)
    startpoint[i] = val[i];

  /* Scaling the bounds, because tha parameters are now scaled. */
  for (i = 0; i < nopt; i++) {
    lowerb[i] = lowerb[i] / initialval[i];
    upperb[i] = upperb[i] / initialval[i];
    if (lowerb[i] > upperb[i]) {
      /* If bounds smaller than 0 */
      tmp = lowerb[i];
      lowerb[i] = upperb[i];
      upperb[i] = tmp;
    }
  }

  for (i = 0; i < nopt; i++) {
    if (iszero(val[i])) {
      count++;
      cerr << "Error: for switch " << optswitches[i] << " starting value is zero\n"
        << "which will give poor convergence for Hooke and Jeeves optimisation\n";
    }
  }

  if (count > 0) {
    cerr << "Exiting with " << count << " errors in the parameter values\n";
    exit(EXIT_FAILURE);
  }

  Finalvalue += hooke(&f, nopt, startpoint, endpoint, upperb, lowerb,
    Rho_begin, lambda, epsmin, HookeMaxIter);

  for (i = 0; i < nopt; i++)
    val[i] = initialval[i] * endpoint[i];

  cout << "\nOptimization finished with final likelihood score of " << EcoSystem->Likelihood()
    << "\nafter " << EcoSystem->GetFuncEval() << " function evaluations at the point\n";
  EcoSystem->PrintOptValues();

  delete[] startpoint;
  delete[] endpoint;
  delete[] vmstep;
  delete[] cstep;
  delete[] lowerb;
  delete[] upperb;
}
