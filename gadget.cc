#include "ecosystem.h"
#include "maininfo.h"
#include "runid.h"
#include "gadget.h"
#include "errorhandler.h"
#include "stochasticdata.h"
#include "interrupthandler.h"

RunID RUNID;
Ecosystem* EcoSystem;
ErrorHandler handle;

int main(int aNumber, char* const aVector[]) {

  MainInfo main;
  StochasticData* data = 0;
  int check = 0;

  //Initialise random number generator with system time [MNAA 02.02.26]
  srand(time(NULL));

  //Test to see if the function double lgamma(double) is returning an integer.
  //lgamma is a non-ansi function and on some platforms when compiled with the
  //-ansi flag lgamma returns an integer value. [MNAA&AJ 05.2001]
  assert(lgamma(1.2) != floor(lgamma(1.2)));

  char* workingdir = getenv("GADGET_WORKING_DIR");
  if (workingdir == 0) {
    if ((workingdir = (char*)malloc(LongString)) == NULL)
      handle.logMessage(LOGFAIL, "Error - failed to malloc space for current working directory");
    check = 1;
    if (getcwd(workingdir, LongString) == NULL)
      handle.logMessage(LOGFAIL, "Error - failed to get current working directory");
  }

  char* inputdir = getenv("GADGET_DATA_DIR");
  if (inputdir == 0)
    inputdir = workingdir;

  if (chdir(inputdir) != 0)
    handle.logMessage(LOGFAIL, "Error - failed change working directory to", inputdir);

  if (aNumber > 1)
    main.read(aNumber, aVector);

  //JMB - dont print output if doing a network run
  if (!(main.runNetwork()))
    RUNID.Print(cout);
  main.checkUsage(inputdir, workingdir);

  if (aNumber == 1)
    handle.logMessage(LOGWARN, "Warning - no command line options specified, using default values");

  EcoSystem = new Ecosystem(main, inputdir, workingdir);
  handle.logMessage(LOGMESSAGE, "");  //write blank line to log file

  #ifdef INTERRUPT_HANDLER
    //JMB - dont register interrupt if doing a network run
    if (!(main.runNetwork()))
      registerInterrupt(SIGINT, &EcoSystem->interrupted);
  #endif

  chdir(workingdir);
  if ((main.getPI()).getPrint())
    EcoSystem->writeInitialInformation((main.getPI()).getOutputFile());

  if (main.runStochastic()) {
    if (main.runNetwork()) {
      #ifdef GADGET_NETWORK //to help compiling when pvm libraries are unavailable
        data = new StochasticData();
        while (data->getDataFromNetwork()) {
          EcoSystem->Update(data);
          EcoSystem->Simulate(main.runPrint());
          data->sendDataToNetwork(EcoSystem->getLikelihood());
          data->readNextLineFromNetwork();
        }
        delete data;
      #endif

    } else if (main.getInitialParamGiven()) {
      data = new StochasticData(main.getInitialParamFile());
      EcoSystem->Update(data);
      EcoSystem->checkBounds();

      if (main.printInitial()) {
        EcoSystem->Reset();  //JMB only need to call reset() before the print commands
        EcoSystem->writeStatus(main.getPrintInitialFile());
      }

      EcoSystem->Simulate(main.runPrint());
      if ((main.getPI()).getPrint())
        EcoSystem->writeValues();
      while (data->isDataLeft()) {
        data->readNextLine();
        EcoSystem->Update(data);
        EcoSystem->checkBounds();
        EcoSystem->Simulate(main.runPrint());
        if ((main.getPI()).getPrint())
          EcoSystem->writeValues();
      }
      delete data;

    } else {
      if (EcoSystem->numVariables() != 0)
        handle.logMessage(LOGWARN, "Warning - no parameter input file given, using default values");

      if (main.printInitial()) {
        EcoSystem->Reset();  //JMB only need to call reset() before the print commands
        EcoSystem->writeStatus(main.getPrintInitialFile());
      }

      EcoSystem->Simulate(main.runPrint());
      if ((main.getPI()).getPrint())
        EcoSystem->writeValues();
    }

  } else if (main.runOptimise()) {
    if (main.getInitialParamGiven()) {
      data = new StochasticData(main.getInitialParamFile());
      EcoSystem->Update(data);
      EcoSystem->checkBounds();
      delete data;
    } else
      handle.logMessage(LOGWARN, "Error - no parameter input file specified");

    if (main.printInitial()) {
      EcoSystem->Reset();  //JMB only need to call reset() before the print commands
      EcoSystem->writeStatus(main.getPrintInitialFile());
    }

    EcoSystem->Optimise();
    if (main.getForcePrint())
      EcoSystem->Simulate(main.getForcePrint());
  }

  handle.logMessage(LOGMESSAGE, "");  //write blank line to log file
  if (main.printFinal() && !(main.runNetwork()))
    EcoSystem->writeStatus(main.getPrintFinalFile());

  //JMB - print final values of parameters
  if (!(main.runNetwork()))
    EcoSystem->writeParams((main.getPI()).getParamOutFile(), (main.getPI()).getPrecision());

  if (check == 1)
    free(workingdir);

  delete EcoSystem;
  handle.logFinish();
  return EXIT_SUCCESS;
}
