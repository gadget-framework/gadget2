#ifndef doublevector_h
#define doublevector_h

#include "mathfunc.h"    //for min
#include "gadget.h"

class doublevector {
public:
  doublevector() { size = 0; v = 0; };
  doublevector(int sz);
  doublevector(int sz, double initial);
  doublevector(const doublevector& initial);
  doublevector(double* initial, int sz);
  ~doublevector();
  void resize(int add, double value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return size; };
  double& operator [] (int pos);
  const double& operator [] (int pos) const;
  void Reset();
  void setElementsTo(double d);
  doublevector& operator *= (double d);
  doublevector& operator * (double d) const;
  doublevector& operator += (double d);
  doublevector& operator + (double d) const;
  doublevector& operator -= (double d);
  doublevector& operator - (double d) const;
  doublevector& operator += (const doublevector& d);
  doublevector& operator -= (const doublevector& d);
  friend ostream& operator << (ostream& out, const doublevector& d);
  double operator * (const doublevector& d) const;
  int operator == (const doublevector& d) const;
  doublevector& operator = (const doublevector& d);
  doublevector& subVector(const doublevector& d, int sz);
  doublevector& addVector(const doublevector& d, int sz);
protected:
  double* v;
  int size;
};

#ifdef GADGET_INLINE
#include "doublevector.icc"
#endif

#endif
