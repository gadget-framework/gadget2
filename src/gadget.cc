#include "ecosystem.h"
#include "maininfo.h"
#include "runid.h"
#include "gadget.h"
#include "errorhandler.h"
#include "stochasticdata.h"
#include "interrupthandler.h"
#include "global.h"

Ecosystem* EcoSystem;


int main(int aNumber, char* const aVector[]) {

  MainInfo main;
  StochasticData* data = 0;
  int check = 0;

  //Initialise random number generator with system time [MNAA 02.02.26]
  srand((int)time(NULL));

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
  if (chdir(workingdir) != 0)
    handle.logMessage(LOGFAIL, "Error - failed to change working directory to", workingdir);

  char* inputdir = getenv("GADGET_DATA_DIR");
  if (inputdir == 0)
    inputdir = workingdir;
  if (chdir(inputdir) != 0)
    handle.logMessage(LOGFAIL, "Error - failed to change input directory to", inputdir);
  if (chdir(workingdir) != 0) //JMB change back to where we were ...
    handle.logMessage(LOGFAIL, "Error - failed to change working directory to", workingdir);

  main.read(aNumber, aVector);
  main.checkUsage(inputdir, workingdir);

  if (chdir(inputdir) != 0)
    handle.logMessage(LOGFAIL, "Error - failed to change input directory to", inputdir);
  EcoSystem = new Ecosystem(main);

#ifdef INTERRUPT_HANDLER
  //JMB dont register interrupt if doing a network run
  if (!(main.runNetwork()))
    registerInterrupts(&EcoSystem->interrupted);
#endif

  if (chdir(workingdir) != 0)
    handle.logMessage(LOGFAIL, "Error - failed to change working directory to", workingdir);
  if ((main.getPI()).getPrint())
    EcoSystem->writeInitialInformation((main.getPI()).getOutputFile());

  if (main.runStochastic()) {
    if (main.runNetwork()) {
#ifdef GADGET_NETWORK //to help compiling when pvm libraries are unavailable
      EcoSystem->Initialise();
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
      if (chdir(inputdir) != 0) //JMB need to change back to inputdir to read the file
        handle.logMessage(LOGFAIL, "Error - failed to change input directory to", inputdir);
      data = new StochasticData(main.getInitialParamFile());
      if (chdir(workingdir) != 0)
        handle.logMessage(LOGFAIL, "Error - failed to change working directory to", workingdir);

      EcoSystem->Update(data);
      EcoSystem->checkBounds();

      EcoSystem->Initialise();
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
        //EcoSystem->checkBounds();
        EcoSystem->Simulate(main.runPrint());
        if ((main.getPI()).getPrint())
          EcoSystem->writeValues();
      }
      delete data;

    } else {
      if (EcoSystem->numVariables() != 0)
        handle.logMessage(LOGWARN, "Warning - no parameter input file given, using default values");

      EcoSystem->Initialise();
      if (main.printInitial()) {
        EcoSystem->Reset();  //JMB only need to call reset() before the print commands
        EcoSystem->writeStatus(main.getPrintInitialFile());
      }

      EcoSystem->Simulate(main.runPrint());
      if ((main.getPI()).getPrint())
        EcoSystem->writeValues();
    }

  } else if (main.runOptimise()) {
    if (EcoSystem->numVariables() == 0)
      handle.logMessage(LOGFAIL, "Error - no parameters can be optimised");

    if (main.getInitialParamGiven()) {
      if (chdir(inputdir) != 0) //JMB need to change back to inputdir to read the file
        handle.logMessage(LOGFAIL, "Error - failed to change input directory to", inputdir);
      data = new StochasticData(main.getInitialParamFile());
      if (chdir(workingdir) != 0)
        handle.logMessage(LOGFAIL, "Error - failed to change working directory to", workingdir);

      EcoSystem->Update(data);
      EcoSystem->checkBounds();
      delete data;
    } else
      handle.logMessage(LOGFAIL, "Error - no parameter input file specified");

    EcoSystem->Initialise();
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

  //JMB print final values of parameters
  if (!(main.runNetwork()))
    EcoSystem->writeParams((main.getPI()).getParamOutFile(), (main.getPI()).getPrecision());

  if (check)
    free(workingdir);

  delete EcoSystem;
  handle.logFinish();
  return EXIT_SUCCESS;
}
