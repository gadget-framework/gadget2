#include "naturalm.h"
#include "readfunc.h"
#include "areatime.h"
#include "readword.h"
#include "keeper.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

NaturalMortality::NaturalMortality(CommentStream& infile, int minage, int numage,
  const char* givenname, const IntVector& Areas, const TimeClass* const TimeInfo,
  Keeper* const keeper) : HasName(givenname), LivesOnAreas(Areas) {

  minStockAge = minage;
  readoption = 0;
  proportion.AddRows(areas.Size(), numage, 0.0);
  keeper->addString("naturalmortality");

  infile >> ws;
  char c = infile.peek();
  if ((c == 'm') || (c == 'M')) {
    //JMB experimental mortality function
    readoption = 1;
    char text[MaxStrLength];
    strncpy(text, "", MaxStrLength);
    readWordAndValue(infile, "mortalityfunction", text);
    if (strcasecmp(text, "constant") == 0)
      fnMortality = new ConstSelectFunc();
    else if (strcasecmp(text, "straightline") == 0)
      fnMortality = new StraightSelectFunc();
    else if (strcasecmp(text, "exponential") == 0)
      fnMortality = new ExpSelectFunc();
    else
      handle.logFileMessage(LOGFAIL, "unrecognised mortality function", text);
    fnMortality->readConstants(infile, TimeInfo, keeper);

  } else {
    mortality.setsize(numage);
    mortality.read(infile, TimeInfo, keeper);
  }

  keeper->clearLast();
}

NaturalMortality::~NaturalMortality() {
  if (readoption == 1)
    delete fnMortality;
}

void NaturalMortality::Reset(const TimeClass* const TimeInfo) {
  if (readoption == 0) {
    mortality.Update(TimeInfo);
    if (mortality.didChange(TimeInfo) || TimeInfo->didStepSizeChange()) {
      int i, j;
      for (i = 0; i < proportion.Nrow(); i++) {
        for (j = 0; j < proportion.Ncol(i); j++) {
          if (mortality[j] > verysmall)
            proportion[i][j] = exp(-mortality[j] * TimeInfo->getTimeStepSize());
          else
            proportion[i][j] = 1.0; //use mortality rate 0
        }
      }

      if (handle.getLogLevel() >= LOGMESSAGE)
        handle.logMessage(LOGMESSAGE, "Reset natural mortality data for stock", this->getName());
    }

  } else if (readoption == 1) {
    //JMB experimental mortality function
    fnMortality->updateConstants(TimeInfo);
    if (fnMortality->didChange(TimeInfo) || TimeInfo->didStepSizeChange()) {
      int i, j;
      double m;
      for (i = 0; i < proportion.Nrow(); i++) {
        for (j = 0; j < proportion.Ncol(i); j++) {
          m = fnMortality->calculate(double(minStockAge + j));
          if (m > verysmall)
            proportion[i][j] = exp(-m * TimeInfo->getTimeStepSize());
          else
            proportion[i][j] = 1.0; //use mortality rate 0
        }
      }

      if (handle.getLogLevel() >= LOGMESSAGE)
        handle.logMessage(LOGMESSAGE, "Reset natural mortality data for stock", this->getName());
    }

  } else
    handle.logMessage(LOGFAIL, "Error in natural mortality - unrecognised mortality function", readoption);
}

void NaturalMortality::Print(ofstream& outfile) {
  int i;
  outfile << "Natural mortality\n\t";
  if (readoption == 0) {
    for (i = 0; i < mortality.Size(); i++)
      outfile << mortality[i] << sep;
    outfile << endl;

  } else if (readoption == 1) {
    double m;
    for (i = 0; i < proportion.Ncol(); i++) {
      m = fnMortality->calculate(double(minStockAge + i));
      outfile << m << sep;
    }
    outfile << endl;

  } else
    handle.logMessage(LOGFAIL, "Error in natural mortality - unrecognised mortality function", readoption);
}
