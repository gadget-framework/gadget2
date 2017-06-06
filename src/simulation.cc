#include "ecosystem.h"
#include "errorhandler.h"
#include "gadget.h"
#include "interruptinterface.h"
#include "global.h"

void Ecosystem::updatePredationOneArea(int area) {
  int i;
  // calculate the number of preys and predators in area.
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->isInArea(area))
      basevec[i]->calcNumbers(area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->isInArea(area))
      basevec[i]->calcEat(area, Area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->isInArea(area))
      basevec[i]->checkEat(area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->isInArea(area))
      basevec[i]->adjustEat(area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->isInArea(area))
      basevec[i]->reducePop(area, TimeInfo);
}

void Ecosystem::updatePopulationOneArea(int area) {
  int i;
  // under updates are movements to mature stock, renewal, spawning and straying.
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->isInArea(area))
      basevec[i]->Grow(area, Area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->isInArea(area))
      basevec[i]->updatePopulationPart1(area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->isInArea(area))
      basevec[i]->updatePopulationPart2(area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->isInArea(area))
      basevec[i]->updatePopulationPart3(area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->isInArea(area))
      basevec[i]->updatePopulationPart4(area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->isInArea(area))
      basevec[i]->updatePopulationPart5(area, TimeInfo);
}

void Ecosystem::updateAgesOneArea(int area) {
  int i;
  // age related update and movements between stocks.
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->isInArea(area))
      basevec[i]->updateAgePart1(area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->isInArea(area))
      basevec[i]->updateAgePart2(area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->isInArea(area))
      basevec[i]->updateAgePart3(area, TimeInfo);
}

void Ecosystem::Simulate(int print) {
  int i, j, k;

  handle.logMessage(LOGMESSAGE, "");  //write blank line to log file
  for (j = 0; j < likevec.Size(); j++)
    likevec[j]->Reset(keeper);
  for (j = 0; j < likevec.Size(); j++)
    likevec[j]->addLikelihoodKeeper(TimeInfo, keeper);

  for (j = 0; j < tagvec.Size(); j++)
    tagvec[j]->Reset();

  TimeInfo->Reset();
  for (i = 0; i < TimeInfo->numTotalSteps(); i++) {
    for (j = 0; j < basevec.Size(); j++)
      basevec[j]->Reset(TimeInfo);

    // add in any new tagging experiments
    tagvec.updateTags(TimeInfo);
    for (j = 0; j < likevec.Size(); j++)  //only proglikelihood
      likevec[j]->Reset(TimeInfo);

    if (print)
      for (j = 0; j < printvec.Size(); j++)
        printvec[j]->Print(TimeInfo, 1);  //start of timestep, so printtime is 1

    // migration between areas
    if (Area->numAreas() > 1)    //no migration if there is only one area
      for (j = 0; j < basevec.Size(); j++)
        basevec[j]->Migrate(TimeInfo);

    // predation can be split into substeps
    for (k = 0; k < TimeInfo->numSubSteps(); k++) {
      for (j = 0; j < Area->numAreas(); j++)
        this->updatePredationOneArea(j);
      TimeInfo->IncrementSubstep();
    }

    // maturation, spawning, recruits etc
    for (j = 0; j < Area->numAreas(); j++)
      this->updatePopulationOneArea(j);

    for (j = 0; j < likevec.Size(); j++)
      likevec[j]->addLikelihood(TimeInfo);

    if (print)
      for (j = 0; j < printvec.Size(); j++)
        printvec[j]->Print(TimeInfo, 0);  //end of timestep, so printtime is 0

    for (j = 0; j < Area->numAreas(); j++)
      this->updateAgesOneArea(j);

#ifdef INTERRUPT_HANDLER
    if (interrupted) {
      InterruptInterface ui;
      if (!ui.menu()) {
        handle.logMessage(LOGMESSAGE, "\n** Gadget interrupted - quitting current simulation **");
        char interruptfile[15];
        strncpy(interruptfile, "", 15);
        strcpy(interruptfile, "interrupt.out");
        this->writeParams(interruptfile, 0);
        handle.logMessage(LOGMESSAGE, "** Gadget interrupted - quitting current simulation **");
        exit(EXIT_SUCCESS);
      }
      interrupted = 0;
    }
#endif

    // remove any expired tagging experiments
    tagvec.deleteTags(TimeInfo);

    // increase the time in the simulation
    TimeInfo->IncrementTime();
  }

  // remove all the tagging experiments - they must have expired now
  tagvec.deleteAllTags();

  likelihood = 0.0;
  for (j = 0; j < likevec.Size(); j++)
    likelihood += likevec[j]->getLikelihood();

  if (handle.getLogLevel() >= LOGMESSAGE) {
    handle.logMessage(LOGMESSAGE, "\nThe current likelihood scores for each component are:");
    for (j = 0; j < likevec.Size(); j++)
      handle.logMessage(LOGMESSAGE, likevec[j]->getName(), likevec[j]->getLikelihood());
    handle.logMessage(LOGMESSAGE, "\nThe current overall likelihood score is", likelihood);
  }
}
