#include "optinfo.h"
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
  if (MainInfo->getOptInfoGiven()){      
    ReadOptInfo(MainInfo->optInfoFile());
    MainInfo->closeOptInfoFile();
  } else {
    handle.logMessage("Warning - no optinfofile given, using default information");
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
      handle.logMessage("Reading seed from optinfofile");
      infile >> seed >> ws >> text >> ws;
      srand(seed);	

    } else if (strcasecmp(text, "[simann]") == 0) {
      handle.logMessage("Reading Simulated Annealing parameters from optinfofile");
      optSimann = new OptInfoSimann();

      if (!infile.eof()) {
        infile >> text >> ws;
        optSimann->Read(infile, text);
      } else 
        handle.logWarning("Warning - no parameters specified for Simulated Annealing, using default values");
      useSimann = 1;

    } else if (strcasecmp(text, "[hooke]") == 0) {
      handle.logMessage("Reading Hooke & Jeeves parameters from optinfofile");
      optHJ = new OptInfoHooke();

      if (!infile.eof()) {
        infile >> text >> ws;
        optHJ->Read(infile,text);
      } else
        handle.logWarning("Warning - no parameters specified for Hooke & Jeeves, using default values");
      useHJ = 1;

    } else if (strcasecmp(text, "[bfgs]") == 0) {
      handle.logMessage("Reading BFGS parameters from optinfofile");
      optBFGS = new OptInfoBfgs();

      if(!infile.eof()) {
        infile >> text >> ws;
        optBFGS->Read(infile,text);
      } else
        handle.logWarning("Warning - no parameters specified for BFGS, using default values");
      useBFGS = 1;

    } else {
      handle.Unexpected("[hooke], [simann], [bfgs] or seed", text);
    }
  } while (!infile.eof());

  if (useSimann == 0 && useHJ == 0 && useBFGS == 0)
    handle.logFailure("Error in optinfofile - no valid optimisation methods found\n");
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
