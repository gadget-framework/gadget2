#include "intvector.h"
#include "agebandmatrixratioptrvector.h"
#include "commentstream.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "agebandmatrixratioptrvector.icc"
#endif

agebandmatrixratioptrvector::agebandmatrixratioptrvector(int size1, int minage,
  const intvector& minl, const intvector& size2) {

  size = size1;
  int i;
  if (size == 0) {
    v = 0;
  } else {
    v = new Agebandmatrixratio*[size];
    for (i = 0; i < size; i++)
      v[i] = new Agebandmatrixratio(minage, minl, size2);
  }
}

agebandmatrixratioptrvector::~agebandmatrixratioptrvector() {
  int i;
  for (i = 0; i < tagid.Size(); i++)
    delete[] tagid[i];

  if (v != 0) {
    for (i = 0; i < size; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void agebandmatrixratioptrvector::ChangeElement(int nr, const Agebandmatrixratio& value) {
  // value must have same number of tagging experiments as myself
  assert(0 <= nr && nr < size);
  assert(this->NrOfTagExp() == value.NrOfTagExp());
  delete v[nr];
  v[nr] = new Agebandmatrixratio(value);
}

void agebandmatrixratioptrvector::resize(int addsize, Agebandmatrixratio* matr) {
  assert(addsize > 0);
  int i;
  if (v == 0) {
    size = addsize;
    v = new Agebandmatrixratio*[size];
    for (i = 0; i < size; i++)
      v[i] = matr;

  } else {
    Agebandmatrixratio** vnew = new Agebandmatrixratio*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    for (i = size; i < addsize + size; i++)
      v[i] = matr;
    size += addsize;
  }
}

void agebandmatrixratioptrvector::resize(int addsize, int minage,
  const intvector& minl, const intvector& lsize) {

  Agebandmatrixratio** vnew = new Agebandmatrixratio*[size + addsize];
  int i;
  for (i = 0; i < size; i++)
    vnew[i] = v[i];
  for (i = size; i < size + addsize; i++)
    vnew[i] = new Agebandmatrixratio(minage, minl, lsize);
  delete[] v;
  v = vnew;
  size += addsize;
}

// New memory has been allocated for each v[i][age][length][tag].N.
// All v[i][age][length][tag].N, v[i][age][length][tag].R added equal -1.0.
void agebandmatrixratioptrvector::addTag(const char* id) {

  double* num;
  int minlength, maxlength, age, length, i;

  this->addTagName(id);
  double rat = -1.0;
  int minage = v[0]->Minage();
  int maxage = v[0]->Maxage();
  for (i = 0; i < size; i++) {
    for (age = minage; age <= maxage; age++) {
      minlength = v[i]->Minlength(age);
      maxlength = v[i]->Maxlength(age);
      for (length = minlength; length < maxlength; length++) {
        num = new double[1];
        num[0] = -1.0;
        (*v[i])[age][length].resize(1, num, rat);
      }
    }
  }
}

// No memory has been allocated. v[i][age][length][tag].N points to
// the same memory location as initial[i][age][length].N.
void agebandmatrixratioptrvector::addTag(agebandmatrixptrvector* initial,
  const agebandmatrixptrvector& Alkeys, const char* id) {

  int minlength, maxlength, i, age, length;

  this->addTagName(id);
  int minage = v[0]->Minage();
  int maxage = v[0]->Maxage();
  for (i = 0; i < size; i++) {
    for (age = minage; age <= maxage; age++) {
      minlength = v[0]->Minlength(age);
      maxlength = v[0]->Maxlength(age);
      for (length = minlength; length < maxlength; length++) {
        if (!(Alkeys[i][age][length].N > 0))
          (*v[i])[age][length].resize(1, (&(*initial)[i][age][length].N), 0.0);
        else
          (*v[i])[age][length].resize(1, (&(*initial)[i][age][length].N), (*initial)[i][age][length].N / Alkeys[i][age][length].N);
      }
    }
  }
}

void agebandmatrixratioptrvector::resize() {
  size = 0;
}

void agebandmatrixratioptrvector::addTagName(const char* name) {
  char* tempid;
  tempid = new char[strlen(name) + 1];
  strcpy(tempid, name);
  tagid.resize(1, tempid);
}

// Returns -1 if do not contain tag with name == id.
// Else return the index into the location of the tag with name == id.
int agebandmatrixratioptrvector::getId(const char* id) {

  int i = 0;
  int found = 0;

  while (i < tagid.Size() && found == 0) {
    if (strcasecmp(tagid[i], id) == 0)
      found = 1;
    i++;
  }

  if (i == tagid.Size() && found == 0)
    return -1;
  else
    return i - 1;
}

void agebandmatrixratioptrvector::deleteTag(const char* tagname) {

  int minlength, maxlength, i, age, length;

  int index = getId(tagname);
  int minage = v[0]->Minage();
  int maxage = v[0]->Maxage();
  if (index >= 0)  {
    delete [] tagid[index];
    tagid.Delete(index);
    for (i = 0; i < size; i++) {
      for (age = minage; age <= maxage; age++) {
        minlength = v[i]->Minlength(age);
        maxlength = v[i]->Maxlength(age);
        for (length = minlength; length < maxlength; length++)
          (*v[i])[age][length].Delete(index);
      }
    }
  }
}

const char* agebandmatrixratioptrvector::getName(int id) const {
  assert(id >= 0);
  assert(id < this->NrOfTagExp());
  return tagid[id];
}

void agebandmatrixratioptrvector::Migrate(const doublematrix& MI, const agebandmatrixptrvector& Total) {

  assert(MI.Nrow() == size);
  doublevector tmp(size);
  int i, j, age, length, tag;
  int NrOfTagExp = tagid.Size();
  if (NrOfTagExp > 0) {
    for (age = v[0]->Minage(); age <= v[0]->Maxage(); age++) {
      for (length = v[0]->Minlength(age); length < v[0]->Maxlength(age); length++) {
        for (tag = 0; tag < NrOfTagExp; tag++) {
          for (j = 0; j < size; j++)
            tmp[j] = 0;

          for (j = 0; j < size; j++)
            for (i = 0; i < size; i++)
              tmp[j] += (*(*v[i])[age][length][tag].N) * MI[j][i];
        }

        for (j = 0; j < size; j++)
          for (tag = 0; tag < NrOfTagExp; tag++)
            (*(*v[j])[age][length][tag].N) = tmp[j];
      }
    }
    for (i = 0; i < size; i++)
      v[i]->UpdateRatio(Total[i]);
  }
}

void agebandmatrixratioptrvector::print(char* filename) {

  ofstream outfile;
  outfile.open(filename);
  if (!outfile) {
    cerr << "Error - Can't open outputfile for printing tagging information\n";
    exit(EXIT_FAILURE);
  }

  int areas, age, length, minlength, maxlength;
  int minage = v[0]->Minage();
  int maxage = v[0]->Maxage();

  outfile << "Tagging numbers\n\n";
  for (areas = 0; areas < size; areas++) {
    for (age = minage; age <= maxage; age++) {
      minlength = v[areas]->Minlength(age);
      maxlength = v[areas]->Maxlength(age);
      for (length = minlength; length < maxlength; length++)
        outfile << (*(*v[areas])[age][length][0].N) << sep;
      outfile << endl;
    }
  }

  outfile << "Tagging ratios\n\n";
  for (areas = 0; areas < size; areas++) {
    for (age = minage; age <= maxage; age++) {
      minlength = v[areas]->Minlength(age);
      maxlength = v[areas]->Maxlength(age);
      for (length = minlength; length < maxlength; length++)
        outfile << (*v[areas])[age][length][0].R << sep;
      outfile << endl;
    }
  }
}

const charptrvector agebandmatrixratioptrvector::tagids() const {
  return tagid;
}
