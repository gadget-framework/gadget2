#ifndef migration_h
#define migration_h

#include "livesonareas.h"
#include "keeper.h"
#include "variableinfoptrvector.h"
#include "intmatrix.h"
#include "bandmatrix.h"
#include "doublematrixptrvector.h"
#include "migvariableptrvector.h"

typedef doublematrixptrvector MigrationList;

class Migration : protected LivesOnAreas {
public:
  Migration(CommentStream& infile, int AgeDepMig, const intvector& areas,
    const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper);
  ~Migration();
  const doublematrix& Migrationmatrix(const TimeClass* const TimeInfo, int age = 0);
  void MigrationRecalc(int year);
  void Reset();
  void Reset(const TimeClass* const TimeInfo);
  int Error() const;
  void Clear();
  void Print(ofstream& outfile) const;
  void Print(int nr, ofstream& outfile) const;
  int ErrorNr() { return errornr; };
  const doublevector& Penalty() const;
protected:
  void CopyFromReadToCalc();
  void AdjustMigListAndCheckIfError(MigrationList& MigList);
  int AgeDepMigration;
  intmatrix MatrixNumbers;
  MigrationList ReadMigList;
  MigrationList CalcMigList;
  VariableInfoptrvector OptInfo;
  doublevector OptVariables;
  MigVariableptrvector OptVar;
  int error;
  intmatrix ages;
  intvector AgeNr;
  int errornr;
  doublevector penalty;  //Addition Oct 1997
private:
  void ReadNoMigrationMatrices(CommentStream& infile,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  void ReadOptVariables(CommentStream& infile, intvector& novariables,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  void ReadCoefficients(CommentStream& infile,
    const AreaClass* const Area, Keeper* const keeper);
  void CheckInfoAndDelete(intvector& novariables, Keeper* const keeper);
};

#endif
