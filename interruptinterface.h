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
  int menu();
private:
  void printMenu();
  charptrvector& readArgs(char*);
  void dumpStock(charptrvector& args);
  void dumpAll(charptrvector& args);
  void dumpSuitability(charptrvector& args);
  void dumpNaturalM(charptrvector& args);
  void dumpc_hat(charptrvector& args);
  void dumpmean_n(charptrvector& args);
  void dumpz(charptrvector& args);
  void dumpcannibalism(charptrvector& args);
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
