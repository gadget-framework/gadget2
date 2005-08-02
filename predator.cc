#include "keeper.h"
#include "prey.h"
#include "suits.h"
#include "predator.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "gadget.h"

extern ErrorHandler handle;

Predator::Predator(const char* givenname, const IntVector& Areas)
  : HasName(givenname), LivesOnAreas(Areas), suitable(0) {
}

Predator::~Predator() {
  if (suitable != 0)
    delete suitable;
}

void Predator::setPrey(PreyPtrVector& preyvec, Keeper* const keeper) {
  int i, j;
  int found = 0;

  if (suitable == 0)
    handle.logMessage(LOGFAIL, "Error in predator - found no suitability values");

  preys.resize(suitable->numPreys(), 0);
  for (i = 0; i < preyvec.Size(); i++) {
    found = 0;
    for (j = 0; j < suitable->numPreys(); j++) {
      if (strcasecmp(suitable->getPreyName(j), preyvec[i]->getName()) == 0) {
        if (found == 0) {
          preys[j] = preyvec[i];
          found++;
        } else
          handle.logMessage(LOGFAIL, "Error in predator - repeated suitability values for prey", preyvec[i]->getName());

      }
    }
  }

  found = 0;
  for (i = 0; i < preys.Size(); i++) {
    //If we find a prey that we have read the suitability for, but not
    //received a pointer to, we issue a warning and delete it
    if (preys[i] == 0) {
      found++;
      handle.logMessage(LOGWARN, "Warning in predator - failed to match prey", this->getPreyName(i));
      preys.Delete(i);
      suitable->deletePrey(i, keeper);
      if (found != preys.Size())
        i--;
    }
  }
  if (this->numPreys() == 0)
    handle.logMessage(LOGFAIL, "Error in predator - no preys for predator", this->getName());
}

int Predator::doesEat(const char* preyname) const {
  int i;
  for (i = 0; i < suitable->numPreys(); i++)
    if (strcasecmp(suitable->getPreyName(i), preyname) == 0)
      return 1;
  return 0;
}

void Predator::Print(ofstream& outfile) const {
  int i;
  outfile << "\tName" << sep << this->getName()
    << "\n\tNames of preys:";
  for (i = 0; i < suitable->numPreys(); i++)
    outfile << sep << suitable->getPreyName(i);
  outfile << endl;
  for (i = 0; i < suitable->numPreys(); i++) {
    outfile << "\tSuitability for " << suitable->getPreyName(i) << endl;
    suitable->getSuitability(i).Print(outfile);
  }
}

void Predator::Reset(const TimeClass* const TimeInfo) {
  suitable->Reset(this, TimeInfo);
}

void Predator::readSuitability(CommentStream& infile,
  const char* strFinal, const TimeClass* const TimeInfo, Keeper* const keeper) {

  int i, j;
  char preyname[MaxStrLength];
  char text[MaxStrLength];
  strncpy(preyname, "", MaxStrLength);
  strncpy(text, "", MaxStrLength);
  suitable = new Suits();
  SuitFuncPtrVector suitf;
  keeper->addString("suitabilityfor");

  infile >> preyname >> ws;
  while (!(strcasecmp(preyname, strFinal) == 0) && (!infile.eof())) {
    keeper->addString(preyname);

    infile >> text >> ws;
    if (strcasecmp(text, "function") == 0) {
      infile >> text >> ws;
      if (readSuitFunction(suitf, infile, text, TimeInfo, keeper) == 1)
        suitable->addPrey(preyname, suitf[suitf.Size() - 1]);
      else
        handle.logFileMessage(LOGFAIL, "unrecognised suitability function", text);

    } else if (strcasecmp(text, "suitfile") == 0) {
      handle.logFileMessage(LOGFAIL, "\nReading suitability values directly from file is no longer supported\nGadget version 2.0.07 was the last version to allow this functionality");

    } else
      handle.logFileMessage(LOGFAIL, "unrecognised predation format", text);

    infile >> preyname >> ws;
    keeper->clearLast();
  }

  keeper->clearLast();
  handle.logMessage(LOGMESSAGE, "Read predation data - number of preys", suitable->numPreys());
}
