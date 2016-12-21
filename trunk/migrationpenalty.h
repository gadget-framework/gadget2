#ifndef migrationpenalty_h
#define migrationpenalty_h

#include "migration.h"
#include "stock.h"

/**
 * \class MigrationPenalty
 * \brief This is the class used to calculate the likelihood scores based on population errors arising from incorrect migration
 *
 * This class calculates a penalty that is applied if there are insufficient fish of a particular stock on a particular area, due to changes in the migration of the stock.  This can lead to values for the migration matrices that are meaningless.  This can quickly lead to a model that is \b very wrong, and so this gets a high penalty.  A 'reasonable' model will have a zero likelihood score from this component.
 */
class MigrationPenalty : public Likelihood {
public:
  /**
   * \brief This is the default MigrationPenalty constructor
   * \param infile is the CommentStream to read the likelihood parameters from
   * \param weight is the weight for the likelihood component
   * \param name is the name for the likelihood component
   */
  MigrationPenalty(CommentStream& infile, double weight, const char* name);
  /**
   * \brief This is the default MigrationPenalty destructor
   */
  virtual ~MigrationPenalty();
  /**
   * \brief This function will calculate the likelihood score for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void addLikelihood(const TimeClass* const TimeInfo);
  /**
   * \brief This will select the fleets and stocks required to calculate the MigrationPenalty likelihood score
   * \param Fleets is the FleetPtrVector of all the available fleets
   * \param Stocks is the StockPtrVector of all the available stocks
   */
  void setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
  /**
   * \brief This function will reset the MigrationPenalty likelihood information
   * \param keeper is the Keeper for the current model
   */
  virtual void Reset(const Keeper* const keeper);
  /**
   * \brief This function will print summary information from the MigrationPenalty likelihood calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   */
  virtual void printSummary(ofstream& outfile);
  /**
   * \brief This function will print the summary MigrationPenalty likelihood information
   * \param outfile is the ofstream that all the model information gets sent to
   * \note This function is not used for this likelihood component
   */
  virtual void Print(ofstream& outfile) const;
private:
  /**
   * \brief This is the name of the stock that will be checked for incorrect migration
   */
  char* stockname;
  /**
   * \brief This is the Stock that will be checked for incorrect migration
   */
  Stock* stock;
  /**
   * \brief This is the DoubleVector of the coefficients to be applied when there is incorrect migration
   */
  DoubleVector powercoeffs;
};

#endif
