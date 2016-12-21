#ifndef readword_h
#define readword_h

#include "commentstream.h"
#include "modelvariable.h"
#include "formula.h"

/**
 * \brief This is the function that will read a text string from file, compare this text to a label and if it matches what is expected will then read and store a number
 * \param infile is the CommentStream to read from
 * \param str is the char* that the text label will be compared to
 * \param number is the integer that will store the value that has been read
 */
void readWordAndVariable(CommentStream& infile, const char* str, int& number);
/**
 * \brief This is the function that will read a text string from file, compare this text to a label and if it matches what is expected will then read and store a number
 * \param infile is the CommentStream to read from
 * \param str is the char* that the text label will be compared to
 * \param number is the double that will store the value that has been read
 */
void readWordAndVariable(CommentStream& infile, const char* str, double& number);
/**
 * \brief This is the function that will read a text string from file, compare this text to a label and if it matches what is expected will then read and store a Formula
 * \param infile is the CommentStream to read from
 * \param str is the char* that the text label will be compared to
 * \param formula is the Formula that will store the value that has been read
 */
void readWordAndVariable(CommentStream& infile, const char* str, Formula& formula);
/**
 * \brief This is the function that will read a text string from file, compare this text to a label and if it matches what is expected will then read and store a ModelVariable
 * \param infile is the CommentStream to read from
 * \param str is the char* that the text label will be compared to
 * \param modelvariable is the ModelVariable that will store the value that has been read
 * \param TimeInfo is the TimeClass for the current model
 * \param keeper is the Keeper for the current model
 */
void readWordAndModelVariable(CommentStream& infile, const char* str, ModelVariable& modelvariable,
  const TimeClass* const TimeInfo, Keeper* const keeper);
/**
 * \brief This is the function that will read a text string from file, compare this text to a label and if it matches what is expected will then read and store a second string
 * \param infile is the CommentStream to read from
 * \param str is the char* that the text label will be compared to
 * \param value is the char* that will store the value that has been read
 */
void readWordAndValue(CommentStream& infile, const char* str, char* value);
/**
 * \brief This is the function that will read a text string from file, compare this text to a label and if it matches what is expected will then read and store 2 number
 * \param infile is the CommentStream to read from
 * \param str is the char* that the text label will be compared to
 * \param number1 is the integer that will store the first value that has been read
 * \param number2 is the integer that will store the second value that has been read
 */
void readWordAndTwoVariables(CommentStream& infile, const char* str, int& number1, int& number2);
/**
 * \brief This is the function that will read a text string from file, compare this text to a label and if it matches what is expected will then read and store 2 number
 * \param infile is the CommentStream to read from
 * \param str is the char* that the text label will be compared to
 * \param number1 is the double that will store the first value that has been read
 * \param number2 is the double that will store the second value that has been read
 */
void readWordAndTwoVariables(CommentStream& infile, const char* str, double& number1, double& number2);

#endif
