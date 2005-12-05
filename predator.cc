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
    handle.logMessage(LOGFAIL, "Error in predator - found no suitability values for predator", this->getName());

  preys.resizeBlank(suitable->numPreys());
  IntVector check(suitable->numPreys(), 0);
  for (i = 0; i < preyvec.Size(); i++) {
    found = 0;
    for (j = 0; j < suitable->numPreys(); j++) {
      if (strcasecmp(suitable->getPreyName(j), preyvec[i]->getName()) == 0) {
        if (found == 0) {
          preys[j] = preyvec[i];
          check[j]++;
          found++;
        } else
          handle.logMessage(LOGFAIL, "Error in predator - repeated suitability values for prey", preyvec[i]->getName());

      }
    }
  }

  found = 0;
  for (i = 0; i < check.Size(); i++) {
    //If we find a prey that we have read the suitability for, but not
    //received a pointer to, we issue a warning and delete it
    if (check[i] == 0) {
      found++;
      handle.logMessage(LOGWARN, "Warning in predator - failed to match prey", this->getPreyName(i));
      preys.Delete(i);
      if (this->getType() == STOCKPREDATOR)
        preference.Delete(i, keeper);
      suitable->deletePrey(i, keeper);
      if (found != preys.Size())
        i--;
    }
  }

  if (this->numPreys() == 0)
    handle.logMessage(LOGFAIL, "Error in predator - found no preys for predator", this->getName());
  // check that the preference vector is still the right size
  if ((this->getType() == STOCKPREDATOR) && (this->numPreys() != preference.Size()))
    handle.logMessage(LOGFAIL, "Error in predator - invalid prey preference data");
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
  outfile << "\tName" << sep << this->getName() << "\n\tNames of preys:";
  for (i = 0; i < suitable->numPreys(); i++)
    outfile << sep << suitable->getPreyName(i);
  outfile << endl;
  for (i = 0; i < suitable->numPreys(); i++) {
    if (preference.Size() != 0)
      outfile << "\tPreference for prey " << suitable->getPreyName(i) << sep << preference[i] << endl;
    outfile << "\tSuitability for prey " << suitable->getPreyName(i) << endl;
    suitable->getSuitability(i).Print(outfile);
  }
}

void Predator::readSuitability(CommentStream& infile,
  const TimeClass* const TimeInfo, Keeper* const keeper) {

  char preyname[MaxStrLength];
  char text[MaxStrLength];
  strncpy(preyname, "", MaxStrLength);
  strncpy(text, "", MaxStrLength);
  suitable = new Suits();
  SuitFuncPtrVector suitf;
  keeper->addString("suitabilityfor");

  //JMB - this is the next word to look for after the list of suitability values
  char strcheck[15];
  strncpy(strcheck, "", 15);
  if (this->getType() == STOCKPREDATOR)
    strcpy(strcheck, "preference");
  else
    strcpy(strcheck, "amount");

  infile >> preyname >> ws;
  while (!(strcasecmp(preyname, strcheck) == 0) && (!infile.eof())) {
    keeper->addString(preyname);

    infile >> text >> ws;
    if (strcasecmp(text, "function") == 0) {
      infile >> text >> ws;
      if (suitf.readSuitFunction(infile, text, TimeInfo, keeper) == 1)
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

  //resize the prey preference vector - only used for stockpredators
  if (this->getType() == STOCKPREDATOR)
    preference.resize(suitable->numPreys(), keeper);
  keeper->clearLast();
  handle.logMessage(LOGMESSAGE, "Read predation data - number of preys", suitable->numPreys());
}
