#ifndef print_h
#define print_h

#include "agebandmatrix.h"
#include "gadget.h"

extern void Printagebandm(ofstream& outfile, const Agebandmatrix& Alkeys);
extern void PrintWeightinagebandm(ofstream& outfile, const Agebandmatrix& Alkeys);
extern void BandmatrixPrint(const bandmatrix& matrix, ofstream& outfile);

#endif
