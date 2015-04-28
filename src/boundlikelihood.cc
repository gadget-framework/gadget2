#include "boundlikelihood.h"
#include "readfunc.h"
#include "readword.h"
#include "areatime.h"
#include "keeper.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

BoundLikelihood::BoundLikelihood(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const Keeper* const keeper, double weight, const char* name)
  : Likelihood(BOUNDLIKELIHOOD, weight, name) {

  int i, j;
  Parameter tempParam;
  double temp;
  int count = 0;
  //set flag to initialise the bounds - called in Reset
  checkInitialised = 0;

  infile >> ws;
  if (countColumns(infile) != 4)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 4");
  while (!infile.eof()) {
    infile >> tempParam >> ws;
    if (strcasecmp(tempParam.getName(), "default") == 0) {
      infile >> defPower >> defLW >> defUW >> ws;
      count++;

    } else {
      switches.resize(tempParam);
      infile >> temp >> ws;
      powers.resize(1, temp);
      infile >> temp >> ws;
      lowerweights.resize(1, temp);
      infile >> temp >> ws;
      upperweights.resize(1, temp);
      switchnr.resize(1, -1);
      count++;
    }
    infile >> ws;
  }
  handle.logMessage(LOGMESSAGE, "Read penalty file - number of entries", count);
}

void BoundLikelihood::Reset(const Keeper* const keeper) {

  Likelihood::Reset(keeper);
  handle.setNaNFlag(0);  // reset the NaN count
  if (isZero(weight))
    handle.logMessage(LOGWARN, "Warning in boundlikelihood - zero weight for", this->getName());

  if (!checkInitialised) {
    if (!keeper->boundsGiven())
      handle.logMessage(LOGWARN, "Warning in boundlikelihood - no bounds have been set in input file");

    int i, j, k, numvar, numset, numfail;
    numvar = keeper->numVariables();
    numset = switches.Size();

    if (numset != 0) {
      numfail = 0;
      ParameterVector sw(numvar);
      keeper->getSwitches(sw);
      for (i = 0; i < numset; i++)
        for (j = 0; j < numvar; j++)
          if (switches[i] == sw[j])
            switchnr[i] = j;

      for (i = 0; i < numset; i++) {
        if (switchnr[i] == -1) {
          handle.logMessage(LOGWARN, "Warning in boundlikelihood - failed to match switch", switches[i].getName());
          numfail++;
          // delete the entries for the non-existant switch
          switches.Delete(i);
          powers.Delete(i);
          lowerweights.Delete(i);
          upperweights.Delete(i);
          switchnr.Delete(i);
          if (numfail != numset)
            i--;
        }
      }
      numset -= numfail;
    }

    IntVector done(numset, 0);
    // resize vectors to store data
    likelihoods.resize(numvar, 0.0);
    lowerbound.resize(numvar, 0.0);
    upperbound.resize(numvar, 0.0);
    values.resize(numvar, 0.0);

    powers.resize(numvar - numset, 0.0);
    lowerweights.resize(numvar - numset, 0.0);
    upperweights.resize(numvar - numset, 0.0);
    switchnr.resize(numvar - numset, -1);

    DoubleVector lbs(numvar);
    DoubleVector ubs(numvar);
    keeper->getLowerBounds(lbs);
    keeper->getUpperBounds(ubs);

    k = 0;
    for (i = 0; i < numvar; i++) {
      if (switchnr[i] != -1) {
        lowerbound[i] = lbs[switchnr[i]];
        upperbound[i] = ubs[switchnr[i]];
        if (i < numset)
          done[i] = switchnr[i];
        else
          handle.logMessage(LOGFAIL, "Error in boundlikelihood - received invalid variable to check bounds");

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

    for (i = 0; i < powers.Size(); i++)
      if (powers[i] < verysmall)
        handle.logMessage(LOGFAIL, "Error in boundlikelihood - invalid value for power", powers[i]);

    checkInitialised = 1;
  }
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset boundlikelihood component", this->getName());
}

void BoundLikelihood::addLikelihoodKeeper(const TimeClass* const TimeInfo, Keeper* const keeper) {

  int i;
  double temp;
  keeper->getCurrentValues(values);
  for (i = 0; i < switchnr.Size(); i++) {
    if (values[switchnr[i]] < lowerbound[i]) {
      temp = fabs(values[switchnr[i]] - lowerbound[i]);
      if (temp < 1.0)
        likelihoods[i] = lowerweights[i] * pow(temp, (1.0 / powers[i]));
      else
        likelihoods[i] = lowerweights[i] * pow(temp, powers[i]);
      likelihood += likelihoods[i];
      keeper->Update(switchnr[i], lowerbound[i]);

    } else if (values[switchnr[i]] > upperbound[i]) {
      temp = fabs(values[switchnr[i]] - upperbound[i]);
      if (temp < 1.0)
        likelihoods[i] = upperweights[i] * pow(temp, (1.0 / powers[i]));
      else
        likelihoods[i] = upperweights[i] * pow(temp, powers[i]);
      likelihood += likelihoods[i];
      keeper->Update(switchnr[i], upperbound[i]);

    } else
      likelihoods[i] = 0.0;
  }

  if ((handle.getLogLevel() >= LOGMESSAGE) && (isZero(likelihood)))
    handle.logMessage(LOGMESSAGE, "For this model simulation, no parameters are outside the bounds");

  if (handle.getNaNFlag()) {
    likelihood += verybig;
    if (handle.getLogLevel() >= LOGMESSAGE)
      handle.logMessage(LOGMESSAGE, "For this model simulation, a NaN was found within the model");
  }

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Calculated likelihood score for boundlikelihood component to be", likelihood);
}

void BoundLikelihood::printSummary(ofstream& outfile) {
  //JMB there is only one likelihood score here ...
  if (!(isZero(likelihood))) {
    outfile << "all   all        all" << sep << setw(largewidth) << this->getName() << sep
      << setprecision(smallprecision) << setw(smallwidth) << weight << sep
      << setprecision(largeprecision) << setw(largewidth) << likelihood << endl;
    outfile.flush();
  }
}

void BoundLikelihood::Print(ofstream& outfile) const {
  outfile << "\nBoundlikelihood " << this->getName() << " - likelihood value "
    << likelihood << endl;
  outfile.flush();
}
