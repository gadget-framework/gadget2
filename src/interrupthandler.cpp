#ifdef INTERRUPT_HANDLER

#include "interrupthandler.h"
#include "gadget.h"

volatile int* irhInterrupted;

void registerInterrupts(volatile int* interrupted) {
  irhInterrupted = interrupted;
#ifdef NOT_WINDOWS
  struct sigaction act;
  sigemptyset(&act.sa_mask);
  sigaddset(&act.sa_mask, SIGINT);
  sigaddset(&act.sa_mask, SIGTSTP);
  act.sa_handler = interruptHandler;
  act.sa_flags = 0;
  sigaction(SIGINT, &act, 0);
  sigaction(SIGTSTP, &act, 0);
#endif
}

void interruptHandler(int signal) {
  *irhInterrupted = 1;
}

#endif
