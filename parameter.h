#ifndef parameter_h
#define parameter_h

#include "commentstream.h"
#include "gadget.h"

/**
 * \class Parameter
 * \brief This is the class used to store the names of the variables used in the model simulation
 */
class Parameter {
public:
  /**
   * \brief This is the default Parameter constructor
   */
  Parameter() { name = NULL; };
  /**
   * \brief This is the Parameter constructor that creates a copy of an existing Parameter
   * \param p is the Parameter to copy
   */
  Parameter(const Parameter& p);
  /**
   * \brief This is the Parameter constructor for a given string value
   * \param value is the string value that will be set to the name of the parameter
   */
  Parameter(char* value);
  /**
   * \brief This is the default Parameter destructor
   */
  ~Parameter();
  /**
   * \brief This function will return the name of the parameter
   * \return name
   */
  char* getName() const { return name; };
  friend CommentStream& operator >> (CommentStream& in, Parameter& p);
  friend istream& operator >> (istream& in, Parameter& p);
  Parameter& operator = (const Parameter& p);
  int operator == (const Parameter& p) const;
private:
  /**
   * \brief This function will check to see if the string that is going to be the name of the parameter is valid or not
   * \return 1 if the name is valid, 0 otherwise
   */
  int isValidName(char* value);
  /**
   * \brief This function will check to see if a character from the name of the parameter is valid or not
   * \return 1 if the character is valid, 0 otherwise
   * \note valid characters are alpha-numeric characters, '_' and '.'
   */
  int isValidChar(int c);
  /**
   * \brief This is the name of the parameter
   */
  char* name;
};

#endif
