#include "ecosystem.h"
#include "runid.h"

extern RunID RUNID;
extern ErrorHandler handle;
extern int FuncEval;

Ecosystem::Ecosystem() {
}

Ecosystem::Ecosystem(const char* const filename, int optimize, int netrun,
  int calclikelihood, const char* const inputdir,
  const char* const workingdir, const PrintInfo& pi) : printinfo(pi) {

  funceval = 0;
  interrupted = 0;
  TimeInfo = 0;
  keeper = new Keeper;
  Area = 0;
  likelihood = 0.0;

  // initialise details used when printing the params.out file
  convergeSA = 0;
  funcevalSA = 0;
  likelihoodSA = 0.0;
  convergeHJ = 0;
  funcevalHJ = 0;
  likelihoodHJ = 0.0;
  convergeBFGS = 0;
  funcevalBFGS = 0;
  likelihoodBFGS = 0.0;

  chdir(workingdir);
  ifstream infile;
  infile.open(filename, ios::in);
  CommentStream commin(infile);
  handle.checkIfFailure(infile, filename);
  handle.Open(filename);
  chdir(inputdir);
  //New parameter netrun, to prevent reading of printfiles. 07.04.00 AJ & mnaa
  readMain(commin, optimize, netrun, calclikelihood, inputdir, workingdir);
  handle.Close();
  infile.close();
  infile.clear();

  //Dont print output line if doing a network run
  if (!netrun)
    cout << "\nFinished reading model input files" << endl;
  handle.logMessage("Finished reading model input files");
  handle.logMessage("");  //write a blank line to the log file

  Initialise(calclikelihood);
  basevec.resize(stockvec.Size() + otherfoodvec.Size() + fleetvec.Size(), 0);

  int i;
  for (i = 0; i < stockvec.Size(); i++)
    basevec[i] = stockvec[i];
  for (i = 0; i < otherfoodvec.Size(); i++)
    basevec[i + stockvec.Size()] = otherfoodvec[i];
  for (i = 0; i < fleetvec.Size(); i++)
    basevec[i + stockvec.Size() + otherfoodvec.Size()] = fleetvec[i];

}

Ecosystem::~Ecosystem() {

  int i;
  for (i = 0; i < fleetnames.Size(); i++)
    delete[] fleetnames[i];
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

void Ecosystem::writeStatus(const char* filename) const {
  int i;
  ofstream outfile;
  outfile.open(filename, ios::out);
  handle.checkIfFailure(outfile, filename);

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

void Ecosystem::Reset() {
  int i;
  for (i = 0; i < basevec.Size(); i++)
    basevec[i]->Reset(TimeInfo);
  for (i = 0; i < tagvec.Size(); i++)
    tagvec[i]->Reset(TimeInfo);
}

void Ecosystem::Update(const StochasticData* const Stochastic) const {
  keeper->Update(Stochastic);
}

void Ecosystem::Update(const DoubleVector& values) const {
  keeper->Update(values);
}

double Ecosystem::SimulateAndUpdate(double* x, int n) {
  ::FuncEval++;
  static int PrintCounter1 = printinfo.getPrint1() - 1;
  static int PrintCounter2 = printinfo.getPrint2() - 1;
  PrintCounter1++;
  PrintCounter2++;

  likelihood = 0.0;
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
  this->Simulate(1, 0);  //optimise and dont print
  if (PrintCounter1 == printinfo.getPrint1() && printinfo.getPrint()) {
    this->writeValues(printinfo.getOutputFile(), printinfo.getPrecision());
    PrintCounter1 = 0;
  }
  if (PrintCounter2 == printinfo.getPrint2() && printinfo.getPrintColumn()) {
    this->writeValuesInColumns(printinfo.getColumnOutputFile(), printinfo.getPrecision());
    PrintCounter2 = 0;
  }

  funceval++;
  return likelihood;
}

void Ecosystem::writeInitialInformation(const char* const filename) const {
  keeper->writeInitialInformation(filename, Likely);
}

void Ecosystem::writeInitialInformationInColumns(const char* const filename) const {
  keeper->writeInitialInformationInColumns(filename);
}

void Ecosystem::writeValues(const char* const filename, int prec) const {
  keeper->writeValues(filename, Likely, prec);
}

void Ecosystem::writeValuesInColumns(const char* const filename, int prec) const {
  keeper->writeValuesInColumns(filename, prec);
}

void Ecosystem::writeParamsInColumns(const char* const filename, int prec) const {

  if (funceval > 0) {
    //JMB - print the final values to any output files specified
    //in case they have been missed by the -print1 or -print2 values
    if (printinfo.getPrint())
      this->writeValues(printinfo.getOutputFile(), printinfo.getPrecision());
    if (printinfo.getPrintColumn())
      this->writeValuesInColumns(printinfo.getColumnOutputFile(), printinfo.getPrecision());
  }

  keeper->writeParamsInColumns(filename, prec);
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

void Ecosystem::writeOptValues() const {
  keeper->writeOptValues(likelihood, Likely);
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

void Ecosystem::writeLikelihoodInformation(const char* filename) const {
  ofstream outfile;
  outfile.open(filename, ios::out);
  handle.checkIfFailure(outfile, filename);
  handle.Open(filename);
  RUNID.print(outfile);
  int i;
  for (i = 0; i < Likely.Size(); i++)
    Likely[i]->LikelihoodPrint(outfile);
  handle.Close();
  outfile.close();
  outfile.clear();
}

void Ecosystem::writeLikelihoodInformation(const char* filename, int id) const {
  ofstream outfile;
  outfile.open(filename, ios::out);
  handle.checkIfFailure(outfile, filename);
  handle.Open(filename);
  RUNID.print(outfile);
  //JMB check that id is valid
  if (id < Likely.Size())
    Likely[id]->LikelihoodPrint(outfile);
  handle.Close();
  outfile.close();
  outfile.clear();
}

void Ecosystem::writeLikeSummaryInformation(const char* filename) const {
  ofstream outfile;
  outfile.open(filename, ios::out);
  handle.checkIfFailure(outfile, filename);
  handle.Open(filename);
  outfile << "; ";
  RUNID.print(outfile);
  outfile << "; Summary likelihood information from the current run" << endl
    << "; year-step-area-component-weight-likelihood value" << endl;
  outfile.flush();

  int i;
  for (i = 0; i < Likely.Size(); i++)
    Likely[i]->SummaryPrint(outfile);
    
  handle.Close();
  outfile.close();
  outfile.clear();
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

void Ecosystem::checkBounds() const {
  keeper->checkBounds();
}
