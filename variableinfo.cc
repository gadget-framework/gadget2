#include "migration.h"
#include "variableinfo.h"
#include "errorhandler.h"
#include "gadget.h"

//File format is:
//NoMatrix row column c1 x1 ... cm xm (All on the same line).
CommentStream& operator >> (CommentStream& infile, VariableInfo& varinfo) {

  ErrorHandler handle;
  while (varinfo.indices.Size())
    varinfo.indices.Delete(0);
  while (varinfo.coefficients.Size())
    varinfo.coefficients.Delete(0);
  infile >> varinfo.NoMatrix >> varinfo.row >> varinfo.column;

  int i, j;
  char line[MaxStrLength];
  strncpy(line, "", MaxStrLength);
  infile.getline(line, MaxStrLength);

  istringstream istr(line);
  CommentStream comm(istr);

  FormulaVector tmp(10);
  i = 0;
  while (!comm.eof() && comm.good()) {
    comm >> ws;
    varinfo.indices.resize(1);
    comm >> varinfo.indices[i];
    if (varinfo.indices[i] < 0)
      varinfo.error = 1;

    if (!(comm >> tmp[i]))
      handle.Message("Incorrect format while reading additions to the migration matrices");
    tmp[i].Inform(varinfo.keeper);
    i++;
    comm >> ws;
    if (i == tmp.Size()) {
      FormulaVector change(tmp);
      for (j = 0; j < change.Size(); j++)
        tmp[j].Interchange(change[j], varinfo.keeper);

      tmp.resize(tmp.Size(), varinfo.keeper);
      for (j = 0; j < change.Size(); j++)
        change[j].Interchange(tmp[j], varinfo.keeper);

    }
  }
  varinfo.coefficients.resize(varinfo.indices.Size());
  for (j = 0; j < varinfo.coefficients.Size(); j++) {
    varinfo.coefficients[j] = tmp[j];
    varinfo.keeper->ChangeVariable(tmp[j], varinfo.coefficients[j]);
  }

  if (infile.fail() || varinfo.NoMatrix < 0 || varinfo.row < 0 || varinfo.column < 0
     || varinfo.row > varinfo.NoAreas || varinfo.column > varinfo.NoAreas)
    varinfo.error = 1;
  else
    varinfo.error = 0;

  return infile;
}

ostream& operator << (ostream& out, VariableInfo& var) {
  int i;
  out << "Matrix number " << var.NoMatrix << ", row " << var.row
    << ", column " << var.column << "\nvariable\tcoefficient\n";
  for (i = 0; i < var.indices.Size(); i++)
    out << TAB << var.indices[i] << TAB << var.coefficients[i] << endl;
  out << endl;
  return out;
}
