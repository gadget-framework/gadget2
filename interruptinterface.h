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
  CharPtrVector& readArgs(char*);
  void dumpStock(CharPtrVector& args);
  void dumpAll(CharPtrVector& args);
  void dumpSuitability(CharPtrVector& args);
  void dumpNaturalM(CharPtrVector& args);
  void dumpc_hat(CharPtrVector& args);
  void dumpmean_n(CharPtrVector& args);
  void dumpz(CharPtrVector& args);
  void dumpcannibalism(CharPtrVector& args);
  const Ecosystem* eco;
  static const int MAX_INPUT_LENGTH;
  static const char* FULL_FILE;
  static const char* STOCK_N_FILE;
  static const char* STOCK_W_FILE;
  static const char* SUITABLE_FILE;
  static const char* NATURALM_FILE;
  static const char* C_HAT_FILE;
  static const char* MEAN_N_FILE;
  static const char* Z_FILE;
  static const char* CANNIBALISM_FILE;
};

#endif
