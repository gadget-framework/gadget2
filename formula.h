#ifndef formula_h
#define formula_h

#include "keeper.h"
#include "doublevector.h"
#include "parameter.h"
#include "parametervector.h"
#include "gadget.h"

class Keeper;

class Formula {
public:
  Formula();
  Formula(const Formula& form);
  ~Formula() {};
  void setValue(double initValue);
  friend CommentStream& operator >> (CommentStream&, Formula&);
  operator double() const {
    double value = init;
    int i;
    for (i = 0; i < multipliers.Size(); i++)
      value *= multipliers[i];
    return value;
  };
  //Updated keeper with new address of value in NewF instead
  //of addresses used in this if any attributes have been set in this
  //If there are no values in NewF are equal to values in this...
  void Interchange(Formula& NewF, Keeper* keeper) const;
  void Inform(Keeper* keeper);
  void Delete(Keeper* keeper) const;
private:
  double init;
  Parameter inattr;
  DoubleVector multipliers;
  ParameterVector attributes;
};

#endif

