#include "boundlikelihood.h"
#include "readfunc.h"
#include "readword.h"
#include "areatime.h"
#include "keeper.h"
#include "gadget.h"

extern ErrorHandler handle;

BoundLikelihood::BoundLikelihood(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const Keeper* const keeper, double weight)
  : Likelihood(BOUNDLIKELIHOOD, weight) {

  ParameterVector switches;
  int i, j;
  Parameter tempParam;
  DoubleVector tmpvec;
  int count = 0;

  infile >> ws;
  while (!infile.eof()) {
    infile >> tempParam >> ws;
    if (!readVectorInLine(infile, tmpvec))
      handle.Message("Error in boundlikelihood - failed to read values");
    if (tmpvec.Size() != 3)  //3 values plus the name ...
      handle.Message("Error in boundlikelihood - should be 4 columns");

    if (strcasecmp(tempParam.getValue(), "default") == 0) {
      count++;
      defPower = tmpvec[0];
      defLW = tmpvec[1];
      defUW = tmpvec[2];

    } else {
      count++;
      switches.resize(1, tempParam);
      powers.resize(1, tmpvec[0]);
      lowerweights.resize(1, tmpvec[1]);
      upperweights.resize(1, tmpvec[2]);
      switchnr.resize(1, -1);
    }
    infile >> ws;
  }

  if (switchnr.Size() != 0) {
    ParameterVector sw(keeper->numVariables());
    keeper->Switches(sw);
    for (i = 0; i < switches.Size(); i++)
      for (j = 0; j < sw.Size(); j++)
        if (switches[i] == sw[j])
          switchnr[i] = j;

    for (i = 0; i < switches.Size(); i++)
      if (switchnr[i] == -1)
        handle.logWarning("Warning in boundlikelihood - failed to match switch", switches[i].getValue());
  }

  handle.logMessage("Read penalty file - number of entries", count);
  //set flag to initialise the bounds - called in Reset
  checkInitialised = 0;
}

void BoundLikelihood::Reset(const Keeper* const keeper) {

  Likelihood::Reset(keeper);

  if (checkInitialised == 0) {
    if (keeper->boundsGiven() == 0)
      handle.logWarning("Warning in boundlikelihood - no bounds have been set in input file");
      
    int i, j, k, numvar, numset;
    numvar = keeper->numVariables();
    numset = switchnr.Size();

    IntVector done(numset, 0);
    // resize vectors to store data
    likelihoods.resize(numvar, 0.0);
    lowerbound.resize(numvar, 0.0);
    upperbound.resize(numvar, 0.0);

    powers.resize(numvar - numset, 0.0);
    lowerweights.resize(numvar - numset, 0.0);
    upperweights.resize(numvar - numset, 0.0);
    switchnr.resize(numvar - numset, -1);

    DoubleVector lbs(numvar);
    DoubleVector ubs(numvar);
    keeper->LowerBds(lbs);
    keeper->UpperBds(ubs);

    k = 0;
    for (i = 0; i < numvar; i++) {
      if (switchnr[i] != -1) {
        lowerbound[i] = lbs[switchnr[i]];
        upperbound[i] = ubs[switchnr[i]];
        if (i < numset)
          done[i] = switchnr[i];
        else
          handle.Message("Error in setting bounds in boundlikelihood");

      } else {
        for (j = 0; j < numset; j++)
          if (k == done[j])
            k++;

        switchnr[i] = k;
        lowerbound[i] = lbs[k];
        upperbound[i] = ubs[k];
        powers[i] = defPower;
        lowerweights[i] = defLW;
        upperweights[i] = defUW;
        k++;
      }
    }
    checkInitialised = 1;
  }
  handle.logMessage("Reset boundlikelihood component");
}

void BoundLikelihood::addLikelihoodKeeper(const TimeClass* const TimeInfo, Keeper* const keeper) {

  int i;
  DoubleVector values(keeper->numVariables());
  keeper->ValuesOfVariables(values);
  for (i = 0; i < switchnr.Size(); i++) {
    if (values[switchnr[i]] < lowerbound[i]) {
      likelihoods[i] = lowerweights[i] * pow(absolute(values[switchnr[i]] - lowerbound[i]), powers[i]);
      likelihood += likelihoods[i];
      keeper->Update(switchnr[i], lowerbound[i]);

    } else if (values[switchnr[i]] > upperbound[i]) {
      likelihoods[i] = upperweights[i] * pow(absolute(values[switchnr[i]] - upperbound[i]), powers[i]);
      likelihood += likelihoods[i];
      keeper->Update(switchnr[i], upperbound[i]);

    } else
      likelihoods[i] = 0.0;
  }
  handle.logMessage("Calculated likelihood score for boundlikelihood component to be", likelihood);
  if (isZero(likelihood))
    handle.logMessage("For this model simulation, no parameters are outside the bounds");
}
