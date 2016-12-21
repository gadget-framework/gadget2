#include "intvector.h"
#include "agebandmatrixratioptrvector.h"
#include "commentstream.h"
#include "gadget.h"

AgeBandMatrixRatioPtrVector::AgeBandMatrixRatioPtrVector(int sz, int minage,
  const IntVector& minl, const IntVector& lsize) {

  int i;
  size = (sz > 0 ? sz : 0);
  if (size == 0) {
    v = 0;
  } else {
    v = new AgeBandMatrixRatio*[size];
    for (i = 0; i < size; i++)
      v[i] = new AgeBandMatrixRatio(minage, minl, lsize);
  }
}

AgeBandMatrixRatioPtrVector::~AgeBandMatrixRatioPtrVector() {
  int i;
  for (i = 0; i < tagID.Size(); i++)
    delete[] tagID[i];

  if (v != 0) {
    for (i = 0; i < size; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void AgeBandMatrixRatioPtrVector::resize(int addsize, AgeBandMatrixRatio* matr) {
  if (addsize <= 0)
    return;

  int i;
  if (v == 0) {
    size = addsize;
    v = new AgeBandMatrixRatio*[size];
    for (i = 0; i < size; i++)
      v[i] = matr;

  } else {
    AgeBandMatrixRatio** vnew = new AgeBandMatrixRatio*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    for (i = size; i < addsize + size; i++)
      v[i] = matr;
    size += addsize;
  }
}

void AgeBandMatrixRatioPtrVector::resize(int addsize, int minage,
  const IntVector& minl, const IntVector& lsize) {

  if (addsize <= 0)
    return;

  int i;
  if (v == 0) {
    size = addsize;
    v = new AgeBandMatrixRatio*[size];
    for (i = 0; i < size; i++)
      v[i] = new AgeBandMatrixRatio(minage, minl, lsize);

  } else {
    AgeBandMatrixRatio** vnew = new AgeBandMatrixRatio*[size + addsize];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    for (i = size; i < size + addsize; i++)
      v[i] = new AgeBandMatrixRatio(minage, minl, lsize);
    size += addsize;
  }
}

// New memory has been allocated for each v[i][age][length][tag].N.
// All v[i][age][length][tag].N, v[i][age][length][tag].R added equal -1.0.
void AgeBandMatrixRatioPtrVector::addTag(const char* tagname) {

  double* num;
  int minlength, maxlength, age, length, i;

  this->addTagName(tagname);
  int minage = v[0]->minAge();
  int maxage = v[0]->maxAge();
  for (i = 0; i < size; i++) {
    for (age = minage; age <= maxage; age++) {
      minlength = v[i]->minLength(age);
      maxlength = v[i]->maxLength(age);
      for (length = minlength; length < maxlength; length++) {
        num = new double[1];
        num[0] = 0.0;
        (*v[i])[age][length].resize(num, 0.0);
      }
    }
  }
}

// No memory has been allocated. v[i][age][length][tag].N points to
// the same memory location as initial[i][age][length].N.
void AgeBandMatrixRatioPtrVector::addTag(AgeBandMatrixPtrVector* initial,
  const AgeBandMatrixPtrVector& Alkeys, const char* tagname, double tagloss) {

  int minlength, maxlength, i, age, length;
  double totalnum;

  this->addTagName(tagname);
  tagLoss.resize(1, tagloss);
  int minage = v[0]->minAge();
  int maxage = v[0]->maxAge();
  for (i = 0; i < size; i++) {
    for (age = minage; age <= maxage; age++) {
      minlength = v[i]->minLength(age);
      maxlength = v[i]->maxLength(age);
      for (length = minlength; length < maxlength; length++) {
        totalnum = Alkeys[i][age][length].N;
        if (totalnum < verysmall)
          (*v[i])[age][length].resize((&(*initial)[i][age][length].N), 0.0);
        else
          (*v[i])[age][length].resize((&(*initial)[i][age][length].N), (*initial)[i][age][length].N / totalnum);
      }
    }
  }
}

void AgeBandMatrixRatioPtrVector::addTagName(const char* tagname) {
  char* tempid;
  tempid = new char[strlen(tagname) + 1];
  strcpy(tempid, tagname);
  tagID.resize(tempid);
}

// Returns -1 if do not contain tag with name == tagname.
// Else return the index into the location of the tag with name == tagname.
int AgeBandMatrixRatioPtrVector::getTagID(const char* tagname) {
  int i = 0;
  int found = 0;
  while (i < tagID.Size() && found == 0) {
    if (strcasecmp(tagID[i], tagname) == 0)
      found = 1;
    i++;
  }
  if (i == tagID.Size() && found == 0)
    return -1;
  else
    return i - 1;
}

void AgeBandMatrixRatioPtrVector::deleteTag(const char* tagname) {
  int minlength, maxlength, i, age, length;
  int index = getTagID(tagname);
  int minage = v[0]->minAge();
  int maxage = v[0]->maxAge();
  if (index >= 0)  {
    delete[] tagID[index];
    tagID.Delete(index);
    tagLoss.Delete(index);
    for (i = 0; i < size; i++) {
      for (age = minage; age <= maxage; age++) {
        minlength = v[i]->minLength(age);
        maxlength = v[i]->maxLength(age);
        for (length = minlength; length < maxlength; length++)
          (*v[i])[age][length].Delete(index);
      }
    }
  }
}
