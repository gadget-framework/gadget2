#include "migrationpenalty.h"
#include "errorhandler.h"
#include "readword.h"
#include "gadget.h"

extern ErrorHandler handle;

MigrationPenalty::MigrationPenalty(CommentStream& infile, double weight)
  : Likelihood(MIGRATIONPENALTYLIKELIHOOD, weight), powercoeffs(2) {

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
  likelihood = 0;
  int i;

  if ((stock->doesMigrate()) && (TimeInfo->CurrentTime() == TimeInfo->TotalNoSteps())) {
    DoubleVector penalty(stock->returnMigration()->Penalty());
    for (i = 0; i < penalty.Size(); i++)
      likelihood += pow(penalty[i], powercoeffs[0]);
    likelihood = pow(likelihood, powercoeffs[1]);
  }
}

void MigrationPenalty::setStocks(StockPtrVector Stocks) {
  int i;
  int found = 0;
  for (i = 0; i < Stocks.Size(); i++)
    if (strcasecmp(stockname, Stocks[i]->Name()) == 0) {
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
