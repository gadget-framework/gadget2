#ifndef renewal_h
#define renewal_h

#include "formulavector.h"
#include "conversionindex.h"
#include "commentstream.h"
#include "stock.h"
#include "livesonareas.h"

class RenewalData;

class RenewalData : protected LivesOnAreas {
public:
  RenewalData(CommentStream& infile, const IntVector& areas,
    const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper);
  ~RenewalData();
  void AddRenewal(AgeBandMatrix& Alkeys, int area,
    const TimeClass* const TimeInfo, double ratio = 0.0);
  void SetCI(const LengthGroupDivision* const GivenLDiv);
  void Print(ofstream& outfile) const;
  void Reset();
protected:
  int ReadOption;  //if meanlengths, a and b are read
  IntVector RenewalTime;
  IntVector RenewalArea;
  AgeBandMatrixPtrVector Distribution;
  FormulaVector Number;
  FormulaVector Meanlengths;
  FormulaVector Sdev;
  FormulaVector Wcoeff1;
  FormulaVector Wcoeff2;
  ConversionIndex* CI;
  LengthGroupDivision* LgrpDiv;
};

#endif
