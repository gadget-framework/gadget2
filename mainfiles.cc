#include "mainfiles.h"
#include "errorhandler.h"
#include "gadget.h"

void MainInfo::ShowCorrectUsage() {
  cerr << " The options must be -l -s -m file -i file -o file -co file "
   << "-opt file\n -print1 int or -print2 int -likelihoodprint filename\n";
  exit(EXIT_FAILURE);
}

MainInfo::MainInfo()
  : optinfocommentfile(optinfofile), OptInfoFileisGiven(0),
    InitialCondareGiven(0),  CalcLikelihood(0), Optimize(0),
    Stochastic(0), PrintInitialcond(0), PrintFinalcond(0),
    PrintLikelihoodInfo(0), Net(0) {

  optinfofilename = NULL;
  InitialCommentFilename = NULL;
  PrintInitialcondfilename = NULL;
  PrintFinalcondfilename = NULL;
  PrintLikelihoodFilename = NULL;
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

        //Change AJ, 13.03.00
        #ifndef GADGET_NETWORK
          cout << "Warning! Trying to run the network version of Gadget\n"
           << "         without network support compiled in.\n";
        #endif

        if (Stochastic != 1)  {
          cout << "Warning, netrun should be used with -s option\n";
          Stochastic = 1;
          CalcLikelihood = 1;
        }
        //InitialCondareGiven = 1;
        //STOP CHANGE AJ

      } else if (strcasecmp(aVector[k], "-s") == 0) {
        Stochastic = 1;
        CalcLikelihood = 1;

      } else if (strcmp(aVector[k], "-M") == 0) {
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

      } else if (strcmp(aVector[k], "-i") == 0) {
        len = strlen(aVector[k + 1]);
        InitialCommentFilename = new char[len + 1];
        if (k == aNumber - 1)
          ShowCorrectUsage();
        strcpy(InitialCommentFilename, aVector[k + 1]);
        k++;
        InitialCondareGiven = 1;

      } else if (strcmp(aVector[k], "-o") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage();
        printinfo.SetOutputFile(aVector[k + 1]);
        k++;

      } else if (strcmp(aVector[k], "-print") == 0) {
        printinfo.forcePrint = 1;

      } else if (strcmp(aVector[k], "-surveyprint") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage();
        printinfo.surveyprint = atoi(aVector[k + 1]);
        k++;

      } else if (strcmp(aVector[k], "-stomachprint") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage();
        printinfo.stomachprint = atoi(aVector[k + 1]);
        k++;

      } else if (strcmp(aVector[k], "-catchprint") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage();
        printinfo.catchprint = atoi(aVector[k + 1]);
        k++;

      } else if (strcmp(aVector[k], "-co") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage();
        printinfo.SetColumnOutputFile(aVector[k + 1]);
        k++;

      } else if (strcmp(aVector[k], "-printinitial") == 0) {
        SetPrintInitialCondFilename(aVector[k + 1]);
        k++;

      } else if (strcmp(aVector[k], "-printfinal") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage();
        SetPrintFinalCondFilename(aVector[k + 1]);
        k++;

      } else if (strcmp(aVector[k], "-opt") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage();
        OpenOptinfofile(aVector[k + 1]);
        k++;

      } else if (strcmp(aVector[k], "-likelihoodprint") == 0) {
        if (k == aNumber - 1)
          ShowCorrectUsage();
        SetPrintLikelihoodFilename(aVector[k + 1]);
        k++;

      } else if (strcmp(aVector[k], "-print1") == 0) {
        strstream str;
        if (k == aNumber - 1)
          ShowCorrectUsage();
        str << aVector[k + 1];
        k++;
        str >> printinfo.PrintInterVal1;
        if (str.fail())  //str.eof() depends on compilers
          ShowCorrectUsage();

      } else if (strcmp(aVector[k], "-print2") == 0) {
        strstream str;
        if (k == aNumber - 1)
          ShowCorrectUsage();
        str << aVector[k + 1];
        k++;
        str >> printinfo.PrintInterVal2;
        if (str.fail())  //str.eof() depends on compilers
          ShowCorrectUsage();

      } else
        ShowCorrectUsage();

      k++;
    }
  }
}

void MainInfo::SetPrintLikelihoodFilename(char* filename) {
  //name of file seems to cause problem.
  ErrorHandler handle;
  handle.Open(filename);
  int len = strlen(filename);
  PrintLikelihoodFilename = new char[len + 1];
  strcpy(PrintLikelihoodFilename, filename);
  PrintLikelihoodInfo = 1;
  handle.Close();
}

void MainInfo::Read(CommentStream& infile) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  while (!infile.eof())
    infile >> ws >> text;

  if (strcmp(text, "-i") == 0) {
    infile >> ws >> text;
    InitialCommentFilename = new char[strlen(text) + 1];
    strcpy(InitialCommentFilename, text);
    InitialCondareGiven = 1;
  } else if (strcmp(text, "-o") == 0) {
    infile >> ws >> text;
    printinfo.SetOutputFile(text);
  } else if (strcmp(text, "-co") == 0) {
    infile >> text;
    printinfo.SetColumnOutputFile(text);
  } else if (strcmp(text, "-printinitial") == 0) {
    infile >> ws >> text;
    SetPrintInitialCondFilename(text);
  } else if (strcmp(text, "-printfinal") == 0) {
    infile >> ws >> text;
    SetPrintFinalCondFilename(text);
  } else if (strcmp(text, "-opt") == 0) {
    infile >> ws >> text;
    OpenOptinfofile(text);
  } else if (strcmp(text, "-print1") == 0) {
    infile >> printinfo.PrintInterVal1;
  } else if (strcmp(text, "-print2") == 0) {
    infile >> printinfo.PrintInterVal2;
  } else
    ShowCorrectUsage();

  if (infile.eof() || infile.bad())
    ShowCorrectUsage();
}

void MainInfo::SetPrintInitialCondFilename(char* filename) {
  int len = strlen(filename);
  PrintInitialcondfilename = new char[len + 1];
  strcpy(PrintInitialcondfilename, filename);
  PrintInitialcond = 1;
}

void MainInfo::SetPrintFinalCondFilename(char* filename) {
  int len = strlen(filename);
  PrintFinalcondfilename = new char[len + 1];
  strcpy(PrintFinalcondfilename, filename);
  PrintFinalcond = 1;
}

