#include "intvector.h"
#include "agebandmatrixratio.h"
#include "commentstream.h"
#include "gadget.h"

Agebandmatrixratio::Agebandmatrixratio(const Agebandmatrixratio& initial)
  : minage(initial.Minage()), nrow(initial.Nrow()) {

  v = new popratioindexvector*[nrow];
  int i;
  for (i = 0; i < nrow; i++)
    v[i] = new popratioindexvector(initial[i + minage]);
}

Agebandmatrixratio::Agebandmatrixratio(int MinAge, const intvector& minl,
  const intvector& size) : minage(MinAge), nrow(size.Size()) {

  assert(minl.Size() == size.Size());
  v = new popratioindexvector*[nrow];
  int i;
  for (i = 0; i < nrow; i++)
    v[i] = new popratioindexvector(size[i], minl[i]);
}

Agebandmatrixratio::Agebandmatrixratio(int MinAge, const popratioindexvector& initial)
  : minage(MinAge), nrow(1) {

  v = new popratioindexvector*[1];
  v[0] = new popratioindexvector(initial);
}

Agebandmatrixratio::~Agebandmatrixratio() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
  }
}

Agebandmatrixratiovector::Agebandmatrixratiovector(int size1, int minage,
  const intvector& minl, const intvector& size2) {

  assert(size1 >= 0);
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

Agebandmatrixratiovector::~Agebandmatrixratiovector() {
  int i;
  for (i = 0; i < tagid.Size(); i++)
      delete [] tagid[i];

  if (v != 0) {
    for (i = 0; i < size; i++)
      delete v[i];
    delete[] v;
  }
}

void Agebandmatrixratiovector::ChangeElement(int nr, const Agebandmatrixratio& value) {
  assert(0 <= nr && nr < size);
  delete v[nr];
  v[nr] = new Agebandmatrixratio(value);
}

void Agebandmatrixratiovector::resize(int addsize, Agebandmatrixratio* matr) {
  assert(addsize > 0);
  int i;
  if (v == 0) {
    size = addsize;
    v = new Agebandmatrixratio*[size];
    for (i = 0; i < size; i++)
      v[i] = matr;

  } else {
    Agebandmatrixratio** vnew = new Agebandmatrixratio*[addsize+size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete [] v;
    v = vnew;
    for (i = size; i < addsize + size; i++)
      v[i] = matr;
    size = addsize + size;
  }
}

void Agebandmatrixratiovector::resize(int addsize, int minage,
  const intvector& minl, const intvector& lsize) {

  assert(addsize > 0);
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

void Agebandmatrixratiovector::addTag(Agebandmatrixvector* initial, const Agebandmatrixvector& Alkeys, const char* id) {
  char* tempid = new char[strlen(id)+1];
  strcpy(tempid, id);
  tagid.resize(1, tempid);

  assert(v != 0);
  int minage = v[0]->Minage();
  int maxage = v[0]->Maxage();
  int minlength, maxlength;
  int i, age, length;

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

void Agebandmatrixratiovector::resize() {
  size = 0;
}

int Agebandmatrixratiovector::getId(const char* id) {

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

void Agebandmatrixratiovector::deleteTag(const char* tagname) {

  int index = getId(tagname);
  tagid.Delete(index);
  assert(v != 0);
  int minlength, maxlength;
  int i, age, length;
  int minage = v[0]->Minage();
  int maxage = v[0]->Maxage();

  for (i = 0; i < size; i++) {
    for (age = minage; age < maxage; age++) {
      minlength = v[0]->Minlength(age);
      maxlength = v[0]->Maxlength(age);
      for (length = minlength; length < maxlength; length++) {
        (*v[i])[age][length].Delete(index);
      }
    }
  }
}

//Function that implements the migration
void Agebandmatrixratiovector::Migrate(const doublematrix& MI, const Agebandmatrixvector& Total) {
  assert(MI.Nrow() == size);
  doublevector tmp(size);
  int i, j, age, length, tag;
  int NrOfTagExp = tagid.Size();

  for (age = v[0]->Minage(); age <= v[0]->Maxage(); age++) {
    for (length = v[0]->Minlength(age); length < v[0]->Maxlength(age); length++) {
      for (tag = 0; tag < NrOfTagExp; tag++) {
        for (j = 0; j < size; j++) {
          tmp[j] = 0;
        }
        for (j = 0; j < size; j++) {
          for (i = 0; i < size; i++) {
            tmp[j] += (*(*v[i])[age][length][tag].N) * MI[j][i];
          }
        }
      }

      for (j = 0; j < size; j++) {
        for (tag = 0; tag < NrOfTagExp; tag++) {
          (*(*v[j])[age][length][tag].N) = tmp[j];
        }
        v[j]->UpdateRatio(Total[j], NrOfTagExp);
      }
    }
  }
}

void Agebandmatrixratiovector::print(char* filename) {

  ofstream outfile;
  outfile.open(filename);
  if (!outfile) {
    cout << "Can't open outputfile for tagging\n";
    exit(EXIT_FAILURE);
  }

  int areas, age, length;
  int minlength, maxlength;
  int minage = v[0]->Minage();
  int maxage = v[0]->Maxage();

  for (areas = 0; areas < size; areas++) {
    for (age = minage; age <= maxage; age++) {
      minlength = v[areas]->Minlength(age);
      maxlength = v[areas]->Maxlength(age);
      for (length = minlength; length < maxlength; length++) {
        outfile << (*(*v[areas])[age][length][0].N) << sep;
      }
      outfile << endl;
    }
  }

  outfile << "RATIO\n";
  for (areas = 0; areas < size; areas++) {
    for (age = minage; age <= maxage; age++) {
      minlength = v[areas]->Minlength(age);
      maxlength = v[areas]->Maxlength(age);
      for (length = minlength; length < maxlength; length++) {
        outfile << (*v[areas])[age][length][0].R << sep;
      }
      outfile << endl;
    }
  }
}

