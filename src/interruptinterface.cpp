#include "interruptinterface.h"
#include "charptrvector.h"
#include "ecosystem.h"
#include "gadget.h"

extern Ecosystem* EcoSystem;

void InterruptInterface::printMenu() {
  if (EcoSystem->getFuncEval() != 0)
    Rcpp::Rcout << "\nInterrupted after " << EcoSystem->getFuncEval() << " iterations ...";
  Rcpp::Rcout << "\nInterrupted at year " << EcoSystem->getCurrentYear() << ", step "
    << EcoSystem->getCurrentStep() << " (" << EcoSystem->getCurrentTime()
    << " of " << EcoSystem->numTotalSteps() << " timesteps)\n"
    << " q   ->  quit simulation\n"
    << " c   ->  continue simulation\n"
    << " p   ->  write current parameters to file\n"
    << " f   ->  dump current model to file\n";
  Rcpp::Rcout.flush();
}

int InterruptInterface::menu() {

  char userinput[MaxStrLength];
  char interruptfile[MaxStrLength];
  strncpy(userinput, "", MaxStrLength);
  strncpy(interruptfile, "", MaxStrLength);

  this->printMenu();
  while (1) {
    Rcpp::Rcout << "> ";
    Rcpp::Rcout.flush();
    while (fgets(userinput, MaxStrLength, stdin) == 0) {};
    switch (userinput[0]) {
      case 'c':
      case 'C':
        Rcpp::Rcout << "\nContinuing current simulation ...\n";
        Rcpp::Rcout.flush();
        return 1;
      case 'q':
      case 'Q':
        Rcpp::Rcout << "\nQuitting current simulation ...\nThe best parameter values will be written to file (called interrupt.out)\n";
        Rcpp::Rcout.flush();
        return 0;
      case 'f':
      case 'F':
        Rcpp::Rcout << "\nWriting current model to file (called modeldump.out) ...\n";
        strcpy(interruptfile, "modeldump.out");
        EcoSystem->writeStatus(interruptfile);
        strncpy(interruptfile, "", MaxStrLength);  //JMB clear the text string
        break;
      case 'p':
      case 'P':
        Rcpp::Rcout << "\nWriting current parameters to file (called current.out) ...\n";
        strcpy(interruptfile, "current.out");
        EcoSystem->writeParams(interruptfile, 0);
        strncpy(interruptfile, "", MaxStrLength);  //JMB clear the text string
        break;
      case 'h':
      case 'H':
      case '?':
        this->printMenu();
        break;
      default:
        break;
    }
  }
  return 1;  //JMB default return value
}
