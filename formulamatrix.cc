#include "formulamatrix.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "formulamatrix.icc"
#endif

//constructor for a rectangular Formulamatrix.
Formulamatrix::Formulamatrix(int nr, int nc) {
  assert(nr > 0);
  nrow = nr;
  v = new Formulavector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new Formulavector(nc);
}

//constructor for a rectangular Formulamatrix with initial value.
Formulamatrix::Formulamatrix(int nr, int nc, Formula value) {
  nrow = nr;
  v = new Formulavector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new Formulavector(nc);
  for (i = 0; i < nr; i++) {
    for (j = 0; j < nc; j++)
      (*v[i])[j] = value;
  }
}
//constructor for a possibly nonrectangular Formulamatrix
//the rows need not have the same number of columns.
Formulamatrix::Formulamatrix(int nr, const intvector& nc) {
  assert(nr > 0);
  nrow = nr;
  v = new Formulavector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new Formulavector(nc[i]);
}

Formulamatrix::~Formulamatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
  }
}

Formulamatrix& Formulamatrix::operator=(const Formulamatrix& formulaM) {
  if (this == &formulaM) {
    //same object just return
    return *this;
  }
  int i;
  if (v != 0) {
    //this has some vectors to return memory
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
  nrow = formulaM.nrow;
  if (nrow >= 0) {
    v = new Formulavector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new Formulavector(formulaM[i]);
  } else {
    v = 0;
    nrow = 0;
  }
  return *this;
}

//Adds rows to a Formulamatrix.
void Formulamatrix::AddRows(int add, int length) {
  assert(nrow >= 0 && add > 0);
  Formulavector** vnew = new Formulavector*[nrow + add];
  int i;
  for (i = 0; i < nrow; i++)
    vnew[i] = v[i];
  delete[] v;
  v = vnew;
  for (i = nrow; i < nrow + add; i++)
    v[i] = new Formulavector(length);
  nrow += add;
}

void Formulamatrix::AddRows(int add, int length, Formula formula) {
  if (v == 0)
    nrow = 0;
  int oldnrow = nrow;
  //Add vectors to matrix
  this->AddRows(add, length);
  int i, j;
  //Initiate added vectors with formula
  for (i = oldnrow; i < nrow; i++)
    for (j = 0; j < length; j++)
      (*v[i])[j] = formula;
}

void Formulamatrix::Inform(Keeper* keeper) {
  int i;
  for (i = 0; i < nrow; i++) {
    ostringstream ostr;
    ostr << i + 1 << ends;
    keeper->AddString(ostr.str());
    v[i]->Inform(keeper);
    keeper->ClearLast();
  }
}

CommentStream& operator >> (CommentStream& infile, Formulamatrix& Fmatrix) {
  if (infile.fail()) {
    infile.makebad();
    return infile;
  }
  int i;
  for (i = 0; i < Fmatrix.Nrow(); i++) {
    if (!(infile >> Fmatrix[i])) {
      infile.makebad();
      return infile;
    }
  }
  return infile;
}
