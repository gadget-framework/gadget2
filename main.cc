#include "ecosystem.h"
#include "mainfiles.h"
#include "runid.h"
#include "gadget.h"

#ifdef INTERRUPT_HANDLER
#include "interrupthandler.h"
#endif

//Ecosystem must be global due to the optimization functions.
RunId RUNID;
Ecosystem* EcoSystem;
int FuncEval = 0;

void StochasticRun(Ecosystem *EcoSystem, MainInfo* MainInfo) {
  StochasticData* Stochasticdata;

  int print = 1;
  if (MainInfo->Net) {
    print = 0;  //do not print in optimizing

    #ifdef GADGET_NETWORK //to help compiling when pvm libraries are unavailable
      Stochasticdata = new StochasticData(1);
      while (Stochasticdata->getDataFromNet()) {
        EcoSystem->Update(Stochasticdata);
        EcoSystem->Simulate(MainInfo->CalcLikelihood, print);
        if ((MainInfo->printinfo).Print())
          EcoSystem->PrintValues((MainInfo->printinfo).OutputFile, (MainInfo->printinfo).givenPrecision);
        if ((MainInfo->printinfo).PrintinColumns())
          EcoSystem->PrintValuesinColumns((MainInfo->printinfo).ColumnOutputFile, (MainInfo->printinfo).givenPrecision);
        Stochasticdata->SendDataToMaster(EcoSystem->Likelihood());
        Stochasticdata->ReadNextLineFromNet();
      }
      delete Stochasticdata;
    #endif

  } else if (MainInfo->InitialCondareGiven) {
    Stochasticdata = new StochasticData(MainInfo->InitialCommentFilename);
    EcoSystem->Update(Stochasticdata);
    EcoSystem->Simulate(MainInfo->CalcLikelihood, print);
    if ((MainInfo->printinfo).Print())
      EcoSystem->PrintValues((MainInfo->printinfo).OutputFile, (MainInfo->printinfo).givenPrecision);
    if ((MainInfo->printinfo).PrintinColumns())
      EcoSystem->PrintValuesinColumns((MainInfo->printinfo).ColumnOutputFile, (MainInfo->printinfo).givenPrecision);
    while (Stochasticdata->DataIsLeft()) {
      Stochasticdata->ReadDataFromNextLine();
      EcoSystem->Update(Stochasticdata);
      EcoSystem->Simulate(MainInfo->CalcLikelihood, print);
      if ((MainInfo->printinfo).Print())
        EcoSystem->PrintValues((MainInfo->printinfo).OutputFile, (MainInfo->printinfo).givenPrecision);
      if ((MainInfo->printinfo).PrintinColumns())
        EcoSystem->PrintValuesinColumns((MainInfo->printinfo).ColumnOutputFile, (MainInfo->printinfo).givenPrecision);
    }
    delete Stochasticdata;

  } else {
    EcoSystem->Simulate(MainInfo->CalcLikelihood, print);
    if ((MainInfo->printinfo).Print())
      EcoSystem->PrintValues((MainInfo->printinfo).OutputFile, (MainInfo->printinfo).givenPrecision);
    if ((MainInfo->printinfo).PrintinColumns())
      EcoSystem->PrintValuesinColumns((MainInfo->printinfo).ColumnOutputFile, (MainInfo->printinfo).givenPrecision);
  }
}

int main(int aNumber, char *const aVector[]) {

  MainInfo MainInfo;
  OptInfo* Optinfo = 0;
  StochasticData* Stochasticdata = 0;
  ErrorHandler handle;

  //Test to see if the function double lgamma(double) is returning an integer.
  //lgamma is a non-ansi function and on some platforms when compiled with the
  //-ansi flag lgamma returns an integer value. [MNAA&AJ 05.2001]
  assert(lgamma(1.2) != floor(lgamma(1.2)));

  //JMB - Changed to use GADGET_ .. instead of BORMICON_ ..
  char* workingdir = getenv("GADGET_WORKING_DIR");
  if (workingdir == 0) {
    if ((workingdir = (char*)malloc(pathmax)) == NULL) {
      cerr << "Failed to malloc space for current working directory\n";
      exit(EXIT_FAILURE);
    }
    if (getcwd(workingdir, pathmax) == NULL) {
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
    MainInfo.Read(aNumber, aVector);

  //JMB - dont print output if doing a network run
  if (!(MainInfo.Net)) {
    RUNID.print(cout);
    cout << "Starting Gadget from directory: " << workingdir
      << "\nusing data from directory: " << inputdir << endl;
  }

  //Added MainInfo.Net to Ecosystem constructor, to let EcoSystem know if
  //we are doing a net run. 07.04.00 AJ
  EcoSystem = new Ecosystem(MainInfo.MainGadgetFilename, MainInfo.Optimize,
    MainInfo.Net, MainInfo.CalcLikelihood, inputdir, workingdir, MainInfo.printinfo);

  #ifdef INTERRUPT_HANDLER
    registerInterrupt(SIGINT, &EcoSystem->interrupted);
  #endif

  chdir(workingdir);
  if (MainInfo.OptInfoFileisGiven) {
    char type[MaxStrLength];
    strncpy(type, "", MaxStrLength);

    MainInfo.optinfocommentfile >> ws >> type;
    if ((strcasecmp(type, "hookeandsimann") == 0) || (strcasecmp(type, "simannandhooke") == 0))
      Optinfo = new OptInfoHookeAndSimann();
    else if (strcasecmp(type, "hooke") == 0)
      Optinfo = new OptInfoHooke();
    else if (strcasecmp(type, "simann") == 0)
      Optinfo = new OptInfoSimann();
    //JMB - removed the BFGS stuff
    else
      handle.Unexpected("hooke, simann, or simannandhooke", type);

    Optinfo->Read(MainInfo.optinfocommentfile);
    MainInfo.CloseOptinfofile();

  } else {
    Optinfo = new OptInfoHooke();
    //JMB - no optinfo file specified so only use default values
    //Optinfo->Read(MainInfo.optinfocommentfile);
  }

  if (MainInfo.PrintInitialcond)
    EcoSystem->PrintStatus(MainInfo.PrintInitialcondfilename);
  if ((MainInfo.printinfo).Print())
    EcoSystem->PrintInitialInformation((MainInfo.printinfo).OutputFile);
  if ((MainInfo.printinfo).PrintinColumns())
    EcoSystem->PrintInitialInformationinColumns((MainInfo.printinfo).ColumnOutputFile);
  if (MainInfo.Stochastic)
    StochasticRun(EcoSystem, &MainInfo);

  if (MainInfo.Optimize) {
    if (MainInfo.InitialCondareGiven) {
      Stochasticdata = new StochasticData(MainInfo.InitialCommentFilename);
      EcoSystem->Update(Stochasticdata);
    }
    Optinfo->MaximizeLikelihood();
    if ((MainInfo.printinfo).forcePrint)
      EcoSystem->Simulate(0, 1);

    if (MainInfo.InitialCondareGiven)
      delete Stochasticdata;
  }

  if (MainInfo.PrintLikelihoodInfo)
    EcoSystem->PrintLikelihoodInfo(MainInfo.PrintLikelihoodFilename);
  if (MainInfo.PrintFinalcond)
    EcoSystem->PrintStatus(MainInfo.PrintFinalcondfilename);

  //JMB - print final values of parameters
  if (!(MainInfo.Net))
    EcoSystem->PrintParamsinColumns((MainInfo.printinfo).ParamOutFile, (MainInfo.printinfo).givenPrecision);

  free(workingdir);
  delete Optinfo;
  delete EcoSystem;
}
