#ifndef parameter_h
#define parameter_h

#include "commentstream.h"
#include "gadget.h"

/**
 * \class Parameter
 * \brief This is the class used to store the name of a variable used in the model simulation
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
   * \param value is the string value that will be set to the name of the Parameter
   */
  Parameter(char* value);
  /**
   * \brief This is the default Parameter destructor
   */
  ~Parameter();
  /**
   * \brief This function will return the name of the Parameter
   * \return name
   */
  char* getName() const { return name; };
  /**
   * \brief This operator will read data from a CommentStream into a Parameter
   * \param infile is the CommentStream to read from
   * \param p is the Parameter used to store the data
   */
  friend CommentStream& operator >> (CommentStream& infile, Parameter& p);
  /**
   * \brief This operator will read data from a istream into a Parameter
   * \param infile is the istream to read from
   * \param p is the Parameter used to store the data
   */
  friend istream& operator >> (istream& infile, Parameter& p);
  /**
   * \brief This operator will set the Parameter equal to an existing Parameter
   * \param p is the Parameter to copy
   */
  Parameter& operator = (const Parameter& p);
  /**
   * \brief This operator will check to see if the Parameter is equal to an existing Parameter
   * \param p is the Parameter to check
   * \return 1 if the two Parameters are equal, 0 otherwise
   */
  int operator == (const Parameter& p) const;
  /**
   * \brief This function will check to see if the string that is going to be the name of the Parameter is valid or not
   * \return 1 if the name is valid, 0 otherwise
   */
  int isValidName(char* value);
private:
  /**
   * \brief This function will check to see if a character from the name of the Parameter is valid or not
   * \return 1 if the character is valid, 0 otherwise
   * \note Valid characters are alpha-numeric characters, '_' and '.'
   */
  int isValidChar(int c);
  /**
   * \brief This is the name of the Parameter
   */
  char* name;
};

#endif
