#ifndef interrupthandler_h
#define interrupthandler_h

/* Methods for handeling interrupts.
 *
 * Call registerInterrupt() to install the handler, when there is an interrupt
 * the function interruptHandler() will be called. Call restoreInterrupt()
 * to restore the original handler.
 * See interrupthandler.cc for more information.
 */

int registerInterrupt(int signal, volatile int* interrupted);
int restoreInterrupt(int signal);
void interruptHandler(int signal);

#endif
