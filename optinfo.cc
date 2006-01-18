#include "optinfo.h"
#include "ecosystem.h"
#include "errorhandler.h"
#include "gadget.h"

extern Ecosystem* EcoSystem;
extern ErrorHandler handle;

OptInfo::OptInfo(int readfile, const char* filename) {
  useSimann = 0;
  useHJ     = 0;
  useBFGS   = 0;
  optSimann = NULL;
  optHJ     = NULL;
  optBFGS   = NULL;

  if (readfile) {
    ifstream infile;
    infile.open(filename, ios::in);
    CommentStream optfile(infile);
    handle.checkIfFailure(infile, filename);
    handle.Open(filename);
    this->readOptInfo(optfile);
    handle.Close();
    infile.close();
    infile.clear();
  } else {
    handle.logMessage(LOGINFO, "Warning - no optimisation file specified, using default values");
    optHJ = new OptInfoHooke();
    useHJ = 1;
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

void OptInfo::readOptInfo(CommentStream& infile) {
  char* text = new char[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  handle.logMessage(LOGMESSAGE, "Reading optimisation information");
  infile >> text;
  while (!infile.eof()) {
    infile >> ws;  //trim whitespace from infile
    if ((strcasecmp(text, "seed")) == 0 && (!infile.eof())) {
      int seed = 0;
      infile >> seed >> ws >> text;
      handle.logMessage(LOGMESSAGE, "Initialising random number generator with", seed);
      srand(seed);

    } else if (strcasecmp(text, "[simann]") == 0) {
      optSimann = new OptInfoSimann();
      useSimann = 1;

      if (!infile.eof()) {
        infile >> text;
        if ((text[0] == '[') || (strcasecmp(text, "seed") == 0))
          handle.logMessage(LOGINFO, "Warning - no optimisation parameters specified for Simulated Annealing algorithm");
        else
          optSimann->read(infile, text);

      } else
        handle.logMessage(LOGINFO, "Warning - no optimisation parameters specified for Simulated Annealing algorithm");

    } else if (strcasecmp(text, "[hooke]") == 0) {
      optHJ = new OptInfoHooke();
      useHJ = 1;

      if (!infile.eof()) {
        infile >> text;
        if ((text[0] == '[') || (strcasecmp(text, "seed") == 0))
          handle.logMessage(LOGINFO, "Warning - no optimisation parameters specified for Hooke & Jeeves algorithm");
        else
          optHJ->read(infile, text);

      } else
        handle.logMessage(LOGINFO, "Warning - no optimisation parameters specified for Hooke & Jeeves algorithm");

    } else if (strcasecmp(text, "[bfgs]") == 0) {
      optBFGS = new OptInfoBFGS();
      useBFGS = 1;

      if (!infile.eof()) {
        infile >> text;
        if ((text[0] == '[') || (strcasecmp(text, "seed") == 0))
          handle.logMessage(LOGINFO, "Warning - no optimisation parameters specified for BFGS algorithm");
        else
          optBFGS->read(infile, text);

      } else
        handle.logMessage(LOGINFO, "Warning - no optimisation parameters specified for BFGS algorithm");

    } else {
      handle.logFileUnexpected(LOGFAIL, "[hooke], [simann], [bfgs] or seed", text);
    }
  }

  delete[] text;
  if (useSimann == 0 && useHJ == 0 && useBFGS == 0) {
    handle.logFileMessage(LOGWARN, "no optimisation methods found, using default values");
    optHJ = new OptInfoHooke();
    useHJ = 1;
  }
}

void OptInfo::Optimise() {
  if (useSimann) {
    handle.logMessage(LOGINFO, "\nStarting Simulated Annealing optimisation algorithm\n");
    optSimann->OptimiseLikelihood();
    handle.logMessage(LOGINFO, "\nSimulated Annealing finished with a final likelihood score of", EcoSystem->getLikelihood());
    handle.logMessage(LOGINFO, "after a total of", EcoSystem->getFuncEval(), "function evaluations at the point");
    EcoSystem->writeOptValues();
  }
  if (useHJ) {
    handle.logMessage(LOGINFO, "\nStarting Hooke & Jeeves optimisation algorithm\n");
    optHJ->OptimiseLikelihood();
    handle.logMessage(LOGINFO, "\nHooke & Jeeves finished with a final likelihood score of", EcoSystem->getLikelihood());
    handle.logMessage(LOGINFO, "after a total of", EcoSystem->getFuncEval(), "function evaluations at the point");
    EcoSystem->writeOptValues();
  }
  if (useBFGS) {
    handle.logMessage(LOGINFO, "\nStarting BFGS optimisation algorithm\n");
    optBFGS->OptimiseLikelihood();
    handle.logMessage(LOGINFO, "\nBFGS finished with a final likelihood score of", EcoSystem->getLikelihood());
    handle.logMessage(LOGINFO, "after a total of", EcoSystem->getFuncEval(), "function evaluations at the point");
    EcoSystem->writeOptValues();
  }
}
