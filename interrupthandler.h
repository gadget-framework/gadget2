#ifndef interrupthandler_h
#define interrupthandler_h

/**
 * \brief This is the function that registers the interrupt handler
 * \param interrupted is a pointer to a flag that will be set to 1 when an interrupt occurs
 */
void registerInterrupts(volatile int* interrupted);
/**
 * \brief This is the function that gets called when an interrupt occurs
 * \param signal is the signal to handle
 */
void interruptHandler(int signal);

#endif
