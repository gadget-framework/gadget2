#include "agebandmatrixratio.h"
#include "agebandmatrixratioptrvector.h"

void Agebandmatrixratio::UpdateNumbers(const Agebandmatrix& Total) {

  int NrOfTagExp = this->NrOfTagExp();
  int maxage, minlen, maxlen, age, length, tag;
  maxage = this->Maxage();
  if (NrOfTagExp > 0) {
    for (age = minage; age <= maxage; age++) {
      minlen = this->Minlength(age);
      maxlen = this->Maxlength(age);
      for (length = minlen; length < maxlen; length++)
        for (tag = 0; tag < NrOfTagExp; tag++)
          *((this->operator[](age))[length][tag].N) = (this->operator[](age))[length][tag].R * Total[age][length].N;
    }
  }
}

void Agebandmatrixratio::UpdateRatio(const Agebandmatrix& Total) {

  int NrOfTagExp = this->NrOfTagExp();
  int maxage, minlen, maxlen, age, length, tag;
  maxage = this->Maxage();
  if (NrOfTagExp > 0) {
    for (age = minage; age <= maxage; age++) {
      minlen = this->Minlength(age);
      maxlen = this->Maxlength(age);
      for (length = minlen; length < maxlen; length++) {
        for (tag = 0; tag < NrOfTagExp; tag++) {
          if (*((this->operator[](age))[length][tag].N) == 0 || Total[age][length].N == 0)
            (this->operator[](age))[length][tag].R = 0.0;
          else
            (this->operator[](age))[length][tag].R = *((this->operator[](age))[length][tag].N) / Total[age][length].N;
        }
      }
    }
  }
}

void Agebandmatrixratio::IncrementAge(const Agebandmatrix& Total) {

  int NrOfTagExp = this->NrOfTagExp();
  int i, j, j1, j2, tag;

  if (nrow <= 1)
    return;

  if (NrOfTagExp > 0) {
    i = nrow - 1;
    j1 = max(v[i]->Mincol(), v[i - 1]->Mincol());
    j2 = min(v[i]->Maxcol(), v[i - 1]->Maxcol());
    //For the highest age group
    for (j = j1; j < j2; j++)
      for (tag = 0; tag < NrOfTagExp; tag++)
        (*(*v[i])[j][tag].N) += (*(*v[i - 1])[j][tag].N);

    for (j = v[i - 1]->Mincol(); j < v[i - 1]->Maxcol(); j++)
      for (tag = 0; tag < NrOfTagExp; tag++) {
        (*(*v[i - 1])[j][tag].N) = 0.0;
        (*v[i - 1])[j][tag].R = 0.0;
      }

    //For the other age groups.
    //At the end of each for (i=nrow-2...) loop, the intersection of v[i-1] with
    //v[i] has been copied from v[i-1] to v[i] and v[i-1] has been set to 0.
    for (i = nrow - 2; i > 0; i--) {
      j1 = max(v[i]->Mincol(), v[i - 1]->Mincol());
      j2 = min(v[i]->Maxcol(), v[i - 1]->Maxcol());
      for (j = v[i - 1]->Mincol(); j < j1; j++) {
        for (tag = 0; tag < NrOfTagExp; tag++) {
          (*(*v[i - 1])[j][tag].N) = 0.0;
          (*v[i - 1])[j][tag].R = 0.0;
        }
      }

      for (j = j1; j < j2; j++) {
        for (tag = 0; tag < NrOfTagExp; tag++) {
          (*(*v[i])[j][tag].N) = (*(*v[i - 1])[j][tag].N);
          (*(*v[i - 1])[j][tag].N) = 0.0;
          (*v[i - 1])[j][tag].R = 0.0;
        }
      }

      for (j = j2; j < v[i - 1]->Maxcol(); j++) {
        for (tag = 0; tag < NrOfTagExp; tag++) {
          (*(*v[i - 1])[j][tag].N) = 0.0;
          (*v[i - 1])[j][tag].R = 0.0;
        }
      }
    }

    //Set number in age zero to zero.
    for (j = v[0]->Mincol(); j < v[0]->Maxcol(); j++)
      for (tag = 0; tag < NrOfTagExp; tag++)
        (*(*v[0])[j][tag].N) = 0;

    this->UpdateRatio(Total);
  }
}

void AgebandmratioAdd(agebandmatrixratioptrvector& Alkeys, int AlkeysArea,
  const agebandmatrixratioptrvector& Addition, int AdditionArea,
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
