#ifndef migvariables_h
#define migvariables_h

#include "formulavector.h"
#include "commentstream.h"
#include "keeper.h"

class MigVariable {
public:
 MigVariable(CommentStream& infile, int firstyear, int lastyear, Keeper* const keeper);
 MigVariable(Formula* const number, int firstyear, int lastyear, Keeper* const keeper);
 double ValueOfVariable(int year);
 ~MigVariable();
private:
 IntVector years;
 FormulaVector values;
 DoubleVector temperature;
 FormulaVector coeff;
 Formula value;
 int ValuesReadFromFile;
};

#endif
