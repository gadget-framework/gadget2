#ifndef print_h
#define print_h

#include "agebandmatrix.h"
#include "gadget.h"

extern void Printagebandm(ofstream& outfile, const AgeBandMatrix& Alkeys);
extern void PrintWeightinagebandm(ofstream& outfile, const AgeBandMatrix& Alkeys);
extern void BandmatrixPrint(const BandMatrix& matrix, ofstream& outfile);

#endif
