#ifndef formulamatrix_h
#define formulamatrix_h

#include "intvector.h"
#include "formulavector.h"

class Formulamatrix {
public:
  Formulamatrix() : nrow(0), v(0) {};
  Formulamatrix(int nrow, int ncol);
  Formulamatrix(int nrow, const intvector& ncol);
  ~Formulamatrix();
  //Do I have to let keeper know if delete old formulavector and make a new one ????
  Formulamatrix& operator = (const Formulamatrix& formulaM);
  int Ncol(int i = 0) const { return(v[i]->Size()); };
  int Nrow() const { return nrow; };
  Formulavector& operator [] (int pos);
  const Formulavector& operator [] (int pos) const;
  void AddRows(int add, int length);
  void AddRows(int add, int length, Formula formula);
  void Inform(Keeper* keeper);
  friend CommentStream& operator >> (CommentStream& infile, Formulamatrix& Fmatrix);
protected:
  int nrow;
  Formulavector** v;
};

#endif
