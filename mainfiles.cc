#include "mainfiles.h"
#include "errorhandler.h"
#include "gadget.h"

void MainInfo::ShowCorrectUsage() {
  cerr << "Error in command line values used when starting Gadget\n"
    << "Options must be from -l -s -n -i file -o file -co file -p file -opt file\n"
    << "-m file -print1 int -print2 int -printinitial filename -printfinal filename\n";
  exit(EXIT_FAILURE);
}

MainInfo::MainInfo()
  : optinfocommentfile(optinfofile), OptInfoFileisGiven(0),
    InitialCondareGiven(0),  CalcLikelihood(0), Optimize(0),
    Stochastic(0), PrintInitialcond(0), PrintFinalcond(0),
    PrintLikelihoodInfo(0), Net(0) {

  char mainfile[10];
  strncpy(mainfile, "", 10);
  strcpy(mainfile, "main");

  optinfofilename = NULL;
  InitialCommentFilename = NULL;
  PrintInitialcondfilename = NULL;
  PrintFinalcondfilename = NULL;
  PrintLikelihoodFilename = NULL;
  MainGadgetFilename = NULL;
  SetMainGadgetFilename(mainfile);
}

MainInfo::~MainInfo() {
  if (optinfofilename != NULL) {
    delete[] optinfofilename;
    optinfofilename = NULL;
  }
  if (InitialCommentFilename != NULL) {
    delete[] InitialCommentFilename;
    InitialCommentFilename = NULL;
  }
  if (PrintInitialcondfilename != NULL) {
    delete[] PrintInitialcondfilename;
    PrintInitialcondfilename = NULL;
  }
  if (PrintFinalcondfilename != NULL) {
    delete[] PrintFinalcondfilename;
    PrintFinalcondfilename = NULL;
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
  ErrorHandler handle;
  handle.Open(filename);
  optinfofilename = new char[strlen(filename) + 1];
  strcpy(optinfofilename, filename);
  optinfofile.open(optinfofilename, ios::in);
  CheckIfFailure(optinfofile, filename);
  OptInfoFileisGiven = 1;
  handle.Close();
}

void MainInfo::CloseOptinfofile() {
  optinfofile.close();
  optinfofile.clear();
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
          ShowCorrectUsage();
        infile.open(aVector[k + 1]);
        if (infile.fail())
          ShowCorrectUsage();
        this->Read(incomment);
        infile.close();
        infile.clear();

      } else if (strcasecmp(aVector[k], "-i") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage();
        SetInitialCommentFilename(aVector[k + 1]);
        k++;

      } else if (strcasecmp(aVector[k], "-o") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage();
        printinfo.SetOutputFile(aVector[k + 1]);
        k++;

      } else if (strcasecmp(aVector[k], "-p") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage();
        printinfo.SetParamOutFile(aVector[k + 1]);
        k++;

      } else if (strcasecmp(aVector[k], "-print") == 0) {
        printinfo.forcePrint = 1;

      } else if (strcasecmp(aVector[k], "-surveyprint") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage();
        printinfo.surveyprint = atoi(aVector[k + 1]);
        k++;

      } else if (strcasecmp(aVector[k], "-stomachprint") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage();
        printinfo.stomachprint = atoi(aVector[k + 1]);
        k++;

      } else if (strcasecmp(aVector[k], "-catchprint") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage();
        printinfo.catchprint = atoi(aVector[k + 1]);
        k++;

      } else if (strcasecmp(aVector[k], "-co") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage();
        printinfo.SetColumnOutputFile(aVector[k + 1]);
        k++;

      } else if (strcasecmp(aVector[k], "-printinitial") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage();
        SetPrintInitialCondFilename(aVector[k + 1]);
        k++;

      } else if (strcasecmp(aVector[k], "-printfinal") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage();
        SetPrintFinalCondFilename(aVector[k + 1]);
        k++;

      } else if (strcasecmp(aVector[k], "-main") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage();
        SetMainGadgetFilename(aVector[k + 1]);
        k++;

      } else if (strcasecmp(aVector[k], "-opt") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage();
        OpenOptinfofile(aVector[k + 1]);
        k++;

      } else if (strcasecmp(aVector[k], "-likelihoodprint") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage();
        SetPrintLikelihoodFilename(aVector[k + 1]);
        k++;

      } else if (strcasecmp(aVector[k], "-print1") == 0) {
        stringstream str;
        if (k == aNumber - 1)
          ShowCorrectUsage();
        str << aVector[k + 1];
        k++;
        str >> printinfo.PrintInterVal1;
        if (str.fail())  //str.eof() depends on compilers
          ShowCorrectUsage();

      } else if (strcasecmp(aVector[k], "-print2") == 0) {
        stringstream str;
        if (k == aNumber - 1)
          ShowCorrectUsage();
        str << aVector[k + 1];
        k++;
        str >> printinfo.PrintInterVal2;
        if (str.fail())  //str.eof() depends on compilers
          ShowCorrectUsage();

      } else if (strcasecmp(aVector[k], "-precision") == 0) {
        //JMB - experimental setting of printing precision
        stringstream str;
        if (k == aNumber - 1)
          cerr << "Error in specifying precision\n";
        str << aVector[k + 1];
        k++;
        str >> printinfo.givenPrecision;
        if (str.fail())  //str.eof() depends on compilers
          cerr << "Error in specifying precision\n";

      } else
        ShowCorrectUsage();

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
  while (!infile.eof())
    infile >> ws >> text;

  if (strcasecmp(text, "-i") == 0) {
    infile >> ws >> text;
    InitialCommentFilename = new char[strlen(text) + 1];
    strcpy(InitialCommentFilename, text);
    InitialCondareGiven = 1;
  } else if (strcasecmp(text, "-o") == 0) {
    infile >> ws >> text;
    printinfo.SetOutputFile(text);
  } else if (strcasecmp(text, "-co") == 0) {
    infile >> ws >> text;
    printinfo.SetColumnOutputFile(text);
  } else if (strcasecmp(text, "-p") == 0) {
    infile >> ws >> text;
    printinfo.SetParamOutFile(text);
  } else if (strcasecmp(text, "-main") == 0) {
    infile >> ws >> text;
    SetMainGadgetFilename(text);
  } else if (strcasecmp(text, "-printinitial") == 0) {
    infile >> ws >> text;
    SetPrintInitialCondFilename(text);
  } else if (strcasecmp(text, "-printfinal") == 0) {
    infile >> ws >> text;
    SetPrintFinalCondFilename(text);
  } else if (strcasecmp(text, "-opt") == 0) {
    infile >> ws >> text;
    OpenOptinfofile(text);
  } else if (strcasecmp(text, "-print1") == 0) {
    infile >> ws >> printinfo.PrintInterVal1;
  } else if (strcasecmp(text, "-print2") == 0) {
    infile >> ws >> printinfo.PrintInterVal2;
  } else if (strcasecmp(text, "-precision") == 0) {
    infile >> ws >> printinfo.givenPrecision;
  } else
    ShowCorrectUsage();

  if (infile.eof() || infile.bad())
    ShowCorrectUsage();
  printinfo.CheckNumbers();
}

void MainInfo::SetPrintInitialCondFilename(char* filename) {
  PrintInitialcondfilename = new char[strlen(filename) + 1];
  strcpy(PrintInitialcondfilename, filename);
  PrintInitialcond = 1;
}

void MainInfo::SetPrintFinalCondFilename(char* filename) {
  PrintFinalcondfilename = new char[strlen(filename) + 1];
  strcpy(PrintFinalcondfilename, filename);
  PrintFinalcond = 1;
}

void MainInfo::SetPrintLikelihoodFilename(char* filename) {
  PrintLikelihoodFilename = new char[strlen(filename) + 1];
  strcpy(PrintLikelihoodFilename, filename);
  PrintLikelihoodInfo = 1;
}

void MainInfo::SetInitialCommentFilename(char* filename) {
  InitialCommentFilename = new char[strlen(filename) + 1];
  strcpy(InitialCommentFilename, filename);
  InitialCondareGiven = 1;
}

void MainInfo::SetMainGadgetFilename(char* filename) {
  MainGadgetFilename = new char[strlen(filename) + 1];
  strcpy(MainGadgetFilename, filename);
}
