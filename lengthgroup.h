#ifndef lengthgroup_h
#define lengthgroup_h

#include "doublevector.h"

class LengthGroupDivision;

extern void checkLengthGroupIsFiner(const LengthGroupDivision* finer,
  const LengthGroupDivision* coarser);

extern int lengthGroupIsFiner(const LengthGroupDivision* finer,
  const LengthGroupDivision* coarser, int& BogusLengthGroup);

class LengthGroupDivision {
public:
  LengthGroupDivision(double minlength, double maxlength, double dl);
  LengthGroupDivision(const DoubleVector& vec);
  LengthGroupDivision(const LengthGroupDivision& lgrpdiv);
  ~LengthGroupDivision();
  double Meanlength(int i) const;
  double Minlength(int i) const;
  double Maxlength(int i) const;
  double minLength() const { return minlen; };
  double maxLength() const { return maxlen; };
  double dl() const { return Dl; };
  int NoLengthGroups() const { return size; };
  int NoLengthGroup(double length) const;
  int Combine(const LengthGroupDivision* const addition);
  int Error() const { return error; }
  void printError() const;
protected:
  int error;
  int size;
  double Dl;
  double minlen;
  double maxlen;
  DoubleVector meanlength;
  DoubleVector minlength;
};

#endif
