#ifndef migrationpenalty_h
#define migrationpenalty_h

#include "migration.h"
#include "stock.h"

class MigrationPenalty : public Likelihood {
public:
  MigrationPenalty(CommentStream& infile, double likweight);
  virtual~MigrationPenalty();
  virtual void AddToLikelihood(const TimeClass* const);
  void SetStocks(Stockptrvector Stocks);
  virtual void Print(ofstream&) const;
  virtual void LikelihoodPrint(ofstream&) const {};
private:
  char* stockname;
  Stock* stock;
  doublevector powercoeffs;
};

#endif
