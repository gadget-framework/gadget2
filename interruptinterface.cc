#include "interruptinterface.h"
#include "charptrvector.h"
#include "gadget.h"

InterruptInterface::InterruptInterface(const Ecosystem& ecosystem) {
  assert(&ecosystem != NULL);
  eco = &ecosystem;
}

void InterruptInterface::printMenu() {
  cout << "\nInterrupted at year " << eco->TimeInfo->CurrentYear() << ", step "
    << eco->TimeInfo->CurrentStep() << " (" << eco->TimeInfo->CurrentTime()
    << " of " << eco->TimeInfo->TotalNoSteps() << " timesteps)\n"
    << " q   ->  quit simulation\n"
    << " c   ->  continue simulation\n"
    << " f   ->  dump current model to file\n";
  cout.flush();
}

int InterruptInterface::menu() {
  printMenu();
  char s[MaxStrLength];
  s[0] = 0;
  while ((s[0] != 'c') && (s[0] != 'C')) {
    cout << "> ";
    cout.flush();
    while (fgets(s, MaxStrLength, stdin) == 0);
    switch(s[0]) {
      case 'q':
      case 'Q':
        cout << "\nQuitting current simulation ...\nThe best parameter values will be written to file (called interrupt.out)\n";
        return 0;
      case 'f':
      case 'F':
        cout << "\nWriting current model to file (called modeldump.out) ...\n";
        dumpAll();
        break;
      case 'h':
      case 'H':
      case '?':
        printMenu();
        break;
    }
  }
  cout << "\nContinuing current simulation ...\n";
  cout.flush();
  return 1;
}

void InterruptInterface::dumpAll() {
  char interruptfile[15];
  strncpy(interruptfile, "", 15);
  strcpy(interruptfile, "modeldump.out");
  eco->writeStatus(interruptfile);
}
