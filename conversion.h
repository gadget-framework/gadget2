#ifndef conversion_h
#define conversion_h

#include "doublevector.h"
#include "intvector.h"

class LengthGroupDivision;
class ConversionIndex;
class Keeper;

extern void Interp(doublevector& Vf, const doublevector& Vc,
  const ConversionIndex* CI);
extern void LengthGroupPrintError(double minl, double maxl,
  double dl, const char* str);
extern void LengthGroupPrintError(double minl, double maxl,
  double dl, const Keeper* const keeper);
extern void LengthGroupPrintError(const doublevector& breaks,
  const Keeper* const keeper);
extern void LengthGroupPrintError(const doublevector& breaks, const char* str);

class LengthGroupDivision {
public:
  LengthGroupDivision(double minlength, double maxlength, double dl);
  LengthGroupDivision(const doublevector& vec);
  LengthGroupDivision(const LengthGroupDivision& lgrpdiv);
  ~LengthGroupDivision();
  double Meanlength(int i) const { return meanlength[i]; };
  double Minlength(int i) const {
    if (iszero(Dl))
      return minlength[i];
    else
      return (meanlength[i] - (0.5 * Dl));
  };
  double Maxlength(int i) const {
    if (iszero(Dl))
      return minlength[i] + 2 * (meanlength[i] - minlength[i]);
    else
      return (meanlength[i] + (0.5 * Dl));
  };
  double dl() const { return Dl; };
  int Size() const { return size; };
  int NoLengthGroups () const  { return size; };
  int NoLengthGroup(double length) const;
  int Combine(const LengthGroupDivision* const addition);
  int Error() const { return error; }
protected:
  doublevector meanlength;
  int size;
  double Dl;
  doublevector minlength;
  int error;
};

class ConversionIndex {
public:
  ConversionIndex(const LengthGroupDivision* const LF, const LengthGroupDivision* const LC, int interp = 0);
  ~ConversionIndex();
  int Pos(int j) const { return pos[j]; };
  int Minlength() const { return minlength; };
  int Maxlength() const { return maxlength; };
  int Minpos(int i) const { return minpos[i]; };
  int Maxpos(int i) const { return maxpos[i]; };
  int Nrof(int i) const { return nrof[i]; };
  int Offset() const { return offset; };
  int SameDl() const { return samedl; };
  int TargetIsFiner() const { return targetisfiner; };
  double InterpRatio(int i) const { return interpratio[i]; };
  int InterpPos(int i) const { return interppos[i]; };
  int Nf() const { return nf; };
  int Nc() const { return nc; };
protected:
  int targetisfiner;
  int samedl;
  int offset;
  int nf;
  int nc;
  int minlength;
  int maxlength;
  intvector pos;
  intvector nrof;
  intvector minpos;
  intvector maxpos;
  doublevector interpratio;
  intvector interppos;
};

#endif
