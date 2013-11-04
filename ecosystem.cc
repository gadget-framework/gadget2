#include "ecosystem.h"
#include "runid.h"
#include "global.h"

Ecosystem::Ecosystem(const MainInfo& main) : printinfo(main.getPI()) {

  funceval = 0;
  interrupted = 0;
  likelihood = 0.0;
  keeper = new Keeper;

  // initialise counter used when printing output files
  printcount = printinfo.getPrintIteration() - 1;

  // read the model specification from the main file
  char* filename = main.getMainGadgetFile();
  ifstream infile;
  infile.open(filename, ios::in);
  CommentStream commin(infile);
  handle.checkIfFailure(infile, filename);
  handle.Open(filename);
  this->readMain(commin, main);
  handle.Close();
  infile.close();
  infile.clear();

  // if this is an optimising run then read the optimisation parameters from file
  if (main.runOptimise()) {
    handle.logMessage(LOGMESSAGE, "");  //write blank line to log file
    if (main.getOptInfoGiven()) {
      filename = main.getOptInfoFile();
      infile.open(filename, ios::in);
      handle.checkIfFailure(infile, filename);
      handle.Open(filename);
      this->readOptimisation(commin);
      handle.Close();
      infile.close();
      infile.clear();
    } else {
      handle.logMessage(LOGINFO, "Warning - no optimisation file specified, using default values");
      optvec.resize(new OptInfoHooke());
    }
  }

  if (main.runOptimise())
    handle.logMessage(LOGINFO, "\nFinished reading model data files, starting to run optimisation");
  else
    handle.logMessage(LOGINFO, "\nFinished reading model data files, starting to run simulation");
  handle.logMessage(LOGMESSAGE, "");  //write blank line to log file
}

Ecosystem::~Ecosystem() {
  int i;
  for (i = 0; i < optvec.Size(); i++)
    delete optvec[i];
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
  TimeInfo->Reset();
  for (i = 0; i < likevec.Size(); i++)
    likevec[i]->Reset(keeper);
  for (i = 0; i < basevec.Size(); i++)
    basevec[i]->Reset(TimeInfo);
}

void Ecosystem::Optimise() {
  int i;
  for (i = 0; i < optvec.Size(); i++) {
    optvec[i]->OptimiseLikelihood();
    this->writeOptValues();
  }
}

double Ecosystem::SimulateAndUpdate(const DoubleVector& x) {
  int i, j;

  if (funceval == 0) {
    // JMB - only need to create these vectors once
    initialval.resize(keeper->numVariables(), 0.0);
    currentval.resize(keeper->numVariables(), 0.0);
    optflag.resize(keeper->numVariables(), 0);
    keeper->getOptFlags(optflag);
  }

  j = 0;
  keeper->getCurrentValues(currentval);
  keeper->getInitialValues(initialval);
  for (i = 0; i < currentval.Size(); i++) {
    if (optflag[i]) {
      currentval[i] = x[j] * initialval[i];
      j++;
    }
  }

  keeper->Update(currentval);
  this->Simulate(0);  //dont print whilst optimising

  if (printinfo.getPrint()) {
    printcount++;
    if (printcount == printinfo.getPrintIteration()) {
      keeper->writeValues(likevec, printinfo.getPrecision());
      printcount = 0;
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

  handle.logMessage(LOGINFO, "\nAfter a total of", funceval, "function evaluations the best point found is");
  keeper->writeBestValues();
  handle.logMessage(LOGINFO, "\nThe scores from each likelihood component are");
  handle.logMessage(LOGINFO, tmpvec);
  if (!isZero(keeper->getBestLikelihoodScore())) // no better point has been found
    handle.logMessage(LOGINFO, "\nThe overall likelihood score is", keeper->getBestLikelihoodScore());
  else
    handle.logMessage(LOGINFO, "\nThe overall likelihood score is", this->getLikelihood());
}

void Ecosystem::writeInitialInformation(const char* const filename) {
  keeper->openPrintFile(filename);
  keeper->writeInitialInformation(likevec);
}

void Ecosystem::writeValues() {
  keeper->writeValues(likevec, printinfo.getPrecision());
}

void Ecosystem::writeParams(const char* const filename, int prec) const {
  if ((funceval > 0) && (interrupted == 0)) {
    //JMB - print the final values to any output files specified
    //in case they have been missed by the -print value
    if (printinfo.getPrint())
      keeper->writeValues(likevec, printinfo.getPrecision());
  }
  keeper->writeParams(optvec, filename, prec, interrupted);
}
