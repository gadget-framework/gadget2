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
  int ReadMeanData(CommentStream& infile, Keeper* const keeper,
     int noareas, int noagegr, int minage, const AreaClass* const Area);
  int ReadDistributionData(CommentStream& infile, Keeper* const keeper,
     int noareas, int noagegr, int minage, const AreaClass* const Area);
  int ReadBasisData(CommentStream& infile, Keeper* const keeper,
     int numfunc, int noareas, int noagegr, const AreaClass* const Area);
  int ReadLengthBasisData(CommentStream& infile, Keeper* const keeper,
     int numfunc, int noareas, int noagegr, const AreaClass* const Area);
  int ReadNumberData(CommentStream& infile, Keeper* const keeper,
     int noareas, int noagegr, int nolengr, int minage, const AreaClass* const Area);
protected:
  void Recalc();
  LengthGroupDivision* LgrpDiv;
  ConversionIndex* CI;
  Agebandmatrixvector AreaAgeLength;
  Formulamatrix Distribution;
  Formulamatrix RelCondition;
  Formulaindexvector agemultiple;
  intvector areas;
  Formulamatrix LengthBasisPar;
  doublematrix LengthBasis;
  doublematrix MeanBasis;
  doublematrix SdevBasis;
  Formulamatrix MeanPar;
  Formulamatrix SdevPar;
  Formulamatrix Mean;
  Formulamatrix Sdev;
  Formula SdevMult;
  int use_lengthbasisfunctions;
  int readNumbers;
};

#endif
