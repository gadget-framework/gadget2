#include "ecosystem.h"
#include "keeper.h"
#include "stockstdprinter.h"
#include "stockprinter.h"
#include "predatorprinter.h"
#include "predatoroverprinter.h"
#include "preyoverprinter.h"
#include "stockpreyfullprinter.h"
#include "predpreystdprinter.h"
#include "stockfullprinter.h"
#include "formatedstockprinter.h"
#include "formatedchatprinter.h"
#include "formatedpreyprinter.h"
#include "likelihoodprinter.h"
#include "mortprinter.h"
#include "biomassprinter.h"
#include "surveyindices.h"
#include "understocking.h"
#include "catchdistribution.h"
#include "stockdistribution.h"
#include "catchstatistics.h"
#include "stomachcontent.h"
#include "tagdata.h"
#include "predatorindex.h"
#include "migrationpenalty.h"
#include "catchintons.h"
#include "lenstock.h"
#include "boundlikelihood.h"
#include "logcatchfunction.h"
#include "readword.h"
#include "gadget.h"

//
// A function to read fleet information
//
int Ecosystem::ReadFleet(CommentStream& infile) {
  ErrorHandler handle;
  char text[MaxStrLength];
  char value[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  strncpy(value, "", MaxStrLength);

  int i = 0;
  while (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[fleetcomponent]") == 0))
      handle.Unexpected("[fleetcomponent]", text);

    infile >> text >> value;
    fleetnames.resize(1);
    fleetvec.resize(1);
    i = fleetnames.Size() - 1;
    fleetnames[i] = new char[strlen(value) + 1];

    if (strcasecmp(text, "totalfleet") == 0) {
      strcpy(fleetnames[i], value);
      fleetvec[i] = new Fleet(infile, fleetnames[i], Area, TimeInfo, keeper, TOTALAMOUNTGIVEN);

    } else if (strcasecmp(text, "linearfleet") == 0) {
      strcpy(fleetnames[i], value);
      fleetvec[i] = new Fleet(infile, fleetnames[i], Area, TimeInfo, keeper, LINEARFLEET);

    } else if (strcasecmp(text, "mortalityfleet") == 0) {
      strcpy(fleetnames[i], value);
      fleetvec[i] = new Fleet(infile, fleetnames[i], Area, TimeInfo, keeper, MORTALITYFLEET);

    } else
      handle.Message("Error in main file - unrecognised fleet", text);

  }
  return 1;
}

//
// A function to read tagging information
//
int Ecosystem::ReadTagging(CommentStream& infile) {
  ErrorHandler handle;
  char text[MaxStrLength];
  char value[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  strncpy(value, "", MaxStrLength);

  int i = 0;
  while (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[tagcomponent]") == 0))
      handle.Unexpected("[tagcomponent]", text);

    infile >> text >> value;
    tagnames.resize(1);
    tagvec.resize(1);
    i = tagnames.Size() - 1;
    tagnames[i] = new char[strlen(value) + 1];

    if (strcasecmp(text, "tagid") == 0) {
      strcpy(tagnames[i], value);
      tagvec[i] = new Tags(infile, tagnames[i], Area, TimeInfo, keeper);
    } else
      handle.Unexpected("tagid", text);

  }
  return 1;
}

//
// A function to read otherfood information
//
int Ecosystem::ReadOtherFood(CommentStream& infile) {
  ErrorHandler handle;
  char text[MaxStrLength];
  char value[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  strncpy(value, "", MaxStrLength);

  int i = 0;
  while (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[foodcomponent]") == 0))
      handle.Unexpected("[foodcomponent]", text);

    infile >> text >> value;
    otherfoodnames.resize(1);
    otherfoodvec.resize(1);
    i = otherfoodnames.Size() - 1;
    otherfoodnames[i] = new char[strlen(value) + 1];

    if (strcasecmp(text, "foodname") == 0) {
      strcpy(otherfoodnames[i], value);
      otherfoodvec[i] = new OtherFood(infile, otherfoodnames[i], Area, TimeInfo, keeper);
    } else
      handle.Unexpected("foodname", text);

  }
  return 1;
}

//
// A function to read stock information
// Note: there is only ever one stock in each file
//
int Ecosystem::ReadStock(CommentStream& infile, int mortmodel) {
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  if (!infile.eof())
    infile >> text >> ws;
  else
    strcpy(text, "");

  if (!(strcasecmp(text, "stockname") == 0))
    handle.Unexpected("stockname", text);

  infile >> text;
  stocknames.resize(1);
  stockvec.resize(1);
  int i = stocknames.Size() - 1;

  stocknames[i] = new char[strlen(text) + 1];
  strcpy(stocknames[i], text);

  if (mortmodel == 1)   //fleksibest model, mortalities applied to stock
    stockvec[i] = new LenStock(infile, stocknames[i], Area, TimeInfo, keeper);
  else                  //original bormicon stock model used
    stockvec[i] = new Stock(infile, stocknames[i], Area, TimeInfo, keeper);

  return 1;
}

//
// A function to read information on printing
//
int Ecosystem::ReadPrinters(CommentStream& infile) {
  ErrorHandler handle;
  char text[MaxStrLength];
  char type[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  strncpy(type, "", MaxStrLength);

  if (!infile.eof())
    infile >> text >> ws;
  else
    strcpy(text, "");

  if (!(strcasecmp(text, "[component]") == 0))
    handle.Unexpected("[component]", text);

  while (!infile.eof() && !infile.fail()) {
    readWordAndValue(infile, "type", type);

    if (strcasecmp(type, "stockstdprinter") == 0)
      printvec.resize(1, new StockStdPrinter(infile, Area, TimeInfo));
    else if (strcasecmp(type, "stockfullprinter") == 0)
      printvec.resize(1, new StockFullPrinter(infile, Area, TimeInfo));
    else if (strcasecmp(type, "stockprinter") == 0)
      printvec.resize(1, new StockPrinter(infile, Area, TimeInfo));
    else if (strcasecmp(type, "predatorprinter") == 0)
      printvec.resize(1, new PredatorPrinter(infile, Area, TimeInfo));
    else if (strcasecmp(type, "predatoroverprinter") == 0)
      printvec.resize(1, new PredatorOverPrinter(infile, Area, TimeInfo));
    else if (strcasecmp(type, "preyoverprinter") == 0)
      printvec.resize(1, new PreyOverPrinter(infile, Area, TimeInfo));
    else if (strcasecmp(type, "stockpreyfullprinter") == 0)
      printvec.resize(1, new StockPreyFullPrinter(infile, Area, TimeInfo));
    else if (strcasecmp(type, "predpreystdlengthprinter") == 0)
      printvec.resize(1, new PredPreyStdLengthPrinter(infile, Area, TimeInfo));
    else if (strcasecmp(type, "predpreystdageprinter") == 0)
      printvec.resize(1, new PredPreyStdAgePrinter(infile, Area, TimeInfo));
    else if (strcasecmp(type, "formatedstockprinter") == 0)
      printvec.resize(1, new FormatedStockPrinter(infile, Area, TimeInfo));
    else if (strcasecmp(type, "formatedchatprinter") == 0)
      printvec.resize(1, new FormatedCHatPrinter(infile, Area, TimeInfo));
    else if (strcasecmp(type, "formatedpreyprinter") == 0)
      printvec.resize(1, new FormatedPreyPrinter(infile, Area, TimeInfo));
    else if (strcasecmp(type, "mortprinter") == 0)
      printvec.resize(1, new MortPrinter(infile, Area, TimeInfo));
    else if (strcasecmp(type, "biomassprinter") == 0)
      printvec.resize(1, new BiomassPrinter(infile, Area, TimeInfo));
    else if (strcasecmp(type, "formatedcatchprinter") == 0)
      cout << "The formatedcatchprinter printer class is no longer supported\n";
    else if (strcasecmp(type, "likelihoodprinter") == 0)
      likprintvec.resize(1, new LikelihoodPrinter(infile, Area, TimeInfo));
    else
      handle.Message("Error in main file - unrecognised printer", type);

  }
  return 1;
}

//
// A function to read likelihood information
//
int Ecosystem::ReadLikelihood(CommentStream& infile) {
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  if (!infile.eof())
    infile >> text >> ws;
  else
    strcpy(text, "");

  int i = 0;
  double weight = 0.0;
  char type[MaxStrLength];
  char name[MaxStrLength];
  char datafilename[MaxStrLength];
  strncpy(type, "", MaxStrLength);
  strncpy(name, "", MaxStrLength);
  strncpy(datafilename, "", MaxStrLength);

  ifstream datafile;
  CommentStream subdata(datafile);

  if (!(strcasecmp(text, "[component]") == 0))
    handle.Unexpected("[component]", text);

  while (!infile.eof() && !infile.fail()) {
    readWordAndValue(infile, "name", name);
    readWordAndVariable(infile, "weight", weight);
    readWordAndValue(infile, "type", type);

    keeper->AddComponent(name);
    Likely.resize(1);
    i = Likely.Size() - 1;

    if (strcasecmp(type, "penalty") == 0) {
      readWordAndValue(infile, "datafile", datafilename);
      datafile.open(datafilename);
      checkIfFailure(datafile, datafilename);
      handle.Open(datafilename);
      Likely[i] = new BoundLikelihood(subdata, Area, TimeInfo, keeper, weight);
      handle.Close();
      datafile.close();
      datafile.clear();

      //prepare for next likelihood component
      infile >> ws;
      if (!infile.eof()) {
        infile >> text >> ws;
        if (!(strcasecmp(text, "[component]") == 0))
          handle.Unexpected("[component]", text);
      }

    } else if (strcasecmp(type, "understocking") == 0) {
      Likely[i] = new UnderStocking(infile, Area, TimeInfo, weight);

    } else if (strcasecmp(type, "catchstatistics") == 0) {
      Likely[i] = new CatchStatistics(infile, Area, TimeInfo, weight, name);

    } else if (strcasecmp(type, "catchdistribution") == 0) {
      Likely[i] = new CatchDistribution(infile, Area, TimeInfo, weight, name);

    } else if (strcasecmp(type, "stockdistribution") == 0) {
      Likely[i] = new StockDistribution(infile, Area, TimeInfo, weight);

    } else if (strcasecmp(type, "surveyindices") == 0) {
      Likely[i] = new SurveyIndices(infile, Area, TimeInfo, keeper, weight, name);

    } else if (strcasecmp(type, "logcatch") == 0) {
      Likely[i] = new LogCatches(infile, Area, TimeInfo, weight);

    } else if (strcasecmp(type, "stomachcontent") == 0) {
      Likely[i] = new StomachContent(infile, Area, TimeInfo, keeper, weight, name);

    } else if (strcasecmp(type, "tagdata") == 0) {
      Likely[i] = new TagData(infile, Area, TimeInfo, weight);

    } else if (strcasecmp(type, "catchintons") == 0) {
      Likely[i] = new CatchInTons(infile, Area, TimeInfo, weight);

    } else if (strcasecmp(type, "predatorindices") == 0) {
      Likely[i] = new PredatorIndices(infile, Area, TimeInfo, weight, name);

    } else if (strcasecmp(type, "migrationpenalty") == 0) {
      Likely[i] = new MigrationPenalty(infile, weight);

    } else if (strcasecmp(type, "randomwalk") == 0) {
      cout << "The random walk likelihood component is no longer supported\n";

    } else if (strcasecmp(type, "logsurveyindices") == 0) {
      cout << "The log survey indices likelihood component is no longer supported\n"
        << "Use a survey indices component with a log function instead\n";

    } else if (strcasecmp(type, "aggregatedcatchdist") == 0) {
      cout << "The aggregated catch distribution likelihood component is no longer supported\n";

    } else {
      handle.Message("Error in main file - unrecognised likelihood", type);
    }
  }
  return 1;
}

//
//The main reading function
//
void Ecosystem::Readmain(CommentStream& infile, int optimize, int netrun,
  int calclikelihood, const char* const inputdir, const char* const workingdir) {

  ErrorHandler handle;
  char text[MaxStrLength];
  char filename[MaxStrLength];
  int mort_mod = 0;
  strncpy(text, "", MaxStrLength);
  strncpy(filename, "", MaxStrLength);

  ifstream subfile;
  CommentStream subcomment(subfile);

  //first, read in the time information
  readWordAndValue(infile, "timefile", filename);
  subfile.open(filename);
  checkIfFailure(subfile, filename);
  handle.Open(filename);
  TimeInfo = new TimeClass(subcomment);
  handle.Close();
  subfile.close();
  subfile.clear();

  //second, read in the area information
  readWordAndValue(infile, "areafile", filename);
  subfile.open(filename);
  checkIfFailure(subfile, filename);
  handle.Open(filename);
  Area = new AreaClass(subcomment, TimeInfo);
  handle.Close();
  subfile.close();
  subfile.clear();

  //next, read in the printing information
  infile >> text >> ws;
  if (!(strcasecmp(text, "printfile") == 0))
    handle.Unexpected("printfile", text);

  //check if a printfile is specified, or jump to [stock]
  infile >> text >> ws;
  if (!(strcasecmp(text, "[stock]") == 0)) {
    //Do not read printfile if we are doing a net run, or if we are
    //optimizing without the forcePrint option set. 07.04.00 AJ & mnaa.
    if (!netrun  && (printinfo.forcePrint || !optimize)) {
      subfile.open(text);
      checkIfFailure(subfile, text);
      handle.Open(text);
      chdir(workingdir);
      this->ReadPrinters(subcomment);
      chdir(inputdir);
      handle.Close();
      subfile.close();
      subfile.clear();
    }
    infile >> text >> ws;
  }

  //then read in the stock information
  if (!(strcasecmp(text, "[stock]") == 0))
    handle.Unexpected("[stock]", text);
  infile >> text >> ws;

  if (strcasecmp(text, "mortalitymodel") == 0) {
    mort_mod = 1;
    infile >> text >> ws;
  } else
    mort_mod = 0;

  if (!(strcasecmp(text, "stockfiles") == 0))
    handle.Unexpected("stockfiles", text);

  //Now we have found the string "stockfiles" we can create stock
  infile >> text >> ws;
  while (!((strcasecmp(text, "[tagging]") == 0) || infile.eof())) {
    subfile.open(text);
    checkIfFailure(subfile, text);
    handle.Open(text);
    chdir(workingdir);
    this->ReadStock(subcomment, mort_mod);
    chdir(inputdir);
    handle.Close();
    subfile.close();
    subfile.clear();

    if (!infile.eof())
      infile >> text >> ws;
  }

  //Now we read the names of the tagging files
  if (!(strcasecmp(text, "[tagging]") == 0))
    handle.Unexpected("[tagging]", text);

  infile >> text >> ws;
  if (strcasecmp(text, "tagfiles") == 0) {
    //There might not be any tagging files
    infile >> text >> ws;
    while (!((strcasecmp(text, "[otherfood]") == 0) || infile.eof())) {
      subfile.open(text);
      checkIfFailure(subfile, text);
      handle.Open(text);
      chdir(workingdir);
      this->ReadTagging(subcomment);
      chdir(inputdir);
      handle.Close();
      subfile.close();
      subfile.clear();

      if (!infile.eof())
        infile >> text >> ws;
    }
  }

  //Now we read the names of the otherfood files
  if (!(strcasecmp(text, "[otherfood]") == 0))
    handle.Unexpected("[otherfood]", text);

  infile >> text >> ws;
  if (strcasecmp(text, "otherfoodfiles") == 0) {
    //There might not be any otherfood files
    infile >> text >> ws;
    while (!((strcasecmp(text, "[fleet]") == 0) || infile.eof())) {
      subfile.open(text);
      checkIfFailure(subfile, text);
      handle.Open(text);
      chdir(workingdir);
      this->ReadOtherFood(subcomment);
      chdir(inputdir);
      handle.Close();
      subfile.close();
      subfile.clear();

      if (!infile.eof())
        infile >> text >> ws;
    }
  }

  //Now we read the names of the fleet files
  if (!(strcasecmp(text, "[fleet]") == 0))
    handle.Unexpected("[fleet]", text);

  infile >> text >> ws;
  if (strcasecmp(text, "fleetfiles") == 0) {
    //There might not be any fleet files
    infile >> text >> ws;
    while (!((strcasecmp(text, "[likelihood]") == 0) || infile.eof())) {
      subfile.open(text);
      checkIfFailure(subfile, text);
      handle.Open(text);
      chdir(workingdir);
      this->ReadFleet(subcomment);
      chdir(inputdir);
      handle.Close();
      subfile.close();
      subfile.clear();

      if (!infile.eof())
        infile >> text >> ws;
    }
  }

  //Now we have either read the word likelihood or reached end of file.
  if (!infile.eof() && calclikelihood) {
    infile >> text >> ws;
    if (strcasecmp(text, "likelihoodfiles") == 0) {
      //There might not be any likelihood files
      infile >> text;
      while (!infile.eof()) {
        subfile.open(text);
        checkIfFailure(subfile, text);
        handle.Open(text);
        chdir(workingdir);
        this->ReadLikelihood(subcomment);
        chdir(inputdir);
        handle.Close();
        subfile.close();
        subfile.clear();

        if (!infile.eof())
          infile >> text >> ws;
      }
    }
  }

  //Dont print output line if doing a network run
  if (!netrun)
    cout << "\nFinished reading input files\n";
}
