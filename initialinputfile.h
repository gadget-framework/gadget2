#ifndef initialinputfile_h
#define initialinputfile_h

#include "vectorofcharptr.h"
#include "parametervector.h"
#include "doublevector.h"
#include "intvector.h"
#include "commentstream.h"

/* Class InitialInfoFile can read and store information of a specified fileformat.
 * It can read files of one of the following 3 formats:
 *
 * 1. Just plain vector value or values.
 * x11 x12 x13 .. x1n
 * ..
 * xm1 xm2 xm3 .. xmn
 * where each vector is located on a separate line and m, n > 0.
 *
 * 2. Fileformat witch starts with the keyword
 * switches
 * sw1 sw2 sw3 .. swn
 * x11 x12 x12 .. x1n
 * ...
 * xm1 xm2 xm3 .. xmn
 * where m, n > 0 and number of switches equals number of parameters in vector.
 *
 * 3. Fileformat witch starts with the keywords in the same exact order:
 * switch value lowerbound upperbound optimize/optimise
 * sw1 x1 lower1 upper1 opt1
 * ..
 * swn xn lowern uppern optn
 * where n > 0. */

class InitialInputFile {
private:
  VectorOfCharPtr header;
  ParameterVector switches;
  DoubleVector values;
  DoubleVector lowerbound;
  DoubleVector upperbound;
  IntVector optimize;
  CommentStream infile;
  ifstream tmpinfile;
  int repeatedValues;
public:
  InitialInputFile(const char* const filename);
  ~InitialInputFile();
  void getValues(ParameterVector& sw, DoubleVector& val,
    DoubleVector& low, DoubleVector& upp, IntVector& opt);
  void getVectorValue(DoubleVector& val);
  void getSwitchValue(ParameterVector& sw);
  void correctHeaderText(int index, const char* name);
  void readHeader();
  void readVectorFromLine();
  void checkIfFailure();
  void readFromFile();
  int NoVariables() const;
  int readSwitches() const;
  int Optimize(int i) const;
  double Values(int i) const;
  double Lower(int i) const;
  double Upper(int i) const;
  Parameter Switches(int i) const;
  int reachedEndOfFile();
  int repeatedValuesFileFormat();
};

#endif
