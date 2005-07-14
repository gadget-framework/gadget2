#ifndef formula_h
#define formula_h

#include "keeper.h"
#include "doublevector.h"
#include "parameter.h"
#include "gadget.h"

class Keeper;

enum FormulaType { CONSTANT = 1, PARAMETER, FUNCTION };
enum FunctionType { NONE = 1, MULT, DIV, PLUS, MINUS, SIN, COS, LOG, EXP, SQRT };

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
   * \brief This is the Formula constructor for a given function type and list of paramters
   * \param ft is the FunctionType for the new Formula function
   * \param formlist is the vector of arguments for the new Formula function
   */
  Formula(FunctionType ft, vector<Formula*> formlist);
  /**
   * \brief This is the default Formula destructor
   */
  ~Formula();
  /**
   * \brief This function will set the value of the Formula
   * \param init is the value of the Formula
   */
  void setValue(double init);
  /**
   * \brief This operator will return the value of the Formula
   * \return the value of the Formula
   */
  operator double() const;
  /**
   * \brief This function will calculate the value of the function
   * \return function value
   */
  double evalFunction() const;
  /**
   * \brief This function will swap the Formula value for a new value
   * \param newF is the new Formula value
   * \param keeper is the Keeper for the current model
   */
  void Interchange(Formula& newF, Keeper* keeper) const;
  /**
   * \brief This function will inform the Keeper for the current model of the new Formula
   * \param keeper is the Keeper for the current model
   */
  void Inform(Keeper* keeper);
  /**
   * \brief This function will delete a TimeVariable value
   * \param keeper is the Keeper for the current model
   */
  void Delete(Keeper* keeper) const;
  Formula& operator = (const Formula& F);
  friend CommentStream& operator >> (CommentStream&, Formula&);
private:
  /**
   * \brief This denotes what type of Formula class has been created
   */
  FormulaType type;
  /**
   * \brief This denotes what type of function has been used for the Formula
   */
  FunctionType functiontype;
  /**
   * \brief This is the value of the Formula
   */
  double value;
  /**
   * \brief This Parameter is the name of the Formula
   */
  Parameter name;
  /**
   * \brief This is the vector of arguments for the Formula function
   */
  vector<Formula*> argList;
};

#endif
