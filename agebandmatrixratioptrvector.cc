#include "intvector.h"
#include "agebandmatrixratioptrvector.h"
#include "commentstream.h"
#include "gadget.h"

AgeBandMatrixRatioPtrVector::AgeBandMatrixRatioPtrVector(int size1, int minage,
  const IntVector& minl, const IntVector& size2) {

  size = size1;
  int i;
  if (size == 0) {
    v = 0;
  } else {
    v = new AgeBandMatrixRatio*[size];
    for (i = 0; i < size; i++)
      v[i] = new AgeBandMatrixRatio(minage, minl, size2);
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

  AgeBandMatrixRatio** vnew = new AgeBandMatrixRatio*[size + addsize];
  int i;
  for (i = 0; i < size; i++)
    vnew[i] = v[i];
  for (i = size; i < size + addsize; i++)
    vnew[i] = new AgeBandMatrixRatio(minage, minl, lsize);
  delete[] v;
  v = vnew;
  size += addsize;
}

// New memory has been allocated for each v[i][age][length][tag].N.
// All v[i][age][length][tag].N, v[i][age][length][tag].R added equal -1.0.
void AgeBandMatrixRatioPtrVector::addTag(const char* id) {

  double* num;
  int minlength, maxlength, age, length, i;

  this->addTagName(id);
  double rat = 0.0;
  int minage = v[0]->minAge();
  int maxage = v[0]->maxAge();
  for (i = 0; i < size; i++) {
    for (age = minage; age <= maxage; age++) {
      minlength = v[i]->minLength(age);
      maxlength = v[i]->maxLength(age);
      for (length = minlength; length < maxlength; length++) {
        num = new double[1];
        num[0] = 0.0;
        (*v[i])[age][length].resize(1, num, rat);
      }
    }
  }
}

// No memory has been allocated. v[i][age][length][tag].N points to
// the same memory location as initial[i][age][length].N.
void AgeBandMatrixRatioPtrVector::addTag(AgeBandMatrixPtrVector* initial,
  const AgeBandMatrixPtrVector& Alkeys, const char* id, double tagloss) {

  int minlength, maxlength, i, age, length;
  double totalnum;

  this->addTagName(id);
  tagLoss.resize(1, tagloss);
  int minage = v[0]->minAge();
  int maxage = v[0]->maxAge();
  for (i = 0; i < size; i++) {
    for (age = minage; age <= maxage; age++) {
      minlength = v[0]->minLength(age);
      maxlength = v[0]->maxLength(age);
      for (length = minlength; length < maxlength; length++) {
        totalnum = Alkeys[i][age][length].N;
        if (totalnum < verysmall)
          (*v[i])[age][length].resize(1, (&(*initial)[i][age][length].N), 0.0);
        else
          (*v[i])[age][length].resize(1, (&(*initial)[i][age][length].N), (*initial)[i][age][length].N / totalnum);
      }
    }
  }
}

void AgeBandMatrixRatioPtrVector::addTagName(const char* name) {
  char* tempid;
  tempid = new char[strlen(name) + 1];
  strcpy(tempid, name);
  tagID.resize(1, tempid);
}

// Returns -1 if do not contain tag with name == id.
// Else return the index into the location of the tag with name == id.
int AgeBandMatrixRatioPtrVector::getID(const char* id) {

  int i = 0;
  int found = 0;
  while (i < tagID.Size() && found == 0) {
    if (strcasecmp(tagID[i], id) == 0)
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
  int index = getID(tagname);
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

const char* AgeBandMatrixRatioPtrVector::getName(int id) const {
  assert(0 <= id && id < this->numTagExperiments());
  return tagID[id];
}

void AgeBandMatrixRatioPtrVector::Migrate(const DoubleMatrix& MI, const AgeBandMatrixPtrVector& Total) {

  assert(MI.Nrow() == size);
  DoubleVector tmp(size);
  int i, j, age, length, tag;
  int numTagExperiments = tagID.Size();
  if (numTagExperiments > 0) {
    for (age = v[0]->minAge(); age <= v[0]->maxAge(); age++) {
      for (length = v[0]->minLength(age); length < v[0]->maxLength(age); length++) {
        for (tag = 0; tag < numTagExperiments; tag++) {
          for (j = 0; j < size; j++)
            tmp[j] = 0;

          for (j = 0; j < size; j++)
            for (i = 0; i < size; i++)
              tmp[j] += *((*v[i])[age][length][tag].N) * MI[j][i];

          for (j = 0; j < size; j++)
            *((*v[j])[age][length][tag].N) = tmp[j];
        }
      }
    }
    for (i = 0; i < size; i++)
      v[i]->updateRatio(Total[i]);
  }
}
