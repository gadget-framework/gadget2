#ifndef migration_h
#define migration_h

#include "livesonareas.h"
#include "keeper.h"
#include "variableinfoptrvector.h"
#include "intmatrix.h"
#include "bandmatrix.h"
#include "doublematrixptrvector.h"
#include "migvariableptrvector.h"

typedef DoubleMatrixPtrVector MigrationList;

class Migration : protected LivesOnAreas {
public:
  Migration(CommentStream& infile, int AgeDepMig, const IntVector& areas,
    const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper);
  ~Migration();
  const DoubleMatrix& Migrationmatrix(const TimeClass* const TimeInfo, int age = 0);
  void MigrationRecalc(int year);
  void Reset();
  void Reset(const TimeClass* const TimeInfo);
  int Error() const;
  void Clear();
  void Print(ofstream& outfile) const;
  void Print(int nr, ofstream& outfile) const;
  int ErrorNr() { return errornr; };
  const DoubleVector& Penalty() const;
protected:
  void CopyFromReadToCalc();
  void adjustMigListAndCheckIfError(MigrationList& MigList);
  int AgeDepMigration;
  IntMatrix MatrixNumbers;
  MigrationList ReadMigList;
  MigrationList CalcMigList;
  VariableInfoPtrVector OptInfo;
  DoubleVector OptVariables;
  MigVariablePtrVector OptVar;
  int error;
  IntMatrix ages;
  IntVector AgeNr;
  int errornr;
  DoubleVector penalty;  //Addition Oct 1997
private:
  void readNoMigrationMatrices(CommentStream& infile,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  void readOptVariables(CommentStream& infile, IntVector& novariables,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  void readCoefficients(CommentStream& infile,
    const AreaClass* const Area, Keeper* const keeper);
  void CheckInfoAndDelete(IntVector& novariables, Keeper* const keeper);
};

#endif
