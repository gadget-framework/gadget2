#include "intvector.h"
#include "agebandmatrixratio.h"
#include "commentstream.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "agebandmatrixratio.icc"
#endif

Agebandmatrixratio::Agebandmatrixratio(const Agebandmatrixratio& initial)
  : minage(initial.Minage()), nrow(initial.Nrow()) {

  int i;
  if (nrow > 0) {
    v = new popratioindexvector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new popratioindexvector(initial[i + minage]);
  } else
      v = 0;
}

Agebandmatrixratio::Agebandmatrixratio(int MinAge, const intvector& minl,
  const intvector& size) : minage(MinAge), nrow(size.Size()) {

  int i;
  if (nrow > 0) {
    v = new popratioindexvector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new popratioindexvector(size[i], minl[i]);
  } else
      v = 0;
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
    v = 0;
  }
}

int Agebandmatrixratio::NrOfTagExp() const {
  int minlength;
  if (nrow > 0) {
    minlength = this->Minlength(minage);
    return (this->operator[](minage))[minlength].Size();
  } else
    return 0;
}

Agebandmatrixratiovector::Agebandmatrixratiovector(int size1, int minage,
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

Agebandmatrixratiovector::~Agebandmatrixratiovector() {
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

void Agebandmatrixratiovector::ChangeElement(int nr, const Agebandmatrixratio& value) {
  // value must have same number of tagging experiments as myself
  assert(0 <= nr && nr < size);
  assert(this->NrOfTagExp() == value.NrOfTagExp());
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

void Agebandmatrixratiovector::resize(int addsize, int minage,
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
void Agebandmatrixratiovector::addTag(const char* id) {

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
void Agebandmatrixratiovector::addTag(Agebandmatrixvector* initial,
  const Agebandmatrixvector& Alkeys, const char* id) {

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

void Agebandmatrixratiovector::resize() {
  size = 0;
}

void Agebandmatrixratiovector::addTagName(const char* name) {
  char* tempid;
  tempid = new char[strlen(name) + 1];
  strcpy(tempid, name);
  tagid.resize(1, tempid);
}

// Returns -1 if do not contain tag with name == id.
// Else return the index into the location of the tag with name == id.
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

const char* Agebandmatrixratiovector::getName(int id) const {
  assert(id >= 0);
  assert(id < this->NrOfTagExp());
  return tagid[id];
}

void Agebandmatrixratiovector::Migrate(const doublematrix& MI, const Agebandmatrixvector& Total) {

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

void Agebandmatrixratiovector::print(char* filename) {

  ofstream outfile;
  outfile.open(filename);
  if (!outfile) {
    cout << "Error - Can't open outputfile for printing tagging information\n";
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

void AgebandmratioAdd(Agebandmatrixratiovector& Alkeys, int AlkeysArea,
  const Agebandmatrixratiovector& Addition, int AdditionArea,
  const ConversionIndex &CI, double ratio, int minage, int maxage) {

  minage =  max(Alkeys[AlkeysArea].Minage(), Addition[AdditionArea].Minage(), minage);
  maxage =  min(Alkeys[AlkeysArea].Maxage(), Addition[AdditionArea].Maxage(), maxage);
  if (maxage < minage)
    return;

  int age, minl, maxl, i, l, tagid, numtags, offset;
  double numfish;

  numtags = Addition.NrOfTagExp();

  if (numtags > Alkeys.NrOfTagExp()) {
    cerr << "Error when trying to add together two agebandmatrixratiovectors\n";
    exit(EXIT_FAILURE);
  }

  if (numtags > 0) {
    intvector tagconversion(numtags);
    for (i = 0; i < numtags; i++) {
      tagconversion[i] = Alkeys.getId(Addition.getName(i));
      if (tagconversion[i] < 0) {
        cerr << "Error when searching for a tagging experiment\n"
          << "Did not find any name matching " << Addition.getName(i) << endl;
        exit(EXIT_FAILURE);
      }
    }

    numfish = 0.0;
    if (CI.SameDl()) { //Same dl on length distributions
      offset = CI.Offset();
      for (age = minage; age <= maxage; age++) {
        minl = max(Alkeys[AlkeysArea].Minlength(age), Addition[AdditionArea].Minlength(age) + offset);
        maxl = min(Alkeys[AlkeysArea].Maxlength(age), Addition[AdditionArea].Maxlength(age) + offset);
        for (l = minl; l < maxl; l++) {
          for (tagid = 0; tagid < numtags; tagid++) {
            numfish = *(Addition[AdditionArea][age][l - offset][tagid].N);
            numfish *= ratio;
            (*Alkeys[AlkeysArea][age][l][tagconversion[tagid]].N) += numfish;
          }
        }
      }

    } else { //Not same dl.
      if (CI.TargetIsFiner()) {
        //Stock that is added to has finer division than the stock that is added to it.
        for (age = minage; age <= maxage; age++) {
          minl = max(Alkeys[AlkeysArea].Minlength(age), CI.Minpos(Addition[AdditionArea].Minlength(age)));
          maxl = min(Alkeys[AlkeysArea].Maxlength(age), CI.Maxpos(Addition[AdditionArea].Maxlength(age) - 1) + 1);
          for (l = minl; l < maxl; l++) {
            for (tagid = 0; tagid < numtags; tagid++) {
              numfish = *(Addition[AdditionArea][age][CI.Pos(l)][tagid].N);
              numfish *= ratio;
              numfish /= CI.Nrof(l);
              *(Alkeys[AlkeysArea][age][l][tagconversion[tagid]].N) += numfish;
            }
          }
        }

      } else {
        //Stock that is added to has coarser division than the stock that is added to it.
        for (age = minage; age <= maxage; age++) {
          minl = max(CI.Minpos(Alkeys[AlkeysArea].Minlength(age)), Addition[AdditionArea].Minlength(age));
          maxl = min(CI.Maxpos(Alkeys[AlkeysArea].Maxlength(age) - 1) + 1, Addition[AdditionArea].Maxlength(age));
          if (maxl > minl && CI.Pos(maxl - 1) < Alkeys[AlkeysArea].Maxlength(age)
            && CI.Pos(minl) >= Alkeys[AlkeysArea].Minlength(age)) {

            for (l = minl; l < maxl; l++) {
              for (tagid = 0; tagid < numtags; tagid++) {
                numfish = *(Addition[AdditionArea][age][l][tagid].N);
                numfish *= ratio;
                *(Alkeys[AlkeysArea][age][CI.Pos(l)][tagconversion[tagid]].N) += numfish;
              }
            }
          }
        }
      }
    }
  }
}

const charptrvector Agebandmatrixratiovector::tagids() const {
  return tagid;
}
