#ifndef interrupthandler_h
#define interrupthandler_h

/**
 * \brief This is the function that registers the interrupt handler
 * \param signal is the signal to handle
 * \param interrupted is a pointer to a flag that will be set to 1 when an interrupt occurs
 * \return 0 if successful, else -1 (see man page for sigaction())
 */
int registerInterrupt(int signal, volatile int* interrupted);
/**
 * \brief This is the function that restores the original handle after an interrupt
 * \param signal is the signal to handle
 * \return 0 if successful, else -1 (see man page for sigaction())
 */
int restoreInterrupt(int signal);
/**
 * \brief This is the function that gets called when an interrupt occurs
 * \param signal is the signal to handle
 */
void interruptHandler(int signal);

#endif
