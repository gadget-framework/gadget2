#ifndef formula_h
#define formula_h

#include "keeper.h"
#include "doublevector.h"
#include "parameter.h"
#include "parametervector.h"
#include "gadget.h"
#include <vector>

class Keeper;

enum FormulaType { CONSTANT, PARAMETER, FUNCTION };
enum FunctionType { NONE, MULT, DIV, PLUS, MINUS, PRINT };

class Formula {
public:
  Formula();
  Formula(const Formula& form);
  ~Formula();
  void setValue(double initValue);
  friend CommentStream& operator >> (CommentStream&, Formula&);
  /**
   * \brief This operator will return the value of the formula
   * \return the value of the formula
   */
  operator double() const;
  double evalFunction() const;
  void Interchange(Formula& NewF, Keeper* keeper) const;
  void Inform(Keeper* keeper);
  void Delete(Keeper* keeper) const;
private:
  FormulaType type;
  FunctionType functiontype;
  double value;
  Parameter name;
  vector<Formula*> argList;
};

#endif

