#include "print.h"

void Printagebandm(ofstream& outfile, const AgeBandMatrix& Alkeys) {

  int i, j;
  int maxcol = 0;
  double zero = 0.0;

  for (i = Alkeys.Minage(); i <= Alkeys.Maxage(); i++)
    if (Alkeys.Maxlength(i) > maxcol)
      maxcol = Alkeys.Maxlength(i);

  for (i = Alkeys.Minage(); i <= Alkeys.Maxage(); i++) {
    outfile << TAB;
    if (Alkeys.Minlength(i) > 0) {
      for (j = 0; j < Alkeys.Minlength(i); j++) {
        outfile.precision(smallprecision);
        outfile.width(smallwidth);
        outfile << zero << sep;
      }
    }
    for (j = Alkeys.Minlength(i); j <Alkeys.Maxlength(i); j++) {
      outfile.precision(smallprecision);
      outfile.width(smallwidth);
      outfile << double(Alkeys[i][j].N) << sep;
    }
    if (Alkeys.Maxlength(i) < maxcol) {
      for (j = Alkeys.Maxlength(i); j < maxcol; j++) {
        outfile.precision(smallprecision);
        outfile.width(smallwidth);
        outfile << zero << sep;
      }
    }
    outfile << endl;
  }
}

void PrintWeightinagebandm(ofstream& outfile, const AgeBandMatrix& Alkeys) {

  int i, j;
  int maxcol = 0;
  double zero = 0.0;

  for (i = Alkeys.Minage(); i <= Alkeys.Maxage(); i++)
    if (Alkeys.Maxlength(i) > maxcol)
      maxcol = Alkeys.Maxlength(i);

  for (i = Alkeys.Minage(); i <= Alkeys.Maxage(); i++) {
    outfile << TAB;
    if (Alkeys.Minlength(i) > 0) {
      for (j = 0; j < Alkeys.Minlength(i); j++) {
        outfile.precision(smallprecision);
        outfile.width(smallwidth);
        outfile << zero << sep;
      }
    }
    for (j = Alkeys.Minlength(i); j < Alkeys.Maxlength(i); j++) {
      outfile.precision(smallprecision);
      outfile.width(smallwidth);
      outfile << double(Alkeys[i][j].W) << sep;
    }
    if (Alkeys.Maxlength(i) < maxcol) {
      for (j = Alkeys.Maxlength(i); j < maxcol; j++) {
        outfile.precision(smallprecision);
        outfile.width(smallwidth);
        outfile << zero << sep;
      }
    }
    outfile << endl;
  }
}

void BandmatrixPrint(const BandMatrix& matrix, ofstream& outfile) {

  int i, j;
  int maxcol = 0;
  double zero = 0.0;
  for (i = matrix.Minage(); i <= matrix.Maxage(); i++)
    if (matrix.Maxlength(i) > maxcol)
      maxcol = matrix.Maxlength(i);
  for (i = matrix.Minage(); i <= matrix.Maxage(); i++) {
    outfile << TAB;
    if (matrix.Minlength(i) > 0) {
      for (j = 0; j < matrix.Minlength(i); j++) {
        outfile.precision(smallprecision);
        outfile.width(smallwidth);
        outfile << zero << sep;
      }
    }
    for (j = matrix.Minlength(i); j <matrix.Maxlength(i); j++) {
      outfile.precision(smallprecision);
      outfile.width(smallwidth);
      outfile << double(matrix[i][j]) << sep;
    }
    if (matrix.Maxlength(i) < maxcol) {
      for (j = matrix.Maxlength(i); j < maxcol; j++) {
        outfile.precision(smallprecision);
        outfile.width(smallwidth);
        outfile << zero << sep;
      }
    }
    outfile << endl;
  }
}
