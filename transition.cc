#include "transition.h"
#include "errorhandler.h"
#include "keeper.h"
#include "readword.h"
#include "gadget.h"

Transition::Transition(CommentStream& infile, const IntVector& Areas, int Age,
  const LengthGroupDivision* const lgrpdiv, Keeper* const keeper)
  : LivesOnAreas(Areas), LgrpDiv(new LengthGroupDivision(*lgrpdiv)), age(Age) {

  int i;
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  keeper->AddString("transition");

  infile >> text;
  if (strcasecmp(text, "transitionstocksandratios") == 0) {
    infile >> text;
    i = 0;
    while (strcasecmp(text, "transitionstep") != 0 && infile.good()) {
      TransitionStockNames.resize(1);
      TransitionStockNames[i] = new char[strlen(text) + 1];
      strcpy(TransitionStockNames[i], text);
      Ratio.resize(1);
      infile >> Ratio[i];
      i++;
      infile >> text;
    }
  } else
    handle.Unexpected("transitionstocksandratios", text);

  infile >> TransitionStep >> ws;
  keeper->ClearLast();
}

Transition::~Transition() {
  int i;
  for (i = 0; i < TransitionStockNames.Size(); i++)
    delete[] TransitionStockNames[i];
  for (i = 0; i < CI.Size(); i++)
    delete CI[i];
  delete LgrpDiv;
}

void Transition::SetStock(StockPtrVector& stockvec) {
  int index = 0;
  int i, j, numstocks;
  DoubleVector tmpratio;

  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < TransitionStockNames.Size(); j++)
      if (strcasecmp(stockvec[i]->Name(), TransitionStockNames[j]) == 0) {
        TransitionStocks.resize(1);
        tmpratio.resize(1);
        TransitionStocks[index] = stockvec[i];
        tmpratio[index] = Ratio[j];
        index++;
      }

  if (index != TransitionStockNames.Size()) {
    cerr << "Error: Did not find the stock(s) matching:\n";
    for (i = 0; i < TransitionStockNames.Size(); i++)
      cerr << (const char*)TransitionStockNames[i] << sep;
    cerr << "\nwhen searching for transition stock(s) - found only:\n";
    for (i = 0; i < stockvec.Size(); i++)
      cerr << stockvec[i]->Name() << sep;
    cerr << endl;
    exit(EXIT_FAILURE);
  }

  for (i = Ratio.Size(); i > 0; i--)
    Ratio.Delete(0);
  Ratio.resize(tmpratio.Size());
  for (i = 0; i < tmpratio.Size(); i++)
    Ratio[i] = tmpratio[i];

  numstocks = TransitionStocks.Size();
  CI.resize(numstocks, 0);
  for (i = 0; i < numstocks; i++)
    CI[i] = new ConversionIndex(LgrpDiv, TransitionStocks[i]->ReturnLengthGroupDiv());

  //JMB - check that the tranistion stocks are defined on the areas
  for (i = 0; i < numstocks; i++) {
    index = 0;
    for (j = 0; j < areas.Size(); j++)
      if (!TransitionStocks[i]->IsInArea(areas[j]))
        index++;

    if (index != 0)
      cerr << "Warning: transition requested to stock " << (const char*)TransitionStocks[i]->Name()
        << "\nwhich might not be defined on " << index << " areas\n";
  }

  IntVector minlv(2, 0);
  IntVector sizev(2, LgrpDiv->NoLengthGroups());
  Agegroup.resize(areas.Size(), age, minlv, sizev);
}

void Transition::Print(ofstream& outfile) const {
  int i;
  outfile << "\nTransition\n\tRead names of transition stocks:";
  for (i = 0; i < TransitionStockNames.Size(); i++)
    outfile << sep << (const char*)(TransitionStockNames[i]);
  outfile << "\n\tNames of transition stocks (through pointers):";
  for (i = 0; i < TransitionStocks.Size(); i++)
    outfile << sep << (const char*)(TransitionStocks[i]->Name());
  outfile << "\n\tTransition step " << TransitionStep  << endl;
}

void Transition::KeepAgegroup(int area, AgeBandMatrix& Alkeys, const TimeClass* const TimeInfo) {
  int inarea = AreaNr[area];
  int l, minl, maxl;

  if (TimeInfo->CurrentStep() == TransitionStep) {
    Agegroup[inarea].SettoZero();
    assert(Alkeys.Minage() <= age && age <= Alkeys.Maxage());
    minl = Agegroup[inarea].Minlength(age);
    maxl = Agegroup[inarea].Maxlength(age);
    for (l = minl; l < maxl; l++) {
      Agegroup[inarea][age][l].N = Alkeys[age][l].N;
      Agegroup[inarea][age][l].W = Alkeys[age][l].W;
      Alkeys[age][l].N = 0.0;
      Alkeys[age][l].W = 0.0;
    }
  }
}

//area in the call to this routine is not in the local area numbering of the stock.
void Transition::MoveAgegroupToTransitionStock(int area, const TimeClass* const TimeInfo, int HasLgr) {
  int s, inarea = AreaNr[area];

  if (TimeInfo->CurrentStep() == TransitionStep) {
    for (s = 0; s < TransitionStocks.Size(); s++) {
      if (!TransitionStocks[s]->IsInArea(area)) {
        cerr << "Error: Transition to stock " << (const char*)(TransitionStocks[s]->Name())
          << " cannot take place on area " << area << " since it doesnt live there!\n";
        exit(EXIT_FAILURE);
      }

      if (TransitionStocks[s]->Birthday(TimeInfo))
        Agegroup[inarea].IncrementAge();

      TransitionStocks[s]->Add(Agegroup[inarea], CI[s], area, Ratio[s],
        Agegroup[inarea].Minage(), Agegroup[inarea].Maxage());
    }
  }
}
