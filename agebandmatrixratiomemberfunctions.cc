#include "agebandmatrixratio.h"

void Agebandmatrixratio::UpdateNumbers(const Agebandmatrix& Total, const int NrOfTagExp) {

  int maxage, minlen, maxlen, age, length, tag;
  maxage = this->Maxage();
  for (age = 0; age < maxage - minage ; age++) {
    minlen = this->Minlength(age + minage);
    maxlen = this->Maxlength(age + minage);
    for (length = minlen; length < maxlen; length++) {
      for (tag = 0; tag < NrOfTagExp; tag++) {
        (*(*v[age])[length][tag].N) = (*v[age])[length][tag].R * Total[age + minage][length].N;
      }
    }
  }
}

void Agebandmatrixratio::UpdateRatio(const Agebandmatrix& Total, const int NrOfTagExp) {

  int maxage, minlen, maxlen, age, length, tag;
  maxage = this->Maxage();
  for (age = minage; age < maxage; age++) {
    minlen = this->Minlength(age);
    maxlen = this->Maxlength(age);
    for (length = minlen; length < maxlen; length++) {
      for (tag = 0; tag < NrOfTagExp; tag++) {
        if ((*(*v[age-minage])[length][tag].N) == 0 || Total[age][length].N == 0)
          (*v[age-minage])[length][tag].R = 0.0;
        else
          (*v[age-minage])[length][tag].R = (*(*v[age-minage])[length][tag].N) / Total[age][length].N;
      }
    }
  }
}

//This function increases the age.  When moving from one age class to another
//only the intersection of the agegroups of the length classes is moved
void Agebandmatrixratio::IncrementAge(const Agebandmatrix& Total, const int NrOfTagExp) {

  int i, j, j1, j2, tag;

  if (nrow <= 1)
    return;

  i = nrow - 1;
  j1 = max(v[i]->Mincol(), v[i - 1]->Mincol());
  j2 = min(v[i]->Maxcol(), v[i - 1]->Maxcol());
  for (j = j1; j < j2; j++)
    for (tag = 0; tag < NrOfTagExp; tag++)
      (*(*v[i])[j][tag].N) += (*(*v[i - 1])[j][tag].N);

  for (j = v[i - 1]->Mincol(); j < v[i - 1]->Maxcol(); j++) {
    for (tag = 0; tag < NrOfTagExp; tag++) {
      (*(*v[i - 1])[j][tag].N) = 0.0;
      (*v[i - 1])[j][tag].R  = 0.0;
    }
  }

  //Other agegroups.
  //At the end of each for (i=nrow-2...) loop, the intersection of
  //v[i-1] with v[i] has been copied from v[i-1] to v[i] and v[i-1]
  //has been set to 0.
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

  for (j = v[0]->Mincol(); j < v[0]->Maxcol(); j++)
    for (tag = 0; tag < NrOfTagExp; tag++)
      (*(*v[0])[j][tag].N) = 0;

  this->UpdateRatio(Total, NrOfTagExp);
}
