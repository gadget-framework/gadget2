#ifndef migrationpenalty_h
#define migrationpenalty_h

#include "migration.h"
#include "stock.h"

class MigrationPenalty : public Likelihood {
public:
  MigrationPenalty(CommentStream& infile, double likweight);
  virtual ~MigrationPenalty();
  virtual void AddToLikelihood(const TimeClass* const);
  void SetStocks(StockPtrVector Stocks);
  virtual void Print(ofstream&) const {};
private:
  char* stockname;
  Stock* stock;
  DoubleVector powercoeffs;
};

#endif
