#ifndef conversionindex_h
#define conversionindex_h

#include "doublevector.h"
#include "intvector.h"
#include "lengthgroup.h"

class ConversionIndex;

extern void interpolateLengths(DoubleVector& Vf, const DoubleVector& Vc, const ConversionIndex* CI);

class ConversionIndex {
public:
  ConversionIndex(const LengthGroupDivision* const LF, const LengthGroupDivision* const LC, int interp = 0);
  ~ConversionIndex() {};
  int Pos(int i) const { return pos[i]; };
  int minLength() const { return minlength; };
  int maxLength() const { return maxlength; };
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
