#ifdef INTERRUPT_HANDLER

#include "interrupthandler.h"
#include "gadget.h"

//Global variables
struct sigaction irhOldAct;
volatile int* irhInterrupted;

/* Purpose:          register interrupt handler
 * In:  signal:      signal to handle
 *      interrupted: pointer to int that will be set to 1 when an interrupt happens
 * Out: int:         0 on success, else -1 (see man page for sigaction())
 * Usage:            registerInterrupt(sig, &interrupted)
 * Pre:              interrupted != NULL
 */
int registerInterrupt(int signal, volatile int* interrupted) {
  assert(interrupted != NULL);
  irhInterrupted = interrupted;
  struct sigaction act;
  act.sa_handler = interruptHandler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  return sigaction(signal, &act, &irhOldAct);
}

/* Purpose:       restore interrupt handler
 * In:   signal:  signal to handle
 * Out:  int:     0 on success, else -1 (see man page for sigaction())
 * Usage:         restoreInterrupt(sig)
 */
int restoreInterrupt(int signal) {
  return sigaction(signal, &irhOldAct, 0);
}

/* Purpose:       handler that gets called when an interrupt occurs
 * In:   signal:  signal
 */
void interruptHandler(int signal) {
  *irhInterrupted = 1;
}

#endif
