#include "transition.h"
#include "errorhandler.h"
#include "keeper.h"
#include "readword.h"
#include "gadget.h"

Transition::Transition(CommentStream& infile, const intvector& Areas, int Age,
  const LengthGroupDivision* const lgrpdiv, Keeper* const keeper)
  : LivesOnAreas(Areas), LgrpDiv(new LengthGroupDivision(*lgrpdiv)), age(Age) {

  int i;
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  keeper->AddString("transition");

  infile >> text;
  if (strcasecmp(text, "tranistionstocksandratios") == 0) {
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

  infile >> TransitionStep;

  //Now we are finished reading from infile and can construct some objects.
  //Construct the age-length key Agegroup and initialize to zero.
  /*intvector minlv(2, minl);
  intvector sizev(2, size);
  Agegroup.resize(areas.Size(), age, minlv, sizev);
  for (i = 0; i < Agegroup.Size(); i++)
    Agegroup[i].SettoZero();*/

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

void Transition::SetStock(Stockptrvector& stockvec) {
  int index = 0;
  int i, j;
  doublevector tmpratio;

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

  CI.resize(TransitionStocks.Size(), 0);
  for (i = 0; i < TransitionStocks.Size(); i++)
    CI[i] = new ConversionIndex(LgrpDiv, TransitionStocks[i]->ReturnLengthGroupDiv());

  //JMB - get minimum length of the transition stock
  minlen.resize(TransitionStocks.Size(), 0);
  for (i = 0; i < TransitionStocks.Size(); i++)
    minlen[i] = TransitionStocks[i]->ReturnLengthGroupDiv()->Minlength(0);

  //JMB - and get corresponding length group
  lenindex.resize(TransitionStocks.Size(), 0);
  for (i = 0; i < TransitionStocks.Size(); i++)
    lenindex[i] = LgrpDiv->NoLengthGroup(minlen[i]);

  for (i = 0; i < TransitionStocks.Size(); i++) {
    index = 0;
    for (j = 0; j < areas.Size(); j++)
      if (!TransitionStocks[i]->IsInArea(areas[j]))
        index++;

    if (index != 0)
      cerr << "Warning: transition requested to stock " << (const char*)TransitionStocks[i]->Name()
        << "\nwhich might not be defined on " << index << " areas\n";
  }
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

void Transition::KeepAgegroup(int area, Agebandmatrix& Alkeys, const TimeClass* const TimeInfo) {
/*  if (!TransitionStock->IsInArea(area))
    return;
  int inarea = AreaNr[area];
  int length, minl, maxl;

  if (TimeInfo->CurrentStep() == TransitionStep) {
    Agegroup[inarea].SettoZero();
    assert(Alkeys.Minage() <= age && age <= Alkeys.Maxage());
    minl = Agegroup[inarea].Minlength(age);
    maxl = Agegroup[inarea].Maxlength(age);
    for (length = minl; length < maxl; length++) {
      Agegroup[inarea][age][length].N = Alkeys[age][length].N;
      Agegroup[inarea][age][length].W = Alkeys[age][length].W;
      if (length >= lenindex) {
        Alkeys[age][length].N = 0.0;
        Alkeys[age][length].W = 0.0;  //JMB
      }
    }
  }*/
}

//area in the call to this routine is not in the local area numbering of the stock.
void Transition::MoveAgegroupToTransitionStock(int area, const TimeClass* const TimeInfo, int HasLgr) {
/*  if (!TransitionStock->IsInArea(area) || TimeInfo->CurrentStep() != TransitionStep)
    return;
  int inarea = AreaNr[area];

  if (!HasLgr) {
    //JMB - this is never called ...
    double NumberInArea = Agegroup[inarea][age][Agegroup[inarea].Minlength(age)].N;
    TransitionStock->Renewal(area, TimeInfo, NumberInArea);

  } else {
    if (TransitionStock->Birthday(TimeInfo))
      Agegroup[inarea].IncrementAge();

    TransitionStock->Add(Agegroup[inarea], CI, area, 1, Agegroup[inarea].Minage(), Agegroup[inarea].Maxage());
  }
  Agegroup[inarea].SettoZero();*/
}
