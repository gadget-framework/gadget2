#include "ecosystem.h"
#include "runid.h"
extern RunID RUNID;
extern int FuncEval;

Ecosystem::Ecosystem() {
}

Ecosystem::Ecosystem(const char* const filename, int optimize, int netrun,
  int calclikelihood, const char* const inputdir,
  const char* const workingdir, const PrintInfo& pi) : printinfo(pi) {

  funceval = 0;
  converge = 0;
  interrupted = 0;
  TimeInfo = 0;
  keeper = new Keeper;
  Area = 0;
  ErrorHandler handle;
  chdir(workingdir);
  ifstream infile(filename);
  CommentStream commin(infile);
  handle.Open(filename);
  checkIfFailure(infile, filename);
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
  for (i = 0; i < fleetnames.Size(); i++)
    delete[] fleetnames[i];
  for (i = 0; i < catchnames.Size(); i++)
    delete[] catchnames[i];
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
  for (i = 0; i < tagnames.Size(); i++)
    delete[] tagnames[i];
  for (i = 0; i < otherfoodnames.Size(); i++)
    delete[] otherfoodnames[i];
  for (i = 0; i < printvec.Size(); i++)
    delete printvec[i];
  for (i = 0; i < likprintvec.Size(); i++)
    delete likprintvec[i];
  for (i = 0; i < Likely.Size(); i++)
    delete Likely[i];
  for (i = 0; i < tagvec.Size(); i++)
    delete tagvec[i];
  for (i = 0; i < basevec.Size(); i++)
    delete basevec[i];

  delete Area;
  delete TimeInfo;
  delete keeper;
}

void Ecosystem::PrintStatus(const char* filename) const {
  int i;
  ofstream outfile;
  outfile.open(filename, ios::out);
  checkIfFailure(outfile, filename);

  RUNID.print(outfile);
  outfile << "The current simulation time is " << TimeInfo->CurrentYear()
    << ", step " << TimeInfo->CurrentStep() << endl;
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

void Ecosystem::Update(const DoubleVector& values) const {
  keeper->Update(values);
}

double Ecosystem::SimulateAndUpdate(double* x, int n) {
  int optimize = 1;
  ::FuncEval++;
  static int PrintCounter1 = printinfo.PrintInterVal1 - 1;
  static int PrintCounter2 = printinfo.PrintInterVal2 - 1;

  PrintCounter1++;
  PrintCounter2++;
  int numvar = this->NoVariables();
  DoubleVector val(numvar);
  DoubleVector initialvalues(numvar);
  IntVector opt(numvar);
  this->InitialValues(initialvalues);
  this->ValuesOfVariables(val);
  this->Opt(opt);

  int i, j;
  j = 0;
  for (i = 0; i < numvar; i++)
    if (opt[i] == 1) {
      val[i] = x[j] * initialvalues[i];
      j++;
    }
  this->Update(val);
  likelihood = 0.0;
  if (!Simulate(optimize))
    likelihood = verybig;

  if (PrintCounter1 == printinfo.PrintInterVal1 && printinfo.Print()) {
    this->PrintValues(printinfo.OutputFile, printinfo.givenPrecision);
    PrintCounter1 = 0;
  }
  if (PrintCounter2 == printinfo.PrintInterVal2 && printinfo.PrintinColumns()) {
    this->PrintValuesinColumns(printinfo.ColumnOutputFile, printinfo.givenPrecision);
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

void Ecosystem::PrintValues(const char* const filename, int prec) const {
  keeper->WriteValues(filename, likelihood, Likely, prec);
}

void Ecosystem::PrintValuesinColumns(const char* const filename, int prec) const {
  keeper->WriteValuesInColumns(filename, likelihood, Likely, prec);
}

void Ecosystem::PrintParamsinColumns(const char* const filename, int prec) const {

  //JMB - print the final values to any output files specified
  //in case they have been missed by the -print1 or -print2 values
  if (printinfo.Print())
    this->PrintValues(printinfo.OutputFile, printinfo.givenPrecision);
  if (printinfo.PrintinColumns())
    this->PrintValuesinColumns(printinfo.ColumnOutputFile, printinfo.givenPrecision);

  keeper->WriteParamsInColumns(filename, likelihood, Likely, prec);
}

void Ecosystem::Opt(IntVector& opt) const {
  keeper->Opt(opt);
}

void Ecosystem::InitialValues(DoubleVector& initialval) const {
  keeper->InitialValues(initialval);
}

void Ecosystem::ValuesOfVariables(DoubleVector& val) const {
  keeper->ValuesOfVariables(val);
}

void Ecosystem::ScaleVariables() const {
  keeper->ScaleVariables();
}

void Ecosystem::PrintOptValues() const {
  keeper->WriteOptValues(likelihood, Likely);
}

void Ecosystem::ScaledValues(DoubleVector& val) const {
  keeper->ScaledValues(val);
}

void Ecosystem::ScaledOptValues(DoubleVector& val) const {
  keeper->ScaledOptValues(val);
}

void Ecosystem::InitialOptValues(DoubleVector& val) const {
  keeper->InitialOptValues(val);
}

void Ecosystem::OptSwitches(ParameterVector& sw) const {
  keeper->OptSwitches(sw);
}

void Ecosystem::PrintLikelihoodInfo(const char* filename) const {
  ofstream outfile;
  outfile.open(filename, ios::app);
  checkIfFailure(outfile, filename);
  int i;
  for (i = 0; i < Likely.Size(); i++)
    Likely[i]->LikelihoodPrint(outfile);
}

void Ecosystem::OptValues(DoubleVector& val) const {
  keeper->OptValues(val);
}

void Ecosystem::LowerBds(DoubleVector& lbds) const {
  keeper->LowerOptBds(lbds);
}

void Ecosystem::UpperBds(DoubleVector& ubds) const {
  keeper->UpperOptBds(ubds);
}

void Ecosystem::CheckBounds() const {
  keeper->CheckBounds();
}
