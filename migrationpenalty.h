#ifndef migrationpenalty_h
#define migrationpenalty_h

#include "migration.h"
#include "stock.h"

/**
 * \class MigrationPenalty
 * \brief This is the class used to calculate the likelihood scores based on population errors arising from incorrect migration
 */
class MigrationPenalty : public Likelihood {
public:
  /**
   * \brief This is the default MigrationPenalty constructor
   * \param infile is the CommentStream to read the likelihood parameters from
   * \param w is the weight for the likelihood component
   */
  MigrationPenalty(CommentStream& infile, double w);
  /**
   * \brief This is the default MigrationPenalty destructor
   */
  virtual ~MigrationPenalty();
  /**
   * \brief This function will calculate the likelihood score for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  /**
   * \brief This will select the stocks required for the likelihood class to calculate the requested information
   * \param Stocks is the StockPtrVector listing all the available stocks
   */
  void SetStocks(StockPtrVector Stocks);
  /**
   * \brief This function will print the summary likelihood information
   * \param outfile is the ofstream that all the model information gets sent to
   * \note This function is not used for this likelihood component
   */
  virtual void Print(ofstream& outfile) const {};
private:
  /**
   * \brief This is the name of the stock that will be checked for incorrect migration
   */
  char* stockname;
  /**
   * \brief This is the stock that will be checked for incorrect migration
   */
  Stock* stock;
  /**
   * \brief This is the vector of the coefficients to be applied when there is incorrect migration
   */
  DoubleVector powercoeffs;
};

#endif
