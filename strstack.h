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
  StrStack() { sz = 0; };
  /**
   * \brief This is the default StrStack destructor
   * \note This will free all the memory allocated to the strings in the stack
   */
  ~StrStack();
  void OutOfStack();
  void PutInStack(const char*);
  void ClearStack();
  char* SendAll() const;
  char* SendTop() const;
private:
  /**
   * \brief This is size of the string stack
   */
  int sz;
  /**
   * \brief This is the CharPtrVector containing the strings in the stack
   */
  CharPtrVector v;
};

#endif
