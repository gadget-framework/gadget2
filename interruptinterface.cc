#include "interruptinterface.h"
#include "charptrvector.h"
#include "gadget.h"

/*  InterruptInterface(const Ecosystem& const ecosystem)
 *
 *  Purpose: Constructor
 *  In: ecosystem: ecosystem object to print from.
 *  Usage: InterruptInterface interface(ecosystem)
 *  Pre: &ecosystem != NULL
 */
InterruptInterface::InterruptInterface(const Ecosystem& ecosystem) {
  assert(&ecosystem != NULL);
  eco = &ecosystem;
}

/*  printMenu()
 *
 *  Purpose:    Print menu to standard output.
 */
void InterruptInterface::printMenu() {
  cout << "\nInterrupted at year " << eco->TimeInfo->CurrentYear() << ", step "
    << eco->TimeInfo->CurrentStep() << " (" << eco->TimeInfo->CurrentTime()
    << " of " << eco->TimeInfo->TotalNoSteps() << " timesteps)\n"
    << " q   ->  quit simulation\n"
    << " c   ->  continue simulation\n"
    << " f   ->  dump current model to file\n";
  cout.flush();
}

/*  int menu()
 *
 *  Purpose: Interact with user.
 *  Out: int: 1 if simulation should continue, 0 if simulation should end.
 *  Usage: if (menu() == 0) exit(0)
 *  Pre: none
 */
int InterruptInterface::menu() {
  printMenu();
  char s[MaxStrLength];
  s[0] = 0;
  while (s[0] != 'c') {
    cout << "> ";
    cout.flush();
    while (fgets(s, MaxStrLength, stdin) == 0);
      switch(s[0]) {
        case 'q':
          cout << "\nQuitting current simulation ...\ncurrent parameter values have been written to file (called interrupt.out)\n";
          return 0;
        case 'f':
          cout << "\nWriting current model to file (called modeldump.out) ...\n";
          dumpAll();
          break;
        case 'h':
          printMenu();
          break;
      }
  }
  cout << "\nContinuing current simulation ...\n";
  cout.flush();
  return 1;
}


/*  void dumpAll()
 *
 *  Purpose: Dump all ecosystem data to file.
 *  Usage:  dumpall()
 */
void InterruptInterface::dumpAll() {
  char interruptfile[15];
  strncpy(interruptfile, "", 15);
  strcpy(interruptfile, "modeldump.out");
  eco->PrintStatus(interruptfile);
}
