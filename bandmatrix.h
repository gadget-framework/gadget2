#ifndef bandmatrix_h
#define bandmatrix_h

#include "intvector.h"
#include "doublematrix.h"
#include "doubleindexvector.h"

class bandmatrix;
class bandmatrixvector;

class bandmatrix {
public:
  bandmatrix(const bandmatrix& initial);
  bandmatrix() { minage = 0; nrow = 0; v = 0; };
  bandmatrix(const intvector& minl, const intvector& size,
    int Minage = 0, double initial = 0);
  bandmatrix(const doublematrix& initial, int Minage = 0, int minl = 0);
  bandmatrix(const doubleindexvector& initial, int age);
  bandmatrix(int minl, int lengthsize, int minage,
    int nrow, double initial = 0);
  ~bandmatrix();
  int Nrow() const { return nrow; };
  doubleindexvector& operator [] (int row);
  const doubleindexvector& operator [] (int row) const;
  bandmatrix& operator += (bandmatrix& b);
  int Ncol(int row) const { return (operator[](row).Size()); };
  int Ncol() const { return (operator[](minage).Size()); };
  int Minrow() const { return minage; };
  int Maxrow() const { return minage + nrow - 1; };
  int Mincol(int row) const { return (operator[](row).Mincol()); };
  int Maxcol(int row) const { return (operator[](row).Maxcol()); };
  int Minage() const { return minage; };
  int Maxage() const { return minage + nrow - 1; };
  int Minlength(int age) const { return (operator[](age).Mincol()); };
  int Maxlength(int age) const { return (operator[](age).Maxcol()); };
  void Colsum(doublevector& Result) const;
  //void Print() const;
protected:
  doubleindexvector** v;
  int nrow;
  int minage;
};

class bandmatrixvector {
public:
  bandmatrixvector() { size = 0; v = 0; };
  bandmatrixvector(int sz);
  ~bandmatrixvector();
  void ChangeElement(int nr, const bandmatrix& value);
  bandmatrix& operator [] (int pos);
  const bandmatrix& operator [] (int pos) const;
  void resize(int add, const bandmatrix& initial);
  void resize(int add);
  int Size() const { return size; };
  void Delete(int pos);
protected:
  int size;
  bandmatrix** v;
};

class bandmatrixmatrix {
public:
  bandmatrixmatrix() { nrow = 0; v = 0; };
  bandmatrixmatrix(int nrow, int ncol);
  ~bandmatrixmatrix();
  void ChangeElement(int nrow, int ncol, const bandmatrix& value);
  bandmatrixvector& operator [] (int pos);
  const bandmatrixvector& operator [] (int pos) const;
  void AddRows(int addrow, int ncol);
  void DeleteRow(int row);
  int Nrow() const { return nrow; };
  int Ncol(int i = 0) const { return v[i]->Size(); };
protected:
  int nrow;
  bandmatrixvector** v;
};

#ifdef GADGET_INLINE
#include "bandmatrix.icc"
#endif

#endif
