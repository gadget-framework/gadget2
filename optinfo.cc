#include "optinfo.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

OptInfo::OptInfo(MainInfo* MainInfo) {
  useSimann = 0;
  useHJ     = 0;
  useBFGS   = 0;
  optSimann = NULL;
  optHJ     = NULL;
  optBFGS   = NULL;

  // Initialise random number generator with system time [morten 02.02.26]
  srand(time(NULL));

  if (MainInfo->getOptInfoGiven()) {
    ReadOptInfo(MainInfo->optInfoFile());
    MainInfo->closeOptInfoFile();
  } else {
    handle.logWarning("\nWarning - no optimisation file specified, using default values");
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

void OptInfo::ReadOptInfo(CommentStream& infile) {
  char* text = new char[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  handle.logMessage("\nReading optimisation information");
  infile >> text;
  while (!infile.eof()) {
    infile >> ws;  //trim whitespace from infile
    if ((strcasecmp(text, "seed")) == 0 && (!infile.eof())) {
      int seed;
      infile >> seed >> ws >> text;
      handle.logMessage("Initialising random number generator with", seed);
      srand(seed);

    } else if (strcasecmp(text, "[simann]") == 0) {
      handle.logMessage("Reading Simulated Annealing parameters");
      optSimann = new OptInfoSimann();
      useSimann = 1;

      if (!infile.eof()) {
        infile >> text;
        if ((text[0] == '[') || (strcasecmp(text, "seed") == 0))
          handle.logWarning("Warning - no optimisation parameters specified for Simulated Annealing algorithm");
        else
          optSimann->Read(infile, text);

      } else
        handle.logWarning("Warning - no optimisation parameters specified for Simulated Annealing algorithm");

    } else if (strcasecmp(text, "[hooke]") == 0) {
      handle.logMessage("Reading Hooke and Jeeves parameters");
      optHJ = new OptInfoHooke();
      useHJ = 1;

      if (!infile.eof()) {
        infile >> text;
        if ((text[0] == '[') || (strcasecmp(text, "seed") == 0))
          handle.logWarning("Warning - no optimisation parameters specified for Hooke and Jeeves algorithm");
        else
          optHJ->Read(infile, text);

      } else
        handle.logWarning("Warning - no optimisation parameters specified for Hooke and Jeeves algorithm");

    } else if (strcasecmp(text, "[bfgs]") == 0) {
      handle.logMessage("Reading BFGS parameters");
      optBFGS = new OptInfoBfgs();
      useBFGS = 1;

      if (!infile.eof()) {
        infile >> text;
        if ((text[0] == '[') || (strcasecmp(text, "seed") == 0))
          handle.logWarning("Warning - no optimisation parameters specified for BFGS algorithm");
        else
          optBFGS->Read(infile, text);

      } else
        handle.logWarning("Warning - no optimisation parameters specified for BFGS algorithm");

    } else {
      handle.Unexpected("[hooke], [simann], [bfgs] or seed", text);
    }
  }

  delete[] text;
  if (useSimann == 0 && useHJ == 0 && useBFGS == 0)
    handle.logFailure("Error in optinfofile - no valid optimisation methods found");
}

void OptInfo::Optimize() {
  if (useSimann)
    optSimann->MaximizeLikelihood();
  if (useHJ)
    optHJ->MaximizeLikelihood();
  if (useBFGS)
    optBFGS->MaximizeLikelihood();
}
