#include "maininfo.h"
#include "errorhandler.h"
#include "gadget.h"
#include "runid.h"

extern RunID RUNID;
extern ErrorHandler handle;

void MainInfo::showCorrectUsage(char* error) {
  RUNID.print(cerr);
  cerr << "\nError in command line value - unrecognised option " << error << endl
    << "Common options are -l or -s, -i <filename> -o <filename>\n"
    << "For more information try running Gadget with the -h switch\n";
  exit(EXIT_FAILURE);
}

void MainInfo::showUsage() {
  RUNID.print(cout);
  cout << "\nOptions for running Gadget:\n"
    << " -l                          perform a likelihood (optimising) model run\n"
    << " -s                          perform a single (stochastic) model run\n"
    << " -n                          perform a network run (using paramin)\n"
    << " -v --version                display version information and exit\n"
    << " -h --help                   display this help screen and exit\n"
    << "\nOptions for specifying the input to Gadget models:\n"
    << " -i <filename>               read model parameters from <filename>\n"
    << " -opt <filename>             read optimising parameters from <filename>\n"
    << " -main <filename>            read model information from <filename>\n"
    << "                             (default filename is 'main')\n"
    << " -m <filename>               read other commandline parameters from <filename>\n"
    << "\nOptions for specifying the output from Gadget models:\n"
    << " -p <filename>               print final model parameters to <filename>\n"
    << "                             (default filename is 'params.out')\n"
    << " -o <filename>               print likelihood output to <filename>\n"
    << " -co <filename>              print likelihood column output to <filename>\n"
    << " -print1 <number>            print -o output every <number> iterations\n"
    << " -print2 <number>            print -co output every <number> iterations\n"
    << " -precision <number>         set the precision to <number> in output files\n"
    << "\nOptions for debugging Gadget models:\n"
    << " -log <filename>             print logging information to <filename>\n"
    << " -printinitial <filename>    print initial model information to <filename>\n"
    << " -printfinal <filename>      print final model information to <filename>\n"
    << " -printlikelihood <filename> print model likelihood information to <filename>\n"
    << "\nFor more information see the Gadget web page at http://www.hafro.is/gadget\n";
  exit(EXIT_SUCCESS);
}

MainInfo::MainInfo()
  : optInfoComment(optInfoStream), givenOptInfo(0), givenInitialParam(0),
    runlikelihood(0), runoptimize(0), runstochastic(0), runnetwork(0),
    printInitialInfo(0), printFinalInfo(0), printLikelihoodInfo(0) {

  char tmpname[10];
  strncpy(tmpname, "", 10);
  strcpy(tmpname, "main");

  strOptInfoFile = NULL;
  strInitialParamFile = NULL;
  strPrintInitialFile = NULL;
  strPrintFinalFile = NULL;
  strPrintLikelihoodFile = NULL;
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
  if (strPrintLikelihoodFile != NULL) {
    delete[] strPrintLikelihoodFile;
    strPrintLikelihoodFile = NULL;
  }
  if (strMainGadgetFile != NULL) {
    delete[] strMainGadgetFile;
    strMainGadgetFile = NULL;
  }
}

void MainInfo::openOptInfoFile(char* filename) {
  if (strOptInfoFile != NULL) {
    delete[] strOptInfoFile;
    strOptInfoFile = NULL;
  }
  strOptInfoFile = new char[strlen(filename) + 1];
  strcpy(strOptInfoFile, filename);

  optInfoStream.open(strOptInfoFile, ios::in);
  handle.checkIfFailure(optInfoStream, strOptInfoFile);
  givenOptInfo = 1;
}

void MainInfo::closeOptInfoFile() {
  optInfoStream.close();
  optInfoStream.clear();
}

void MainInfo::read(int aNumber, char* const aVector[]) {
  int k, len;
  if (aNumber > 1) {
    k = 1;
    while (k < aNumber) {
      if (strcasecmp(aVector[k], "-l") == 0) {
        runlikelihood = 1;
        runoptimize = 1;

      } else if (strcasecmp(aVector[k], "-n") == 0) {
        runnetwork = 1;

        #ifndef GADGET_NETWORK
          cout << "\nWarning - Gadget is trying to run in the network mode for paramin without\n"
            << "the network support being compiled - no network communication can take place!\n";
        #endif

      } else if (strcasecmp(aVector[k], "-s") == 0) {
        runstochastic = 1;
        runlikelihood = 1;

      } else if (strcasecmp(aVector[k], "-m") == 0) {
        ifstream infile;
        CommentStream incomment(infile);
        if (k == aNumber - 1)
          showCorrectUsage(aVector[k]);

        k++;
        infile.open(aVector[k], ios::in);
        if (infile.fail())
          showCorrectUsage(aVector[k]);
        this->read(incomment);
        infile.close();
        infile.clear();

      } else if (strcasecmp(aVector[k], "-i") == 0) {
        if (k == aNumber - 1)
          showCorrectUsage(aVector[k]);
        k++;
        setInitialParamFile(aVector[k]);

      } else if (strcasecmp(aVector[k], "-o") == 0) {
        if (k == aNumber - 1)
          showCorrectUsage(aVector[k]);
        k++;
        printinfo.setOutputFile(aVector[k]);

      } else if (strcasecmp(aVector[k], "-p") == 0) {
        if (k == aNumber - 1)
          showCorrectUsage(aVector[k]);
        k++;
        printinfo.setParamOutFile(aVector[k]);

      } else if (strcasecmp(aVector[k], "-print") == 0) {
        printinfo.setForcePrint(1);

      } else if (strcasecmp(aVector[k], "-co") == 0) {
        if (k == aNumber - 1)
          showCorrectUsage(aVector[k]);
        k++;
        printinfo.setColumnOutputFile(aVector[k]);

      } else if (strcasecmp(aVector[k], "-printinitial") == 0) {
        if (k == aNumber - 1)
          showCorrectUsage(aVector[k]);
        k++;
        setPrintInitialFile(aVector[k]);

      } else if (strcasecmp(aVector[k], "-printfinal") == 0) {
        if (k == aNumber - 1)
          showCorrectUsage(aVector[k]);
        k++;
        setPrintFinalFile(aVector[k]);

      } else if (strcasecmp(aVector[k], "-main") == 0) {
        if (k == aNumber - 1)
          showCorrectUsage(aVector[k]);
        k++;
        setMainGadgetFile(aVector[k]);

      } else if (strcasecmp(aVector[k], "-opt") == 0) {
        if (k == aNumber - 1)
          showCorrectUsage(aVector[k]);
        k++;
        openOptInfoFile(aVector[k]);

      } else if ((strcasecmp(aVector[k], "-printlikelihood") == 0) || (strcasecmp(aVector[k], "-likelihoodprint") == 0)) {
        if (k == aNumber - 1)
          showCorrectUsage(aVector[k]);
        k++;
        setPrintLikelihoodFile(aVector[k]);

      } else if (strcasecmp(aVector[k], "-print1") == 0) {
        if (k == aNumber - 1)
          showCorrectUsage(aVector[k]);
        k++;
        printinfo.setPrint1(atoi(aVector[k]));

      } else if (strcasecmp(aVector[k], "-print2") == 0) {
        if (k == aNumber - 1)
          showCorrectUsage(aVector[k]);
        k++;
        printinfo.setPrint2(atoi(aVector[k]));

      } else if (strcasecmp(aVector[k], "-precision") == 0) {
        //JMB - experimental setting of printing precision
        if (k == aNumber - 1)
          showCorrectUsage(aVector[k]);
        k++;
        printinfo.setPrecision(atoi(aVector[k]));

      } else if ((strcasecmp(aVector[k], "-v") == 0) || (strcasecmp(aVector[k], "--version") == 0)) {
        RUNID.print(cout);
        exit(EXIT_SUCCESS);

      } else if ((strcasecmp(aVector[k], "-h") == 0) || (strcasecmp(aVector[k], "--help") == 0)) {
        showUsage();

      } else if (strcasecmp(aVector[k], "-log") == 0) {
        //JMB - experimental logging facility
        if (k == aNumber - 1)
          showCorrectUsage(aVector[k]);
        k++;
        handle.setLogFile(aVector[k]);

      } else
        showCorrectUsage(aVector[k]);

      k++;
    }
  }

  printinfo.checkNumbers();
  //JMB some error checking to make sure we are doing something sensible
  if ((runstochastic != 1) && (runnetwork == 1)) {
    cout << "\nWarning - Gadget for the paramin network should be used with -s option\n"
      << "Gadget will now set the -s switch to perform a stochastic run\n";
    runstochastic = 1;
    runlikelihood = 1;
  }
  if ((runstochastic == 1) && (runoptimize == 1)) {
    cout << "\nWarning - Gadget has been started with both the -s switch and the -l switch\n"
      << "However, it is not possible to do both a stochastic run and an optimizing run!\n"
      << "Gadget will perform only the stochastic run (and ignore the -l switch)\n";
    runoptimize = 0;
  }
  if ((handle.checkLogFile()) && (runoptimize == 1)) {
    cout << "\nWarning - logging information from a Gadget likelihood run is not recommended\n"
      << "Since this can lead to a very large log file being created!\n";
  }
  if ((handle.checkLogFile()) && (runnetwork == 1)) {
    cout << "\nWarning - logging information from a Gadget run using paramin is not recommended\n";
  }
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
      setInitialParamFile(text);
    } else if (strcasecmp(text, "-o") == 0) {
      infile >> text >> ws;
      printinfo.setOutputFile(text);
    } else if (strcasecmp(text, "-co") == 0) {
      infile >> text >> ws;
      printinfo.setColumnOutputFile(text);
    } else if (strcasecmp(text, "-p") == 0) {
      infile >> text >> ws;
      printinfo.setParamOutFile(text);
    } else if (strcasecmp(text, "-main") == 0) {
      infile >> text >> ws;
      setMainGadgetFile(text);
    } else if (strcasecmp(text, "-printinitial") == 0) {
      infile >> text >> ws;
      setPrintInitialFile(text);
    } else if (strcasecmp(text, "-printfinal") == 0) {
      infile >> text >> ws;
      setPrintFinalFile(text);
    } else if (strcasecmp(text, "-printlikelihood") == 0) {
      infile >> text >> ws;
      setPrintLikelihoodFile(text);
    } else if (strcasecmp(text, "-opt") == 0) {
      infile >> text >> ws;
      openOptInfoFile(text);
    } else if (strcasecmp(text, "-print1") == 0) {
      infile >> dummy >> ws;
      printinfo.setPrint1(dummy);
    } else if (strcasecmp(text, "-print2") == 0) {
      infile >> dummy >> ws;
      printinfo.setPrint2(dummy);
    } else if (strcasecmp(text, "-precision") == 0) {
      infile >> dummy >> ws;
      printinfo.setPrecision(dummy);
    } else
      showCorrectUsage(text);
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

void MainInfo::setPrintLikelihoodFile(char* filename) {
  if (strPrintLikelihoodFile != NULL) {
    delete[] strPrintLikelihoodFile;
    strPrintLikelihoodFile = NULL;
  }
  strPrintLikelihoodFile = new char[strlen(filename) + 1];
  strcpy(strPrintLikelihoodFile, filename);
  printLikelihoodInfo = 1;
}

void MainInfo::setInitialParamFile(char* filename) {
  if (strInitialParamFile != NULL) {
    delete[] strInitialParamFile;
    strInitialParamFile = NULL;
  }
  strInitialParamFile = new char[strlen(filename) + 1];
  strcpy(strInitialParamFile, filename);
  givenInitialParam = 1;

  //JMB check to see if we can actually open the file ...
  ifstream tmpfile;
  tmpfile.open(strInitialParamFile);
  handle.checkIfFailure(tmpfile, strInitialParamFile);
  tmpfile.close();
  tmpfile.clear();
}

void MainInfo::setMainGadgetFile(char* filename) {
  if (strMainGadgetFile != NULL) {
    delete[] strMainGadgetFile;
    strMainGadgetFile = NULL;
  }
  strMainGadgetFile = new char[strlen(filename) + 1];
  strcpy(strMainGadgetFile, filename);
}
