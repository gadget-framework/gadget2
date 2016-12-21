#include "migrationpenalty.h"
#include "errorhandler.h"
#include "readword.h"
#include "gadget.h"
#include "global.h"

MigrationPenalty::MigrationPenalty(CommentStream& infile, double weight, const char* name)
  : Likelihood(MIGRATIONPENALTYLIKELIHOOD, weight, name), powercoeffs(2) {

  char text[MaxStrLength];
  stockname = new char[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  strncpy(stockname, "", MaxStrLength);

  readWordAndValue(infile, "stockname", stockname);
  readWordAndTwoVariables(infile, "powercoeffs", powercoeffs[0], powercoeffs[1]);

  //prepare for next likelihood component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (strcasecmp(text, "[component]") != 0)
      handle.logFileUnexpected(LOGFAIL, "[component]", text);
  }
}

void MigrationPenalty::addLikelihood(const TimeClass* const TimeInfo) {
  if (TimeInfo->getTime() != TimeInfo->numTotalSteps())
    return;

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Calculating likelihood score for migrationpenalty component", this->getName());

  int i;
  double l = 0.0;
  if (stock->doesMigrate()) {
    DoubleVector penalty(stock->getMigration()->getPenalty());
    for (i = 0; i < penalty.Size(); i++)
      l += pow(penalty[i], powercoeffs[0]);
    l = pow(l, powercoeffs[1]);
    likelihood += l;
  }
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "The likelihood score for this component on this timestep is", l);
}

void MigrationPenalty::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i;
  int found = 0;
  for (i = 0; i < Stocks.Size(); i++)
    if (strcasecmp(stockname, Stocks[i]->getName()) == 0) {
      found++;
      stock = Stocks[i];
    }

  if (found != 1)
    handle.logMessage(LOGFAIL, "Error in migrationpenalty - failed to match stock", stockname);
  if (!(stock->doesMigrate()))
    handle.logMessage(LOGWARN, "Warning in migrationpenalty - stock doesnt migrate");
}

MigrationPenalty::~MigrationPenalty() {
  delete[] stockname;
}

void MigrationPenalty::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset migrationpenalty component");
}

void MigrationPenalty::printSummary(ofstream& outfile) {
  //JMB there is only one likelihood score here ...
  if (!(isZero(likelihood))) {
    outfile << "all   all        all" << sep << setw(largewidth) << this->getName() << sep
      << setprecision(smallprecision) << setw(smallwidth) << weight << sep
      << setprecision(largeprecision) << setw(largewidth) << likelihood << endl;
    outfile.flush();
  }
}

void MigrationPenalty::Print(ofstream& outfile) const {
  outfile << "\nMigration Penalty " << this->getName() << " - likelihood value "
    << likelihood << endl;
  outfile.flush();
}
