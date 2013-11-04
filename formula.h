#ifndef formula_h
#define formula_h

#include "keeper.h"
#include "parameter.h"
#include "gadget.h"

class Keeper;

enum FormulaType { CONSTANT = 1, PARAMETER, FUNCTION };
enum FunctionType { NONE = 1, MULT, DIV, PLUS, MINUS, SIN, COS, LOG, EXP,
  LOG10, POWER, SQRT, RAND, LESS, GREATER, EQUAL, AND, OR, NOT, ABS, IF, PI };

/**
 * \class Formula
 * \brief This is the class used to calculate the value of the variables used in the model simulation
 */
class Formula {
public:
  /**
   * \brief This is the default Formula constructor
   */
  Formula();
  /**
   * \brief This is the Formula constructor that creates a Formula with a given initial value
   * \param initial is the initial vlaue for the Formula
   */
  Formula(double initial);
  /**
   * \brief This is the Formula constructor that creates a copy of an existing Formula
   * \param initial is the Formula to copy
   */
  Formula(const Formula& initial);
  /**
   * \brief This is the Formula constructor for a given function type and list of parameters
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
   * \brief This function will delete a Formula value
   * \param keeper is the Keeper for the current model
   */
  void Delete(Keeper* keeper) const;
  /**
   * \brief This operator will set the Formula equal to an existing Formula
   * \param F is the Formula to copy
   */
  Formula& operator = (const Formula& F);
  /**
   * \brief This operator will read data from a CommentStream into a Formula
   * \param infile is the CommentStream to read from
   * \param F is the Formula used to store the data
   */
  friend CommentStream& operator >> (CommentStream& infile, Formula& F);
  /**
   * \brief This will return the type of Formula class
   * \return type
   */
  FormulaType getType() const { return type; };
private:
  /**
   * \brief This function will calculate the value of the function
   * \return the value of the function
   */
  double evalFunction() const;
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
