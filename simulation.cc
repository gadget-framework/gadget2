#include "ecosystem.h"
#include "agebandm.h"
#include "runid.h"
#include "gadget.h"

#ifdef INTERRUPT_HANDLER
#include "interruptinterface.h"
#endif

extern RunId RUNID;

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
      basevec[i]->CalcEat(area, Area, TimeInfo);  //Calculate consumption
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->CheckEat(area, Area, TimeInfo);   //Check consumption
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->AdjustEat(area, Area, TimeInfo);  //Adjust consumption
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->ReducePop(area, Area, TimeInfo); //NaturalM, catch
}

void Ecosystem::GrowthAndSpecialTransactions(int area) {
  int i;
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->Grow(area, Area, TimeInfo);

  //Under SpecialTransactions are movements to mature stock, Spawning and Renewal.
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->FirstSpecialTransactions(area, Area, TimeInfo);
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->IsInArea(area))
      basevec[i]->SecondSpecialTransactions(area, Area, TimeInfo);
}

void Ecosystem::UpdateOneTimestepOneArea(int area) {
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

void Ecosystem::SimulateOneTimestep(int print) {
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

  if (print)
    for (i = 0; i < printvec.Size(); i++)
      printvec[i]->Print(TimeInfo);

  for (i = 0; i < Area->NoAreas(); i++)
    UpdateOneTimestepOneArea(i);
}

//Returns 1 when success, 0 when failure.
int Ecosystem::Simulate(int Optimize, int print) {
  likelihood = 0.0;
  int i, j;

  for (i = 0; i < Likely.Size(); i++)
    Likely[i]->Reset(keeper);

  if (Optimize)
    //Added 24 sept 1998 is the function(AddToLikelihood(TimeInfo, keeper);
    //put here for boundlikelihood is allowed to change values
    for (i = 0; i < Likely.Size(); i++)
      Likely[i]->AddToLikelihoodTimeAndKeeper(TimeInfo, keeper);

  TimeInfo->ResetToBeginning();
  for (i = 0; i < basevec.Size(); i++)
    basevec[i]->Reset(TimeInfo);

  int error = 0;
  for (i = 0; i < basevec.Size(); i++)
    if (basevec[i]->Error()) {
      basevec[i]->Clear();
      error = 1;
    }

  if (error)
    return 0;

  for (i = 0; i < TimeInfo->TotalNoSteps(); i++) {
    if (i > 0) //do not need to call reset again on the first step.
      for (j = 0; j < basevec.Size(); j++)
        basevec[j]->Reset(TimeInfo);

    //Update any tagging data
    tagvec.UpdateTags(TimeInfo);

    if (TimeInfo->CurrentStep() == 1) //Migrations recalculated once per year.
      for (j = 0; j < basevec.Size(); j++)
        basevec[j]->RecalcMigration(TimeInfo);

    SimulateOneTimestep(print);
    if (Optimize)
      for (j = 0; j < Likely.Size(); j++)
        Likely[j]->AddToLikelihood(TimeInfo);

    if (print)
      for (j = 0; j < likprintvec.Size(); j++)
        likprintvec[j]->Print(TimeInfo);

    #ifdef INTERRUPT_HANDLER
      if (interrupted) {
        InterruptInterface ui(*this);
        if (!ui.menu()) {
          //JMB - dump *current* switch values to a file - these wont usually be the *best* values
          char interruptfile[15];
          strncpy(interruptfile, "", 15);
          strcpy(interruptfile, "interrupt.out");
          this->PrintParamsinColumns(interruptfile, 0);
          exit(EXIT_SUCCESS);
        }
        interrupted = 0;
      }
    #endif

    if (i != TimeInfo->TotalNoSteps() - 1)
      TimeInfo->IncrementTime();

    tagvec.DeleteTags(TimeInfo);
  }

  for (i = 0; i < Likely.Size(); i++)
    likelihood += Likely[i]->ReturnLikelihood();

  return 1;
}
