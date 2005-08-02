#include "ecosystem.h"
#include "runid.h"

extern RunID RUNID;
extern ErrorHandler handle;

Ecosystem::Ecosystem(const MainInfo& main, const char* const inputdir, const char* const workingdir)   : printinfo(main.getPI()) {

  funceval = 0;
  interrupted = 0;
  likelihood = 0.0;
  keeper = new Keeper;

  // initialise counters used when printing output files
  printcount1 = printinfo.getPrint1() - 1;
  printcount2 = printinfo.getPrint2() - 1;

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
    handle.logMessage(LOGINFO, "\nFinished reading model data files, starting to run optimisation");
  else
    handle.logMessage(LOGINFO, "\nFinished reading model data files, starting to run simulation");
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
  for (i = 0; i < tagvec.Size(); i++)
    tagvec[i]->Print(outfile);

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

  if (printinfo.getPrint()) {
    printcount1++;
    if (printcount1 == printinfo.getPrint1()) {
      keeper->writeValues(likevec, printinfo.getPrecision());
      printcount1 = 0;
    }
  }
  if (printinfo.getPrintColumn()) {
    printcount2++;
    if (printcount2 == printinfo.getPrint2()) {
      keeper->writeValuesInColumns(printinfo.getPrecision());
      printcount2 = 0;
    }
  }

  funceval++;
  return likelihood;
}

void Ecosystem::writeOptValues() {
  int i;
  DoubleVector tmpvec(likevec.Size(), 0.0);
  for (i = 0; i < likevec.Size(); i++)
    tmpvec[i] = likevec[i]->getUnweightedLikelihood();
  keeper->writeBestValues();
  handle.logMessage(LOGINFO, "\nThe scores from each likelihood component are");
  handle.logMessage(LOGINFO, tmpvec);
  handle.logMessage(LOGINFO, "\nThe overall likelihood score is", likelihood);
}

void Ecosystem::writeInitialInformation(const char* const filename) {
  keeper->openPrintFile(filename);
  keeper->writeInitialInformation(likevec);
}

void Ecosystem::writeInitialInformationInColumns(const char* const filename) {
  keeper->openPrintFile(filename);
}

void Ecosystem::writeValues() {
  keeper->writeValues(likevec, printinfo.getPrecision());
}

void Ecosystem::writeValuesInColumns() {
  keeper->writeValuesInColumns(printinfo.getPrecision());
}

void Ecosystem::writeParams(const char* const filename, int prec) const {
  if ((funceval > 0) && (interrupted == 0)) {
    //JMB - print the final values to any output files specified
    //in case they have been missed by the -print1 or -print2 values
    if (printinfo.getPrint())
      keeper->writeValues(likevec, printinfo.getPrecision());
    if (printinfo.getPrintColumn())
      keeper->writeValuesInColumns(printinfo.getPrecision());
  }
  keeper->writeParams(filename, prec, interrupted);
}
