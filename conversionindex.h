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
   * \param interp is the flag to denote whether the ConversionIndex will be used to interpolate between the 2 LengthGroupDivision objects (default value 0)
   */
  ConversionIndex(const LengthGroupDivision* const L1,
    const LengthGroupDivision* const L2, int interp = 0);
  /**
   * \brief This is the default ConversionIndex destructor
   */
  ~ConversionIndex() {};
  /**
   * \brief This function will return the flag denoting whether an error has occured or not
   * \return error
   */
  int Error() const { return error; }
  /**
   * \brief This function will return the minimum length group that is contained in both LengthGroupDivision objects
   * \return minlength
   */
  int minLength() const { return minlength; };
  /**
   * \brief This function will return the maximum length group that is contained in both LengthGroupDivision objects
   * \return maxlength
   */
  int maxLength() const { return maxlength; };
  /**
   * \brief This function will return the flag denoting whether the 2 LengthGroupDivision objects have the same length group structure or not
   * \return samedl
   */
  int isSameDl() const { return samedl; };
  /**
   * \brief This function will return the offset that specifies the different starting points for 2 LengthGroupDivision objects that have the same length group structure
   * \return offset
   */
  int getOffset() const { return offset; };
  /**
   * \brief This function will return the flag denoting whether the target LengthGroupDivision object has a finer length group structure or not
   * \return isfiner
   */
  int isFiner() const { return isfiner; };
  int getPos(int i) const { return pos[i]; };
  int minPos(int i) const { return minpos[i]; };
  int maxPos(int i) const { return maxpos[i]; };
  int getNumPos(int i) const { return numpos[i]; };
  void interpolateLengths(DoubleVector& Vf, const DoubleVector& Vc);
protected:
  /**
   * \brief This is the flag to denote whether an error has occured or not
   */
  int error;
  /**
   * \brief This is the flag to denote whether the 2 LengthGroupDivision objects have the same length group structure
   */
  int samedl;
  /**
   * \brief This is the difference in the minimum lengths if the 2 LengthGroupDivision objects have the same length group structure
   */
  int offset;
  /**
   * \brief This is the flag to denote whether the target (L2) LengthGroupDivision object has a finer or coarser length group structure
   */
  int isfiner;
  /**
   * \brief This is the minimum length group that is contained in both LengthGroupDivision objects
   */
  int minlength;
  /**
   * \brief This is the maximum length group that is contained in both LengthGroupDivision objects
   */
  int maxlength;
  /**
   * \brief This is the flag to denote whether the ConversionIndex will be used to interpolate between the 2 LengthGroupDivision objects
   */
  int interpolate;
  /**
   * \brief This is the IntVector of the indices used to convert between the length groups
   */
  IntVector pos;
  IntVector numpos;
  IntVector minpos;
  IntVector maxpos;
  /**
   * \brief This is the DoubleVector of the ratios used when interpolating between the length groups
   */
  DoubleVector iratio;
  /**
   * \brief This is the IntVector of the indices used when interpolating between the length groups
   */
  IntVector ipos;
};

#endif
