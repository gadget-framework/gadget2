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
#include "likelihoodprinter.h"
#include "summaryprinter.h"
#include "surveyindices.h"
#include "understocking.h"
#include "catchdistribution.h"
#include "stockdistribution.h"
#include "catchstatistics.h"
#include "recstatistics.h"
#include "stomachcontent.h"
#include "recapture.h"
#include "migrationpenalty.h"
#include "catchintons.h"
#include "boundlikelihood.h"
#include "surveydistribution.h"
#include "readword.h"
#include "gadget.h"

extern ErrorHandler handle;

//
// A function to read fleet information
//
void Ecosystem::readFleet(CommentStream& infile) {
  char text[MaxStrLength];
  char value[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  strncpy(value, "", MaxStrLength);

  while (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[fleetcomponent]") == 0))
      handle.Unexpected("[fleetcomponent]", text);

    infile >> text >> value;
    fleetvec.resize(1);
    if (strcasecmp(text, "totalfleet") == 0)
      fleetvec[fleetvec.Size() - 1] = new Fleet(infile, value, Area, TimeInfo, keeper, TOTALFLEET);
    else if (strcasecmp(text, "linearfleet") == 0)
      fleetvec[fleetvec.Size() - 1] = new Fleet(infile, value, Area, TimeInfo, keeper, LINEARFLEET);
    else if (strcasecmp(text, "mortalityfleet") == 0)
      handle.Message("The mortalityfleet fleet type is no longer supported");
    else
      handle.Message("Error in fleet file - unrecognised fleet", text);

    handle.logMessage("Read fleet OK - created fleet", value);
  }
}

//
// A function to read tagging information
//
void Ecosystem::readTagging(CommentStream& infile) {
  char text[MaxStrLength];
  char value[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  strncpy(value, "", MaxStrLength);

  while (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[tagcomponent]") == 0))
      handle.Unexpected("[tagcomponent]", text);

    infile >> text >> value;
    tagvec.resize(1);
    if (strcasecmp(text, "tagid") == 0)
      tagvec[tagvec.Size() - 1] = new Tags(infile, value, Area, TimeInfo, keeper, stockvec);
    else
      handle.Unexpected("tagid", text);

    handle.logMessage("Read tagging experiment OK - created tag", value);
  }
}

//
// A function to read otherfood information
//
void Ecosystem::readOtherFood(CommentStream& infile) {
  char text[MaxStrLength];
  char value[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  strncpy(value, "", MaxStrLength);

  while (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[foodcomponent]") == 0))
      handle.Unexpected("[foodcomponent]", text);

    infile >> text >> value;
    otherfoodvec.resize(1);
    if (strcasecmp(text, "foodname") == 0)
      otherfoodvec[otherfoodvec.Size() - 1] = new OtherFood(infile, value, Area, TimeInfo, keeper);
    else
      handle.Unexpected("foodname", text);

    handle.logMessage("Read otherfood OK - created otherfood", value);
  }
}

//
// A function to read stock information
// Note: there is only ever one stock in each file
//
void Ecosystem::readStock(CommentStream& infile) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  if (!infile.eof())
    infile >> text >> ws;

  if (!(strcasecmp(text, "stockname") == 0))
    handle.Unexpected("stockname", text);

  infile >> text;
  stockvec.resize(1);
  stockvec[stockvec.Size() - 1] = new Stock(infile, text, Area, TimeInfo, keeper);

  handle.logMessage("Read stock OK - created stock", text);
}

//
// A function to read information on printing
//
void Ecosystem::readPrinters(CommentStream& infile) {
  char text[MaxStrLength];
  char type[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  strncpy(type, "", MaxStrLength);

  if (!infile.eof())
    infile >> text >> ws;

  if (!(strcasecmp(text, "[component]") == 0))
    handle.Unexpected("[component]", text);

  while (!infile.eof()) {
    readWordAndValue(infile, "type", type);

    if (strcasecmp(type, "stockstdprinter") == 0)
      printvec.resize(1, new StockStdPrinter(infile, TimeInfo));
    else if (strcasecmp(type, "stockfullprinter") == 0)
      printvec.resize(1, new StockFullPrinter(infile, TimeInfo));
    else if (strcasecmp(type, "stockprinter") == 0)
      printvec.resize(1, new StockPrinter(infile, Area, TimeInfo));
    else if (strcasecmp(type, "predatorprinter") == 0)
      printvec.resize(1, new PredatorPrinter(infile, Area, TimeInfo));
    else if (strcasecmp(type, "predatoroverprinter") == 0)
      printvec.resize(1, new PredatorOverPrinter(infile, Area, TimeInfo));
    else if (strcasecmp(type, "preyoverprinter") == 0)
      printvec.resize(1, new PreyOverPrinter(infile, Area, TimeInfo));
    else if (strcasecmp(type, "stockpreyfullprinter") == 0)
      printvec.resize(1, new StockPreyFullPrinter(infile, TimeInfo));
    else if (strcasecmp(type, "predpreystdlengthprinter") == 0)
      printvec.resize(1, new PredPreyStdLengthPrinter(infile, TimeInfo));
    else if (strcasecmp(type, "predpreystdageprinter") == 0)
      printvec.resize(1, new PredPreyStdAgePrinter(infile, TimeInfo));
    else if (strcasecmp(type, "likelihoodprinter") == 0)
      printvec.resize(1, new LikelihoodPrinter(infile, TimeInfo));
    else if (strcasecmp(type, "likelihoodsummaryprinter") == 0)
      printvec.resize(1, new SummaryPrinter(infile));

    else if (strcasecmp(type, "formatedstockprinter") == 0)
      handle.Message("The formatedstockprinter printer class is no longer supported");
    else if (strcasecmp(type, "formatedchatprinter") == 0)
      handle.Message("The formatedchatprinter printer class is no longer supported");
    else if (strcasecmp(type, "formatedpreyprinter") == 0)
      handle.Message("The formatedpreyprinter printer class is no longer supported");
    else if (strcasecmp(type, "mortprinter") == 0)
      handle.Message("The mortprinter printer class is no longer supported");
    else if (strcasecmp(type, "biomassprinter") == 0)
      handle.Message("The biomassprinter printer class is no longer supported");
    else if (strcasecmp(type, "formatedcatchprinter") == 0)
      handle.Message("The formatedcatchprinter printer class is no longer supported");

    else
      handle.Message("Error in printer file - unrecognised printer", type);

    handle.logMessage("Read printer OK - created printer class", type);
  }
}

//
// A function to read likelihood information
//
void Ecosystem::readLikelihood(CommentStream& infile) {
  int i = 0;
  double weight = 0.0;
  char text[MaxStrLength];
  char type[MaxStrLength];
  char name[MaxStrLength];
  char datafilename[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  strncpy(type, "", MaxStrLength);
  strncpy(name, "", MaxStrLength);
  strncpy(datafilename, "", MaxStrLength);

  ifstream datafile;
  CommentStream subdata(datafile);

  if (!infile.eof())
    infile >> text >> ws;

  if (!(strcasecmp(text, "[component]") == 0))
    handle.Unexpected("[component]", text);

  while (!infile.eof()) {
    readWordAndValue(infile, "name", name);
    readWordAndVariable(infile, "weight", weight);
    readWordAndValue(infile, "type", type);

    likevec.resize(1);
    i = likevec.Size() - 1;

    if (strcasecmp(type, "penalty") == 0) {
      readWordAndValue(infile, "datafile", datafilename);
      datafile.open(datafilename, ios::in);
      handle.checkIfFailure(datafile, datafilename);
      handle.Open(datafilename);
      likevec[i] = new BoundLikelihood(subdata, Area, TimeInfo, keeper, weight, name);
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
      likevec[i] = new UnderStocking(infile, Area, TimeInfo, weight, name);

    } else if (strcasecmp(type, "catchstatistics") == 0) {
      likevec[i] = new CatchStatistics(infile, Area, TimeInfo, weight, name);

    } else if (strcasecmp(type, "catchdistribution") == 0) {
      likevec[i] = new CatchDistribution(infile, Area, TimeInfo, keeper, weight, name);

    } else if (strcasecmp(type, "stockdistribution") == 0) {
      likevec[i] = new StockDistribution(infile, Area, TimeInfo, weight, name);

    } else if (strcasecmp(type, "surveyindices") == 0) {
      likevec[i] = new SurveyIndices(infile, Area, TimeInfo, weight, name);

    } else if (strcasecmp(type, "surveydistribution") == 0) {
      likevec[i] = new SurveyDistribution(infile, Area, TimeInfo, keeper, weight, name);

    } else if (strcasecmp(type, "stomachcontent") == 0) {
      likevec[i] = new StomachContent(infile, Area, TimeInfo, keeper, weight, name);

    } else if (strcasecmp(type, "recaptures") == 0) {
      likevec[i] = new Recaptures(infile, Area, TimeInfo, weight, tagvec, name);

    } else if (strcasecmp(type, "catchintons") == 0) {
      likevec[i] = new CatchInTons(infile, Area, TimeInfo, weight, name);

    } else if (strcasecmp(type, "migrationpenalty") == 0) {
      likevec[i] = new MigrationPenalty(infile, weight, name);

    } else if (strcasecmp(type, "recstatistics") == 0) {
      likevec[i] = new RecStatistics(infile, Area, TimeInfo, weight, tagvec, name);

    } else if (strcasecmp(type, "predatorindices") == 0) {
      handle.Message("The predatorindices likelihood component is no longer supported\nUse the sibyfleet surveyindices likelihood component instead\nThis is done by setting the sitype to 'fleets' in the likelihood file");

    } else if (strcasecmp(type, "logcatch") == 0) {
      handle.Message("The logcatch likelihood component is no longer supported\nUse the log function from the catchdistribution likelihood component instead");

    } else if (strcasecmp(type, "logsurveyindices") == 0) {
      handle.Message("The logsurveyindices likelihood component is no longer supported\nUse the log function from the surveyindices likelihood component instead");

    } else {
      handle.Message("Error in likelihood file - unrecognised likelihood", type);
    }

    handle.logMessage("Read likelihood OK - created likelihood component", name);
  }
}

//
// The main reading function
//
void Ecosystem::readMain(CommentStream& infile, int optimise, int netrun,
  int calclikelihood, const char* const inputdir, const char* const workingdir) {

  int i, j;
  char text[MaxStrLength];
  char filename[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  strncpy(filename, "", MaxStrLength);

  ifstream subfile;
  CommentStream subcomment(subfile);

  //first, read in the time information
  readWordAndValue(infile, "timefile", filename);
  subfile.open(filename, ios::in);
  handle.checkIfFailure(subfile, filename);
  handle.Open(filename);
  TimeInfo = new TimeClass(subcomment);
  handle.Close();
  subfile.close();
  subfile.clear();

  //second, read in the area information
  readWordAndValue(infile, "areafile", filename);
  subfile.open(filename, ios::in);
  handle.checkIfFailure(subfile, filename);
  handle.Open(filename);
  Area = new AreaClass(subcomment, TimeInfo);
  handle.Close();
  subfile.close();
  subfile.clear();

  //next, read in the printing information
  infile >> text >> ws;
  if (!((strcasecmp(text, "printfile") == 0) || (strcasecmp(text, "printfiles") == 0)))
    handle.Unexpected("printfiles", text);

  //Now we have found the string "printfiles" we can create printer clases
  infile >> text >> ws;
  while (!((strcasecmp(text, "[stock]") == 0) || infile.eof())) {
    //Do not read printfile if we are doing a net run, or if we are
    //optimising without the getForcePrint option set. 07.04.00 AJ & mnaa.
    if (!netrun  && (printinfo.getForcePrint() || !optimise)) {
      subfile.open(text, ios::in);
      handle.checkIfFailure(subfile, text);
      handle.Open(text);
      chdir(workingdir);
      this->readPrinters(subcomment);
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

  if (strcasecmp(text, "mortalitymodel") == 0)
    handle.Message("Fleksibest-style mortality models are no longer supported\nGadget version 2.0.07 was the last version to allow this functionality");

  if (!(strcasecmp(text, "stockfiles") == 0))
    handle.Unexpected("stockfiles", text);

  //Now we have found the string "stockfiles" we can create stock
  infile >> text >> ws;
  while (!((strcasecmp(text, "[tagging]") == 0) || infile.eof())) {
    subfile.open(text, ios::in);
    handle.checkIfFailure(subfile, text);
    handle.Open(text);
    chdir(workingdir);
    this->readStock(subcomment);
    chdir(inputdir);
    handle.Close();
    subfile.close();
    subfile.clear();
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
      subfile.open(text, ios::in);
      handle.checkIfFailure(subfile, text);
      handle.Open(text);
      chdir(workingdir);
      this->readTagging(subcomment);
      chdir(inputdir);
      handle.Close();
      subfile.close();
      subfile.clear();
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
      subfile.open(text, ios::in);
      handle.checkIfFailure(subfile, text);
      handle.Open(text);
      chdir(workingdir);
      this->readOtherFood(subcomment);
      chdir(inputdir);
      handle.Close();
      subfile.close();
      subfile.clear();
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
      subfile.open(text, ios::in);
      handle.checkIfFailure(subfile, text);
      handle.Open(text);
      chdir(workingdir);
      this->readFleet(subcomment);
      chdir(inputdir);
      handle.Close();
      subfile.close();
      subfile.clear();
      infile >> text >> ws;
    }
  }

  if (!(strcasecmp(text, "[likelihood]") == 0))
    handle.Unexpected("[likelihood]", text);

  //Now we have either read the word likelihood or reached end of file.
  if (!infile.eof() && calclikelihood) {
    infile >> text >> ws;
    if (strcasecmp(text, "likelihoodfiles") == 0) {
      //There might not be any likelihood files
      infile >> text;
      while (!infile.eof()) {
        subfile.open(text, ios::in);
        handle.checkIfFailure(subfile, text);
        handle.Open(text);
        chdir(workingdir);
        this->readLikelihood(subcomment);
        chdir(inputdir);
        handle.Close();
        subfile.close();
        subfile.clear();

        infile >> ws;
        if (!infile.eof())
          infile >> text;
      }
    }
  }
}
