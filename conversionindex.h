#ifndef conversionindex_h
#define conversionindex_h

#include "doublevector.h"
#include "intvector.h"
#include "lengthgroup.h"

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
  int Size() const { return pos.Size(); };
  void interpolateLengths(DoubleVector& Vf, const DoubleVector& Vc);
protected:
  int targetisfiner;
  int samedl;
  int interpolate;
  int offset;
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
