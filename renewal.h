#ifndef renewal_h
#define renewal_h

#include "formulavector.h"
#include "commentstream.h"
#include "stock.h"
#include "livesonareas.h"

class RenewalData;
class ConversionIndex;
class LengthGroupDivision;

class RenewalData : protected LivesOnAreas {
public:
  RenewalData(CommentStream& infile, const intvector& areas,
    const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper);
  ~RenewalData();
  void AddRenewal(Agebandmatrix& Alkeys, int area,
    const TimeClass* const TimeInfo, double ratio = 0.0);
  void SetCI(const LengthGroupDivision* const GivenLDiv);
  void Print(ofstream& outfile) const;
  void Reset();
protected:
  int ReadOption;  //if meanlengths, a and b are read
  intvector RenewalTime;
  intvector RenewalArea;
  Agebandmatrixvector Distribution;
  Formulavector Number;
  Formulavector Meanlengths;
  Formulavector Sdev;
  Formulavector Wcoeff1;
  Formulavector Wcoeff2;
  int Minr;
  int Maxr;
  ConversionIndex* CI;
  LengthGroupDivision* LgrpDiv;
};

#endif
