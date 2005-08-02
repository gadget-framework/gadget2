#ifndef migration_h
#define migration_h

#include "livesonareas.h"
#include "actionattimes.h"
#include "keeper.h"
#include "intmatrix.h"
#include "timevariable.h"
#include "rectangle.h"
#include "doublematrixptrvector.h"
#include "formulamatrixptrvector.h"
#include "migrationareaptrvector.h"

/**
 * \class Migration
 * \brief This is the base class used to calculate the migration of a stock within the model
 *
 * \note This will always be overridden by the derived classes that actually calculate the migration
 */
class Migration : protected LivesOnAreas {
public:
  /**
   * \brief This is the default Migration constructor
   * \param areas is the IntVector of areas that the migration will be calculated on
   */
  Migration(const IntVector& areas);
  /**
   * \brief This is the default Migration destructor
   */
  virtual ~Migration() {};
  /**
   * \brief This function will reset the migration data
   */
  virtual void Reset() = 0;
  /**
   * \brief This function will print the migration data
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) = 0;
  /**
   * \brief This will return the migration matrix that has been calculated
   * \param TimeInfo is the TimeClass for the current model
   * \return 0 (will be overridden in derived classes)
   */
  virtual const DoubleMatrix& getMigrationMatrix(const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This will return the penalty that has been calculated due to invalid migration
   * \return penalty, a DoubleVector of the calculated migration penalty
   */
  const DoubleVector& getPenalty() const { return penalty; };
  /**
   * \brief This will check if the migration process will take place on the current timestep
   * \param TimeInfo is the TimeClass for the current model
   * \return 0 (will be overridden in derived classes)
   */
  virtual int isMigrationStep(const TimeClass* const TimeInfo) = 0;
protected:
  /**
   * \brief This is the DoubleVector used to store the migration penalty information
   */
  DoubleVector penalty;
};

class MigrationNumbers : public Migration {
public:
  /**
   * \brief This is the MigrationNumbers constructor
   * \param areas is the IntVector of areas that the migration will be calculated on
   * \param infile is the CommentStream to read the migration data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  MigrationNumbers(CommentStream& infile, const IntVector& areas,
    const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the default MigrationNumbers destructor
   */
  ~MigrationNumbers();
  /**
   * \brief This function will reset the migration data
   */
  void Reset();
  /**
   * \brief This function will print the migration data
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void Print(ofstream& outfile);
  /**
   * \brief This will return the migration matrix that has been calculated
   * \param TimeInfo is the TimeClass for the current model
   * \return migration, a DoubleMatrix of the calculated migration
   */
  const DoubleMatrix& getMigrationMatrix(const TimeClass* const TimeInfo);
  /**
   * \brief This will check if the migration process will take place on the current timestep
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the migration process will take place, 0 otherwise
   */
  virtual int isMigrationStep(const TimeClass* const TimeInfo);
private:
  void readTimeStepData(CommentStream& infile, const TimeClass* const TimeInfo);
  void readGivenRatios(CommentStream& infile, Keeper* const keeper, const AreaClass* const Area);
  void readGivenMatrices(CommentStream& infile, Keeper* const keeper);
  void setMatrixName(char* name);
  int useMatrix(char* name);
  void checkMatrixIndex();
  CharPtrVector allmatrixnames;
  CharPtrVector usedmatrixnames;
  IntVector timeindex;
  IntMatrix checkvalues;
  DoubleMatrixPtrVector calcMigration;
  FormulaMatrixPtrVector readMigration;
};

class MigrationFunction : public Migration {
public:
  /**
   * \brief This is the MigrationFunction constructor
   * \param areas is the IntVector of areas that the migration will be calculated on
   * \param infile is the CommentStream to read the migration data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  MigrationFunction(CommentStream& infile, const IntVector& areas,
    const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the default MigrationFunction destructor
   */
  ~MigrationFunction();
  /**
   * \brief This function will reset the migration data
   */
  void Reset();
  /**
   * \brief This function will print the migration data
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void Print(ofstream& outfile);
  /**
   * \brief This will return the migration matrix that has been calculated
   * \param TimeInfo is the TimeClass for the current model
   * \return migration, a DoubleMatrix of the calculated migration
   */
  const DoubleMatrix& getMigrationMatrix(const TimeClass* const TimeInfo);
  /**
   * \brief This will check if the migration process will take place on the current timestep
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the migration process will take place, 0 otherwise
   */
  virtual int isMigrationStep(const TimeClass* const TimeInfo);
private:
  void readAreaData(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  int updateVariables(const TimeClass* const TimeInfo);
  void recalcMatrix();
  double getMigrationFunction(Rectangle* fromRec, Rectangle* toRec);
  double f1x(double w, double u, double D, double beta);
  double f2x(double w, double u, double D, double beta);
  DoubleMatrix calcMigration;
  TimeVariable diffusion;
  TimeVariable driftx;
  TimeVariable drifty;
  double lambda;
  double delta;
  MigrationAreaPtrVector oceanareas;
};

#endif
