#include "transition.h"
#include "errorhandler.h"
#include "keeper.h"
#include "readword.h"
#include "gadget.h"

Transition::Transition(CommentStream& infile, const intvector& Areas,
  int Age, int minl, int size, Keeper* const keeper)
  : LivesOnAreas(Areas), TransitionStockName(0), TransitionStock(0), CI(0), age(Age) {

  int i;
  ErrorHandler handle;
  keeper->AddString("transitiondata");

  TransitionStockName = new char[MaxStrLength];
  strncpy(TransitionStockName, "", MaxStrLength);

  ReadWordAndVariable(infile, "transitionstep", TransitionStep);
  ReadWordAndValue(infile, "transitionstock", TransitionStockName);

  //Now we are finished reading from infile and can construct some objects.
  //Construct the age-length key Agegroup and initialize to zero.
  intvector minlv(2, minl);
  intvector sizev(2, size);
  Agegroup.resize(areas.Size(), age, minlv, sizev);
  for (i = 0; i < Agegroup.Size(); i++)
    Agegroup[i].SettoZero();

  minlen = 0.0;
  lenindex = 0;
  keeper->ClearLast();
}

Transition::~Transition() {
  delete[] TransitionStockName;
  delete CI;
}

void Transition::SetStock(Stockptrvector& stockvec) {
  int i, found;

  found = 0;
  for (i = 0; i < stockvec.Size(); i++)
    if (strcasecmp(TransitionStockName, stockvec[i]->Name()) == 0) {
      found++;
      TransitionStock = stockvec[i];
    }

  if (found != 1) {
    cerr << "Error: when pairing together stock and transition stock (" << TransitionStockName
      << "), " << found << " stocks were found\nBut there should have been only 1\n";
    exit(EXIT_FAILURE);
  }

  //JMB - get minimum length of the transition stock
  minlen = TransitionStock->ReturnLengthGroupDiv()->Minlength(0);

  //Check if TransitionStock is defined on all the areas that we work on.
  found = 0;
  for (i = 0; i < areas.Size(); i++)
    if (!TransitionStock->IsInArea(areas[i]))
      found++;

  if (found != 0) {
    cerr << "Warning: transition requested to stock " << (const char*)TransitionStock->Name()
      << "\nwhich might not be defined on " << found << " areas\n";
  }
}

void Transition::SetCI(const LengthGroupDivision* const GivenLDiv) {
  CI = new ConversionIndex(GivenLDiv, TransitionStock->ReturnLengthGroupDiv());
  lenindex = GivenLDiv->NoLengthGroup(minlen);
}

void Transition::Print(ofstream& outfile) const {
  outfile << "\nTransition\n\tTransition stock name " << TransitionStockName
    << "\n\tTransition step " << TransitionStep << "\n\tname of transition stock (from pointer) "
    << TransitionStock->Name() << endl << endl;
}

void Transition::KeepAgegroup(int area, Agebandmatrix& Alkeys, const TimeClass* const TimeInfo) {
  if (!TransitionStock->IsInArea(area))
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
  }
}

//area in the call to this routine is not in the local area numbering of the stock.
void Transition::MoveAgegroupToTransitionStock(int area, const TimeClass* const TimeInfo, int HasLgr) {
  if (!TransitionStock->IsInArea(area) || TimeInfo->CurrentStep() != TransitionStep)
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
  Agegroup[inarea].SettoZero();
}
