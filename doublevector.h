#ifndef doublevector_h
#define doublevector_h

#include "mathfunc.h"    //for min
#include "gadget.h"

class DoubleVector {
public:
  DoubleVector() { size = 0; v = 0; };
  DoubleVector(int sz);
  DoubleVector(int sz, double initial);
  DoubleVector(const DoubleVector& initial);
  DoubleVector(double* initial, int sz);
  ~DoubleVector();
  void resize(int add, double value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return size; };
  double& operator [] (int pos) { return v[pos]; };
  const double& operator [] (int pos) const { return v[pos]; };
  void Reset();
  void setElementsTo(double d);
  DoubleVector& operator *= (double d);
  DoubleVector& operator * (double d) const;
  DoubleVector& operator += (double d);
  DoubleVector& operator + (double d) const;
  DoubleVector& operator -= (double d);
  DoubleVector& operator - (double d) const;
  DoubleVector& operator += (const DoubleVector& d);
  DoubleVector& operator -= (const DoubleVector& d);
  friend ostream& operator << (ostream& out, const DoubleVector& d);
  double operator * (const DoubleVector& d) const;
  int operator == (const DoubleVector& d) const;
  DoubleVector& operator = (const DoubleVector& d);
protected:
  double* v;
  int size;
};

#endif
