#include "ecosystem.h"
#include "maininfo.h"
#include "runid.h"
#include "gadget.h"

#ifdef INTERRUPT_HANDLER
#include "interrupthandler.h"
#endif

//Ecosystem must be global due to the optimization functions.
RunID RUNID;
Ecosystem* EcoSystem;
ErrorHandler handle;
int FuncEval = 0;

void stochasticRun(Ecosystem *EcoSystem, MainInfo* MainInfo) {
  StochasticData* Stochasticdata;

  int print = 1;
  if (MainInfo->runNetwork()) {
    print = 0;  //no printing during a network run
    EcoSystem->Reset();

    #ifdef GADGET_NETWORK //to help compiling when pvm libraries are unavailable
      Stochasticdata = new StochasticData(MainInfo->runNetwork());
      while (Stochasticdata->getDataFromNet()) {
        EcoSystem->Update(Stochasticdata);
        EcoSystem->Simulate(MainInfo->runLikelihood(), print);
	//JMB - no printing during an network run ...
        Stochasticdata->SendDataToMaster(EcoSystem->getLikelihood());
        Stochasticdata->readNextLineFromNet();
      }
      delete Stochasticdata;
    #endif

  } else if (MainInfo->getInitialParamGiven()) {
    Stochasticdata = new StochasticData(MainInfo->getInitialParamFile());
    EcoSystem->Update(Stochasticdata);
    EcoSystem->checkBounds();
    EcoSystem->Reset();
    if (MainInfo->printInitial())
      EcoSystem->writeStatus(MainInfo->getPrintInitialFile());

    EcoSystem->Simulate(MainInfo->runLikelihood(), print);
    if ((MainInfo->getPI()).getPrint())
      EcoSystem->writeValues((MainInfo->getPI()).getOutputFile(), (MainInfo->getPI()).getPrecision());
    if ((MainInfo->getPI()).getPrintColumn())
      EcoSystem->writeValuesInColumns((MainInfo->getPI()).getColumnOutputFile(), (MainInfo->getPI()).getPrecision());
    while (Stochasticdata->DataIsLeft()) {
      Stochasticdata->readDataFromNextLine();
      EcoSystem->Update(Stochasticdata);
      EcoSystem->checkBounds();
      EcoSystem->Simulate(MainInfo->runLikelihood(), print);
      if ((MainInfo->getPI()).getPrint())
        EcoSystem->writeValues((MainInfo->getPI()).getOutputFile(), (MainInfo->getPI()).getPrecision());
      if ((MainInfo->getPI()).getPrintColumn())
        EcoSystem->writeValuesInColumns((MainInfo->getPI()).getColumnOutputFile(), (MainInfo->getPI()).getPrecision());
    }
    delete Stochasticdata;

  } else {
    handle.logWarning("Warning - no parameter input file given, using default values");
    EcoSystem->Reset();
    if (MainInfo->printInitial())
      EcoSystem->writeStatus(MainInfo->getPrintInitialFile());

    EcoSystem->Simulate(MainInfo->runLikelihood(), print);
    if ((MainInfo->getPI()).getPrint())
      EcoSystem->writeValues((MainInfo->getPI()).getOutputFile(), (MainInfo->getPI()).getPrecision());
    if ((MainInfo->getPI()).getPrintColumn())
      EcoSystem->writeValuesInColumns((MainInfo->getPI()).getColumnOutputFile(), (MainInfo->getPI()).getPrecision());
  }
}

int main(int aNumber, char* const aVector[]) {

  MainInfo MainInfo;
  OptInfo* Optinfo = 0;
  StochasticData* Stochasticdata = 0;
  int check = 0;

  //Test to see if the function double lgamma(double) is returning an integer.
  //lgamma is a non-ansi function and on some platforms when compiled with the
  //-ansi flag lgamma returns an integer value. [MNAA&AJ 05.2001]
  assert(lgamma(1.2) != floor(lgamma(1.2)));

  //JMB - changed to use GADGET_ .. instead of BORMICON_ ..
  char* workingdir = getenv("GADGET_WORKING_DIR");
  if (workingdir == 0) {
    if ((workingdir = (char*)malloc(LongString)) == NULL) {
      cerr << "Failed to malloc space for current working directory\n";
      exit(EXIT_FAILURE);
    }
    check = 1;
    if (getcwd(workingdir, LongString) == NULL) {
      cerr << "Failed to get current working directory - pathname too long\n";
      exit(EXIT_FAILURE);
    }
  }

  const char* inputdir = getenv("GADGET_DATA_DIR");
  if (inputdir == 0)
    inputdir = ".";

  if (chdir(inputdir) != 0) {
    cerr << "Failed to change working directory to\n" << inputdir << endl;
    exit(EXIT_FAILURE);
  }

  if (aNumber > 1)
    MainInfo.read(aNumber, aVector);

  //JMB - dont print output if doing a network run
  if (!(MainInfo.runNetwork())) {
    RUNID.print(cout);
    cout << "Starting Gadget from directory: " << workingdir
      << "\nusing data from directory: " << inputdir << endl;
  }
  handle.logMessage("Starting Gadget from directory:", workingdir);
  handle.logMessage("using data from directory:", inputdir);
  handle.logMessage("");  //write a blank line to the log file

  //Added MainInfo.Net to Ecosystem constructor, to let EcoSystem know if
  //we are doing a net run. 07.04.00 AJ
  EcoSystem = new Ecosystem(MainInfo.getMainGadgetFile(), MainInfo.runOptimize(),
    MainInfo.runNetwork(), MainInfo.runLikelihood(), inputdir, workingdir, MainInfo.getPI());

  #ifdef INTERRUPT_HANDLER
    registerInterrupt(SIGINT, &EcoSystem->interrupted);
  #endif

  chdir(workingdir);
  if (MainInfo.getOptInfoGiven()) {
    char type[MaxStrLength];
    strncpy(type, "", MaxStrLength);

    MainInfo.optInfoFile() >> ws >> type;
    if ((strcasecmp(type, "hookeandsimann") == 0) || (strcasecmp(type, "simannandhooke") == 0))
      Optinfo = new OptInfoHookeAndSimann();
    else if (strcasecmp(type, "hooke") == 0)
      Optinfo = new OptInfoHooke();
    else if (strcasecmp(type, "simann") == 0)
      Optinfo = new OptInfoSimann();
    //JMB - removed the BFGS stuff
    else
      handle.Unexpected("hooke, simann, or simannandhooke", type);

    Optinfo->read(MainInfo.optInfoFile());
    MainInfo.closeOptInfoFile();

  } else {
    Optinfo = new OptInfoHooke();
    if ((MainInfo.runOptimize()) && !(MainInfo.runNetwork()))
      handle.logWarning("Warning - no optimisation parameters file given, using default values");
  }

  if ((MainInfo.getPI()).getPrint())
    EcoSystem->writeInitialInformation((MainInfo.getPI()).getOutputFile());
  if ((MainInfo.getPI()).getPrintColumn())
    EcoSystem->writeInitialInformationInColumns((MainInfo.getPI()).getColumnOutputFile());
  if (MainInfo.runStochastic())
    stochasticRun(EcoSystem, &MainInfo);

  if (MainInfo.runOptimize()) {
    if (MainInfo.getInitialParamGiven()) {
      Stochasticdata = new StochasticData(MainInfo.getInitialParamFile());
      EcoSystem->Update(Stochasticdata);
      EcoSystem->checkBounds();
    } else
      handle.logWarning("Warning - no parameter input file given, using default values");

    EcoSystem->Reset();
    if (MainInfo.printInitial())
      EcoSystem->writeStatus(MainInfo.getPrintInitialFile());

    Optinfo->MaximizeLikelihood();
    if ((MainInfo.getPI()).getForcePrint())
      EcoSystem->Simulate(0, 1);  //print and dont optimise

    if (MainInfo.getInitialParamGiven())
      delete Stochasticdata;
  }

  if (MainInfo.printLikelihood()) {
    if (MainInfo.printOneComponent() == -1)
      EcoSystem->writeLikelihoodInformation(MainInfo.getPrintLikelihoodFile());
    else
      EcoSystem->writeLikelihoodInformation(MainInfo.getPrintLikelihoodFile(), MainInfo.printOneComponent());
  }

  if (MainInfo.printLikeSummary())
    EcoSystem->writeLikeSummaryInformation(MainInfo.getPrintLikeSummaryFile());
  if (MainInfo.printFinal())
    EcoSystem->writeStatus(MainInfo.getPrintFinalFile());

  //JMB - print final values of parameters
  if (!(MainInfo.runNetwork()))
    EcoSystem->writeParamsInColumns((MainInfo.getPI()).getParamOutFile(), (MainInfo.getPI()).getPrecision());

  if (check == 1)
    free(workingdir);
  delete Optinfo;
  delete EcoSystem;
  return EXIT_SUCCESS;
}
