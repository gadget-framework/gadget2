#ifdef INTERRUPT_HANDLER

#include "interrupthandler.h"
#include "gadget.h"

//Global variables!
struct sigaction irhOldAct;
volatile int* irhInterrupted;

/*  int registerInterrupt(int signal, volatile int* interrupted)
 *
 *  Purpose:  Register interrupt handler.
 *  In:  signal:    signal to handle
 *   interrupted:   pointer to int that will be set to 1 when an interrupt happens.
 *  Out:  int:      0 on success, else -1 (see man page for sigaction())
 *  Usage:  registerInterrupt(sig, &interrupted)
 *  Pre:  interrupted != NULL
 */
int registerInterrupt(int signal, volatile int* interrupted) {
  assert(interrupted != NULL);
  irhInterrupted = interrupted;
  struct sigaction act;
  act.sa_handler = interruptHandler;
  return sigaction(signal, &act, &irhOldAct);
}

/*  int restoreInterrupt(int signal)
 *
 *  Purpose: Restore interrupt handler.
 *  In:  signal:    signal to handle
 *  Out:  int:      0 on success, else -1 (see man page for sigaction())
 *  Usage:  restoreInterrupt(sig)
 */
int restoreInterrupt(int signal) {
  return sigaction(signal, &irhOldAct, 0);
}

/*  void interruptHandler(int signal)
 *
 *  Purpose: This is the actual handler that gets called when an interrupt ocures.
 *  In:  signal:    signal
 */
void interruptHandler(int signal) {
  *irhInterrupted = 1;
}

#endif
