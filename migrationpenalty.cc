#include "migrationpenalty.h"
#include "errorhandler.h"
#include "readword.h"
#include "gadget.h"

extern ErrorHandler handle;

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
    if (!(strcasecmp(text, "[component]") == 0))
      handle.Unexpected("[component]", text);
  }
}

void MigrationPenalty::addLikelihood(const TimeClass* const TimeInfo) {
  int i;
  double l = 0.0;
  if ((stock->doesMigrate()) && (TimeInfo->CurrentTime() == TimeInfo->TotalNoSteps())) {
    handle.logMessage("Calculating likelihood score for migrationpenalty component");
    DoubleVector penalty(stock->returnMigration()->Penalty());
    for (i = 0; i < penalty.Size(); i++)
      l += pow(penalty[i], powercoeffs[0]);
    l = pow(l, powercoeffs[1]);
    likelihood += l;
    handle.logMessage("The likelihood score for this component on this timestep is", l);
 }
}

void MigrationPenalty::setStocks(StockPtrVector Stocks) {
  int i;
  int found = 0;
  for (i = 0; i < Stocks.Size(); i++)
    if (strcasecmp(stockname, Stocks[i]->getName()) == 0) {
      found++;
      stock = Stocks[i];
    }

  if (found == 0)
    handle.logFailure("Error in migrationpenalty - failed to match stock", stockname);
  if (!(stock->doesMigrate()))
    handle.logWarning("Warning in migrationpenalty - stock doesnt migrate");
}

MigrationPenalty::~MigrationPenalty() {
  delete[] stockname;
}

void MigrationPenalty::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  handle.logMessage("Reset migrationpenalty component");
}
