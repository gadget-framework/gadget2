#ifndef readword_h
#define readword_h

#include "commentstream.h"

//functions to read a variable name and then the numeric value
void ReadWordAndVariable(CommentStream& infile, const char* str, int& number);
void ReadWordAndVariable(CommentStream& infile, const char* str, double& number);

//JMB - added function to read a string value as well
void ReadWordAndValue(CommentStream& infile, const char* str, char* value);

//JMB - also added functions to read 2 numbers
void ReadWordAndTwoVariables(CommentStream& infile, const char* str, int& number1, int& number2);
void ReadWordAndTwoVariables(CommentStream& infile, const char* str, double& number1, double& number2);

#endif
