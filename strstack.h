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
  StrStack();
  /**
   * \brief This is the default StrStack destructor
   * \note This will free all the memory allocated to the strings in the stack
   */
  ~StrStack();
  /**
   * \brief This will delete an entry from the vector
   * \param row is the element of the vector to be deleted
   */
  void OutOfStack();
  void PutInStack(const char*);
  void ClearStack();
  char* SendAll() const;
  char* SendTop() const;
  char* Send(int i) const;
private:
  /**
   * \brief This is size of the string stack
   */
  int sz;
  /**
   * \brief This is the CharPtrVector containing the string stack
   */
  CharPtrVector v;
};

#endif
