#include "ecosystem.h"
#include "agebandmatrix.h"
#include "errorhandler.h"
#include "runid.h"
#include "gadget.h"

#ifdef INTERRUPT_HANDLER
#include "interruptinterface.h"
#endif

extern RunID RUNID;
extern ErrorHandler handle;

void Ecosystem::SimulateOneAreaOneTimeSubstep(int area) {

  //Calculate the number of preys and predators in area.
  //Base includes the classes OtherFood, Fleet and Stock.
  int i;
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->CalcNumbers(area, Area, TimeInfo);

  //Calculate consumption and Growth;
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
      basevec[i]->ReducePop(area, Area, TimeInfo);
}

void Ecosystem::GrowthAndSpecialTransactions(int area) {
  int i;

  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->Grow(area, Area, TimeInfo);

  //Under SpecialTransactions are movements to mature stock, spawning and renewal.
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->FirstSpecialTransactions(area, Area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->SecondSpecialTransactions(area, Area, TimeInfo);
}

void Ecosystem::updateOneTimestepOneArea(int area) {
  //Age related update and movements between stocks.
  int i;
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->FirstUpdate(area, Area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->SecondUpdate(area, Area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->ThirdUpdate(area, Area, TimeInfo);
}

void Ecosystem::SimulateOneTimestep() {
  int i;
  while (TimeInfo->CurrentSubstep() <=  TimeInfo->NrOfSubsteps()) {
    for (i = 0; i < basevec.Size(); i++)
      basevec[i]->Migrate(TimeInfo);
    for (i = 0; i < Area->NoAreas(); i++)
      SimulateOneAreaOneTimeSubstep(i);
    TimeInfo->IncrementSubstep();
  }

  for (i = 0; i < Area->NoAreas(); i++)
    GrowthAndSpecialTransactions(i);
}

void Ecosystem::Simulate(int Optimize, int print) {
  int i, j;

  if (Optimize) {
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

    if (TimeInfo->CurrentStep() == 1) //Migrations recalculated once per year.
      for (j = 0; j < basevec.Size(); j++)
        basevec[j]->RecalcMigration(TimeInfo);

    SimulateOneTimestep();
    if (Optimize)
      for (j = 0; j < Likely.Size(); j++)
        Likely[j]->addLikelihood(TimeInfo);

    if (print) {
      for (j = 0; j < printvec.Size(); j++)
        printvec[j]->Print(TimeInfo);
      for (j = 0; j < likprintvec.Size(); j++)
        likprintvec[j]->Print(TimeInfo);
    }

    for (j = 0; j < Area->NoAreas(); j++)
      updateOneTimestepOneArea(j);

    #ifdef INTERRUPT_HANDLER
      if (interrupted) {
        InterruptInterface ui(*this);
        if (!ui.menu()) {
          handle.logMessage("\n** Gadget interrupted - quitting current simulation **");
          //JMB - dump *current* switch values to a file - these wont usually be the *best* values
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

  if (Optimize) {
    likelihood = 0.0;
    for (j = 0; j < Likely.Size(); j++)
      likelihood += Likely[j]->returnLikelihood();
  }
}
