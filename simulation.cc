#include "ecosystem.h"
#include "agebandmatrix.h"
#include "errorhandler.h"
#include "runid.h"
#include "gadget.h"
#include "interruptinterface.h"

extern RunID RUNID;
extern ErrorHandler handle;

void Ecosystem::SimulateOneAreaOneTimeSubstep(int area) {
  int i;
  // calculate the number of preys and predators in area.
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->calcNumbers(area, Area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->calcEat(area, Area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->checkEat(area, Area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->adjustEat(area, Area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->reducePop(area, Area, TimeInfo);
}

void Ecosystem::updatePopulationOneArea(int area) {
  int i;
  // under updates are movements to mature stock, renewal, spawning and straying.
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->Grow(area, Area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->updatePopulationPart1(area, Area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->updatePopulationPart2(area, Area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->updatePopulationPart3(area, Area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->updatePopulationPart4(area, Area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->updatePopulationPart5(area, Area, TimeInfo);
}

void Ecosystem::updateAgesOneArea(int area) {
  int i;
  // age related update and movements between stocks.
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->updateAgePart1(area, Area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->updateAgePart2(area, Area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->updateAgePart3(area, Area, TimeInfo);
}

void Ecosystem::SimulateOneTimestep() {
  int i;
  while (TimeInfo->CurrentSubstep() <=  TimeInfo->numSubSteps()) {
    for (i = 0; i < basevec.Size(); i++)
      basevec[i]->Migrate(TimeInfo);
    for (i = 0; i < Area->numAreas(); i++)
      SimulateOneAreaOneTimeSubstep(i);
    TimeInfo->IncrementSubstep();
  }

  for (i = 0; i < Area->numAreas(); i++)
    updatePopulationOneArea(i);
}

void Ecosystem::Simulate(int Optimise, int print) {
  int i, j;

  handle.logMessage("");  //write blank line to log file
  if (Optimise) {
    for (j = 0; j < Likely.Size(); j++)
      Likely[j]->Reset(keeper);
    //put here since boundlikelihood is allowed to change values
    for (j = 0; j < Likely.Size(); j++)
      Likely[j]->addLikelihoodKeeper(TimeInfo, keeper);
  }

  TimeInfo->ResetToBeginning();
  for (i = 0; i < TimeInfo->TotalNoSteps(); i++) {

    for (j = 0; j < basevec.Size(); j++)
      basevec[j]->Reset(TimeInfo);
    for (j = 0; j < tagvec.Size(); j++)
      tagvec[j]->Reset(TimeInfo);

    //Add in any new tagging experiments
    tagvec.updateTags(TimeInfo);

    if (print)
      for (j = 0; j < printvec.Size(); j++)
        printvec[j]->Print(TimeInfo, 0);  //start of timestep, so printtime is 0

    if (TimeInfo->CurrentStep() == 1) //Migration calculated once per year.
      for (j = 0; j < basevec.Size(); j++)
        basevec[j]->calcMigration(TimeInfo);

    SimulateOneTimestep();
    if (Optimise)
      for (j = 0; j < Likely.Size(); j++)
        Likely[j]->addLikelihood(TimeInfo);

    if (print)
      for (j = 0; j < printvec.Size(); j++)
        printvec[j]->Print(TimeInfo, 1);  //end of timestep, so printtime is 1

    for (j = 0; j < Area->numAreas(); j++)
      updateAgesOneArea(j);

    #ifdef INTERRUPT_HANDLER
      if (interrupted) {
        InterruptInterface ui(*this);
        if (!ui.menu()) {
          handle.logMessage("\n** Gadget interrupted - quitting current simulation **");
          char interruptfile[15];
          strncpy(interruptfile, "", 15);
          strcpy(interruptfile, "interrupt.out");
          this->writeParamsInColumns(interruptfile, 0);
          handle.logMessage("** Gadget interrupted - quitting current simulation **");
          exit(EXIT_SUCCESS);
        }
        interrupted = 0;
      }
    #endif

    //Remove any expired tagging experiments
    tagvec.deleteTags(TimeInfo);

    //Increase the time in the simulation
    TimeInfo->IncrementTime();
  }

  if (Optimise) {
    likelihood = 0.0;
    for (j = 0; j < Likely.Size(); j++)
      likelihood += Likely[j]->returnLikelihood();
  }
  handle.logMessage("The current overall likelihood score is", likelihood);

  //Remove all the tagging experiments - they must have expired now
  tagvec.deleteAllTags();
}
