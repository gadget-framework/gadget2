#include "ecosystem.h"
#include "maininfo.h"
#include "runid.h"
#include "gadget.h"
#include "optinfo.h"
#include "errorhandler.h"
#include "interrupthandler.h"

RunID RUNID;
Ecosystem* EcoSystem;
ErrorHandler handle;
int FuncEval = 0;

int main(int aNumber, char* const aVector[]) {

  MainInfo main;
  OptInfo* opt = 0;
  StochasticData* data = 0;
  int check = 0;

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

  #ifdef INTERRUPT_HANDLER
    //JMB - dont register interrupt if doing a network run
    if (!(main.runNetwork()))
      registerInterrupt(SIGINT, &EcoSystem->interrupted);
  #endif

  chdir(workingdir);
  if ((main.getPI()).getPrint())
    EcoSystem->writeInitialInformation((main.getPI()).getOutputFile());
  if ((main.getPI()).getPrintColumn())
    EcoSystem->writeInitialInformationInColumns((main.getPI()).getColumnOutputFile());

  if (main.runStochastic()) {
    if (main.runNetwork()) {
      EcoSystem->Reset();
      #ifdef GADGET_NETWORK //to help compiling when pvm libraries are unavailable
        data = new StochasticData();
        while (data->getDataFromNetwork()) {
          EcoSystem->Update(data);
          EcoSystem->Simulate(main.runLikelihood(), main.runPrint());
          data->sendDataToNetwork(EcoSystem->getLikelihood());
          data->readNextLineFromNetwork();
        }
        delete data;
      #endif

    } else if (main.getInitialParamGiven()) {
      data = new StochasticData(main.getInitialParamFile());
      EcoSystem->Update(data);
      EcoSystem->checkBounds();
      EcoSystem->Reset();
      if (main.printInitial())
        EcoSystem->writeStatus(main.getPrintInitialFile());
      EcoSystem->Simulate(main.runLikelihood(), main.runPrint());
      if ((main.getPI()).getPrint())
        EcoSystem->writeValues();
      if ((main.getPI()).getPrintColumn())
        EcoSystem->writeValuesInColumns();
      while (data->isDataLeft()) {
        data->readNextLine();
        EcoSystem->Update(data);
        EcoSystem->checkBounds();
        EcoSystem->Simulate(main.runLikelihood(), main.runPrint());
        if ((main.getPI()).getPrint())
          EcoSystem->writeValues();
        if ((main.getPI()).getPrintColumn())
          EcoSystem->writeValuesInColumns();
      }
      delete data;

    } else {
      if (EcoSystem->numOptVariables() != 0)
        handle.logMessage(LOGWARN, "Warning - no parameter input file given, using default values");
      EcoSystem->Reset();
      if (main.printInitial())
        EcoSystem->writeStatus(main.getPrintInitialFile());
      EcoSystem->Simulate(main.runLikelihood(), main.runPrint());
      if ((main.getPI()).getPrint())
        EcoSystem->writeValues();
      if ((main.getPI()).getPrintColumn())
        EcoSystem->writeValuesInColumns();
    }

  } else if (main.runOptimise()) {
    if (main.getInitialParamGiven()) {
      data = new StochasticData(main.getInitialParamFile());
      EcoSystem->Update(data);
      EcoSystem->checkBounds();
    } else
      handle.logMessage(LOGWARN, "Warning - no parameter input file given, using default values");

    EcoSystem->Reset();
    if (main.printInitial())
      EcoSystem->writeStatus(main.getPrintInitialFile());

    opt = new OptInfo(&main);
    opt->Optimise();
    delete opt;

    if ((main.getPI()).getForcePrint())
      EcoSystem->Simulate(0, 1);  //print and dont optimise

    if (main.getInitialParamGiven())
      delete data;
  }

  handle.logMessage(LOGMESSAGE, "");  //write blank line to log file
  if (main.printFinal())
    EcoSystem->writeStatus(main.getPrintFinalFile());

  //JMB - print final values of parameters
  if (!(main.runNetwork()))
    EcoSystem->writeParams((main.getPI()).getParamOutFile(), (main.getPI()).getPrecision());

  if (check == 1)
    free(workingdir);

  delete EcoSystem;
  handle.logFinish(main.runOptimise());
  return EXIT_SUCCESS;
}
