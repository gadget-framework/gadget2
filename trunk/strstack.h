#ifndef strstack_h
#define strstack_h

#include "charptrvector.h"

/**
 * \class StrStack
 * \brief This class implements a simple stack of string objects
 */
class StrStack {
public:
  /**
   * \brief This is the default StrStack constructor
   */
  StrStack() { size = 0; };
  /**
   * \brief This is the default StrStack destructor
   */
  ~StrStack();
  /**
   * \brief This function will remove the top string from the stack
   */
  void clearString();
  /**
   * \brief This function will put a new string on to the top of the stack
   * \param str is the name of the string
   */
  void storeString(const char* str);
  /**
   * \brief This function will clear all the strings from the stack
   */
  void clearStack() { size = 0; };
  /**
   * \brief This function will send all the strings on the stack as one long new string
   * \return stack
   */
  char* sendAll() const;
  /**
   * \brief This function will send the top string from the stack as a new string
   * \return string
   */
  char* sendTop() const;
  /**
   * \brief This will return the number of strings currently stored on the stack
   * \return the number of strings on the stack
   */
  int getSize() { return size; };
private:
  /**
   * \brief This is size of the string stack
   */
  int size;
  /**
   * \brief This is the CharPtrVector containing the strings in the stack
   */
  CharPtrVector v;
};

#endif
