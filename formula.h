#ifndef formula_h
#define formula_h

#include "keeper.h"
#include "doublevector.h"
#include "parameter.h"
#include "gadget.h"

class Keeper;

class Formula {
public:
  Formula() : init(0), multipliers(0), attributes(0) {};
  //If inform keeper with Formula initiated with this nothing will be done as no switches set ??? OK ????
  Formula(const Formula& form);
  void setValue(double initValue);
  int isMultipleValue();
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
  void Interchange(Formula& NewF, Keeper* keeper);
  //Nothing done if no attribute value or multiply value set.
  void Inform(Keeper* keeper);
  void Delete(Keeper* keeper) const;
private:
  double init;
  Parameter inattr;
  doublevector multipliers;
  Parametervector attributes;
};

#endif

