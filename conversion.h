#ifndef conversion_h
#define conversion_h

#include "doublevector.h"
#include "intvector.h"

class LengthGroupDivision;
class ConversionIndex;

extern void interpolateLengths(DoubleVector& Vf, const DoubleVector& Vc, const ConversionIndex* CI);
extern void printLengthGroupError(double minl, double maxl, double dl, const char* str);
extern void printLengthGroupError(const DoubleVector& breaks, const char* str);

class LengthGroupDivision {
public:
  LengthGroupDivision(double minlength, double maxlength, double dl);
  LengthGroupDivision(const DoubleVector& vec);
  LengthGroupDivision(const LengthGroupDivision& lgrpdiv);
  ~LengthGroupDivision();
  double Meanlength(int i) const;
  double Minlength(int i) const;
  double Maxlength(int i) const;
  double dl() const { return Dl; };
  int NoLengthGroups() const { return size; };
  int NoLengthGroup(double length) const;
  int Combine(const LengthGroupDivision* const addition);
  int Error() const { return error; }
protected:
  int error;
  int size;
  double Dl;
  DoubleVector meanlength;
  DoubleVector minlength;
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
  int Size() const { return pos.Size(); };
protected:
  int targetisfiner;
  int samedl;
  int offset;
  int nf;
  int nc;
  int minlength;
  int maxlength;
  IntVector pos;
  IntVector nrof;
  IntVector minpos;
  IntVector maxpos;
  DoubleVector interpratio;
  IntVector interppos;
};

#endif
