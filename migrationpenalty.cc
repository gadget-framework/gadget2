#include "migrationpenalty.h"
#include "errorhandler.h"
#include "readword.h"
#include "gadget.h"

MigrationPenalty::MigrationPenalty(CommentStream& infile, double likweight)
  :Likelihood(MIGRATIONPENALTYLIKELIHOOD, likweight), powercoeffs(2) {

  ErrorHandler handle;
  char text[MaxStrLength];
  stockname = new char[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  strncpy(stockname, "", MaxStrLength);

  ReadWordAndValue(infile, "stockname", stockname);
  ReadWordAndTwoVariables(infile, "powercoeffs", powercoeffs[0], powercoeffs[1]);

  //prepare for next likelihood component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[component]") == 0))
      handle.Unexpected("[component]", text);
  }
}

void MigrationPenalty::AddToLikelihood(const TimeClass* const TimeInfo) {
  likelihood = 0;
  int i;
  if (TimeInfo->CurrentTime() == TimeInfo->TotalNoSteps()) {
    doublevector penalty(stock->ReturnMigration()->Penalty());
    for (i = 0; i < penalty.Size(); i++)
      likelihood += pow(penalty[i], powercoeffs[0]);
    likelihood = pow(likelihood, powercoeffs[1]);
  }
}

void MigrationPenalty::SetStocks(Stockptrvector Stocks) {
  int i;
  int found = 0;
  for (i = 0; i < Stocks.Size(); i++)
    if (strcasecmp(stockname, Stocks[i]->Name()) == 0) {
      found = 1;
      stock = Stocks[i];
    }

  if (found == 0) {
    cerr << "Error when searching for names of stocks for migrationpenalty."
      << "\nDid not find any name matching " << stockname << endl;
    exit(EXIT_FAILURE);
  }
}

MigrationPenalty::~MigrationPenalty() {
  delete[] stockname;
}
