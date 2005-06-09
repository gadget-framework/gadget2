#ifndef formula_h
#define formula_h

#include "keeper.h"
#include "doublevector.h"
#include "parameter.h"
#include "gadget.h"

class Keeper;

enum FormulaType { CONSTANT, PARAMETER, FUNCTION };
enum FunctionType { NONE, MULT, DIV, PLUS, MINUS, PRINT, SIN, COS, LOG, EXP, SQRT };

class Formula {
public:
  /**
   * \brief This is the default Formula constructor
   */
  Formula();
  /**
   * \brief This is the Formula constructor that creates a copy of an existing Formula
   * \param initial is the Formula to copy
   */
  Formula(const Formula& initial);
  /**
   * \brief This is the default Formula destructor
   */
  ~Formula();
  void setValue(double init);
  friend CommentStream& operator >> (CommentStream&, Formula&);
  /**
   * \brief This operator will return the value of the formula
   * \return the value of the formula
   */
  operator double() const;
  double evalFunction() const;
  void Interchange(Formula& newF, Keeper* keeper) const;
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
