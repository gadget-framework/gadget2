#ifndef formatedprinting_h
#define formatedprinting_h

#include "intmatrix.h"
#include "agebandm.h"
#include "predator.h"
#include "printer.h"
#include "gadget.h"

class Stock;
class LenStock;
class AreaClass;
class MortPredLength;
class MortPrey;

/*  Methods for printing in a format easy to read by external programs.
 *
 *  See formatedprinting.cc for more information.
 */

ostream& printStockHeader(ostream& o, const char* name, const intmatrix& areas,
  const intmatrix& ages, const LengthGroupDivision& ldiv, int indent = 0);
ostream& printAreasHeader(ostream& o, const intvector& areas, int indent = 0);
ostream& printTime(ostream& o, const TimeClass& t, int indent = 0);
ostream& printN(ostream& o, const Agebandmatrix& a, const intvector* ages, int indent = 0);
ostream& printW(ostream& o, const Agebandmatrix& a, const intvector* ages, int indent = 0);
ostream& printNorW(ostream& o, const Agebandmatrix& a, int PrintN,
  const intvector* ages, int indent = 0);
ostream& printSuitability(ostream& o, const Predator& p, int indent = 0);
ostream& printSuitMatrix(ostream& o, const bandmatrix& b, int rowindex, int indent = 0);
ostream& printMatrixHeader(ostream& o, const LengthGroupDivision& rowlengths,
  const LengthGroupDivision& collenghts, const char* value, int withrowind, int indent = 0);
ostream& printMatrixHeader(ostream& o, int minage, int maxage,
  const LengthGroupDivision& collengths, const char* value, int withrowind,  int indent = 0);
char* strcon(const char* a, const char* b);
char* makeSpaces(int nr);
ostream& printc_hat(ostream& o, const MortPredLength& pred, AreaClass area, int indent = 0);
ostream& printmean_n(ostream& o, const MortPrey& prey, AreaClass area, int indent = 0);
ostream& printMatrixHeader(ostream& o, const intvector& ages,
  const LengthGroupDivision& lengths, const char* value, int withrowind, int indent = 0);
ostream& printNaturalM(ostream& o, const Stock& stock, int indent = 0);
ostream& printNaturalM(ostream& o, const LenStock& stock, int indent = 0);
ostream& printVectorHeader(ostream& o, int minage, int maxage, int agedelta,
  const char* value, int indent = 0);
ostream& printVectorHeader(ostream& o, const intvector& ages, const char* value, int indent = 0);
ostream& printVectorHeader(ostream& o, const LengthGroupDivision& ldiv,
  const char* value, int indent = 0);
ostream& printVector(ostream& o, const doublevector& vec, int indent = 0);
ostream& printz(ostream& o, const MortPrey& prey, AreaClass area, int indent = 0);
ostream& printcannibalism(ostream& o, const MortPrey& prey, AreaClass area, int indent = 0);
ostream& printLengthGroupDivision(ostream& o, const LengthGroupDivision& lgrp, int indent = 0);
ostream& printAgeGroups(ostream& o, int minage, int maxage, int agedelta, int indent = 0);
ostream& printAgeGroups(ostream& o, const intvector& ages, int indent = 0);
ostream& printFbyAge(ostream& o, const doublematrix& a, int minage, int firstyear, int indent);
ostream& printNbyAge(ostream& o, const doublematrix& a, int minage, int firstyear, int indent);
ostream& printM1byAge(ostream& o, const doublematrix& a, int minage, int firstyear, int indent);
ostream& printM2byAge(ostream& o, const doublematrix& a, int minage, int firstyear, int indent);
ostream& printByAgeAndYear(ostream& o, const doublematrix& a, int minage,
  int firstyear, int indent = 0);

#endif
