#include "maininfo.h"
#include "errorhandler.h"
#include "gadget.h"

#include "runid.h"
extern RunID RUNID;

void MainInfo::ShowCorrectUsage(char* error) {
  RUNID.print(cerr);
  cerr << "\nError in command line value - unrecognised option " << error << endl
    << "Common options are -l or -s, -i <filename> -o <filename>\n"
    << "For more information try running Gadget with the -h switch\n";
  exit(EXIT_FAILURE);
}

void MainInfo::ShowUsage() {
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
    << "\nFor more information see the Gadget web page at http://www.hafro.is/gadget\n";
  exit(EXIT_SUCCESS);
}

MainInfo::MainInfo()
  : OptinfoCommentFile(OptinfoFile), OptInfoFileisGiven(0), InitialCondareGiven(0),
    CalcLikelihood(0), Optimize(0), Stochastic(0), PrintInitialcond(0), PrintFinalcond(0),
    PrintLikelihoodInfo(0), Net(0) {

  char tmpname[10];
  strncpy(tmpname, "", 10);
  strcpy(tmpname, "main");

  OptinfoFilename = NULL;
  InitialCommentFilename = NULL;
  PrintInitialCondFilename = NULL;
  PrintFinalCondFilename = NULL;
  PrintLikelihoodFilename = NULL;
  MainGadgetFilename = NULL;
  SetMainGadgetFilename(tmpname);
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

void MainInfo::Read(int aNumber, char* const aVector[]) {
  int k, len;
  if (aNumber > 1) {
    k = 1;
    while (k < aNumber) {
      if (strcasecmp(aVector[k], "-l") == 0) {
        CalcLikelihood = 1;
        Optimize = 1;

      } else if (strcasecmp(aVector[k], "-n") == 0) {
        Net = 1;

        #ifndef GADGET_NETWORK
          cout << "\nWarning: Gadget is trying to run in the network mode for paramin without\n"
            << "the network support being compiled - no network communication can take place!\n";
        #endif

      } else if (strcasecmp(aVector[k], "-s") == 0) {
        Stochastic = 1;
        CalcLikelihood = 1;

      } else if (strcasecmp(aVector[k], "-m") == 0) {
        ifstream infile;
        CommentStream incomment(infile);
        if (k == aNumber - 1)
          ShowCorrectUsage(aVector[k]);

        k++;
        infile.open(aVector[k]);
        if (infile.fail())
          ShowCorrectUsage(aVector[k]);
        this->Read(incomment);
        infile.close();
        infile.clear();

      } else if (strcasecmp(aVector[k], "-i") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage(aVector[k]);
        k++;
        SetInitialCommentFilename(aVector[k]);

      } else if (strcasecmp(aVector[k], "-o") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage(aVector[k]);
        k++;
        printinfo.SetOutputFile(aVector[k]);

      } else if (strcasecmp(aVector[k], "-p") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage(aVector[k]);
        k++;
        printinfo.SetParamOutFile(aVector[k]);

      } else if (strcasecmp(aVector[k], "-print") == 0) {
        printinfo.forcePrint = 1;

      } else if (strcasecmp(aVector[k], "-surveyprint") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage(aVector[k]);
        k++;
        printinfo.surveyprint = atoi(aVector[k]);

      } else if (strcasecmp(aVector[k], "-stomachprint") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage(aVector[k]);
        k++;
        printinfo.stomachprint = atoi(aVector[k]);

      } else if (strcasecmp(aVector[k], "-catchprint") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage(aVector[k]);
        k++;
        printinfo.catchprint = atoi(aVector[k]);

      } else if (strcasecmp(aVector[k], "-co") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage(aVector[k]);
        k++;
        printinfo.SetColumnOutputFile(aVector[k]);

      } else if (strcasecmp(aVector[k], "-printinitial") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage(aVector[k]);
        k++;
        SetPrintInitialCondFilename(aVector[k]);

      } else if (strcasecmp(aVector[k], "-printfinal") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage(aVector[k]);
        k++;
        SetPrintFinalCondFilename(aVector[k]);

      } else if (strcasecmp(aVector[k], "-main") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage(aVector[k]);
        k++;
        SetMainGadgetFilename(aVector[k]);

      } else if (strcasecmp(aVector[k], "-opt") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage(aVector[k]);
        k++;
        OpenOptinfofile(aVector[k]);

      } else if ((strcasecmp(aVector[k], "-printlikelihood") == 0) || (strcasecmp(aVector[k], "-likelihoodprint") == 0)) {
        if (k == aNumber - 1)
          ShowCorrectUsage(aVector[k]);
        k++;
        SetPrintLikelihoodFilename(aVector[k]);

      } else if (strcasecmp(aVector[k], "-print1") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage(aVector[k]);
        k++;
        stringstream str;
        str << aVector[k];
        str >> printinfo.PrintInterVal1;
        if (str.fail())  //str.eof() depends on compilers
          ShowCorrectUsage(aVector[k]);

      } else if (strcasecmp(aVector[k], "-print2") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage(aVector[k]);
        k++;
        stringstream str;
        str << aVector[k];
        str >> printinfo.PrintInterVal2;
        if (str.fail())  //str.eof() depends on compilers
          ShowCorrectUsage(aVector[k]);

      } else if (strcasecmp(aVector[k], "-precision") == 0) {
        //JMB - experimental setting of printing precision
        if (k == aNumber - 1)
          ShowCorrectUsage(aVector[k]);
        k++;
        stringstream str;
        str << aVector[k];
        str >> printinfo.givenPrecision;
        if (str.fail())  //str.eof() depends on compilers
          ShowCorrectUsage(aVector[k]);

      } else if ((strcasecmp(aVector[k], "-v") == 0) || (strcasecmp(aVector[k], "--version") == 0)) {
        RUNID.print(cout);
        exit(EXIT_SUCCESS);

      } else if ((strcasecmp(aVector[k], "-h") == 0) || (strcasecmp(aVector[k], "--help") == 0)) {
        ShowUsage();

      } else
        ShowCorrectUsage(aVector[k]);

      k++;
    }
  }

  printinfo.CheckNumbers();
  if ((Stochastic != 1) && (Net == 1)) {
    cout << "\nWarning: Gadget for the paramin network should be used with -s option\n"
      << "Gadget will now set the -s switch to perform a stochastic run\n";
    Stochastic = 1;
    CalcLikelihood = 1;
  }

  if ((Stochastic == 1) && (Optimize == 1)) {
    cout << "\nWarning: Gadget has been started with both the -s switch and the -l switch\n"
      << "However, it is not possible to do both a stochastic run and an optimizing run!\n"
      << "Gadget will perform only the stochastic run (and ignore the -l switch)\n";
    Optimize = 0;
  }
}

void MainInfo::Read(CommentStream& infile) {
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
      printinfo.SetOutputFile(text);
    } else if (strcasecmp(text, "-co") == 0) {
      infile >> text >> ws;
      printinfo.SetColumnOutputFile(text);
    } else if (strcasecmp(text, "-p") == 0) {
      infile >> text >> ws;
      printinfo.SetParamOutFile(text);
    } else if (strcasecmp(text, "-main") == 0) {
      infile >> text >> ws;
      SetMainGadgetFilename(text);
    } else if (strcasecmp(text, "-printinitial") == 0) {
      infile >> text >> ws;
      SetPrintInitialCondFilename(text);
    } else if (strcasecmp(text, "-printfinal") == 0) {
      infile >> text >> ws;
      SetPrintFinalCondFilename(text);
    } else if (strcasecmp(text, "-printlikelihood") == 0) {
      infile >> text >> ws;
      SetPrintLikelihoodFilename(text);
    } else if (strcasecmp(text, "-opt") == 0) {
      infile >> text >> ws;
      OpenOptinfofile(text);
    } else if (strcasecmp(text, "-print1") == 0) {
      infile >> printinfo.PrintInterVal1 >> ws;
    } else if (strcasecmp(text, "-print2") == 0) {
      infile >> printinfo.PrintInterVal2 >> ws;
    } else if (strcasecmp(text, "-precision") == 0) {
      infile >> printinfo.givenPrecision >> ws;
    } else
      ShowCorrectUsage(text);
  }
  printinfo.CheckNumbers();
}

void MainInfo::SetPrintInitialCondFilename(char* filename) {
  if (PrintInitialCondFilename != NULL) {
    delete[] PrintInitialCondFilename;
    PrintInitialCondFilename = NULL;
  }
  PrintInitialCondFilename = new char[strlen(filename) + 1];
  strcpy(PrintInitialCondFilename, filename);
  PrintInitialcond = 1;
}

void MainInfo::SetPrintFinalCondFilename(char* filename) {
  if (PrintFinalCondFilename != NULL) {
    delete[] PrintFinalCondFilename;
    PrintFinalCondFilename = NULL;
  }
  PrintFinalCondFilename = new char[strlen(filename) + 1];
  strcpy(PrintFinalCondFilename, filename);
  PrintFinalcond = 1;
}

void MainInfo::SetPrintLikelihoodFilename(char* filename) {
  if (PrintLikelihoodFilename != NULL) {
    delete[] PrintLikelihoodFilename;
    PrintLikelihoodFilename = NULL;
  }
  PrintLikelihoodFilename = new char[strlen(filename) + 1];
  strcpy(PrintLikelihoodFilename, filename);
  PrintLikelihoodInfo = 1;
}

void MainInfo::SetInitialCommentFilename(char* filename) {
  if (InitialCommentFilename != NULL) {
    delete[] InitialCommentFilename;
    InitialCommentFilename = NULL;
  }
  InitialCommentFilename = new char[strlen(filename) + 1];
  strcpy(InitialCommentFilename, filename);
  InitialCondareGiven = 1;
}

void MainInfo::SetMainGadgetFilename(char* filename) {
  if (MainGadgetFilename != NULL) {
    delete[] MainGadgetFilename;
    MainGadgetFilename = NULL;
  }
  MainGadgetFilename = new char[strlen(filename) + 1];
  strcpy(MainGadgetFilename, filename);
}
