#ifndef migration_h
#define migration_h

#include "livesonareas.h"
#include "actionattimes.h"
#include "keeper.h"
#include "intmatrix.h"
#include "modelvariable.h"
#include "rectangle.h"
#include "doublematrixptrvector.h"
#include "formulamatrixptrvector.h"
#include "migrationareaptrvector.h"

/**
 * \class Migration
 * \brief This is the base class used to calculate the migration of a stock within the model
 *
 * This class moves fish from one area to another within the model.  The proportion of the fish that move is calculated according to a migration matrix.  The migration matrix is either specified in the input files (either directly or by specifying the ratio that move), or is calculated from a migration function, based on drift and diffusion parameters.  Care is taken to ensure that the columns of the migration matrix will sum to 1, so that no fish are created or lost during the migration process.  Once the migration matrix has been calculated, the fish are moved between the areas using a simple matrix multiplication function.
 *
 * \note This will always be overridden by the derived classes that actually calculate the migration
 */
class Migration : public HasName, protected LivesOnAreas {
public:
  /**
   * \brief This is the default Migration constructor
   * \param areas is the IntVector of areas that the migration will be calculated on
   * \param givenname is the name of the stock for this Migration class
   */
  Migration(const IntVector& areas, const char* givenname);
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

/**
 * \class MigrationNumbers
 * \brief This is the class used to calculate the migration of a stock within the model based on migration ratios specified in the input file
 */
class MigrationNumbers : public Migration {
public:
  /**
   * \brief This is the MigrationNumbers constructor
   * \param areas is the IntVector of areas that the migration will be calculated on
   * \param infile is the CommentStream to read the migration data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param givenname is the name of the stock for this Migration class
   * \param keeper is the Keeper for the current model
   */
  MigrationNumbers(CommentStream& infile, const IntVector& areas,
    const AreaClass* const Area, const TimeClass* const TimeInfo,
    const char* givenname, Keeper* const keeper);
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
  /**
   * \brief This function will read the migration timestep data from the input file
   * \param infile is the CommentStream to read the migration timestep data from
   * \param TimeInfo is the TimeClass for the current model
   */
  void readTimeStepData(CommentStream& infile, const TimeClass* const TimeInfo);
  /**
   * \brief This function will read the migration ratio data from the input file
   * \param infile is the CommentStream to read the migration timestep data from
   * \param keeper is the Keeper for the current model
   * \param Area is the AreaClass for the current model
   */
  void readGivenRatios(CommentStream& infile, Keeper* const keeper, const AreaClass* const Area);
  /**
   * \brief This function will read the migration matrix data from the input file
   * \param infile is the CommentStream to read the migration timestep data from
   * \param keeper is the Keeper for the current model
   */
  void readGivenMatrices(CommentStream& infile, Keeper* const keeper);
  /**
   * \brief This will set the name of the migration matrix to be used in the simulation
   * \param name is the name of the migration matrix
   */
  void setMatrixName(char* name);
  /**
   * \brief This will check if the migration matrix is used in the simulation
   * \param name is the name of the migration matrix
   * \return 1 if the migration matrix is used, 0 otherwise
   */
  int useMatrix(char* name);
  /**
   * \brief This will check to ensure that all the required migration information has been specified in the input file
   */
  void checkMatrixIndex();
  /**
   * \brief This is the CharPtrVector used to store the names of the migration matrices that have been read from file
   */
  CharPtrVector allmatrixnames;
  /**
   * \brief This is the CharPtrVector used to store the names of the migration matrices that are used in the simulation
   */
  CharPtrVector usedmatrixnames;
  /**
   * \brief This is the IntVector used to store information about when the migration matrices are to be used in the simulation
   */
  IntVector timeindex;
  /**
   * \brief This is the IntMatrix used to check that sufficient information has been specified to calculate the migration matrices based on the migration ratios given in the input file
   */
  IntMatrix checkvalues;
  /**
   * \brief This is the DoubleMatrixPtrVector used to store the calculated migration parameters
   */
  DoubleMatrixPtrVector calcMigration;
  /**
   * \brief This is the FormulaMatrixPtrVector used to store the migration variables
   */
  FormulaMatrixPtrVector readMigration;
};

/**
 * \class MigrationFunction
 * \brief This is the class used to calculate the migration of a stock within the model based on a migration function
 */
class MigrationFunction : public Migration {
public:
  /**
   * \brief This is the MigrationFunction constructor
   * \param areas is the IntVector of areas that the migration will be calculated on
   * \param infile is the CommentStream to read the migration data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param givenname is the name of the stock for this Migration class
   * \param keeper is the Keeper for the current model
   */
  MigrationFunction(CommentStream& infile, const IntVector& areas,
    const AreaClass* const Area, const TimeClass* const TimeInfo,
    const char* givenname, Keeper* const keeper);
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
  /**
   * \brief This function will read the migration data from the input file
   * \param infile is the CommentStream to read the migration timestep data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  void readAreaData(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  int updateVariables(const TimeClass* const TimeInfo);
  void recalcMatrix();
  double getMigrationFunction(Rectangle* fromRec, Rectangle* toRec);
  double f1x(double w, double u, double D, double beta);
  double f2x(double w, double u, double D, double beta);
  DoubleMatrix calcMigration;
  /**
   * \brief This is the ModelVariable used to store the diffusion parameter
   */
  ModelVariable diffusion;
  /**
   * \brief This is the ModelVariable used to store the longitude drift parameter
   */
  ModelVariable driftx;
  /**
   * \brief This is the ModelVariable used to store the latitude drift parameter
   */
  ModelVariable drifty;
  double lambda;
  double delta;
  /**
   * \brief This is the MigrationAreaPtrVector used to store the migration area information
   */
  MigrationAreaPtrVector oceanareas;
};

#endif
