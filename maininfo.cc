#include "maininfo.h"
#include "errorhandler.h"
#include "gadget.h"

#include "runid.h"
extern RunID RUNID;

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
    << " -printinitial <filename>    print initial model information to <filename>\n"
    << " -printfinal <filename>      print final model information to <filename>\n"
    << " -printlikelihood <filename> print model likelihood information to <filename>\n"
    //    << " -printlog <filename>        print input reading to <filename>\n"      
    << "\nFor more information see the Gadget web page at http://www.hafro.is/gadget\n";
  exit(EXIT_SUCCESS);
}

MainInfo::MainInfo()
  : OptinfoCommentFile(OptinfoFile), OptInfoFileisGiven(0), InitialCondareGiven(0),
    calclikelihood(0), optimize(0), stochastic(0), PrintInitialcond(0), PrintFinalcond(0),
    PrintLikelihoodInfo(0), netrun(0) {

  char tmpname[10];
  strncpy(tmpname, "", 10);
  strcpy(tmpname, "main");

  OptinfoFilename = NULL;
  InitialCommentFilename = NULL;
  PrintInitialCondFilename = NULL;
  PrintFinalCondFilename = NULL;
  PrintLikelihoodFilename = NULL;
  MainGadgetFilename = NULL;
  setMainGadgetFilename(tmpname);
}

MainInfo::~MainInfo() {
  if (OptinfoFilename != NULL) {
    delete[] OptinfoFilename;
    OptinfoFilename = NULL;
  }
  if (InitialCommentFilename != NULL) {
    delete[] InitialCommentFilename;
    InitialCommentFilename = NULL;
  }
  if (PrintInitialCondFilename != NULL) {
    delete[] PrintInitialCondFilename;
    PrintInitialCondFilename = NULL;
  }
  if (PrintFinalCondFilename != NULL) {
    delete[] PrintFinalCondFilename;
    PrintFinalCondFilename = NULL;
  }
  if (PrintLikelihoodFilename != NULL) {
    delete[] PrintLikelihoodFilename;
    PrintLikelihoodFilename = NULL;
  }
  if (MainGadgetFilename != NULL) {
    delete[] MainGadgetFilename;
    MainGadgetFilename = NULL;
  }
}

void MainInfo::OpenOptinfofile(char* filename) {
  if (OptinfoFilename != NULL) {
    delete[] OptinfoFilename;
    OptinfoFilename = NULL;
  }
  OptinfoFilename = new char[strlen(filename) + 1];
  strcpy(OptinfoFilename, filename);

  ErrorHandler handle;
  handle.Open(filename);
  OptinfoFile.open(OptinfoFilename, ios::in);
  checkIfFailure(OptinfoFile, filename);
  OptInfoFileisGiven = 1;
  handle.Close();
}

void MainInfo::CloseOptinfofile() {
  OptinfoFile.close();
  OptinfoFile.clear();
}

void MainInfo::read(int aNumber, char* const aVector[]) {
  int k, len;
  if (aNumber > 1) {
    k = 1;
    while (k < aNumber) {
      if (strcasecmp(aVector[k], "-l") == 0) {
        calclikelihood = 1;
        optimize = 1;

      } else if (strcasecmp(aVector[k], "-n") == 0) {
        netrun = 1;

        #ifndef GADGET_NETWORK
          cout << "\nWarning - Gadget is trying to run in the network mode for paramin without\n"
            << "the network support being compiled - no network communication can take place!\n";
        #endif

      } else if (strcasecmp(aVector[k], "-s") == 0) {
        stochastic = 1;
        calclikelihood = 1;

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
        setInitialParamFilename(aVector[k]);

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
        setPrintInitialCondFilename(aVector[k]);

      } else if (strcasecmp(aVector[k], "-printfinal") == 0) {
        if (k == aNumber - 1)
          showCorrectUsage(aVector[k]);
        k++;
        setPrintFinalCondFilename(aVector[k]);

      } else if (strcasecmp(aVector[k], "-main") == 0) {
        if (k == aNumber - 1)
          showCorrectUsage(aVector[k]);
        k++;
        setMainGadgetFilename(aVector[k]);

      } else if (strcasecmp(aVector[k], "-opt") == 0) {
        if (k == aNumber - 1)
          showCorrectUsage(aVector[k]);
        k++;
        OpenOptinfofile(aVector[k]);

      } else if ((strcasecmp(aVector[k], "-printlikelihood") == 0) || (strcasecmp(aVector[k], "-likelihoodprint") == 0)) {
        if (k == aNumber - 1)
          showCorrectUsage(aVector[k]);
        k++;
        setPrintLikelihoodFilename(aVector[k]);

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

	/*      } else if (strcasecmp(aVector[k], "-printlog") == 0)
	if (k == aNumber - 1)
          showCorrectUsage(aVector[k]);
        k++;
	SetLogFile(*aVector[k]);*/
      } else
        showCorrectUsage(aVector[k]);

      k++;
    }
  }

  printinfo.checkNumbers();
  if ((stochastic != 1) && (netrun == 1)) {
    cout << "\nWarning - Gadget for the paramin network should be used with -s option\n"
      << "Gadget will now set the -s switch to perform a stochastic run\n";
    stochastic = 1;
    calclikelihood = 1;
  }

  if ((stochastic == 1) && (optimize == 1)) {
    cout << "\nWarning - Gadget has been started with both the -s switch and the -l switch\n"
      << "However, it is not possible to do both a stochastic run and an optimizing run!\n"
      << "Gadget will perform only the stochastic run (and ignore the -l switch)\n";
    optimize = 0;
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
      InitialCommentFilename = new char[strlen(text) + 1];
      strcpy(InitialCommentFilename, text);
      InitialCondareGiven = 1;
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
      setMainGadgetFilename(text);
    } else if (strcasecmp(text, "-printinitial") == 0) {
      infile >> text >> ws;
      setPrintInitialCondFilename(text);
    } else if (strcasecmp(text, "-printfinal") == 0) {
      infile >> text >> ws;
      setPrintFinalCondFilename(text);
    } else if (strcasecmp(text, "-printlikelihood") == 0) {
      infile >> text >> ws;
      setPrintLikelihoodFilename(text);
    } else if (strcasecmp(text, "-opt") == 0) {
      infile >> text >> ws;
      OpenOptinfofile(text);
    } else if (strcasecmp(text, "-print1") == 0) {
      infile >> dummy >> ws;
      printinfo.setPrint1(dummy);
    } else if (strcasecmp(text, "-print2") == 0) {
      infile >> dummy >> ws;
      printinfo.setPrint2(dummy);
    } else if (strcasecmp(text, "-precision") == 0) {
      infile >> dummy >> ws;
      printinfo.setPrecision(dummy);
      /*    } else if(strcasecmp(text, "-printlog") == 0) {
      infile >> dummy >> ws;
      SetLogFile(dummy);*/
    } else
      showCorrectUsage(text);
  }
  printinfo.checkNumbers();
}

void MainInfo::setPrintInitialCondFilename(char* filename) {
  if (PrintInitialCondFilename != NULL) {
    delete[] PrintInitialCondFilename;
    PrintInitialCondFilename = NULL;
  }
  PrintInitialCondFilename = new char[strlen(filename) + 1];
  strcpy(PrintInitialCondFilename, filename);
  PrintInitialcond = 1;
}

void MainInfo::setPrintFinalCondFilename(char* filename) {
  if (PrintFinalCondFilename != NULL) {
    delete[] PrintFinalCondFilename;
    PrintFinalCondFilename = NULL;
  }
  PrintFinalCondFilename = new char[strlen(filename) + 1];
  strcpy(PrintFinalCondFilename, filename);
  PrintFinalcond = 1;
}

void MainInfo::setPrintLikelihoodFilename(char* filename) {
  if (PrintLikelihoodFilename != NULL) {
    delete[] PrintLikelihoodFilename;
    PrintLikelihoodFilename = NULL;
  }
  PrintLikelihoodFilename = new char[strlen(filename) + 1];
  strcpy(PrintLikelihoodFilename, filename);
  PrintLikelihoodInfo = 1;
}

void MainInfo::setInitialParamFilename(char* filename) {
  if (InitialCommentFilename != NULL) {
    delete[] InitialCommentFilename;
    InitialCommentFilename = NULL;
  }
  InitialCommentFilename = new char[strlen(filename) + 1];
  strcpy(InitialCommentFilename, filename);
  InitialCondareGiven = 1;
}

void MainInfo::setMainGadgetFilename(char* filename) {
  if (MainGadgetFilename != NULL) {
    delete[] MainGadgetFilename;
    MainGadgetFilename = NULL;
  }
  MainGadgetFilename = new char[strlen(filename) + 1];
  strcpy(MainGadgetFilename, filename);
}
