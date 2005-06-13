#ifndef conversionindex_h
#define conversionindex_h

#include "doublevector.h"
#include "intvector.h"
#include "lengthgroup.h"

class ConversionIndex {
public:
  /**
   * \brief This is the ConversionIndex constructor
   * \param L1 is the first LengthGroupDivision
   * \param L2 is the second LengthGroupDivision
   * \param interp is a flag to denote whether the ConversionIndex will be used to interpolate between the 2 LengthGroupDivision objects (default value 0)
   */
  ConversionIndex(const LengthGroupDivision* const L1,
    const LengthGroupDivision* const L2, int interp = 0);
  /**
   * \brief This is the default ConversionIndex destructor
   */
  ~ConversionIndex() {};
  int getPos(int i) const { return pos[i]; };
  int minLength() const { return minlength; };
  int maxLength() const { return maxlength; };
  int minPos(int i) const { return minpos[i]; };
  int maxPos(int i) const { return maxpos[i]; };
  int Nrof(int i) const { return nrof[i]; };
  int getOffset() const { return offset; };
  int isSameDl() const { return samedl; };
  int isFiner() const { return targetisfiner; };
  int Size() const { return pos.Size(); };
  void interpolateLengths(DoubleVector& Vf, const DoubleVector& Vc);
protected:
  int targetisfiner;
  /**
   * \brief This is a flag to denote whether the 2 LengthGroupDivision objects have the same step length
   */
  int samedl;
  /**
   * \brief This is a flag to denote whether the ConversionIndex will be used to interpolate between the 2 LengthGroupDivision objects
   */
  int interpolate;
  int offset;
  /**
   * \brief This is minimum length group that is contained in both LengthGroupDivision objects
   */
  int minlength;
  /**
   * \brief This is maximum length group that is contained in both LengthGroupDivision objects
   */
  int maxlength;
  IntVector pos;
  IntVector nrof;
  IntVector minpos;
  IntVector maxpos;
  DoubleVector interpratio;
  IntVector interppos;
};

#endif
