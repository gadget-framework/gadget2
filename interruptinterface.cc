#include "interruptinterface.h"
#include "charptrvector.h"
#include "stockptrvector.h"
#include "formatedprinting.h"
#include "stockpredator.h"
#include "lenstock.h"
#include "areatime.h"
#include "gadget.h"

//max length of user input
const int InterruptInterface::MAX_INPUT_LENGTH = 100;

//file names for output
const char* InterruptInterface::FULL_FILE = "fulldump.ir";
const char* InterruptInterface::STOCK_N_FILE = "stockN.ir";
const char* InterruptInterface::STOCK_W_FILE = "stockW.ir";
const char* InterruptInterface::SUITABLE_FILE = "suitable.ir";
const char* InterruptInterface::NATURALM_FILE = "naturalM.ir";
const char* InterruptInterface::C_HAT_FILE = "c_hat.ir";
const char* InterruptInterface::MEAN_N_FILE = "mean_n.ir";
const char* InterruptInterface::Z_FILE = "z.ir";
const char* InterruptInterface::CANNIBALISM_FILE = "cannibalism.ir";

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
    << " of " << eco->TimeInfo->TotalNoSteps() << ")\n"
    << " q   ->  quit simulation\n"
    << " c   ->  continue simulation\n"
    << " h/? ->  display this menu\n"
    << " d   ->  dump Age/Length matrix (number&weight) for Stock\n"
    << " s   ->  dump Suitability matrix from Predator\n"
    << " m   ->  dump Natural M vector from Stock\n"
    << " v   ->  dump c_hat from MortPredLength\n"
    << " n   ->  dump mean_n from MortPrey\n"
    << " z   ->  dump z from MortPrey\n"
    << " a   ->  dump cannibalism from MortPrey\n"
    << " f   ->  full dump to file\n";
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
  char s[MAX_INPUT_LENGTH + 1];
  s[0] = 0;
  while (s[0] != 'c') {
    cout << "> ";
    cout.flush();
    while (fgets(s, MAX_INPUT_LENGTH, stdin) == 0);
      switch(s[0]) {
        case 'q':
          return 0;
        case 'd':
          dumpStock(readArgs(s));
          break;
        case 'f':
          dumpAll(readArgs(s));
          break;
        case 's':
          dumpSuitability(readArgs(s));
          break;
        case 'm':
          dumpNaturalM(readArgs(s));
          break;
        case 'v':
          dumpc_hat(readArgs(s));
          break;
        case 'n':
          dumpmean_n(readArgs(s));
          break;
        case 'z':
          dumpz(readArgs(s));
          break;
        case 'a':
          dumpcannibalism(readArgs(s));
          break;
        case 'h':
        case '?':
          printMenu();
          break;
      }
  }
  cout << "continuing...\n";
  cout.flush();
  return 1;
}

/*  charptrvector& readArgs(char* s)
 *
 *  Purpose: Parse a char array into a vector of words.
 *           words are seperated by whitespace in s.
 *  In: s:   char array to be parsed
 *  Out: charptrvector&:    vector of words found in s
 *  Usage: data_t* d = getDataAtTime(timestep, name)
 *  Pre: s != NULL
 */
charptrvector& InterruptInterface::readArgs(char* s) {
  assert(s != NULL);
  charptrvector* args = new charptrvector;
  char buf[MAX_INPUT_LENGTH + 1];
  char* b = buf;
  while (*s != 0) {
    while ((*s == ' ') || (*s == '\t') || (*s == '\n'))
      s++;
    b = buf;
    while ((*s != ' ') && (*s != '\t') && (*s != 0) && (*s != '\n'))
      *b++ = *s++;
    *b = 0;
    if (*buf != 0)
      args->resize(1, strdup(buf));
  }
  return *args;
}

/*  void dumpNaturalM(charptrvector& args)
 *
 *  Purpose: Dump NaturalM from Stock to file.
 *  In: args:  not used
 *  Usage:  dumpNaturalM(args)
 */
void InterruptInterface::dumpNaturalM(charptrvector& args) {
  ofstream out(NATURALM_FILE);
  int i;
  for (i = 0; i < eco->stockvec.Size(); i++) {
    if (eco->stockvec[i]->stockType() != LENSTOCK_TYPE)
      printNaturalM(out, *eco->stockvec[i]);
    else
      printNaturalM(out, *(const LenStock*)(eco->stockvec[i]));
    out.flush();
  }
}

/*  void dumpSuitability(charptrvector& args)
 *
 *  Purpose: Dump Suitability from Predator in Stock to file.
 *  In: args:  not used
 *  Usage:  dumpSuitability(args)
 */
void InterruptInterface::dumpSuitability(charptrvector& args) {
  ofstream out(SUITABLE_FILE);
  int i;
  for (i = 0; i < eco->stockvec.Size(); i++)
    if (eco->stockvec[i]->DoesEat()) {
      printSuitability(out, *(Predator*)eco->stockvec[i]->ReturnPredator());
      out.flush();
    }
}

/*  void dumpc_hat(charptrvector& args)
 *
 *  Purpose: Dump C_Hat data to file.
 *  In: args:  not used
 *  Usage:  dumpc_hat(args)
 */
void InterruptInterface::dumpc_hat(charptrvector& args) {
  ofstream out(C_HAT_FILE);
  int i;
  for (i = 0; i < eco->stockvec.Size(); i++)
    if (eco->stockvec[i]->DoesEat()) {
      printc_hat(out, *(MortPredLength*)eco->stockvec[i]->ReturnPredator(), *eco->Area);
      out.flush();
    }
}

/*  void dumpmean_n(charptrvector& args)
 *
 *  Purpose: Dump Mean_N data to file.
 *  In: args:  not used
 *  Usage:  dumpmean_n(args)
 */
void InterruptInterface::dumpmean_n(charptrvector& args) {
  ofstream out(MEAN_N_FILE);
  int i;
  for (i = 0; i < eco->stockvec.Size(); i++)
    if (eco->stockvec[i]->IsEaten()) {
      printmean_n(out, *(MortPrey*)eco->stockvec[i]->ReturnPrey(), *eco->Area);
      out.flush();
    }
}

/*  void dumpz(charptrvector& args)
 *
 *  Purpose: Dump z data to file.
 *  In: args:  not used
 *  Usage:  dumpz(args)
 */
void InterruptInterface::dumpz(charptrvector& args) {
  ofstream out(Z_FILE);
  int i;
  for (i = 0; i < eco->stockvec.Size(); i++)
    if (eco->stockvec[i]->IsEaten()) {
      printz(out, *(MortPrey*)eco->stockvec[i]->ReturnPrey(), *eco->Area);
      out.flush();
    }
}

/*  void dumpcannibalism(charptrvector& args)
 *
 *  Purpose: Dump Cannibalism data to file.
 *  In: args:  not used
 *  Usage:  dumpcannibalism(args)
 */
void InterruptInterface::dumpcannibalism(charptrvector& args) {
  ofstream out(CANNIBALISM_FILE);
  int i;
  for (i = 0; i < eco->stockvec.Size(); i++)
    if (eco->stockvec[i]->IsEaten()) {
      printcannibalism(out, *(MortPrey*)eco->stockvec[i]->ReturnPrey(), *eco->Area);
      out.flush();
    }
}

/*  void dumpStock(charptrvector& args)
 *
 *  Purpose: Dump N & W stock data to file.
 *  In: args:  args[i], i>0 contains names of stocks to print from,
 *             or if args.Size() == 1 print from every stock.
 *  Usage:  dumpstock(args)
 */
void InterruptInterface::dumpStock(charptrvector& args) {
  ofstream outn(STOCK_N_FILE);
  ofstream outw(STOCK_W_FILE);
  Stockptrvector svec(0);
  int i, j;

  if (args.Size() == 1)
    for (i = 0; i < eco->stockvec.Size(); i++)
      svec.resize(1, (Stock*)eco->stockvec[i]);
  else
    for (i = 1; i < args.Size(); i++)
      for (j = 0; j < eco->stockvec.Size(); j++) {
        if (strcasecmp(args[i], eco->stockvec[j]->Name()) == 0)
           svec.resize(1, (Stock*)eco->stockvec[j]);
      }

  for (i = 0; i < svec.Size(); i++) {
    intmatrix areas, ages;
    intvector agevector;
    for (j = svec[i]->Minage(); j <= svec[i]->Maxage(); j++) {
      ages.AddRows(1, 1, j);
      agevector.resize(1, j);
    }
    for (j = 0; j < eco->Area->NoAreas(); j++)
      if (svec[i]->IsInArea(j))
        areas.AddRows(1, 1, j);

    printStockHeader(outn, svec[i]->Name(), areas, ages, *(svec[i]->ReturnLengthGroupDiv()));
    printStockHeader(outw, svec[i]->Name(), areas, ages, *(svec[i]->ReturnLengthGroupDiv()));
    printTime(outn, *(eco->TimeInfo));
    printTime(outw, *(eco->TimeInfo));
    for (j = 0; j < areas.Nrow(); j++) {
      printAreasHeader(outn, areas[j]);
      printAreasHeader(outw, areas[j]);
      printMatrixHeader(outn, agevector, *(svec[i]->ReturnLengthGroupDiv()), "number", 1);
      printMatrixHeader(outw, agevector, *(svec[i]->ReturnLengthGroupDiv()), "weight", 1);
      printN(outn, svec[i]->Agelengthkeys(areas[j][0]), 0 /*&agevector*/);
      printW(outw, svec[i]->Agelengthkeys(areas[j][0]), 0 /*&agevector*/);
    }
    outn.flush();
    outw.flush();
  }
}

/*  void dumpAll(charptrvector& args)
 *
 *  Purpose: Dump all ecosystem data to file.
 *  In: args:  not used
 *  Usage:  dumpall(args)
 */
void InterruptInterface::dumpAll(charptrvector& args) {
  eco->PrintStatus(FULL_FILE);
}
