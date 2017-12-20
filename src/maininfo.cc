#include "maininfo.h"
#include "errorhandler.h"
#include "gadget.h"
#include "runid.h"
#include "global.h"

void MainInfo::showCorrectUsage(char* error) {
  RUNID.Print(cerr);
  cerr << "\nError in command line value - unrecognised option " << error << endl
    << "Common options are -l or -s, -i <filename> -o <filename>\n"
    << "For more information try running Gadget with the -h switch\n";
  exit(EXIT_FAILURE);
}

void MainInfo::showUsage() {
  RUNID.Print(cout);
  cout << "\nOptions for running Gadget:\n"
    << " -l                           perform a likelihood (optimising) model run\n"
    << " -s                           perform a single (simulation) model run\n"
    << " -n                           perform a network run (using paramin)\n"
    << " -v --version                 display version information and exit\n"
    << " -h --help                    display this help screen and exit\n"
    << "\nOptions for specifying the input to Gadget models:\n"
    << " -i <filename>                read model parameters from <filename>\n"
    << " -opt <filename>              read optimising parameters from <filename>\n"
    << " -main <filename>             read model information from <filename>\n"
    << "                              (default filename is 'main')\n"
    << " -m <filename>                read other commandline parameters from <filename>\n"
    << "\nOptions for specifying the output from Gadget models:\n"
    << " -p <filename>                print final model parameters to <filename>\n"
    << "                              (default filename is 'params.out')\n"
    << " -o <filename>                print likelihood output to <filename>\n"
    << " -print <number>              print -o output every <number> iterations\n"
    << " -precision <number>          set the precision to <number> in output files\n"
    << "\nOptions for debugging Gadget models:\n"
    << " -log <filename>              print logging information to <filename>\n"
    << " -printinitial <filename>     print initial model information to <filename>\n"
    << " -printfinal <filename>       print final model information to <filename>\n"
    << "\nFor more information see the Gadget web page at http://www.github.com/hafro/gadget\n\n";
  exit(EXIT_SUCCESS);
}

MainInfo::MainInfo()
  : givenOptInfo(0), givenInitialParam(0), runoptimise(0),
    runstochastic(0), runnetwork(0), runprint(1), forceprint(0),
    printInitialInfo(0), printFinalInfo(0), printLogLevel(0), maxratio(0.95) {

  char tmpname[10];
  strncpy(tmpname, "", 10);
  strcpy(tmpname, "main");

  strOptInfoFile = NULL;
  strInitialParamFile = NULL;
  strPrintInitialFile = NULL;
  strPrintFinalFile = NULL;
  strMainGadgetFile = NULL;
  setMainGadgetFile(tmpname);
}

MainInfo::~MainInfo() {
  if (strOptInfoFile != NULL) {
    delete[] strOptInfoFile;
    strOptInfoFile = NULL;
  }
  if (strInitialParamFile != NULL) {
    delete[] strInitialParamFile;
    strInitialParamFile = NULL;
  }
  if (strPrintInitialFile != NULL) {
    delete[] strPrintInitialFile;
    strPrintInitialFile = NULL;
  }
  if (strPrintFinalFile != NULL) {
    delete[] strPrintFinalFile;
    strPrintFinalFile = NULL;
  }
  if (strMainGadgetFile != NULL) {
    delete[] strMainGadgetFile;
    strMainGadgetFile = NULL;
  }
}

void MainInfo::read(int aNumber, char* const aVector[]) {

  if (aNumber == 1) {
    handle.logMessage(LOGWARN, "Warning - no command line options specified, using default values");
    return;
  }

  int k = 1;
  while (k < aNumber) {
    if (strcasecmp(aVector[k], "-l") == 0) {
      runoptimise = 1;

    } else if (strcasecmp(aVector[k], "-n") == 0) {
      runnetwork = 1;

#ifndef GADGET_NETWORK
      handle.logMessage(LOGFAIL, "Error - Gadget cannot currently run in network mode for paramin\nGadget must be recompiled to enable the network communication");
#endif

    } else if (strcasecmp(aVector[k], "-s") == 0) {
      runstochastic = 1;

    } else if (strcasecmp(aVector[k], "-m") == 0) {
      ifstream infile;
      CommentStream incomment(infile);
      if (k == aNumber - 1)
        this->showCorrectUsage(aVector[k]);
      k++;
      infile.open(aVector[k], ios::in);
      handle.checkIfFailure(infile, aVector[k]);
      if (infile.fail())
        this->showCorrectUsage(aVector[k]);
      this->read(incomment);
      infile.close();
      infile.clear();

    } else if (strcasecmp(aVector[k], "-i") == 0) {
      if (k == aNumber - 1)
        this->showCorrectUsage(aVector[k]);
      k++;
      this->setInitialParamFile(aVector[k]);

    } else if (strcasecmp(aVector[k], "-o") == 0) {
      if (k == aNumber - 1)
        this->showCorrectUsage(aVector[k]);
      k++;
      printinfo.setOutputFile(aVector[k]);

    } else if (strcasecmp(aVector[k], "-p") == 0) {
      if (k == aNumber - 1)
        this->showCorrectUsage(aVector[k]);
      k++;
      printinfo.setParamOutFile(aVector[k]);

    } else if (strcasecmp(aVector[k], "-forceprint") == 0) {
      forceprint = 1;

    } else if (strcasecmp(aVector[k], "-co") == 0) {
      handle.logMessage(LOGFAIL, "The -co switch is no longer supported");

    } else if (strcasecmp(aVector[k], "-printinitial") == 0) {
      if (k == aNumber - 1)
        this->showCorrectUsage(aVector[k]);
      k++;
      this->setPrintInitialFile(aVector[k]);

    } else if (strcasecmp(aVector[k], "-printfinal") == 0) {
      if (k == aNumber - 1)
        this->showCorrectUsage(aVector[k]);
      k++;
      this->setPrintFinalFile(aVector[k]);

    } else if (strcasecmp(aVector[k], "-main") == 0) {
      if (k == aNumber - 1)
        this->showCorrectUsage(aVector[k]);
      k++;
      this->setMainGadgetFile(aVector[k]);

    } else if (strcasecmp(aVector[k], "-opt") == 0) {
      if (k == aNumber - 1)
        this->showCorrectUsage(aVector[k]);
      k++;
      this->setOptInfoFile(aVector[k]);

    } else if ((strcasecmp(aVector[k], "-printlikelihood") == 0) || (strcasecmp(aVector[k], "-likelihoodprint") == 0)) {
      handle.logMessage(LOGFAIL, "The -printlikelihood switch is no longer supported\nSpecify a likelihoodprinter class in the model print file instead");

    } else if (strcasecmp(aVector[k], "-printlikesummary") == 0) {
      handle.logMessage(LOGFAIL, "The -printlikesummary switch is no longer supported\nSpecify a likelihoodsummaryprinter class in the model print file instead");

    } else if (strcasecmp(aVector[k], "-printonelikelihood") == 0) {
      handle.logMessage(LOGFAIL, "The -printonelikelihood switch is no longer supported\nSpecify a likelihoodprinter class in the model print file instead");

    } else if ((strcasecmp(aVector[k], "-print") == 0) || (strcasecmp(aVector[k], "-print1") == 0)) {
      if (k == aNumber - 1)
        this->showCorrectUsage(aVector[k]);
      k++;
      printinfo.setPrintIteration(atoi(aVector[k]));

    } else if (strcasecmp(aVector[k], "-precision") == 0) {
      if (k == aNumber - 1)
        this->showCorrectUsage(aVector[k]);
      k++;
      printinfo.setPrecision(atoi(aVector[k]));

    } else if ((strcasecmp(aVector[k], "-v") == 0) || (strcasecmp(aVector[k], "--version") == 0)) {
      RUNID.Print(cout);
      exit(EXIT_SUCCESS);

    } else if ((strcasecmp(aVector[k], "-h") == 0) || (strcasecmp(aVector[k], "--help") == 0)) {
      this->showUsage();

    } else if (strcasecmp(aVector[k], "-log") == 0) {
      if (k == aNumber - 1)
        this->showCorrectUsage(aVector[k]);
      k++;
      handle.setLogFile(aVector[k]);
      printLogLevel = 5;

    } else if (strcasecmp(aVector[k], "-nowarnings") == 0) {
      //handle.logMessage(LOGWARN, "The -nowarnings switch is no longer supported\nSpecify a logging level using the -loglevel <number> instead");
      printLogLevel = 1;

    } else if (strcasecmp(aVector[k], "-loglevel") == 0) {
      if (k == aNumber - 1)
        this->showCorrectUsage(aVector[k]);
      k++;
      printLogLevel = atoi(aVector[k]);

    } else if (strcasecmp(aVector[k], "-noprint") == 0) {
      //JMB disable model printing from the commandline
      runprint = 0;

    } else if (strcasecmp(aVector[k], "-seed") == 0) {
      //JMB experimental setting of random number seed from the commandline
      //if the "seed" is also specified in the optinfo file then that will override
      //any seed that is specified on the commandline
      if (k == aNumber - 1)
        this->showCorrectUsage(aVector[k]);
      k++;
      srand(atoi(aVector[k]));

    } else if (strcasecmp(aVector[k], "-maxratio") == 0) {
      //JMB experimental setting of maximum ratio of stock consumed in one timestep
      if (k == aNumber - 1)
        this->showCorrectUsage(aVector[k]);
      k++;
      maxratio = atof(aVector[k]);

    } else
      this->showCorrectUsage(aVector[k]);

    k++;
  }
}

void MainInfo::checkUsage(const char* const inputdir, const char* const workingdir) {
  int check = 0;
  if (runnetwork)
    check = max(0, printLogLevel);
  else if (runstochastic)
    check = max(3, printLogLevel);
  else
    check = max(2, printLogLevel);
  handle.setLogLevel(check);

  //JMB dont print output if doing a network run
  if (!runnetwork)
    RUNID.Print(cout);
  handle.logMessage(LOGINFO, "Starting Gadget from directory:", workingdir);
  handle.logMessage(LOGINFO, "using data from directory:", inputdir);
  handle.logMessage(LOGMESSAGE, ""); //write a blank line to the log file
  if (strcasecmp(inputdir, workingdir) != 0)
    handle.logMessage(LOGWARN, "Warning - working directory different from input directory");

  if (runnetwork) {
    handle.logMessage(LOGINFO, "\n** Gadget running in network mode for Paramin **");
    if (printInitialInfo) {
      handle.logMessage(LOGINFO, "Warning - cannot print initial model information");
      printInitialInfo = 0;
    }
    if (printFinalInfo) {
      handle.logMessage(LOGINFO, "Warning - cannot print final model information");
      printFinalInfo = 0;
    }
  }

  //JMB check to see if we can actually open required files ...
  ifstream tmpin;
  if (chdir(inputdir) != 0)
    handle.logMessage(LOGFAIL, "Error - failed to change input directory to", inputdir);
  if (givenInitialParam) {
    //JMB check to see that the input and output filenames are different
    //Otherwise Gadget will over-write the inputfile with a blank outputfile ...
    if (strcasecmp(strInitialParamFile, printinfo.getParamOutFile()) == 0)
      handle.logFileMessage(LOGFAIL, "the parameter input and output filenames are the same");

    tmpin.open(strInitialParamFile, ios::in);
    handle.checkIfFailure(tmpin, strInitialParamFile);
    tmpin.close();
    tmpin.clear();
  }
  if (givenOptInfo) {
    tmpin.open(strOptInfoFile, ios::in);
    handle.checkIfFailure(tmpin, strOptInfoFile);
    tmpin.close();
    tmpin.clear();
  }
  ofstream tmpout;
  if (chdir(workingdir) != 0)
    handle.logMessage(LOGFAIL, "Error - failed to change working directory to", workingdir);
  if (printInitialInfo) {
    tmpout.open(strPrintInitialFile, ios::out);
    handle.checkIfFailure(tmpout, strPrintInitialFile);
    tmpout.close();
    tmpout.clear();
  }
  if (printFinalInfo) {
    tmpout.open(strPrintFinalFile, ios::out);
    handle.checkIfFailure(tmpout, strPrintFinalFile);
    tmpout.close();
    tmpout.clear();
  }
  printinfo.checkPrintInfo(runnetwork);

  //JMB check the value of maxratio
  if ((maxratio < rathersmall) || (maxratio > 1.0)) {
    handle.logMessage(LOGWARN, "Warning - value of maxratio outside bounds", maxratio);
    maxratio = 0.95;
  }

  if ((!runstochastic) && (runnetwork)) {
    handle.logMessage(LOGWARN, "\nWarning - Gadget for the paramin network should be used with -s option\nGadget will now set the -s switch to perform a simulation run");
    runstochastic = 1;
  }

  if ((runstochastic) && (runoptimise)) {
    handle.logMessage(LOGWARN, "\nWarning - Gadget has been started with both the -s switch and the -l switch\nHowever, it is not possible to do both a simulation run and a likelihood run!\nGadget will perform only the simulation run (and ignore the -l switch)");
    runoptimise = 0;
  }

  if ((!runstochastic) && (!runoptimise)) {
    handle.logMessage(LOGWARN, "\nWarning - Gadget has been started without either the -s switch or the -l switch\nGadget will now set the -s switch to perform a simulation run");
    runstochastic = 1;
  }

  handle.setRunOptimise(runoptimise);
  if ((printLogLevel == 1) && (!runoptimise))
    handle.logMessage(LOGWARN, "\n** Gadget cannot disable warnings for a simulation run **");

  if ((handle.checkLogFile()) && (runoptimise) && (printLogLevel >= 5))
    handle.logMessage(LOGWARN, "\n** logging model information from a Gadget optimisation is not recommended **");

  if ((handle.checkLogFile()) && (runnetwork))
    handle.logMessage(LOGWARN, "\n** logging model information from a Gadget network run is not recommended **");

  //check the printing options
  if (forceprint)
    handle.logMessage(LOGMESSAGE, "\nPrinting model output has been enabled from the command line");
  if (!runprint)
    handle.logMessage(LOGMESSAGE, "\nPrinting model output has been disabled from the command line");

  check = runprint;
  if (runnetwork)
    check = 0;
  else if ((runoptimise) && (!forceprint))
    check = 0;
  runprint = check;

  handle.logMessage(LOGMESSAGE, "");  //write blank line to log file
}

void MainInfo::read(CommentStream& infile) {
  int dummy = 0;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> ws;
  while (!infile.eof()) {
    infile >> text >> ws;

    if (strcasecmp(text, "-i") == 0) {
      infile >> text >> ws;
      this->setInitialParamFile(text);
    } else if (strcasecmp(text, "-o") == 0) {
      infile >> text >> ws;
      printinfo.setOutputFile(text);
    } else if (strcasecmp(text, "-co") == 0) {
      handle.logMessage(LOGFAIL, "The -co switch is no longer supported");
    } else if (strcasecmp(text, "-p") == 0) {
      infile >> text >> ws;
      printinfo.setParamOutFile(text);
    } else if (strcasecmp(text, "-main") == 0) {
      infile >> text >> ws;
      this->setMainGadgetFile(text);
    } else if (strcasecmp(text, "-printinitial") == 0) {
      infile >> text >> ws;
      this->setPrintInitialFile(text);
    } else if (strcasecmp(text, "-printfinal") == 0) {
      infile >> text >> ws;
      this->setPrintFinalFile(text);
    } else if (strcasecmp(text, "-opt") == 0) {
      infile >> text >> ws;
      this->setOptInfoFile(text);
    } else if (strcasecmp(text, "-forceprint") == 0) {
      forceprint = 1;
    } else if (strcasecmp(text, "-noprint") == 0) {
      runprint = 0;
    } else if ((strcasecmp(text, "-print") == 0) || (strcasecmp(text, "-print1") == 0)) {
      infile >> dummy >> ws;
      printinfo.setPrintIteration(dummy);
    } else if (strcasecmp(text, "-precision") == 0) {
      infile >> dummy >> ws;
      printinfo.setPrecision(dummy);
    } else if (strcasecmp(text, "-log") == 0) {
      infile >> text >> ws;
      handle.setLogFile(text);
      printLogLevel = 5;
    } else if (strcasecmp(text, "-nowarnings") == 0) {
      printLogLevel = 1;
    } else if (strcasecmp(text, "-loglevel") == 0) {
      infile >> printLogLevel >> ws;
    } else if (strcasecmp(text, "-seed") == 0) {
      infile >> dummy >> ws;
      srand(dummy);
    } else if (strcasecmp(text, "-maxratio") == 0) {
      infile >> maxratio >> ws;
    } else if (strcasecmp(text, "-printlikesummary") == 0) {
      handle.logMessage(LOGWARN, "The -printlikesummary switch is no longer supported\nSpecify a likelihoodsummaryprinter class in the model print file instead");
    } else if (strcasecmp(text, "-printlikelihood") == 0) {
      handle.logMessage(LOGWARN, "The -printlikelihood switch is no longer supported\nSpecify a likelihoodprinter class in the model print file instead");
    } else if (strcasecmp(text, "-printonelikelihood") == 0) {
      handle.logMessage(LOGWARN, "The -printonelikelihood switch is no longer supported\nSpecify a likelihoodprinter class in the model print file instead");
    } else
      this->showCorrectUsage(text);
  }
}

void MainInfo::setPrintInitialFile(char* filename) {
  if (strPrintInitialFile != NULL) {
    delete[] strPrintInitialFile;
    strPrintInitialFile = NULL;
  }
  strPrintInitialFile = new char[strlen(filename) + 1];
  strcpy(strPrintInitialFile, filename);
  printInitialInfo = 1;
}

void MainInfo::setPrintFinalFile(char* filename) {
  if (strPrintFinalFile != NULL) {
    delete[] strPrintFinalFile;
    strPrintFinalFile = NULL;
  }
  strPrintFinalFile = new char[strlen(filename) + 1];
  strcpy(strPrintFinalFile, filename);
  printFinalInfo = 1;
}

void MainInfo::setMainGadgetFile(char* filename) {
  if (strMainGadgetFile != NULL) {
    delete[] strMainGadgetFile;
    strMainGadgetFile = NULL;
  }
  strMainGadgetFile = new char[strlen(filename) + 1];
  strcpy(strMainGadgetFile, filename);
}

void MainInfo::setInitialParamFile(char* filename) {
  if (strInitialParamFile != NULL) {
    delete[] strInitialParamFile;
    strInitialParamFile = NULL;
  }
  strInitialParamFile = new char[strlen(filename) + 1];
  strcpy(strInitialParamFile, filename);
  givenInitialParam = 1;
}

void MainInfo::setOptInfoFile(char* filename) {
  if (strOptInfoFile != NULL) {
    delete[] strOptInfoFile;
    strOptInfoFile = NULL;
  }
  strOptInfoFile = new char[strlen(filename) + 1];
  strcpy(strOptInfoFile, filename);
  givenOptInfo = 1;
}
