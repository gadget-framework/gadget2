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
int FuncEval = 0;

void stochasticRun(Ecosystem *EcoSystem, MainInfo* MainInfo) {
  StochasticData* Stochasticdata;

  int print = 1;
  if (MainInfo->Net()) {
    print = 0;  //do not print in optimizing

    #ifdef GADGET_NETWORK //to help compiling when pvm libraries are unavailable
      Stochasticdata = new StochasticData(1);
      while (Stochasticdata->getDataFromNet()) {
        EcoSystem->Update(Stochasticdata);
        EcoSystem->Simulate(MainInfo->CalcLikelihood(), print);
        if ((MainInfo->getPI()).Print())
          EcoSystem->PrintValues((MainInfo->getPI()).getOutputFile(), (MainInfo->getPI()).getPrecision());
        if ((MainInfo->getPI()).PrintinColumns())
          EcoSystem->PrintValuesinColumns((MainInfo->getPI()).getColumnOutputFile(), (MainInfo->getPI()).getPrecision());
        Stochasticdata->SendDataToMaster(EcoSystem->Likelihood());
        Stochasticdata->readNextLineFromNet();
      }
      delete Stochasticdata;
    #endif

  } else if (MainInfo->InitCondGiven()) {
    Stochasticdata = new StochasticData(MainInfo->getInitialParamFilename());
    EcoSystem->Update(Stochasticdata);
    EcoSystem->CheckBounds();
    EcoSystem->Simulate(MainInfo->CalcLikelihood(), print);
    if ((MainInfo->getPI()).Print())
      EcoSystem->PrintValues((MainInfo->getPI()).getOutputFile(), (MainInfo->getPI()).getPrecision());
    if ((MainInfo->getPI()).PrintinColumns())
      EcoSystem->PrintValuesinColumns((MainInfo->getPI()).getColumnOutputFile(), (MainInfo->getPI()).getPrecision());
    while (Stochasticdata->DataIsLeft()) {
      Stochasticdata->readDataFromNextLine();
      EcoSystem->Update(Stochasticdata);
      EcoSystem->Simulate(MainInfo->CalcLikelihood(), print);
      if ((MainInfo->getPI()).Print())
        EcoSystem->PrintValues((MainInfo->getPI()).getOutputFile(), (MainInfo->getPI()).getPrecision());
      if ((MainInfo->getPI()).PrintinColumns())
        EcoSystem->PrintValuesinColumns((MainInfo->getPI()).getColumnOutputFile(), (MainInfo->getPI()).getPrecision());
    }
    delete Stochasticdata;

  } else {
    cerr << "Warning - no parameter input file given, using default values" << endl;
    EcoSystem->Simulate(MainInfo->CalcLikelihood(), print);
    if ((MainInfo->getPI()).Print())
      EcoSystem->PrintValues((MainInfo->getPI()).getOutputFile(), (MainInfo->getPI()).getPrecision());
    if ((MainInfo->getPI()).PrintinColumns())
      EcoSystem->PrintValuesinColumns((MainInfo->getPI()).getColumnOutputFile(), (MainInfo->getPI()).getPrecision());
  }
}

int main(int aNumber, char* const aVector[]) {

  MainInfo MainInfo;
  OptInfo* Optinfo = 0;
  StochasticData* Stochasticdata = 0;
  ErrorHandler handle;
  int check = 0;

  //Test to see if the function double lgamma(double) is returning an integer.
  //lgamma is a non-ansi function and on some platforms when compiled with the
  //-ansi flag lgamma returns an integer value. [MNAA&AJ 05.2001]
  assert(lgamma(1.2) != floor(lgamma(1.2)));

  //JMB - Changed to use GADGET_ .. instead of BORMICON_ ..
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
  if (!(MainInfo.Net())) {
    RUNID.print(cout);
    cout << "Starting Gadget from directory: " << workingdir
      << "\nusing data from directory: " << inputdir << endl;
  }

  //Added MainInfo.Net to Ecosystem constructor, to let EcoSystem know if
  //we are doing a net run. 07.04.00 AJ
  EcoSystem = new Ecosystem(MainInfo.getMainGadgetFilename(), MainInfo.Optimize(),
    MainInfo.Net(), MainInfo.CalcLikelihood(), inputdir, workingdir, MainInfo.getPI());

  #ifdef INTERRUPT_HANDLER
    registerInterrupt(SIGINT, &EcoSystem->interrupted);
  #endif

  chdir(workingdir);
  if (MainInfo.OptInfoGiven()) {
    char type[MaxStrLength];
    strncpy(type, "", MaxStrLength);

    MainInfo.optFile() >> ws >> type;
    if ((strcasecmp(type, "hookeandsimann") == 0) || (strcasecmp(type, "simannandhooke") == 0))
      Optinfo = new OptInfoHookeAndSimann();
    else if (strcasecmp(type, "hooke") == 0)
      Optinfo = new OptInfoHooke();
    else if (strcasecmp(type, "simann") == 0)
      Optinfo = new OptInfoSimann();
    //JMB - removed the BFGS stuff
    else
      handle.Unexpected("hooke, simann, or simannandhooke", type);

    Optinfo->read(MainInfo.optFile());
    MainInfo.CloseOptinfofile();

  } else {
    Optinfo = new OptInfoHooke();
    //JMB - no optinfo file specified so only use default values
  }

  if (MainInfo.printInitial())
    EcoSystem->PrintStatus(MainInfo.getPrintInitialCondFilename());
  if ((MainInfo.getPI()).Print())
    EcoSystem->PrintInitialInformation((MainInfo.getPI()).getOutputFile());
  if ((MainInfo.getPI()).PrintinColumns())
    EcoSystem->PrintInitialInformationinColumns((MainInfo.getPI()).getColumnOutputFile());
  if (MainInfo.Stochastic())
    stochasticRun(EcoSystem, &MainInfo);

  if (MainInfo.Optimize()) {
    if (MainInfo.InitCondGiven()) {
      Stochasticdata = new StochasticData(MainInfo.getInitialParamFilename());
      EcoSystem->Update(Stochasticdata);
      EcoSystem->CheckBounds();
    } else
      cerr << "Warning - no parameter input file given, using default values" << endl;

    Optinfo->MaximizeLikelihood();
    if ((MainInfo.getPI()).getForcePrint())
      EcoSystem->Simulate(0, 1);

    if (MainInfo.InitCondGiven())
      delete Stochasticdata;
  }

  if (MainInfo.printLikelihood())
    EcoSystem->PrintLikelihoodInfo(MainInfo.getPrintLikelihoodFilename());
  if (MainInfo.printFinal())
    EcoSystem->PrintStatus(MainInfo.getPrintFinalCondFilename());

  //JMB - print final values of parameters
  if (!(MainInfo.Net()))
    EcoSystem->PrintParamsinColumns((MainInfo.getPI()).getParamOutFile(), (MainInfo.getPI()).getPrecision());

  if (check == 1)
    free(workingdir);
  delete Optinfo;
  delete EcoSystem;
  return EXIT_SUCCESS;
}
