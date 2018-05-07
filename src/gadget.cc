#include "ecosystem.h"
#include "maininfo.h"
#include "runid.h"
#include "gadget.h"
#include "errorhandler.h"
#include "stochasticdata.h"
#include "interrupthandler.h"
#include "global.h"
#ifdef _OPENMP
#include <omp.h>
#endif

Ecosystem* EcoSystem;
StochasticData* data = 0;
Ecosystem** EcoSystems;
MainInfo _main;

volatile int interrupted = 0;
volatile int interrupted_print = 0;


int main(int aNumber, char* const aVector[]) {
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

  _main.read(aNumber, aVector);
  _main.checkUsage(inputdir, workingdir);

  if (chdir(inputdir) != 0)
    handle.logMessage(LOGFAIL, "Error - failed to change input directory to", inputdir);
  EcoSystem = new Ecosystem(_main);

#ifdef INTERRUPT_HANDLER
  //JMB dont register interrupt if doing a network run
  if (!(_main.runNetwork()))
    registerInterrupts(&interrupted);
#endif

  if (chdir(workingdir) != 0)
    handle.logMessage(LOGFAIL, "Error - failed to change working directory to", workingdir);
  if ((_main.getPI()).getPrint())
    EcoSystem->writeInitialInformation((_main.getPI()).getOutputFile());

  if (_main.runStochastic()) {
    if (_main.runNetwork()) {
#ifdef GADGET_NETWORK //to help compiling when pvm libraries are unavailable
      EcoSystem->Initialise();
      data = new StochasticData();
      while (data->getDataFromNetwork()) {
        EcoSystem->Update(data);
        EcoSystem->Simulate(_main.runPrint());
        data->sendDataToNetwork(EcoSystem->getLikelihood());
        data->readNextLineFromNetwork();
      }
      delete data;
#endif

    } else if (_main.getInitialParamGiven()) {
      if (chdir(inputdir) != 0) //JMB need to change back to inputdir to read the file
        handle.logMessage(LOGFAIL, "Error - failed to change input directory to", inputdir);
      data = new StochasticData(_main.getInitialParamFile());
      if (chdir(workingdir) != 0)
        handle.logMessage(LOGFAIL, "Error - failed to change working directory to", workingdir);

      EcoSystem->Update(data);
      EcoSystem->checkBounds();

      EcoSystem->Initialise();
      if (_main.printInitial()) {
        EcoSystem->Reset();  //JMB only need to call reset() before the print commands
        EcoSystem->writeStatus(_main.getPrintInitialFile());
      }

      EcoSystem->Simulate(_main.runPrint());
      if ((_main.getPI()).getPrint())
        EcoSystem->writeValues();

      while (data->isDataLeft()) {
        data->readNextLine();
        EcoSystem->Update(data);
        //EcoSystem->checkBounds();
        EcoSystem->Simulate(_main.runPrint());
        if ((_main.getPI()).getPrint())
          EcoSystem->writeValues();
      }
      delete data;

    } else {
      if (EcoSystem->numVariables() != 0)
        handle.logMessage(LOGWARN, "Warning - no parameter input file given, using default values");

      EcoSystem->Initialise();
      if (_main.printInitial()) {
        EcoSystem->Reset();  //JMB only need to call reset() before the print commands
        EcoSystem->writeStatus(_main.getPrintInitialFile());
      }

      EcoSystem->Simulate(_main.runPrint());
      if ((_main.getPI()).getPrint())
        EcoSystem->writeValues();
    }

  } else if (_main.runOptimise()) {
	  if (EcoSystem->numVariables() == 0)
		  handle.logMessage(LOGFAIL, "Error - no parameters can be optimised");

	  if (_main.getInitialParamGiven()) {
		  if (chdir(inputdir) != 0) //JMB need to change back to inputdir to read the file
			handle.logMessage(LOGFAIL, "Error - failed to change input directory to", inputdir);
		  data = new StochasticData(_main.getInitialParamFile());
		  if (chdir(workingdir) != 0)
			handle.logMessage(LOGFAIL, "Error - failed to change working directory to", workingdir);

		  EcoSystem->Update(data);
		  EcoSystem->checkBounds();
		} else
		  handle.logMessage(LOGFAIL, "Error - no parameter input file specified");

	  EcoSystem->Initialise();
	  if (_main.printInitial()) {
		  EcoSystem->Reset();  //JMB only need to call reset() before the print commands
		  EcoSystem->writeStatus(_main.getPrintInitialFile());
	  	  }

#ifdef _OPENMP
          if (_main.runParallel()){
	  	int numThr = omp_get_max_threads ( );
		handle.logMessage(LOGINFO, "Info - initialise ", numThr," EcoSystems ");
	  	EcoSystems = new Ecosystem*[numThr];
	  	int i;
#pragma omp parallel for shared(EcoSystems,_main,data,handle)
	  	for (i=0; i<numThr; i++)
	  	{
		        handle.logMessage(LOGINFO, "Info - Ecosystem ", i);
	                if (_main.getInitialParamGiven()) {
		  	    EcoSystems[i] = new Ecosystem(_main);
		  	    EcoSystems[i]->Update(data);
		            EcoSystems[i]->checkBounds();
			}
		  	EcoSystems[i]->Initialise();
	  	}
	  	}
#endif
	  EcoSystem->Optimise();
	  delete data;
	  if (_main.getForcePrint())
		  EcoSystem->Simulate(_main.getForcePrint());
  }

  handle.logMessage(LOGMESSAGE, "");  //write blank line to log file
  if (_main.printFinal() && !(_main.runNetwork()))
    EcoSystem->writeStatus(_main.getPrintFinalFile());

  //JMB print final values of parameters
  if (!(_main.runNetwork()))
    EcoSystem->writeParams((_main.getPI()).getParamOutFile(), (_main.getPI()).getPrecision());

  if (check)
    free(workingdir);

  delete EcoSystem;
  handle.logFinish();
  return EXIT_SUCCESS;
}
