#include "parametervector.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "parametervector.icc"
#endif

ParameterVector::ParameterVector(int sz) {
  size = (sz > 0 ? sz : 0);

  if (size > 0)
    v = new Parameter[size];
  else
    v = 0;
}

ParameterVector::ParameterVector(int sz, Parameter& value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new Parameter[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

ParameterVector::ParameterVector(const ParameterVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new Parameter[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

ParameterVector::~ParameterVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void ParameterVector::resize(int addsize, Parameter& value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void ParameterVector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new Parameter[size];
  } else if (addsize > 0) {
    Parameter* vnew = new Parameter[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void ParameterVector::Delete(int pos) {
  int i;
  if (size > 1) {
    Parameter* vnew = new Parameter[size - 1];
    for (i = 0; i < pos; i++)
      vnew[i] = v[i];
    for (i = pos; i < size - 1; i++)
      vnew[i] = v[i + 1];
    delete[] v;
    v = vnew;
    size--;
  } else {
    delete[] v;
    v = 0;
    size = 0;
  }
}

int ParameterVector::findIndex(Parameter& p) {
  int i;
  for (i = 0; i < size; i++)
    if (v[i] == p)
      return i;
  return -1;
}

CommentStream& operator >> (CommentStream& infile, ParameterVector& paramVec) {
  if (infile.fail()) {
    infile.makebad();
    return infile;
  }
  int i;
  for (i = 0; i < paramVec.size; i++) {
    if (!(infile >> paramVec[i])) {
      infile.makebad();
      return infile;
    }
  }
  return infile;
}

int ParameterVector::readVectorInLine(CommentStream& infile) {
  if (infile.fail())
    return 0;
  char line[MaxStrLength];
  strncpy(line, "", MaxStrLength);
  infile.getLine(line, MaxStrLength);
  if (infile.fail())
    return 0;
  istringstream istr(line);
  istr >> ws;
  int i = 0;
  while (!istr.eof()) {
    if (i == Size())
      resize(1);
    istr >> v[i];
    if (istr.fail())
      //could return the number of the last read element...
      //but compatibility with older code has to be assured
      return 0;
    istr >> ws;
    i++;
  }
  return 1;
}

ParameterVector& ParameterVector::operator = (const ParameterVector& paramv) {
  if (this == &paramv)
    return *this;
  int i;
  if (size == paramv.size) {
    for (i = 0; i < size; i++)
      v[i] = paramv[i];
    return *this;
  }
  delete[] v;
  size = paramv.size;
  if (size > 0) {
    v = new Parameter[size];
    for (i = 0; i < size; i++)
      v[i] = paramv.v[i];
  } else
    v = 0;
  return *this;
}
