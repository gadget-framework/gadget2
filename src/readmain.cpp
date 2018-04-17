#include "ecosystem.h"
#include "keeper.h"
#include "stockstdprinter.h"
#include "stockprinter.h"
#include "predatorprinter.h"
#include "predatorpreyprinter.h"
#include "predatoroverprinter.h"
#include "preyoverprinter.h"
#include "stockpreyfullprinter.h"
#include "stockpreyprinter.h"
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
#include "catchinkilos.h"
#include "boundlikelihood.h"
#include "surveydistribution.h"
#include "migrationproportion.h"
#include "readword.h"
#include "gadget.h"
#include "global.h"
#include "proglikelihood.h"

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
    if ((strcasecmp(text, "[fleetcomponent]") != 0) && (strcasecmp(text, "[component]") != 0))
      handle.logFileUnexpected(LOGFAIL, "[component]", text);

    infile >> text >> value;
    if (strcasecmp(text, "totalfleet") == 0)
      fleetvec.resize(new Fleet(infile, value, Area, TimeInfo, keeper, TOTALFLEET));
    else if (strcasecmp(text, "linearfleet") == 0)
      fleetvec.resize(new Fleet(infile, value, Area, TimeInfo, keeper, LINEARFLEET));
    else if (strcasecmp(text, "numberfleet") == 0)
      fleetvec.resize(new Fleet(infile, value, Area, TimeInfo, keeper, NUMBERFLEET));
    else if (strcasecmp(text, "effortfleet") == 0)
      fleetvec.resize(new Fleet(infile, value, Area, TimeInfo, keeper, EFFORTFLEET));
    else if (strcasecmp(text, "quotafleet") == 0)
      fleetvec.resize(new Fleet(infile, value, Area, TimeInfo, keeper, QUOTAFLEET));
    else if (strcasecmp(text, "mortalityfleet") == 0)
      handle.logFileMessage(LOGFAIL, "\nThe mortalityfleet fleet type is no longer supported");
    else
      handle.logFileMessage(LOGFAIL, "unrecognised fleet class", text);

    handle.logMessage(LOGMESSAGE, "Read fleet OK - created fleet", value);
  }
}

//
// A function to read tagging information
//
void Ecosystem::readTagging(CommentStream& infile) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  while (!infile.eof()) {
    infile >> text >> ws;
    if ((strcasecmp(text, "[tagcomponent]") != 0) && (strcasecmp(text, "[component]") != 0))
      handle.logFileUnexpected(LOGFAIL, "[component]", text);

    readWordAndValue(infile, "tagid", text);
    tagvec.resize(new Tags(infile, text, Area, TimeInfo, keeper, stockvec));
    handle.logMessage(LOGMESSAGE, "Read tagging experiment OK - created tag", text);
  }
}

//
// A function to read otherfood information
//
void Ecosystem::readOtherFood(CommentStream& infile) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  while (!infile.eof()) {
    infile >> text >> ws;
    if ((strcasecmp(text, "[foodcomponent]") != 0) && (strcasecmp(text, "[component]") != 0))
      handle.logFileUnexpected(LOGFAIL, "[component]", text);

    readWordAndValue(infile, "foodname", text);
    otherfoodvec.resize(new OtherFood(infile, text, Area, TimeInfo, keeper));
    handle.logMessage(LOGMESSAGE, "Read otherfood OK - created otherfood", text);
  }
}

//
// A function to read stock information
// Note: there is only ever one stock in each file
//
void Ecosystem::readStock(CommentStream& infile) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  readWordAndValue(infile, "stockname", text);
  stockvec.resize(new Stock(infile, text, Area, TimeInfo, keeper));
  handle.logMessage(LOGMESSAGE, "Read stock OK - created stock", text);
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

  if (strcasecmp(text, "[component]") != 0)
    handle.logFileUnexpected(LOGFAIL, "[component]", text);

  while (!infile.eof()) {
    readWordAndValue(infile, "type", type);

    if (strcasecmp(type, "stockstdprinter") == 0)
      printvec.resize(new StockStdPrinter(infile, TimeInfo));
    else if (strcasecmp(type, "stockfullprinter") == 0)
      printvec.resize(new StockFullPrinter(infile, TimeInfo));
    else if (strcasecmp(type, "stockprinter") == 0)
      printvec.resize(new StockPrinter(infile, TimeInfo));
    else if (strcasecmp(type, "predatorprinter") == 0)
      printvec.resize(new PredatorPrinter(infile, TimeInfo));
    else if (strcasecmp(type, "predatoroverprinter") == 0)
      printvec.resize(new PredatorOverPrinter(infile, TimeInfo));
    else if (strcasecmp(type, "preyoverprinter") == 0)
      printvec.resize(new PreyOverPrinter(infile, TimeInfo));
    else if (strcasecmp(type, "stockpreyprinter") == 0)
      printvec.resize(new StockPreyPrinter(infile, TimeInfo));
    else if (strcasecmp(type, "stockpreyfullprinter") == 0)
      printvec.resize(new StockPreyFullPrinter(infile, TimeInfo));
    else if (strcasecmp(type, "predatorpreyprinter") == 0)
      printvec.resize(new PredatorPreyPrinter(infile, TimeInfo));
    else if (strcasecmp(type, "likelihoodprinter") == 0)
      printvec.resize(new LikelihoodPrinter(infile, TimeInfo));
    else if (strcasecmp(type, "likelihoodsummaryprinter") == 0)
      printvec.resize(new SummaryPrinter(infile));

    else if (strcasecmp(type, "formatedstockprinter") == 0)
      handle.logFileMessage(LOGFAIL, "\nThe formatedstockprinter printer class is no longer supported");
    else if (strcasecmp(type, "formatedchatprinter") == 0)
      handle.logFileMessage(LOGFAIL, "\nThe formatedchatprinter printer class is no longer supported");
    else if (strcasecmp(type, "formatedpreyprinter") == 0)
      handle.logFileMessage(LOGFAIL, "\nThe formatedpreyprinter printer class is no longer supported");
    else if (strcasecmp(type, "mortprinter") == 0)
      handle.logFileMessage(LOGFAIL, "\nThe mortprinter printer class is no longer supported");
    else if (strcasecmp(type, "biomassprinter") == 0)
      handle.logFileMessage(LOGFAIL, "\nThe biomassprinter printer class is no longer supported");
    else if (strcasecmp(type, "formatedcatchprinter") == 0)
      handle.logFileMessage(LOGFAIL, "\nThe formatedcatchprinter printer class is no longer supported");
    else if (strcasecmp(type, "predpreystdlengthprinter") == 0)
      handle.logFileMessage(LOGFAIL, "\nThe predpreystdlengthprinter printer class is no longer supported");
    else if (strcasecmp(type, "predpreystdageprinter") == 0)
      handle.logFileMessage(LOGFAIL, "\nThe predpreystdageprinter printer class is no longer supported");

    else
      handle.logFileMessage(LOGFAIL, "unrecognised printer class", type);

    handle.logMessage(LOGMESSAGE, "Read printer OK - created printer class", type);
  }
}

//
// A function to read likelihood information
//
void Ecosystem::readLikelihood(CommentStream& infile) {
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

  if (strcasecmp(text, "[component]") != 0)
    handle.logFileUnexpected(LOGFAIL, "[component]", text);

  while (!infile.eof()) {
    readWordAndValue(infile, "name", name);
    readWordAndVariable(infile, "weight", weight);
    readWordAndValue(infile, "type", type);

    if (strcasecmp(type, "penalty") == 0) {
      readWordAndValue(infile, "datafile", datafilename);
      datafile.open(datafilename, ios::in);
      handle.checkIfFailure(datafile, datafilename);
      handle.Open(datafilename);
      likevec.resize(new BoundLikelihood(subdata, Area, TimeInfo, keeper, weight, name));
      handle.Close();
      datafile.close();
      datafile.clear();

      //prepare for next likelihood component
      infile >> ws;
      if (!infile.eof()) {
        infile >> text >> ws;
        if (strcasecmp(text, "[component]") != 0)
          handle.logFileUnexpected(LOGFAIL, "[component]", text);
      }

    } else if (strcasecmp(type, "understocking") == 0) {
      likevec.resize(new UnderStocking(infile, Area, TimeInfo, weight, name));

    } else if (strcasecmp(type, "catchstatistics") == 0) {
      likevec.resize(new CatchStatistics(infile, Area, TimeInfo, weight, name));

    } else if (strcasecmp(type, "catchdistribution") == 0) {
      likevec.resize(new CatchDistribution(infile, Area, TimeInfo, keeper, weight, name));

    } else if (strcasecmp(type, "stockdistribution") == 0) {
      likevec.resize(new StockDistribution(infile, Area, TimeInfo, weight, name));

    } else if (strcasecmp(type, "surveyindices") == 0) {
      likevec.resize(new SurveyIndices(infile, Area, TimeInfo, weight, name));

    } else if (strcasecmp(type, "surveydistribution") == 0) {
      likevec.resize(new SurveyDistribution(infile, Area, TimeInfo, keeper, weight, name));

    } else if (strcasecmp(type, "proglikelihood") == 0) {
      likevec.resize(new ProgLikelihood(infile, Area, TimeInfo, keeper, weight, name));


    } else if (strcasecmp(type, "stomachcontent") == 0) {
      likevec.resize(new StomachContent(infile, Area, TimeInfo, keeper, weight, name));

    } else if (strcasecmp(type, "recaptures") == 0) {
      likevec.resize(new Recaptures(infile, Area, TimeInfo, weight, tagvec, name));

    } else if ((strcasecmp(type, "catchintons") == 0) || (strcasecmp(type, "catchinkilos") == 0)) {
      likevec.resize(new CatchInKilos(infile, Area, TimeInfo, weight, name));

    } else if (strcasecmp(type, "migrationpenalty") == 0) {
      likevec.resize(new MigrationPenalty(infile, weight, name));

    } else if (strcasecmp(type, "recstatistics") == 0) {
      likevec.resize(new RecStatistics(infile, Area, TimeInfo, weight, tagvec, name));

    } else if (strcasecmp(type, "migrationproportion") == 0) {
      likevec.resize(new MigrationProportion(infile, Area, TimeInfo, weight, name));

    } else if (strcasecmp(type, "predatorindices") == 0) {
      handle.logFileMessage(LOGFAIL, "\nThe predatorindices likelihood component is no longer supported\nUse the sibyfleet surveyindices likelihood component instead\nThis is done by setting the sitype to 'fleets' in the likelihood file");

    } else if (strcasecmp(type, "logcatch") == 0) {
      handle.logFileMessage(LOGFAIL, "\nThe logcatch likelihood component is no longer supported\nUse the log function from the catchdistribution likelihood component instead");

    } else if (strcasecmp(type, "logsurveyindices") == 0) {
      handle.logFileMessage(LOGFAIL, "\nThe logsurveyindices likelihood component is no longer supported\nUse the log function from the surveyindices likelihood component instead");

    } else {
      handle.logFileMessage(LOGFAIL, "unrecognised likelihood class", type);
    }

    handle.logMessage(LOGMESSAGE, "Read likelihood OK - created likelihood component", name);
  }
}

//
// A function to read optimisation information
//
void Ecosystem::readOptimisation(CommentStream& infile) {
  char* text = new char[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  handle.logMessage(LOGMESSAGE, "Reading optimisation information");

  infile >> ws;
  if (!infile.eof())
    infile >> text;

  int count = 0;
  while (!infile.eof()) {
    infile >> ws;  //trim whitespace from infile
    if (strcasecmp(text, "seed") == 0) {
      int seed = 0;
      infile >> seed >> ws;
      handle.logMessage(LOGMESSAGE, "Initialising random number generator with", seed);
      srand(seed);
      if (!infile.eof())
        infile >> text;
    }

    if (strcasecmp(text, "[hooke]") == 0)
      optvec.resize(new OptInfoHooke());
    else if (strcasecmp(text, "[simann]") == 0)
      optvec.resize(new OptInfoSimann());
    else if (strcasecmp(text, "[bfgs]") == 0)
      optvec.resize(new OptInfoBFGS());
    else
      handle.logFileUnexpected(LOGFAIL, "[hooke], [simann], or [bfgs]", text);

    if (!infile.eof()) {
      infile >> text;
      optvec[count]->read(infile, text);
    } else
      handle.logMessage(LOGINFO, "Warning - no parameters specified for optimisation algorithm");
    count++;
  }

  delete[] text;
  if (count == 0) {
    handle.logFileMessage(LOGWARN, "no optimisation algorithms found, using default values");
    optvec.resize(new OptInfoHooke());
  }
}

//
// The main reading function
//
void Ecosystem::readMain(CommentStream& infile, const MainInfo& main) {

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
  TimeInfo = new TimeClass(subcomment, main.getMaxRatio());
  handle.Close();
  subfile.close();
  subfile.clear();

  //second, read in the area information
  readWordAndValue(infile, "areafile", filename);
  subfile.open(filename, ios::in);
  handle.checkIfFailure(subfile, filename);
  handle.Open(filename);
  Area = new AreaClass(subcomment, keeper, TimeInfo);
  handle.Close();
  subfile.close();
  subfile.clear();

  //next, read in the printing information
  infile >> text >> ws;
  if ((strcasecmp(text, "printfile") != 0) && (strcasecmp(text, "printfiles") != 0))
    handle.logFileUnexpected(LOGFAIL, "printfiles", text);

  //Now we have found the string "printfiles" we can create printer classes
  infile >> text >> ws;
  while ((strcasecmp(text, "[stock]") != 0) && !infile.eof()) {
    //Do not try to read printfile if we dont need it
    if (main.runPrint()) {
      subfile.open(text, ios::in);
      handle.checkIfFailure(subfile, text);
      handle.Open(text);
      this->readPrinters(subcomment);
      handle.Close();
      subfile.close();
      subfile.clear();
    }
    infile >> text >> ws;
  }

  //then read in the stock information
  if (strcasecmp(text, "[stock]") != 0)
    handle.logFileUnexpected(LOGFAIL, "[stock]", text);
  infile >> text >> ws;

  if (strcasecmp(text, "mortalitymodel") == 0)
    handle.logFileMessage(LOGFAIL, "\nFleksibest-style mortality models are no longer supported\nGadget version 2.0.07 was the last version to allow this functionality");

  if (strcasecmp(text, "stockfiles") != 0)
    handle.logFileUnexpected(LOGFAIL, "stockfiles", text);

  //Now we have found the string "stockfiles" we can create stock
  infile >> text >> ws;
  while ((strcasecmp(text, "[tagging]") != 0) && !infile.eof()) {
    subfile.open(text, ios::in);
    handle.checkIfFailure(subfile, text);
    handle.Open(text);
    this->readStock(subcomment);
    handle.Close();
    subfile.close();
    subfile.clear();
    infile >> text >> ws;
  }

  //Now we read the names of the tagging files
  if (strcasecmp(text, "[tagging]") != 0)
    handle.logFileUnexpected(LOGFAIL, "[tagging]", text);

  infile >> text >> ws;
  if (strcasecmp(text, "tagfiles") == 0) {
    //There might not be any tagging files
    infile >> text >> ws;
    while ((strcasecmp(text, "[otherfood]") != 0) && !infile.eof()) {
      subfile.open(text, ios::in);
      handle.checkIfFailure(subfile, text);
      handle.Open(text);
      this->readTagging(subcomment);
      handle.Close();
      subfile.close();
      subfile.clear();
      infile >> text >> ws;
    }
  }

  //Now we read the names of the otherfood files
  if (strcasecmp(text, "[otherfood]") != 0)
    handle.logFileUnexpected(LOGFAIL, "[otherfood]", text);

  infile >> text >> ws;
  if (strcasecmp(text, "otherfoodfiles") == 0) {
    //There might not be any otherfood files
    infile >> text >> ws;
    while ((strcasecmp(text, "[fleet]") != 0) && !infile.eof()) {
      subfile.open(text, ios::in);
      handle.checkIfFailure(subfile, text);
      handle.Open(text);
      this->readOtherFood(subcomment);
      handle.Close();
      subfile.close();
      subfile.clear();
      infile >> text >> ws;
    }
  }

  //Now we read the names of the fleet files
  if (strcasecmp(text, "[fleet]") != 0)
    handle.logFileUnexpected(LOGFAIL, "[fleet]", text);

  infile >> text >> ws;
  if (strcasecmp(text, "fleetfiles") == 0) {
    //There might not be any fleet files
    infile >> text >> ws;
    while ((strcasecmp(text, "[likelihood]") != 0) && !infile.eof()) {
      subfile.open(text, ios::in);
      handle.checkIfFailure(subfile, text);
      handle.Open(text);
      this->readFleet(subcomment);
      handle.Close();
      subfile.close();
      subfile.clear();
      infile >> text >> ws;
    }
  }

  if (strcasecmp(text, "[likelihood]") != 0)
    handle.logFileUnexpected(LOGFAIL, "[likelihood]", text);

  //Now we have either read the word likelihood or reached the end of file
  if (!infile.eof()) {
    infile >> text >> ws;
    if (strcasecmp(text, "likelihoodfiles") == 0) {
      //There might not be any likelihood files
      infile >> text;
      while (!infile.eof()) {
        subfile.open(text, ios::in);
        handle.checkIfFailure(subfile, text);
        handle.Open(text);
        this->readLikelihood(subcomment);
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
