#include "interruptinterface.h"
#include "charptrvector.h"
#include "ecosystem.h"
#include "gadget.h"

extern Ecosystem* EcoSystem;

void InterruptInterface::printMenu() {
  cout << "\nInterrupted at year " << EcoSystem->getCurrentYear() << ", step "
    << EcoSystem->getCurrentStep() << " (" << EcoSystem->getCurrentTime()
    << " of " << EcoSystem->numTotalSteps() << " timesteps)\n"
    << " q   ->  quit simulation\n"
    << " c   ->  continue simulation\n"
    << " p   ->  write current parameters to file\n"
    << " f   ->  dump current model to file\n";
  cout.flush();
}

int InterruptInterface::menu() {
  printMenu();
  char s[MaxStrLength];
  strncpy(s, "", MaxStrLength);
  s[0] = 0;
  while ((s[0] != 'c') && (s[0] != 'C')) {
    cout << "> ";
    cout.flush();
    while (fgets(s, MaxStrLength, stdin) == 0);
    switch (s[0]) {
      case 'q':
      case 'Q':
        cout << "\nQuitting current simulation ...\nThe best parameter values will be written to file (called interrupt.out)\n";
        cout.flush();
        return 0;
      case 'f':
      case 'F':
        cout << "\nWriting current model to file (called modeldump.out) ...\n";
        dumpAll();
        break;
      case 'p':
      case 'P':
        cout << "\nWriting current parameters to file (called current.out) ...\n";
        dumpParams();
        break;
      case 'h':
      case 'H':
      case '?':
        printMenu();
        break;
      default:
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
  EcoSystem->writeStatus(interruptfile);
}

void InterruptInterface::dumpParams() {
  char interruptfile[15];
  strncpy(interruptfile, "", 15);
  strcpy(interruptfile, "current.out");
  EcoSystem->writeParams(interruptfile, 0);
}
