#include "optinfo.h"
#include "gadget.h"

extern ErrorHandler handle;

/*
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

*/
OptInfo::OptInfo(MainInfo* MainInfo) {
  
  useSimann = 0;
  useHJ     = 0;
  useBFGS   = 0;
  optSimann = NULL;
  optHJ     = NULL;
  optBFGS   = NULL;
  // Initialise random number generator with system time [morten 02.02.26]
  srand(time(NULL));
  if (MainInfo->getOptInfoGiven()){      
    ReadOptInfo(MainInfo->optInfoFile());
    MainInfo->closeOptInfoFile();

  } else {
    handle.logMessage("No OptInfofile given - using default infomation");
    useHJ = 1;
    optHJ = new OptInfoHooke();
  } 
}

OptInfo::~OptInfo() {
  if (optSimann != NULL)
    delete optSimann;
  if (optHJ != NULL)
    delete optHJ;
  if (optBFGS != NULL)
    delete optBFGS;
}

void OptInfo::ReadOptInfo(CommentStream& infile) {
  char* text = new char[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  handle.logMessage("\nReading OptInfofile");
  infile >> text >> ws;
  do {
      if ((strcasecmp(text, "seed")) == 0 && (!infile.eof())) {
	handle.logMessage("Reading seed from OptInfofile");
	infile >> seed >> ws >> text >> ws;
	srand(seed);	

    } else if (strcasecmp(text, "[simann]") == 0) {
      handle.logMessage("Reading simulated annealing parameters");
      optSimann = new OptInfoSimann();
      if (!infile.eof()) {
	infile >> text >> ws;
	optSimann->Read(infile, text);
      } else 
	handle.logWarning("No opt parameters for Simulated Annealing - using default values");
      useSimann = 1;
      
    } else if (strcasecmp(text, "[hooke]") == 0) {
      handle.logMessage("Reading Hooke & Jeeves parameters");
      optHJ = new OptInfoHooke();
      if (!infile.eof()) {
	infile >> text >> ws;
	optHJ->Read(infile,text);
      } else
	handle.logWarning("No opt parameters for Hooke & Jeeves - using default values");
      useHJ = 1;

    } else if (strcasecmp(text, "[bfgs]") == 0) {
      handle.logMessage("Reading BFGS parameters");
      optBFGS = new OptInfoBfgs();
      if(!infile.eof()) {
	infile >> text >> ws;
	optBFGS->Read(infile,text);
      } else
	handle.logWarning("No opt parameters for BFGS - using default values");
      useBFGS = 1;
    
    } else {
      handle.Unexpected("[hooke], [simann], [bfgs] or seed", text);
    }
  } while (!infile.eof());

  if (useSimann == 0 && useHJ == 0 && useBFGS == 0)
      handle.logFailure("No valid optimisation methods in optinfofile\n");
  delete[] text;
}

void OptInfo::Optimize() {
  handle.logMessage("\nStarting optimisation");
  if (useSimann == 1) {
    cout << "\nStarting Simulated Annealing\n";
    optSimann->MaximizeLikelihood();
  }
  if (useHJ == 1) {
    cout << "\nStarting Hooke and Jeeves\n";
    optHJ->MaximizeLikelihood();
  }
  if (useBFGS == 1) {
    cout << "\nStarting BFGS\n";
    optBFGS->MaximizeLikelihood();
  }
}
/*
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
      handle.logWarning("Warning in OptInfo - unknown option", text);
      infile >> ws >> ws >> text >> ws;
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

  // Scaling the bounds, because the parameters are scaled. 
  for (i = 0; i < nopt; i++) {
    lowerb[i] = lowerb[i] / initialval[i];
    upperb[i] = upperb[i] / initialval[i];
    if (lowerb[i] > upperb[i]) {
      // If bounds smaller than zero //
      tmp = lowerb[i];
      lowerb[i] = upperb[i];
      upperb[i] = tmp;
    }

    // Warn if the starting point is zero //
    if (isZero(val[i]))
      handle.logWarning("Warning in optinfo - initial value is zero for switch", optswitches[i].getValue());
  }

  count = hooke(&f, nopt, startpoint, endpoint, upperb, lowerb,
    rho, lambda, hookeeps, hookeiter, init, bndcheck);

  cout << "\nHooke & Jeeves finished with final likelihood score of " << EcoSystem->getLikelihood()
    << "\nafter " << EcoSystem->getFuncEval() << " function evaluations at the point\n";
  EcoSystem->writeOptValues();

  delete[] endpoint;
  delete[] startpoint;
  delete[] upperb;
  delete[] lowerb;
  delete[] init;
}

OptInfoSimann::OptInfoSimann()
  : OptSearch(), rt(0.85), simanneps(1e-4), ns(5), nt(2), T(100),
    cs(2), vm(1), simanniter(2000), uratio(0.7), lratio(0.3), check(4) {
  handle.logMessage("Initialising simulated annealing");
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
    
    } else if (strcasecmp(text, "maxim") == 0) {
      // JMB - paramin compatiblity
      // read maxim and ignore it since we always minimise things
      infile >> text >> ws >> text >> ws;
      
    } else {
      handle.logWarning("Warning in OptInfo - unknown option", text);
      infile >> ws >> ws >> text >> ws;
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

  cout << "\nSimulated Annealing finished with final likelihood score of " << EcoSystem->getLikelihood()
    << "\nafter " << EcoSystem->getFuncEval() << " function evaluations at the point\n";
  EcoSystem->writeOptValues();

  delete[] startpoint;
  delete[] endpoint;
  delete[] lowerb;
  delete[] upperb;
}

OptInfoBfgs::OptInfoBfgs() : OptSearch() {

  nopt = EcoSystem->NoOptVariables();
  bb = new bfgs(&f,nopt);
  handle.logMessage("Initialising BFGS");
}

OptInfoBfgs::~OptInfoBfgs() {
  delete bb;
}

void OptInfoBfgs::Read(CommentStream& infile, char* text) {
  bb->Read(infile,text);
}

void OptInfoBfgs::MaximizeLikelihood(){
  int i;
  DoubleVector val(nopt);
  double* startpoint = new double[nopt];
  EcoSystem->ScaledOptValues(val);
  for (i = 0; i < nopt; i++)
    startpoint[i] = val[i];
   
  startpoint = bb->iteration(startpoint);
  cout << "\nBFGS finished with final likelihood score of " << EcoSystem->getLikelihood()
    << "\nafter " << EcoSystem->getFuncEval() << " function evaluations at the point\n";
  EcoSystem->writeOptValues();
  delete[] startpoint;
}
*/
