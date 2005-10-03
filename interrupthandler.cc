#ifdef INTERRUPT_HANDLER

#include "interrupthandler.h"
#include "gadget.h"

struct sigaction irhOldAct;
volatile int* irhInterrupted;

int registerInterrupt(int signal, volatile int* interrupted) {
  irhInterrupted = interrupted;
  struct sigaction act;
  act.sa_handler = interruptHandler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  return sigaction(signal, &act, &irhOldAct);
}

int restoreInterrupt(int signal) {
  return sigaction(signal, &irhOldAct, 0);
}

void interruptHandler(int signal) {
  *irhInterrupted = 1;
}

#endif
