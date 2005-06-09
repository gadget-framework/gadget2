#include "ecosystem.h"
#include "runid.h"

extern RunID RUNID;
extern ErrorHandler handle;
extern int FuncEval;

Ecosystem::Ecosystem(const MainInfo& main, const char* const inputdir, const char* const workingdir)   : printinfo(main.getPI()) {

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

  // read the model specification from the main file
  char* filename = main.getMainGadgetFile();
  chdir(workingdir);
  ifstream infile;
  infile.open(filename, ios::in);
  CommentStream commin(infile);
  handle.checkIfFailure(infile, filename);
  handle.Open(filename);
  chdir(inputdir);
  this->readMain(commin, main, inputdir, workingdir);
  handle.Close();
  infile.close();
  infile.clear();

  // check and initialise the model
  handle.logMessage(LOGMESSAGE, "");  //write a blank line to the log file
  this->Initialise();
  basevec.resize(stockvec.Size() + otherfoodvec.Size() + fleetvec.Size(), 0);

  int i;
  for (i = 0; i < stockvec.Size(); i++)
    basevec[i] = stockvec[i];
  for (i = 0; i < otherfoodvec.Size(); i++)
    basevec[i + stockvec.Size()] = otherfoodvec[i];
  for (i = 0; i < fleetvec.Size(); i++)
    basevec[i + stockvec.Size() + otherfoodvec.Size()] = fleetvec[i];

  if (main.runOptimise())
    handle.logMessage(LOGINFO, "\nFinished reading input files, starting to run optimisation");
  else
    handle.logMessage(LOGINFO, "\nFinished reading input files, starting to run simulation");
  handle.logMessage(LOGMESSAGE, "");  //write a blank line to the log file
}

Ecosystem::~Ecosystem() {
  int i;
  for (i = 0; i < printvec.Size(); i++)
    delete printvec[i];
  for (i = 0; i < likevec.Size(); i++)
    delete likevec[i];
  for (i = 0; i < tagvec.Size(); i++)
    delete tagvec[i];
  for (i = 0; i < basevec.Size(); i++)
    delete basevec[i];

  delete Area;
  delete TimeInfo;
  delete keeper;
}

void Ecosystem::writeStatus(const char* filename) const {
  ofstream outfile;
  outfile.open(filename, ios::out);
  handle.checkIfFailure(outfile, filename);
  handle.Open(filename);
  RUNID.Print(outfile);
  outfile << "The current simulation time is " << TimeInfo->getYear()
    << ", step " << TimeInfo->getStep() << endl;

  int i;
  for (i = 0; i < basevec.Size(); i++)
    basevec[i]->Print(outfile);
  for (i = 0; i < likevec.Size(); i++)
    likevec[i]->Print(outfile);

  handle.Close();
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

double Ecosystem::SimulateAndUpdate(const DoubleVector& x) {
  ::FuncEval++;
  static int PrintCounter1 = printinfo.getPrint1() - 1;
  static int PrintCounter2 = printinfo.getPrint2() - 1;
  PrintCounter1++;
  PrintCounter2++;

  likelihood = 0.0;
  int numvar = keeper->numVariables();
  DoubleVector val(numvar);
  DoubleVector initialvalues(numvar);
  IntVector opt(numvar);
  keeper->getInitialValues(initialvalues);
  keeper->getCurrentValues(val);
  keeper->getOptFlags(opt);

  int i, j;
  j = 0;
  for (i = 0; i < numvar; i++) {
    if (opt[i] == 1) {
      val[i] = x[j] * initialvalues[i];
      j++;
    }
  }

  keeper->Update(val);
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
  keeper->writeInitialInformation(filename, likevec);
}

void Ecosystem::writeInitialInformationInColumns(const char* const filename) const {
  keeper->writeInitialInformationInColumns(filename);
}

void Ecosystem::writeValues(const char* const filename, int prec) const {
  keeper->writeValues(filename, likevec, prec);
}

void Ecosystem::writeValuesInColumns(const char* const filename, int prec) const {
  keeper->writeValuesInColumns(filename, prec);
}

void Ecosystem::writeParamsInColumns(const char* const filename, int prec) const {

  if ((funceval > 0) && (interrupted == 0)) {
    //JMB - print the final values to any output files specified
    //in case they have been missed by the -print1 or -print2 values
    if (printinfo.getPrint())
      this->writeValues(printinfo.getOutputFile(), printinfo.getPrecision());
    if (printinfo.getPrintColumn())
      this->writeValuesInColumns(printinfo.getColumnOutputFile(), printinfo.getPrecision());
  }

  keeper->writeParamsInColumns(filename, prec, interrupted);
}
