#ifndef formatedprinting_h
#define formatedprinting_h

#include "intmatrix.h"
#include "agebandmatrix.h"
#include "predator.h"
#include "printer.h"

class Stock;
class LenStock;
class MortPredLength;
class MortPrey;

/*  Methods for printing in a format easy to read by external programs.
 *
 *  See formatedprinting.cc for more information.
 */

ostream& printStockHeader(ostream& o, const char* name, const IntMatrix& areas,
  const IntMatrix& ages, const LengthGroupDivision& ldiv, int indent = 0);
ostream& printAreasHeader(ostream& o, const IntVector& areas, int indent = 0);
ostream& printTime(ostream& o, const TimeClass& t, int indent = 0);
ostream& printN(ostream& o, const AgeBandMatrix& a, const IntVector* ages, int indent = 0);
ostream& printW(ostream& o, const AgeBandMatrix& a, const IntVector* ages, int indent = 0);
ostream& printNorW(ostream& o, const AgeBandMatrix& a, int PrintN,
  const IntVector* ages, int indent = 0);
ostream& printSuitability(ostream& o, const Predator& p, int indent = 0);
ostream& printSuitMatrix(ostream& o, const BandMatrix& b, int rowindex, int indent = 0);
ostream& printMatrixHeader(ostream& o, const LengthGroupDivision& rowlengths,
  const LengthGroupDivision& collenghts, const char* value, int withrowind, int indent = 0);
ostream& printMatrixHeader(ostream& o, int minage, int maxage,
  const LengthGroupDivision& collengths, const char* value, int withrowind,  int indent = 0);
char* strcon(const char* a, const char* b);
char* makeSpaces(int nr);
ostream& printc_hat(ostream& o, const MortPredLength& pred, AreaClass area, int indent = 0);
ostream& printmean_n(ostream& o, const MortPrey& prey, AreaClass area, int indent = 0);
ostream& printMatrixHeader(ostream& o, const IntVector& ages,
  const LengthGroupDivision& lengths, const char* value, int withrowind, int indent = 0);
ostream& printNaturalM(ostream& o, const Stock& stock, int indent = 0);
ostream& printNaturalM(ostream& o, const LenStock& stock, int indent = 0);
ostream& printVectorHeader(ostream& o, int minage, int maxage, int agedelta,
  const char* value, int indent = 0);
ostream& printVectorHeader(ostream& o, const IntVector& ages, const char* value, int indent = 0);
ostream& printVectorHeader(ostream& o, const LengthGroupDivision& ldiv,
  const char* value, int indent = 0);
ostream& printVector(ostream& o, const DoubleVector& vec, int indent = 0);
ostream& printz(ostream& o, const MortPrey& prey, AreaClass area, int indent = 0);
ostream& printcannibalism(ostream& o, const MortPrey& prey, AreaClass area, int indent = 0);
ostream& printLengthGroupDivision(ostream& o, const LengthGroupDivision& lgrp, int indent = 0);
ostream& printAgeGroups(ostream& o, int minage, int maxage, int agedelta, int indent = 0);
ostream& printAgeGroups(ostream& o, const IntVector& ages, int indent = 0);
ostream& printFbyAge(ostream& o, const DoubleMatrix& a, int minage, int firstyear, int indent);
ostream& printNbyAge(ostream& o, const DoubleMatrix& a, int minage, int firstyear, int indent);
ostream& printM1byAge(ostream& o, const DoubleMatrix& a, int minage, int firstyear, int indent);
ostream& printM2byAge(ostream& o, const DoubleMatrix& a, int minage, int firstyear, int indent);
ostream& printByAgeAndYear(ostream& o, const DoubleMatrix& a, int minage,
  int firstyear, int indent = 0);

#endif
