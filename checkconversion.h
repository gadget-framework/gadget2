#ifndef checkconversion_h
#define checkconversion_h

#include "conversion.h"

//this function checkes whether 'finer' is not surely a finer length
//group division than 'coarser'.
//'finername' is the name of the object to which 'finer' belongs,
//'coarsername' the name of the object to which 'coarser' belongs.
//This function may not exit if 'finer' is not finer than 'coarser'.

extern void checkLengthGroupIsFiner(const LengthGroupDivision* finer,
  const LengthGroupDivision* coarser, const char* finername, const char* coarsername);

extern int lengthGroupIsFiner(const LengthGroupDivision* finer,
  const LengthGroupDivision* coarser, int& BogusLengthGroup);

extern void printLengthGroupDivisionError(const LengthGroupDivision* lgrpdiv);

extern void printLengthGroupDivisionError(const LengthGroupDivision* finer,
  const LengthGroupDivision* coarser, const char* finername, const char* coarsername);

#endif
