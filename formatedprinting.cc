#include "formatedprinting.h"
#include "predator.h"
#include "prey.h"
#include "stock.h"
#include "lenstock.h"
#include "gadget.h"

/*  printTime
 *
 *  Purpose:  Print current time.
 *
 *  In: ostream& o     :output stream
 *  TimeClass& t   :current time
 *      int indent     :number of spaces to indent (default = 0)
 *
 *  Output format:     "year <y> step <s>"
 */
ostream& printTime(ostream& o, const TimeClass& t, int indent) {
  char* ind = makeSpaces(indent);
  o << ind << "year " << t.CurrentYear() << " step " << t.CurrentStep() << endl;
  delete[] ind;
  o.flush();
  return o;
}

/*  printAreasHeader
 *
 *  Purpose:  Print list of area numbers
 *
 *  In: ostream& o        :output stream
 *  IntVector& areas  :vector of areas
 *      int indent    :number of spaces to indent (default = 0)
 *
 *  Output format:        "areas <a_0> <a_1> ... <a_n>"
 */
ostream& printAreasHeader(ostream& o, const IntVector& areas, int indent) {
  int i;
  char* ind = makeSpaces(indent);
  o <<ind << "areas";
  for (i = 0; i < areas.Size(); i++)
    o << sep<< areas[i];
  o << endl;
  delete[] ind;
  o.flush();
  return o;
}

/*  printStockHeader
 *
 *  Purpose:  Print header for Stock
 *
 *  In: ostream& o        :output stream
 *  char* name        :name of the stock
 *  IntMatrix& areas      :matrix of areas
 *  IntMatrix& ages       :matrix of ages
 *  LengthGroupDivision ldiv  :lengthgroup division for the stock
 *      int indent        :number of spaces to indent (default = 0)
 *
 *  Output format:
 *  "stocks"
 *  "  <name>"
 *  "area aggregation"
 *  "  <a_1,1> <a_1,2> ... <a_1,n> "
 *  "  ...                         "
 *      "  <a_m,1> <a_m,2> ... <a_m,n> "
 */
ostream& printStockHeader(ostream& o, const char* name, const IntMatrix& areas,
  const IntMatrix& ages, const LengthGroupDivision& ldiv, int indent) {

  int i, j;
  char* ind = makeSpaces(indent);
  o << ind << "stocks\n " << ind << name << endl << ind << "area aggregation\n";
  for (i = 0; i < areas.Nrow(); i++) {
    o << ind << sep;
    for (j = 0; j < areas[i].Size(); j++)
      o << sep << areas[i][j];
    o << endl;
  }
  delete[] ind;
  o.flush();
  return o;
}

/*  printLengthGroupDivision
 *
 *  Purpose:  Print LengthGroupDivision
 *
 *  In: ostream& o          :output stream
 *  LengthGroupDivision lgrp    :lengthgroup division to print
 *      int indent          :number of spaces to indent (default = 0)
 *
 *  Output format:
 *     "lengthgroupdivision"
 *     "  minlength <minl>"
 *     "  maxlength <maxl>"
 *     "  dl <dl>"                           <-- if dl != 0
 *  or "  meanlengths <l_1> <l_2> ... <l_n>  <-- if dl == 0
 */
ostream& printLengthGroupDivision(ostream& o, const LengthGroupDivision& lgrp, int indent) {
  int i;
  char* ind = makeSpaces(indent);
  o << ind << "lengthgroupdivision\n" << ind << "  minlength "
    << lgrp.Minlength(0) << endl << ind << "  maxlength "
    << lgrp.Maxlength(lgrp.NoLengthGroups() - 1) << endl;
  if (lgrp.dl() != 0)
    o << ind << "  dl " << lgrp.dl() << endl;
  else {
    o << ind << "  meanlengths";
    for (i = 0; i<lgrp.NoLengthGroups(); i++)
      o << sep << lgrp.Meanlength(i);
    o << endl;
  }
  delete[] ind;
  return o.flush();
}

/*  printAgeGroups
 *
 *  Purpose:  Print Age groups
 *
 *  In: ostream& o      :output stream
 *  int minage      :minimum age
 *  int maxage      :maximum age
 *  int agedelta        :age_1 - age_0 = agedelta
 *      int indent      :number of spaces to indent (default = 0)
 *
 *  Output format:
 *  "agegroups"
 *  "  minage <mina>"
 *  "  maxage <maxa>"
 *      "  agedelta <da>"
 */
ostream& printAgeGroups(ostream& o, int minage, int maxage, int agedelta, int indent) {
  char* ind = makeSpaces(indent);
  o << ind << "agegroups\n" << ind << "  minage " << minage << endl << ind
    << "  maxage " << maxage << endl << ind << "  agedelta " << agedelta << endl;
  delete[] ind;
  return o.flush();
}

/*  printAgeGroups
 *
 *  Purpose:  Print Age groups
 *
 *  In: ostream& o      :output stream
 *  IntVector& ages     :ages to print
 *      int indent      :number of spaces to indent (default = 0)
 *
 *  Output format:
 *  "agegroups"
 *  "  minage <mina>"
 *  "  maxage <maxa>"
 *      "  ages <a_1> <a_2> ... <a_n>"
 *   or "  agedelta <da>" <-- if  a_2-a_1 == a_3-a_2 == .. a_n-a_(n-1)
 */
ostream& printAgeGroups(ostream& o, const IntVector& ages, int indent) {
  int i, da;
  if (ages.Size() > 2) {
    da = ages[1] - ages[0];
    for (i = 2; i < ages.Size(); i++)
      if (ages[i] - ages[i - 1] != da) {
    da = 0;
    break;
      }
    if (da != 0)
      return printAgeGroups(o, ages[0], ages[ages.Size() - 1], da, indent);
  }
  char* ind = makeSpaces(indent);
  o << ind << "agegroups\n" << ind << "  ages";
  for (i = 0; i < ages.Size(); i++)
    o << sep << ages[i];
  o << endl;
  delete[] ind;
  return o.flush();
}

/*  printSuitability
 *
 *  Purpose:  Print suitability for predator
 *
 *  In: ostream& o     :output stream
 *  Predator& pred     :predator
 *      int indent     :number of spaces to indent (default = 0)
 *
 *  Output format:
 *  "suitability"
 *      "  predator"
 *      "    <pred>"
 *      "  prey"
 *      "    <prey_1>"
 *  "  [printMatrixHeader]"
 *  "  [printSuitMatrix]"
 *      ...
 *      "  prey"
 *      "    <prey_n>"
 *  "  [printMatrixHeader]"
 *  "  [printSuitMatrix]"
 */
/*ostream& printSuitability(ostream& o, const Predator& pred, int indent) {
  int i;
  char* ind = makeSpaces(indent);
  o << ind << "suitability\n" << ind << "  predator\n" << ind << "    "
    << pred.Name() << endl;
  for (i = 0; i < pred.NoPreys(); i++) {
    o << ind << "  prey\n" << ind << "    " << pred.Preys(i)->Name() << endl;
    printMatrixHeader(o, *pred.Preys(i)->ReturnLengthGroupDiv(),
      *pred.ReturnLengthGroupDiv(), "suitability", 0, indent + 2);
    printSuitMatrix(o, pred.Suitability(i), 0);
  }
  delete[] ind;
  o.flush();
  return o;
}*/

/*  printBandMatrix
 *
 *  Purpose:  Print band matrix
 *
 *  In: ostream& o      :output stream
 *  BandMatrix& b       :BandMatrix to print
 *      int indent      :number of spaces to indent (default = 0)
 *
 *  Output format:
 *  "<d_0.0> <d_0.1> ... <d_0.n>"
 *      "<d_1.0> <d_1.1> ... <d_1.n>"
 *      " ...     ...         ...   "
 *      "<d_m.0> <d_m.1> ... <d_m.n>"
 */
ostream& printBandMatrix(ostream& o, const BandMatrix& b, int rowindex, int indent) {
  int i, j, maxcol = 0;
  char* ind = makeSpaces(indent);
  for (i = b.Minage(); i <= b.Maxage(); i++)
    if (b.Maxlength(i) > maxcol)
      maxcol = b.Maxlength(i);
  if (rowindex)
    o << indent << "with rowindex in first column\n";

  for (i = b.Minage(); i <= b.Maxage(); i++) {
    o << ind;
    if (rowindex)
      o << i << "     ";
    if (b.Minlength(i) > 0) {
      for (j = 0; j < b.Minlength(i); j++) {
        o.precision(smallprecision);
        o.width(smallwidth);
        o << 0.0 << sep;
      }
    }
    for (j = b.Minlength(i); j <b.Maxlength(i); j++) {
      o.precision(smallprecision);
      o.width(smallwidth);
      o << double(b[i][j]) << sep;
    }
    if (b.Maxlength(i) < maxcol) {
      for (j = b.Maxlength(i); j < maxcol; j++) {
        o.precision(smallprecision);
        o.width(smallwidth);
        o << 0.0 << sep;
      }
    }
    o << endl;
  }
  delete[] ind;
  o.flush();
  return o;
}

ostream& printc_hat(ostream& o, const MortPredator& pred, AreaClass area, int indent) {
  int a, p;
  char* ind = makeSpaces(indent);
  o << ind << "c_hat\n" << ind << "  predator\n" << ind << "    "
    << pred.Name() << endl;
  for (a = 0; a < area.NoAreas(); a++) {
    printAreasHeader(o, IntVector(1, a), indent + 2);
    for (p = 0; p < pred.NoPreys(); p++)
      if (pred.Preys(p)->IsInArea(a)) {
        o << ind << "  prey\n" << ind << "    " << pred.Preys(p)->Name() << endl;
        printMatrixHeader(o, pred.minPreyAge(p, a), pred.maxPreyAge(p, a),
          *pred.Preys(p)->ReturnLengthGroupDiv(), "c_hat", 0, indent + 2);
        if (&(pred.c_hat[a][p]) != 0)
          printBandMatrix(o, pred.c_hat[a][p], 0, indent);
        else
         o << ind << "<no matrix!>\n";
      }
  }
  delete[] ind;
  return o.flush();
}

ostream& printz(ostream& o, const MortPrey& prey, AreaClass area, int indent) {
  char* ind = makeSpaces(indent);
  o << ind << "z\n" << ind << "  prey\n" << ind << "     " << prey.Name() << endl;
  int ar;
  for (ar = 0; ar < area.NoAreas(); ar++) {
    printAreasHeader(o, IntVector(1, ar), indent + 2);
    printVectorHeader(o, *prey.ReturnLengthGroupDiv(), "z", indent + 2);
    printVector(o, prey.z[ar], indent);
  }
  delete[] ind;
  return o.flush();
}

ostream& printcannibalism(ostream& o, const MortPrey& prey, AreaClass area, int indent) {
  char* ind = makeSpaces(indent);
  o << ind << "cannibalism\n" << ind << "  prey\n" << ind << "     "
    << prey.Name() << endl;
  int ar;
  for (ar = 0; ar < area.NoAreas(); ar++) {
    printAreasHeader(o, IntVector(1, ar), indent + 2);
    printVectorHeader(o, *prey.ReturnLengthGroupDiv(), "cannibalism", indent + 2);
    printVector(o, prey.cannibalism[ar], indent);
  }
  delete[] ind;
  return o.flush();
}

ostream& printVector(ostream& o, const DoubleVector& vec, int indent) {
  char* ind = makeSpaces(indent);
  o << ind;
  int i;
  for (i = 0; i < vec.Size(); i++) {
    o.precision(printprecision);
    o.width(printwidth);
    o << vec[i] << sep;
  }
  o << endl;
  delete[] ind;
  return o.flush();
}

ostream& printmean_n(ostream& o, const MortPrey& prey, AreaClass area, int indent) {
  char* ind = makeSpaces(indent);
  o << ind << "mean_n\n" << ind << "  prey\n" << ind << "     " << prey.Name() << endl;
  int ar;
  for (ar = 0; ar < area.NoAreas(); ar++) {
    printAreasHeader(o, IntVector(1, ar), indent + 2);
    printMatrixHeader(o, 1, 1, *(prey.ReturnLengthGroupDiv()), "mean_n", 0, indent + 2);
    printN(o, prey.Alkeys[ar], 0, indent);
  }
  delete[] ind;
  return o.flush();
}

/*  printNaturalM
 *
 *  Purpose: Print natural mortality
 *
 *  In: ostream& o  :output stream
 *  Stock& stock    :age groups for data
 *      int indent  :number of spaces to indent (default = 0)
 *
 *  Output format:
 *  "stock"
 *  "  <stockname>"
 *      "[printVectorHeader]"
 *      "M_1 M_2 ... M_n
 */
/*ostream& printNaturalM(ostream& o, const Stock& stock, int indent) {
  int i;
  char* ind = makeSpaces(indent);
  o << ind << "stock\n" << ind << sep << stock.Name() << endl;
  printVectorHeader(o, stock.Minage(), stock.Maxage(), 1, "mortality", indent);
  o << ind;
  for (i = stock.mortality().Mincol(); i < stock.mortality().Maxcol(); i++)
    o << stock.mortality()[i] << sep;
  o << endl;
  delete[] ind;
  return o;
}*/

/*  printNaturalM
 *
 *  Purpose: Print natural mortality
 *
 *  In: ostream& o    :output stream
 *  Stock& stock      :age groups for data
 *      int indent    :number of spaces to indent (default = 0)
 *
 *  Output format:
 *  "stock"
 *  "  <stockname>"
 *      "[printVectorHeader]"
 *      "M_1 M_2 ... M_n
 */
/*ostream& printNaturalM(ostream& o, const LenStock& stock, int indent) {
  int i;
  char* ind = makeSpaces(indent);
  o << ind << "stock\n" << ind << sep << stock.Name() << endl;
  printVectorHeader(o, *stock.ReturnLengthGroupDiv(), "mortality", indent);
  o << ind;
  for (i = 0; i < stock.mortality().Size(); i++)
    o << stock.mortality()[i] << sep;
  o << endl;
  delete[] ind;
  return o;
}*/

/*  printVectorHeader
 *
 *  Purpose: Print header for vector data by age
 *
 *  In: ostream& o    :output stream
 *  int minage    :minimum age for data
 *  int maxage    :maximum age for data
 *  int agedelta      :age delta for data
 *      char* value       :name of data values
 *      int indent    :number of spaces to indent (default = 0)
 *
 *  Output format:
 *  "vector"
 *  "  [printAgeGroups]"
 *      "  value"
 *      "    <valname>"
 */
ostream& printVectorHeader(ostream& o, int min, int max, int agedelta,
  const char* value, int indent) {

  char* ind = makeSpaces(indent);
  o << ind << "vector\n";
  printAgeGroups(o, min, max, agedelta, indent + 2);
  o << ind << "  value\n" << ind << "    " << value << endl;
  delete[] ind;
  return o.flush();
}

/*  printVectorHeader
 *
 *  Purpose: Print header for vector data by age
 *
 *  In: ostream& o    :output stream
 *  IntVector& ages   :ages for data
 *      char* value       :name of data values
 *      int indent    :number of spaces to indent (default = 0)
 *
 *  Output format:
 *  "vector"
 *  "  [printAgeGroups]"
 *      "  value"
 *      "    <valname>"
 */
ostream& printVectorHeader(ostream& o, const IntVector& ages,
  const char* value, int indent) {

  char* ind = makeSpaces(indent);
  o << ind << "vector\n";
  printAgeGroups(o, ages, indent + 2);
  o << ind << "  value\n" << ind << "    " << value << endl;
  delete[] ind;
  return o.flush();
}

/*  printVectorHeader
 *
 *  Purpose: Print header for vector data by length
 *
 *  In: ostream& o        :output stream
 *  LengthGroupDivision& ldiv :length division for data
 *      char* value               :name of data values
 *      int indent        :number of spaces to indent (default = 0)
 *
 *  Output format:
 *  "vector"
 *  "  [printLengthGroupDivision]"
 *      "  value"
 *      "    <valname>"
 */
ostream& printVectorHeader(ostream& o, const LengthGroupDivision& ldiv,
  const char* value, int indent) {

  char* ind = makeSpaces(indent);
  o << ind << "vector\n";
  printLengthGroupDivision(o, ldiv, indent + 2);
  o << ind << "  value\n" << ind << "    " << value << endl;
  delete[] ind;
  return o.flush();
}

/*  printMatrixHeader
 *
 *  Purpose: Print header for matrix data by length x length
 *
 *  In: ostream& o          :output stream
 *  LengthGroupDivision& rowlengths :length division for data
 *  LengthGroupDivision& collengths :length division for data
 *      char* value                     :name of data values
 *      int indent          :number of spaces to indent (default = 0)
 *
 *  Output format:
 *  "matrix"
 *      "  row"
 *  "    [printLengthGroupDivision]"
 *      "  column"
 *  "    [printLengthGroupDivision]"
 *      "  value"
 *      "    <valname>"
 */
ostream& printMatrixHeader(ostream& o, const LengthGroupDivision& rowlengths,
  const LengthGroupDivision& collengths, const char* value, int withrowind, int indent) {

  char* ind = makeSpaces(indent);
  o << ind << "matrix\n";
  if (withrowind)
    o << ind << "with rowindex in first column\n";
  o << ind << "  row\n";
  printLengthGroupDivision(o, rowlengths, indent + 4);
  o << ind << "  column\n";
  printLengthGroupDivision(o, collengths, indent + 4);
  o << ind << "  value\n" << ind << "    " << value << endl;
  delete[] ind;
  return o.flush();
}

/*  printMatrixHeader
 *
 *  Purpose: Print header for matrix data by age x length
 *
 *  In: ostream& o          :output stream
 *  IntVector& rowages              :age groups for data
 *  LengthGroupDivision& collengths :length division for data
 *      char* value                     :name of data values
 *      int indent          :number of spaces to indent (default = 0)
 *
 *  Output format:
 *  "matrix"
 *      "  row"
 *  "    [printAgeGroups]"
 *      "  column"
 *  "    [printLengthGroupDivision]"
 *      "  value"
 *      "    <valname>"
 */
ostream& printMatrixHeader(ostream& o, const IntVector& rowages,
  const LengthGroupDivision& collengths, const char* value, int withrowind, int indent) {

  char* ind = makeSpaces(indent);
  o << ind << "matrix\n";
  if (withrowind)
    o << ind << "with rowindex in first column\n";
  o << ind << "  row\n";
  printAgeGroups(o, rowages, indent + 4);
  o << ind << "  column\n";
  printLengthGroupDivision(o, collengths, indent + 4);
  o << ind << "  value\n" << ind << "    " << value << endl;
  o.flush();
  delete[] ind;
  return o;
}

/*  printMatrixHeader
 *
 *  Purpose: Print header for matrix data by age x length
 *
 *  In: ostream& o          :output stream
 *  IntVector& rowages              :age groups for data
 *  LengthGroupDivision& collengths :length division for data
 *      char* value                     :name of data values
 *      int indent          :number of spaces to indent (default = 0)
 *
 *  Output format:
 *  "matrix"
 *      "  row"
 *  "    [printAgeGroups]"
 *      "  column"
 *  "    [printLengthGroupDivision]"
 *      "  value"
 *      "    <valname>"
 */
ostream& printMatrixHeader(ostream& o, int minage, int maxage,
  const LengthGroupDivision& collengths, const char* value, int withrowind, int indent) {

  char* ind = makeSpaces(indent);
  o << ind << "matrix\n";
  if (withrowind)
    o << ind << "with rowindex in first column\n";
  o << ind << "  row\n";
  printAgeGroups(o, minage, maxage, 1, indent + 4);
  o << ind << "  column\n";
  printLengthGroupDivision(o, collengths, indent + 4);
  o << ind << "  value\n" << ind << "    " << value << endl;
  o.flush();
  delete[] ind;
  return o;
}

/*  printSuitMatrix
 *
 *  Purpose: Print Suitability matrix
 *
 *  In: ostream& o      :output stream
 *  BandMatrix& b       :BandMatrix to print
 *      int indent      :number of spaces to indent (default = 0)
 *
 *  Output format:
 *  "<d_0.0> <d_0.1> ... <d_0.n>"
 *      "<d_1.0> <d_1.1> ... <d_1.n>"
 *      " ...     ...         ...   "
 *      "<d_m.0> <d_m.1> ... <d_m.n>"
 */
ostream& printSuitMatrix(ostream& o, const BandMatrix& b, int rowindex, int indent) {
  int i, j, maxcol = 0;
  char* ind = makeSpaces(indent);
  for (i = b.Minage(); i <= b.Maxage(); i++)
    if (b.Maxlength(i) > maxcol)
      maxcol = b.Maxlength(i);
  if (rowindex)
    o << ind << "with rowindex in first column\n";

  for (i = b.Minage(); i <= b.Maxage(); i++) {
    o << ind;
    if (rowindex)
      o << i << "     ";
    if (b.Minlength(i) > 0) {
      for (j = 0; j < b.Minlength(i); j++) {
        o.precision(smallprecision);
        o.width(smallwidth);
        o << 0.0 << sep;
      }
    }
    for (j = b.Minlength(i); j <b.Maxlength(i); j++) {
      o.precision(smallprecision);
      o.width(smallwidth);
      o << double(b[i][j]) << sep;
    }
    if (b.Maxlength(i) < maxcol) {
      for (j = b.Maxlength(i); j < maxcol; j++) {
        o.precision(smallprecision);
        o.width(smallwidth);
        o << 0.0 << sep;
      }
    }
    o << endl;
  }
  delete[] ind;
  o.flush();
  return o;
}

/*  printN
 *
 *  Purpose: Print N from agebandmatrix
 *
 *  In: ostream& o      :output stream
 *  AgeBandMatrix& a    :AgeBandMatrix to print
 *      IntVector& ages         :ages to print at start of line
 *      int indent      :number of spaces to indent (default = 0)
 *
 *  Output format:
 *      "[printNorW]"
 */
ostream& printN(ostream& o, const AgeBandMatrix& a, const IntVector* ages, int indent) {
  return printNorW(o, a, 1, ages, indent);
}

/*  printW
 *
 *  Purpose: PrintW from agebandmatrix
 *
 *  In: ostream& o      :output stream
 *  AgeBandMatrix& a    :AgeBandMatrix to print
 *      IntVector& ages         :ages to print at start of line
 *      int indent      :number of spaces to indent (default = 0)
 *
 *  Output format:
 *      "[printNorW]"
 */
ostream& printW(ostream& o, const AgeBandMatrix& a, const IntVector* ages, int indent) {
  return printNorW(o, a, 0, ages, indent);
}

/*  printNorW
 *
 *  Purpose: Print N or W from agebandmatrix
 *
 *  In: ostream& o          :output stream
 *  AgeBandMatrix& a    :AgeBandMatrix to print
 *      int PrintN      :true -> print N, false -> print W
 *      IntVector& ages         :ages to print at start of line
 *      int indent      :number of spaces to indent (default = 0)
 *
 *  Output format:
 *      "<d_0.0> <d_0.1> ... <d_0.n>"
 *      "<d_1.0> <d_1.1> ... <d_1.n>"
 *      " ...     ...         ...   "
 *      "<d_m.0> <d_m.1> ... <d_m.n>"
 */
ostream& printNorW(ostream& o, const AgeBandMatrix& a, int PrintN,
  const IntVector* ages, int indent) {

  int maxcol = 0;
  int i, j;
  o.setf(ios::fixed);
  char* ind = makeSpaces(indent);

  for (i = a.Minage(); i <= a.Maxage(); i++)
    if (a.Maxlength(i) > maxcol)
      maxcol = a.Maxlength(i);
  int mincol = maxcol;
  for (i = a.Minage(); i <= a.Maxage(); i++)
    if (a.Minlength(i) < mincol)
      mincol = a.Minlength(i);
  for (i = a.Minage(); i <= a.Maxage(); i++) {
    if (ages != 0)
      o << ind << (*ages)[i - a.Minage()] << "     ";
    for (j = mincol; j < a.Minlength(i); j++) {
      o.precision(printprecision);
      o.width(printwidth);
      o << 0.0 << sep;
    }
    for (j = a.Minlength(i); j < a.Maxlength(i); j++) {
      o.precision(printprecision);
      o.width(printwidth);
      if (PrintN)
        o << a[i][j].N;
      else
        o << a[i][j].W;
      o << sep;
    }
    if (a.Maxlength(i) < maxcol) {
      for (j = a.Maxlength(i); j < maxcol; j++) {
        o.precision(printprecision);
        o.width(printwidth);
        o << 0.0 << sep;
      }
    }
    o << endl;
  }
  o << endl;
  o.flush();
  delete[] ind;
  return o;
}

ostream& printByAgeAndYear(ostream& o, const DoubleMatrix& a, int minage,
  int firstyear, int indent) {

  int i, j;
  o.setf(ios::fixed);
  o << "year      ";
  for (i = 0; i < a.Ncol(0); i++) {
    o.precision(lowprecision);
    o.width(lowwidth);
    o << firstyear + i << sep;
  }
  o << "\nage\n";
  for (i = 0; i < a.Nrow(); i++) {
    o.precision(smallprecision);
    o.width(smallwidth);
    if (i == a.Nrow() - 1)
      o << minage + i << "+      ";
    else
      o << minage + i << "       ";
    for (j = 0; j < a.Ncol(i); j++) {
      o.precision(lowprecision);
      o.width(lowwidth);
      o << a[i][j] << sep;
    }
    o << endl;
  }
  o << endl;
  return o.flush();
}

ostream& printFbyAge(ostream& o, const DoubleMatrix& a, int minage,
  int firstyear, int indent) {

  int i, j;
  double sum;
  o.setf(ios::fixed);
  o << "year      ";
  for (i = 0; i < a.Ncol(0); i++) {
    o.precision(smallprecision);
    o.width(smallwidth);
    o << firstyear + i << sep;
  }
  o << "\nage\n";
  for (i = 0; i < a.Nrow(); i++) {
    o.precision(smallprecision);
    o.width(smallwidth);
    if (i == a.Nrow() - 1)
      o << minage + i << "+      ";
    else
      o << minage + i << "       ";
    for (j = 0; j < a.Ncol(i); j++) {
      o.precision(smallprecision);
      o.width(smallwidth);
      o << a[i][j] << sep;
    }
    o << endl;
  }
  o << "\nF 5-10    ";
  int jmin = 5 - minage; //corrected by kgf 23/4 99
  assert(jmin >= 0);
  for (i = 0; i < a.Ncol(0); i++) {
    sum = 0.0;
    for (j = jmin; j < jmin + 6; j++)
      sum += a[j][i];
    sum /= 6;
    o.precision(smallprecision);
    o.width(smallwidth);
    o << sum << sep;
  }
  o << endl << endl;
  return o.flush();
}

ostream& printNbyAge(ostream& o, const DoubleMatrix& a, int minage, int firstyear, int indent) {
  int i, j;
  double sum;

  o.setf(ios::fixed);
  o << "year      ";
  for (i = 0; i < a.Ncol(0); i++) {
    o.precision(lowprecision);
    o.width(lowwidth);
    o << firstyear + i << sep;
  }
  o << "\nage\n";
  for (i = 0; i < a.Nrow(); i++) {
    o.precision(lowprecision);
    o.width(lowwidth);
    if (i == a.Nrow() - 1)
      o << minage + i << "+      ";
    else
      o << minage + i << "       ";
    for (j = 0; j<a.Ncol(i); j++){
      o.precision(lowprecision);
      o.width(lowwidth);
      o << a[i][j] << sep;
    }
    o << endl;
  }
  o << "\ntotal     ";
  for (i = 0; i < a.Ncol(0); i++) {
    sum = 0.0;
    for (j = 0; j < a.Nrow(); j++)
      sum += a[j][i];
    o.precision(lowprecision);
    o.width(lowwidth);
    o << sum << sep;
  }
  o << endl << endl;
  return o.flush();
}

ostream& printM1byAge(ostream& o, const DoubleMatrix& a, int minage, int firstyear, int indent) {
  int i, j;
  o.setf(ios::fixed);
  o << "year      ";
  for (i = 0; i < a.Ncol(0); i++) {
    o.precision(smallprecision);
    o.width(smallwidth);
    o << firstyear + i << sep;
  }
  o << "\nage\n";
  for (i = 0; i < a.Nrow(); i++) {
    o.precision(smallprecision);
    o.width(smallwidth);
    if (i == a.Nrow() - 1)
      o << minage + i << "+      ";
    else
      o << minage + i << "       ";
    for (j = 0; j < a.Ncol(i); j++) {
      o.precision(smallprecision);
      o.width(smallwidth);
      o << a[i][j] << sep;
    }
    o << endl;
  }
  o << endl;
  return o.flush();
}

ostream& printM2byAge(ostream& o, const DoubleMatrix& a, int minage, int firstyear, int indent) {
  int i, j;
  o.setf(ios::fixed);
  o << "year      ";
  for (i = 0; i < a.Ncol(0); i++) {
    o.precision(smallprecision);
    o.width(smallwidth);
    o << firstyear + i << sep;
  }
  o << "\nage\n";
  for (i = 0; i < a.Nrow(); i++) {
    o.precision(smallprecision);
    o.width(smallwidth);
    o << minage + i << "       ";
    for (j = 0; j < a.Ncol(i); j++) {
      o.precision(smallprecision);
      o.width(smallwidth);
      o << a[i][j] << sep;
    }
    o << endl;
  }
  /*JMB code removed from here - see RemovedCode.txt for details*/
  o << endl;
  return o.flush();
}

/*  makeSpaces
 *
 *  Purpose:  allocate array of spaces
 *
 *  In:  int nr     :number of spaces
 *
 *  Out: char*      :array of spaces
 *
 *  Usage:  char* s = makeSpaces(10); ...; delete[] s;
 */
char* makeSpaces(int nr) {
  char* ind = new char[nr + 1];
  int i;
  for (i = 0; i < nr; i++)
    ind[i] = ' ';
  ind[i] = 0;
  return ind;
}

/*  strcon
 *
 *  Purpose:  concatenate two strings into a new string
 *
 *  In:  char* a          :first string
 *       char* b      :second string
 *
 *  Out: char*            :concatenation of a & b
 *
 *  Usage:  char* s = strcon("aa", "bb"); ...; delete[] s;
 */
char* strcon(const char* a, const char* b) {
  char* c = new char[strlen(a) + strlen(b) + 1];
  strcpy(c, a);
  strcpy(&c[strlen(c)], b);
  return c;
}
