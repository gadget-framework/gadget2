#ifndef parameter_h
#define parameter_h

#include "commentstream.h"
#include "gadget.h"

/*
 * Class for reading, comparing, printing, and assigning parameter names.
 * Basicaly nothing more than a simple string class, but it can be extended
 * if we need to change the parameter name syntax in the future.
 * Parameter names can only be read from file or copied from other Parameter
 * names.  Parameter names can be case sensitive if PARAMETERS_CASE_SENSITIVE
 * is defined.  The member function legalchar defines the valid characters
 * in parameter names (mna 29.09.00)
 *
 * (AJ jan. 2002) Adding new parameter constructor where parameter is set with
 * a given value but error occures if the given value is not a valid parameter.
 */

class Parameter {
public:
  Parameter();
  Parameter(const Parameter& p);
  ~Parameter();
  //reads and discards ' ', '\t', '\n' and EOF from start of pointer in ??????.
  //reads string until find a character which is not legal or have read MaxStrLength-1
  //characters into name. name has been terminated by '\0'.
  //If in fails while trying to read then returns in.makebad ??
  //do not know what this is else returns in.
  //If length of string read == MaxStrLength-1 then a warning message has been
  //printed to screen.
  friend CommentStream& operator >> (CommentStream& in, Parameter& p);
  friend istream& operator >> (istream& in, Parameter& p);
  friend ostream& operator << (ostream& out, const Parameter& p) {
    return (out << p.name);
  };
  Parameter& operator = (const Parameter& p);
  int operator == (const Parameter& p) const;
  int legalchar(int c);
  int Size() const;
  char* getValue();
  int isLegalParameter(char* param);
  Parameter(char* value);
private:
  char* name;
};

#endif
