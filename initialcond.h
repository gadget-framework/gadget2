#ifndef initialcond_h
#define initialcond_h

#include "formulaindexvector.h"
#include "formulamatrix.h"
#include "commentstream.h"
#include "agebandm.h"
#include "keeper.h"

class Stock;
class ConversionIndex;
class LengthGroupDivision;

class InitialCond {
public:
  InitialCond(CommentStream& infile, const intvector& area, Keeper* const keeper,
     const char* refWeightFile, const AreaClass* const Area);
  ~InitialCond();
  void Initialize(Agebandmatrixvector& Alkeys);
  void SetCI(const LengthGroupDivision* const GivenLDiv);
  void Print(ofstream& outfile) const;
protected:
  void ReadMeanData(CommentStream& infile, Keeper* const keeper,
     int noareas, int noagegr, int minage, const AreaClass* const Area);
  void ReadNumberData(CommentStream& infile, Keeper* const keeper,
     int noareas, int noagegr, int nolengr, int minage, const AreaClass* const Area);
  LengthGroupDivision* LgrpDiv;
  ConversionIndex* CI;
  Agebandmatrixvector AreaAgeLength;
  Formulamatrix AreaDist;
  Formulamatrix AgeDist;
  Formulamatrix RelCondition;
  Formulamatrix Mean;
  Formulamatrix Sdev;
  Formula SdevMult;
  intvector areas;
  int readNumbers;
};

#endif
