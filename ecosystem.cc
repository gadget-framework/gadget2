#include "ecosystem.h"

extern int FuncEval;

Ecosystem::Ecosystem() {
}

//New parameter netrun, to prevent reading of printfiles. 07.04.00 AJ & mnaa
Ecosystem::Ecosystem(const char* const filename, int optimize, int netrun,
  int calclikelihood, const char* const inputdir,
  const char* const workingdir, const PrintInfo& pi) : funceval(0), printinfo(pi) {

  interrupted = 0;
  TimeInfo = 0;
  keeper = new Keeper;
  Area = 0;
  ErrorHandler handle;
  chdir(workingdir);
  ifstream infile("main");
  CommentStream commin(infile);
  handle.Open("main");
  CheckIfFailure(infile, "main");
  chdir(inputdir);
  //New parameter netrun, to prevent reading of printfiles. 07.04.00 AJ & mnaa
  Readmain(commin, optimize, netrun, calclikelihood, inputdir, workingdir);
  handle.Close();
  infile.close();
  infile.clear();

  Initialize(calclikelihood);
  basevec.resize(stockvec.Size() + otherfoodvec.Size() + fleetvec.Size(), 0);

  int i;
  for (i = 0; i < stockvec.Size(); i++)
    basevec[i] = stockvec[i];
  for (i = 0; i < otherfoodvec.Size(); i++)
    basevec[i + stockvec.Size()] = otherfoodvec[i];
  for (i = 0; i < fleetvec.Size(); i++)
    basevec[i + stockvec.Size() + otherfoodvec.Size()] = fleetvec[i];

  likelihood = 0.0;
  //The following is done in Simulate, but we want to be able to call
  //Print before the simulation starts, so we do it ourselves first.
  for (i = 0; i < basevec.Size(); i++)
    basevec[i]->Reset(TimeInfo);

}

Ecosystem::~Ecosystem() {
  int i;
  delete Area;
  delete TimeInfo;
  delete keeper;
  for (i = 0; i < fleetnames.Size(); i++)
    delete[] fleetnames[i];
  for (i = 0; i < catchnames.Size(); i++)
    delete[] catchnames[i];
  for (i = 0; i < catchdata.Size(); i++)
    delete catchdata[i];
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
  for (i = 0; i < tagnames.Size(); i++)
    delete[] tagnames[i];
  for (i = 0; i < otherfoodnames.Size(); i++)
    delete[] otherfoodnames[i];
  for (i = 0; i < Likely.Size(); i++)
    delete Likely[i];
  for (i = 0; i < basevec.Size(); i++)
    delete basevec[i];
  for (i = 0; i < printvec.Size(); i++)
    delete printvec[i];
  for (i = 0; i < tagvec.Size(); i++)
    delete tagvec[i];
  for (i = 0; i < likprintvec.Size(); i++)
    delete likprintvec[i];
}

//Print status
void Ecosystem::PrintStatus(const char* filename) const {
  ofstream outfile;
  outfile.open(filename, ios::out);
  CheckIfFailure(outfile, filename);
  int i;
  for (i = 0; i < stockvec.Size(); i++)
    stockvec[i]->Print(outfile);
  for (i = 0; i < otherfoodvec.Size(); i++)
    otherfoodvec[i]->Print(outfile);
  for (i = 0; i < fleetvec.Size(); i++)
    fleetvec[i]->Print(outfile);
  for (i = 0; i < Likely.Size(); i++)
    Likely[i]->Print(outfile);
  outfile.close();
  outfile.clear();
}

void Ecosystem::Update(const StochasticData* const Stochastic) const {
  keeper->Update(Stochastic);
}

void Ecosystem::Update(const doublevector& values) const {
  keeper->Update(values);
}

double Ecosystem::SimulateAndUpdate(double *x, int n) {
  int optimize = 1;
  ::FuncEval++;
  static int PrintCounter1 = printinfo.PrintInterVal1 - 1;
  static int PrintCounter2 = printinfo.PrintInterVal2 - 1;

  PrintCounter1++;
  PrintCounter2++;
  doublevector val(this->NoVariables());
  doublevector initialvalues(this->NoVariables());
  intvector opt(this->NoVariables());
  this->InitialValues(initialvalues);
  this->ValuesOfVariables(val);
  this->Opt(opt);

  int i, k;
  k = 0;
  for (i = 0; i < this->NoVariables(); i++)
    if (opt[i] == 1) {
      val[i] = x[k] * initialvalues[i];
      k++;
    }
  this->Update(val);
  likelihood = 0.0;
  if (!Simulate(optimize))
    likelihood = HUGE_VALUE;

  if (PrintCounter1 == printinfo.PrintInterVal1 && printinfo.Print()) {
    this->PrintValues(printinfo.OutputFile);
    PrintCounter1 = 0;
  }
  if (PrintCounter2 == printinfo.PrintInterVal2 && printinfo.PrintinColumns()) {
    this->PrintValuesinColumns(printinfo.ColumnOutputFile);
    PrintCounter2 = 0;
  }
  funceval++;
  return likelihood;
}

void Ecosystem::PrintInitialInformation(const char* const filename) const {
  keeper->WriteInitialInformation(filename, Likely);
}

void Ecosystem::PrintInitialInformationinColumns(const char* const filename) const {
  keeper->WriteInitialInformationInColumns(filename);
}

void Ecosystem::PrintValues(const char* const filename) const {
  keeper->WriteValues(filename, likelihood, Likely);
}

void Ecosystem::PrintValuesinColumns(const char* const filename) const {
  keeper->WriteValuesInColumns(filename, likelihood, Likely);
}

void Ecosystem::PrintParamsinColumns(const char* const filename) const {
  keeper->WriteParamsInColumns(filename, likelihood, Likely);
}

void Ecosystem::Opt(intvector& opt) const {
  keeper->Opt(opt);
}

void Ecosystem::InitialValues(doublevector& initialval) const {
  keeper->InitialValues(initialval);
}

void Ecosystem::ValuesOfVariables(doublevector& val) const {
  keeper->ValuesOfVariables(val);
}

int Ecosystem::NoOptVariables() const {
  return keeper->NoOptVariables();
}

void Ecosystem::ScaleVariables() const {
  keeper->ScaleVariables();
}

void Ecosystem::PrintOptValues() const {
  keeper->WriteOptValues(likelihood, Likely);
}

void Ecosystem::ScaledValues(doublevector& val) const {
  keeper->ScaledValues(val);
}

void Ecosystem::ScaledOptValues(doublevector& val) const {
  keeper->ScaledOptValues(val);
}

void Ecosystem::InitialOptValues(doublevector& val) const {
  keeper->InitialOptValues(val);
}

void Ecosystem::OptSwitches(Parametervector& sw) const {
  keeper->OptSwitches(sw);
}

void Ecosystem::PrintLikelihoodInfo(const char* filename) const {
  ofstream outfile;
  outfile.open(filename, ios::app);
  CheckIfFailure(outfile, filename);
  int i;
  for (i = 0; i < Likely.Size(); i++)
    Likely[i]->LikelihoodPrint(outfile);
}

void Ecosystem::OptValues(doublevector& val) const {
  keeper->OptValues(val);
}

Stock* Ecosystem::findStock(const char* stockname) const {
  int i;
  for (i = 0; i < stockvec.Size(); i++)
    if (strcasecmp(stockvec[i]->Name(), stockname) == 0)
      return stockvec[i];
  return NULL;
}

Fleet* Ecosystem::findFleet(const char* fleetname) const {
  int i;
  for (i = 0; i < fleetvec.Size(); i++)
    if (strcasecmp(fleetvec[i]->Name(), fleetname) == 0)
      return fleetvec[i];
  return NULL;
}

void Ecosystem::LowerBds(doublevector& lbds) const {
  keeper->LowerBds(lbds);
}

void Ecosystem::UpperBds(doublevector& ubds) const {
  keeper->UpperBds(ubds);
}
