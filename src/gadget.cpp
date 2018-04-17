#include "ecosystem.h"
#include "maininfo.h"
#include "runid.h"
#include "gadget.h"
#include "errorhandler.h"
#include "stochasticdata.h"
#include "interrupthandler.h"
#include "global.h"
#include <Rcpp.h>

Ecosystem* EcoSystem;

MainInfo mainGlobal;
StochasticData* data;
int check;
char* workingdir;

// [[Rcpp::export]]
Rcpp::IntegerVector wholeSim(){
      EcoSystem->Simulate(mainGlobal.runPrint());
      if ((mainGlobal.getPI()).getPrint())
        EcoSystem->writeValues();

      while (data->isDataLeft()) {
        data->readNextLine();
        EcoSystem->Update(data);
        //EcoSystem->checkBounds();
        EcoSystem->Simulate(mainGlobal.runPrint());
        if ((mainGlobal.getPI()).getPrint())
          EcoSystem->writeValues();
      }
      delete data;
      return Rcpp::IntegerVector(1,0);
}

// [[Rcpp::export]]
Rcpp::IntegerVector updateAmountStep(Rcpp::IntegerVector fleetNo, Rcpp::IntegerVector step, Rcpp::IntegerVector area, Rcpp::NumericVector value){

   int fN = fleetNo[0] - 1;
   int st = step[0];
   int ar = area[0] - 1;

   double val = value[0];

   int maxFleet = 0;
   int maxSteps = 0;
   int maxArea = 0;

   FleetPtrVector& fleetvec = EcoSystem->getModelFleetVector();

   maxFleet = fleetvec.Size();

   if(fN < 0 ||fN > maxFleet-1)
	   return Rcpp::IntegerVector(1, 55);

   Fleet *fleet = fleetvec[fN];
	
   FormulaMatrix& amount = fleet->getAmount();

   std::cout << "Change fleet \"" << fleet->getName() << "\" - Step: " << st << " - Area: " << ar + 1 << " with " << val << std::endl;

   //std::cout << "Row Size " << amount.Nrow() << std::endl; 

   maxSteps = amount.Nrow();

   if(st < 1 || st > maxSteps - 1)
           return Rcpp::IntegerVector(1, 55);

   //std::cout << "Column size for " << st << " " << amount.Ncol(st) << std::endl;

   maxArea = amount.Ncol(st);

   if(ar < 0 || ar > maxArea-1)
           return Rcpp::IntegerVector(1, 55);

   Formula& vec = amount[st][ar];

   std::cout << "Value before " << (double) vec << std::endl;

   vec.setValue(val);

   std::cout << "Value after " << (double) vec << std::endl;

   return  Rcpp::IntegerVector(1, 0);
}

// [[Rcpp::export]]
Rcpp::IntegerVector updateAmountYear(Rcpp::IntegerVector fleetNo, Rcpp::IntegerVector year, Rcpp::IntegerVector step, Rcpp::IntegerVector area, Rcpp::NumericVector value){

   TimeClass* TimeInfo = EcoSystem->getTimeInfo();

   int timeid;

   int yy = year[0];
   int ss = step[0];

   if (TimeInfo->isWithinPeriod(yy, ss))
      timeid = TimeInfo->calcSteps(yy, ss);
   else
      return Rcpp::IntegerVector(1, 55);

   std::cout << "Step is" << timeid << std::endl;

   Rcpp::IntegerVector timeidvec(1,timeid);

   return updateAmountStep(fleetNo, timeidvec, area, value);
}

Rcpp::IntegerVector getEcosystemTime(Ecosystem* e, int res){
   return
   Rcpp::IntegerVector::create(Rcpp::_["currentTime"] = e->getCurrentTime(),
                               Rcpp::_["currentYear"] = e->getCurrentYear(),
                               Rcpp::_["currentStep"] = e->getCurrentStep(),
                               Rcpp::_["totalSteps"] = e->numTotalSteps(),
			       Rcpp::_["finished"] = res);
}

// [[Rcpp::export]]
Rcpp::IntegerVector initSim(){
   EcoSystem->initSimulation();
   return getEcosystemTime(EcoSystem, 0);
}

// [[Rcpp::export]]
Rcpp::IntegerVector stepSim(){
   int res;
   res = EcoSystem->stepSimulation(mainGlobal.runPrint());
   return getEcosystemTime(EcoSystem, res);
}

// [[Rcpp::export]]
Rcpp::IntegerVector yearSim(){
   int res;
   res = EcoSystem->yearSimulation(mainGlobal.runPrint());
   return getEcosystemTime(EcoSystem, res);
}


// [[Rcpp::export]]
Rcpp::IntegerVector finalizeSim(){
   EcoSystem->finalizeSimulation();
   if ((mainGlobal.getPI()).getPrint())
        EcoSystem->writeValues();
   while (data->isDataLeft()) {
        data->readNextLine();
        EcoSystem->Update(data);
        //EcoSystem->checkBounds();
        EcoSystem->Simulate(mainGlobal.runPrint());
        if ((mainGlobal.getPI()).getPrint())
          EcoSystem->writeValues();
   }
   delete data;
   return Rcpp::IntegerVector(1, 0);
}

// [[Rcpp::export]]
Rcpp::List finalize(){
  handle.logMessage(LOGMESSAGE, "");  //write blank line to log file
  if (mainGlobal.printFinal() && !(mainGlobal.runNetwork()))
    EcoSystem->writeStatus(mainGlobal.getPrintFinalFile());

  //JMB print final values of parameters
  if (!(mainGlobal.runNetwork()))
    EcoSystem->writeParams((mainGlobal.getPI()).getParamOutFile(), (mainGlobal.getPI()).getPrecision());

  if (check)
    free(workingdir);

  Rcpp::List z = Rcpp::clone(EcoSystem->rdata);
  delete EcoSystem;
  handle.logFinish();
  //return EXIT_SUCCESS;
  //
  return z;
}

// [[Rcpp::export]]
Rcpp::List gadget(Rcpp::StringVector args) {

  //Get parameter number 
  int aNumber = args.size() + 1;

  char** aVector = new char* [255];

  //Convert args to aVector
  for(int i=0; i<args.size(); i++){
	char *tmp = new char [args(i).size() + 1]; 
	strcpy(tmp, Rcpp::as< std::string >(args(i)).c_str());
	aVector[i+1] = tmp;
  }

  data = 0;
  check = 0;

  //Initialise random number generator with system time [MNAA 02.02.26]
  srand((int)time(NULL));

  //Test to see if the function double lgamma(double) is returning an integer.
  //lgamma is a non-ansi function and on some platforms when compiled with the
  //-ansi flag lgamma returns an integer value. [MNAA&AJ 05.2001]
  assert(lgamma(1.2) != floor(lgamma(1.2)));

  workingdir = getenv("GADGET_WORKING_DIR");
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

  mainGlobal.read(aNumber, aVector);
  mainGlobal.checkUsage(inputdir, workingdir);

  if (chdir(inputdir) != 0)
    handle.logMessage(LOGFAIL, "Error - failed to change input directory to", inputdir);
  EcoSystem = new Ecosystem(mainGlobal);

#ifdef INTERRUPT_HANDLER
  //JMB dont register interrupt if doing a network run
  if (!(mainGlobal.runNetwork()))
    registerInterrupts(&EcoSystem->interrupted);
#endif

  if (chdir(workingdir) != 0)
    handle.logMessage(LOGFAIL, "Error - failed to change working directory to", workingdir);
  if ((mainGlobal.getPI()).getPrint())
    EcoSystem->writeInitialInformation((mainGlobal.getPI()).getOutputFile());

  if (mainGlobal.runStochastic()) {
    if (mainGlobal.runNetwork()) {
#ifdef GADGET_NETWORK //to help compiling when pvm libraries are unavailable
      EcoSystem->Initialise();
      data = new StochasticData();
      while (data->getDataFromNetwork()) {
        EcoSystem->Update(data);
        EcoSystem->Simulate(mainGlobal.runPrint());
        data->sendDataToNetwork(EcoSystem->getLikelihood());
        data->readNextLineFromNetwork();
      }
      delete data;
#endif

    } else if (mainGlobal.getInitialParamGiven()) {
      if (chdir(inputdir) != 0) //JMB need to change back to inputdir to read the file
        handle.logMessage(LOGFAIL, "Error - failed to change input directory to", inputdir);
      data = new StochasticData(mainGlobal.getInitialParamFile());
      if (chdir(workingdir) != 0)
        handle.logMessage(LOGFAIL, "Error - failed to change working directory to", workingdir);

      EcoSystem->Update(data);
      EcoSystem->checkBounds();

      EcoSystem->Initialise();
      if (mainGlobal.printInitial()) {
        EcoSystem->Reset();  //JMB only need to call reset() before the print commands
        EcoSystem->writeStatus(mainGlobal.getPrintInitialFile());
      }

      // IU: Try to exit here
      return R_NilValue;

      EcoSystem->Simulate(mainGlobal.runPrint());
      if ((mainGlobal.getPI()).getPrint())
        EcoSystem->writeValues();

      while (data->isDataLeft()) {
        data->readNextLine();
        EcoSystem->Update(data);
        //EcoSystem->checkBounds();
        EcoSystem->Simulate(mainGlobal.runPrint());
        if ((mainGlobal.getPI()).getPrint())
          EcoSystem->writeValues();
      }
      delete data;

    } else {
      if (EcoSystem->numVariables() != 0)
        handle.logMessage(LOGWARN, "Warning - no parameter input file given, using default values");

      EcoSystem->Initialise();
      if (mainGlobal.printInitial()) {
        EcoSystem->Reset();  //JMB only need to call reset() before the print commands
        EcoSystem->writeStatus(mainGlobal.getPrintInitialFile());
      }

      EcoSystem->Simulate(mainGlobal.runPrint());
      if ((mainGlobal.getPI()).getPrint())
        EcoSystem->writeValues();
    }

  } else if (mainGlobal.runOptimise()) {
    if (EcoSystem->numVariables() == 0)
      handle.logMessage(LOGFAIL, "Error - no parameters can be optimised");

    if (mainGlobal.getInitialParamGiven()) {
      if (chdir(inputdir) != 0) //JMB need to change back to inputdir to read the file
        handle.logMessage(LOGFAIL, "Error - failed to change input directory to", inputdir);
      data = new StochasticData(mainGlobal.getInitialParamFile());
      if (chdir(workingdir) != 0)
        handle.logMessage(LOGFAIL, "Error - failed to change working directory to", workingdir);

      EcoSystem->Update(data);
      EcoSystem->checkBounds();
      delete data;
    } else
      handle.logMessage(LOGFAIL, "Error - no parameter input file specified");

    EcoSystem->Initialise();
    if (mainGlobal.printInitial()) {
      EcoSystem->Reset();  //JMB only need to call reset() before the print commands
      EcoSystem->writeStatus(mainGlobal.getPrintInitialFile());
    }

    EcoSystem->Optimise();
    if (mainGlobal.getForcePrint())
      EcoSystem->Simulate(mainGlobal.getForcePrint());
  }

  handle.logMessage(LOGMESSAGE, "");  //write blank line to log file
  if (mainGlobal.printFinal() && !(mainGlobal.runNetwork()))
    EcoSystem->writeStatus(mainGlobal.getPrintFinalFile());

  //JMB print final values of parameters
  if (!(mainGlobal.runNetwork()))
    EcoSystem->writeParams((mainGlobal.getPI()).getParamOutFile(), (mainGlobal.getPI()).getPrecision());

  if (check)
    free(workingdir);

  Rcpp::List z = Rcpp::clone(EcoSystem->rdata);
  delete EcoSystem;
  handle.logFinish();
  //return EXIT_SUCCESS;
  //
  return z;
}
