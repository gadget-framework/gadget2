#ifndef interruptinterface_h
#define interruptinterface_h

#include "ecosystem.h"

/* InterruptInterface
 *
 * Purpose:  Comunication with user after interrupt.
 *
 * Usage: The reference to Ecosystem is needed for printing of data
 *   from Ecosystem. This class is a friend class of Ecosystem, for
 *   easy access to data in Ecosystem. Call the method printMenu()
 *   to start interaction with the user. This method will return 0
 *   if the user ends the session with 'q' (quit) and 1 if the
 *   user pressed 'c' (continue).
 *   see interruptinterface.cc for further information.
 */
class InterruptInterface {
public:
  InterruptInterface(const Ecosystem& ecosystem);
  ~InterruptInterface() {};
  int menu();
private:
  void printMenu();
  void dumpAll();
  const Ecosystem* eco;
};

#endif
