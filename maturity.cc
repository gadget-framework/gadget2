#include "maturity.h"
#include "stock.h"
#include "mathfunc.h"
#include "readfunc.h"
#include "readword.h"
#include "conversionindex.h"
#include "print.h"
#include "errorhandler.h"
#include "gadget.h"

// ********************************************************
// Functions for base Maturity
// ********************************************************
Maturity::Maturity(const IntVector& tmpareas, int minage, const IntVector& minlength,
  const IntVector& size, const LengthGroupDivision* const lgrpdiv)
  : LivesOnAreas(tmpareas), LgrpDiv(new LengthGroupDivision(*lgrpdiv)),
    Storage(tmpareas.Size(), minage, minlength, size) {
    TagStorage.resize(tmpareas.Size(), minage, minlength, size);

}

Maturity::~Maturity() {
  int i;
  for (i = 0; i < MatureStockNames.Size(); i++)
    delete[] MatureStockNames[i];
  for (i = 0; i < CI.Size(); i++)
    delete CI[i];
  delete LgrpDiv;
}

void Maturity::SetStock(StockPtrVector& stockvec) {
  int index = 0;
  int i, j;
  DoubleVector tmpratio;

  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < MatureStockNames.Size(); j++)
      if (strcasecmp(stockvec[i]->Name(), MatureStockNames[j]) == 0) {
        MatureStocks.resize(1);
        tmpratio.resize(1);
        MatureStocks[index] = stockvec[i];
        tmpratio[index] = Ratio[j];
        index++;
      }

  if (index != MatureStockNames.Size()) {
    cerr << "Error: Did not find the stock(s) matching:\n";
    for (i = 0; i < MatureStockNames.Size(); i++)
      cerr << (const char*)MatureStockNames[i] << sep;
    cerr << "\nwhen searching for mature stock(s) - found only:\n";
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

  CI.resize(MatureStocks.Size(), 0);
  for (i = 0; i < MatureStocks.Size(); i++)
    CI[i] = new ConversionIndex(LgrpDiv, MatureStocks[i]->ReturnLengthGroupDiv());

  for (i = 0; i < MatureStocks.Size(); i++) {
    index = 0;
    for (j = 0; j < areas.Size(); j++)
      if (!MatureStocks[i]->IsInArea(areas[j]))
        index++;

    if (index != 0)
      cerr << "Warning: maturation requested to stock " << (const char*)MatureStocks[i]->Name()
        << "\nwhich might not be defined on " << index << " areas\n";
  }
}

void Maturity::Print(ofstream& outfile) const {
  int i;
  outfile << "\nMaturity\n\tRead names of mature stocks:";
  for (i = 0; i < MatureStockNames.Size(); i++)
    outfile << sep << (const char*)(MatureStockNames[i]);
  outfile << "\n\tNames of mature stocks (through pointers):";
  for (i = 0; i < MatureStocks.Size(); i++)
    outfile << sep << (const char*)(MatureStocks[i]->Name());
  outfile << "\n\tStored numbers:\n";
  for (i = 0; i < areas.Size(); i++) {
    outfile << "Area " << areas[i] << endl;
    Printagebandm(outfile, Storage[i]);
  }
  outfile << endl;
}

//Move Immature stock to mature stocks.  Ratio is read in.  Ratio is
//not set to 1/MatureStocks.size for sometimes the mature stocks
//only have different age composition. The class has special
//conversionindex for each mature stock.
void Maturity::Move(int area, const TimeClass* const TimeInfo) {
  assert(this->IsMaturationStep(area, TimeInfo));
  const int inarea = AreaNr[area];
  int i;
  for (i = 0; i < MatureStocks.Size(); i++) {
    MatureStocks[i]->Add(Storage[inarea], CI[i], area, Ratio[i],
      Storage[inarea].Minage(), Storage[inarea].Maxage());

    if (TagStorage.NrOfTagExp() > 0)
      MatureStocks[i]->Add(TagStorage, inarea, CI[i], area, Ratio[i],
        TagStorage[inarea].Minage(), TagStorage[inarea].Maxage());
  }
  Storage[inarea].SettoZero();
}

//Put fish that becomes mature in temporary storage.  Later in the
//simulation the fish is moved from that storage to the Mature stock.
void Maturity::PutInStorage(int area, int age, int length, double number,
  double weight, const TimeClass* const TimeInfo) {

  assert(this->IsMaturationStep(area, TimeInfo));
  Storage[AreaNr[area]][age][length].N = (number > 0 ? number : 0.0);
  Storage[AreaNr[area]][age][length].W = (weight > 0 ? weight : 0.0);
}

//Put fish which becomes mature from tagging experiement with identity = id
//into temporary storage. Number of tagging experiments set must be greater
//then zero and 0 <= id < number of tagging experiments.
void Maturity::PutInStorage(int area, int age, int length, double number,
  const TimeClass* const TimeInfo, int id) {

  assert(this->IsMaturationStep(area, TimeInfo));
  if (TagStorage.NrOfTagExp() <= 0) {
    cerr << "Error in tagging maturity - no tagging experiment\n";
    exit(EXIT_FAILURE);
  } else if ((id >= TagStorage.NrOfTagExp()) || (id < 0)) {
    cerr << "Error in tagging maturity - illegal tagging experiment id\n";
    exit(EXIT_FAILURE);
  } else
    *(TagStorage[AreaNr[area]][age][length][id].N) = (number > 0.0 ? number: 0.0);
}

void Maturity::Precalc(const TimeClass* const TimeInfo) {
  int area, age, l, tag;
  if (TimeInfo->CurrentTime() == 1) {
    for (area = 0; area < areas.Size(); area++) {
      for (age = Storage[area].Minage(); age <= Storage[area].Maxage(); age++) {
        for (l = Storage[area].Minlength(age); l < Storage[area].Maxlength(age); l++) {
          Storage[area][age][l].N = 0.0;
          Storage[area][age][l].W = 0.0;
          for (tag = 0; tag < TagStorage.NrOfTagExp(); tag++)
            *(TagStorage[area][age][l][tag].N) = 0.0;
        }
      }
    }
  }
}

const StockPtrVector& Maturity::GetMatureStocks() {
  return MatureStocks;
}

// Add a new tagging experiment with name = tagname to tagStorage.
// This tagging experiment has id = number of tagging experiment before adding the new tag.
// For all areas, ages and lengths in TagStorage have added a new tag with number of fish = -1.0 and ratio = -1.0, or
// TagStorage[area][age][length][id].N = -1.0,
// TagStorage[area][age][length][id].R = -1.0
// New memory has been allocated for the double TagStorage[area][age][length].N.
void Maturity::AddTag(const char* tagname) {
  TagStorage.addTag(tagname);
}

void Maturity::DeleteTag(const char* tagname) {
  int minage, maxage, minlen, maxlen, age, length, i;
  int id = TagStorage.getId(tagname);

  if (id >= 0) {
    minage = TagStorage[0].Minage();
    maxage = TagStorage[0].Maxage();

    // Remove allocated memory
    for (i = 0; i < TagStorage.Size(); i++) {
      for (age = minage; age <= maxage; age++) {
        minlen = TagStorage[i].Minlength(age);
        maxlen = TagStorage[i].Maxlength(age);
        for (length = minlen; length < maxlen; length++) {
          delete [] (TagStorage[i][age][length][id].N);
          (TagStorage[i][age][length][id].N) = NULL;
        }
      }
    }
    TagStorage.deleteTag(tagname);

  } else {
    cerr << "Error in tagging maturity - trying to delete tag with name: "
      << tagname << " in maturity but there is not any tag with that name\n";
  }
}

// ********************************************************
// Functions for MaturityA
// ********************************************************
MaturityA::MaturityA(CommentStream& infile, const TimeClass* const TimeInfo,
  Keeper* const keeper, int minage, const IntVector& minabslength, const IntVector& size,
  const IntVector& tmpareas, const LengthGroupDivision* const lgrpdiv, int NoMatconst)
  : Maturity(tmpareas, minage, minabslength, size, lgrpdiv), PrecalcMaturation(minabslength, size, minage) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;
  keeper->AddString("maturity");
  infile >> text;
  if ((strcasecmp(text, "nameofmaturestocksandratio") == 0) || (strcasecmp(text, "maturestocksandratios") == 0)) {
    infile >> text;
    i = 0;
    while (strcasecmp(text, "coefficients") != 0 && infile.good()) {
      MatureStockNames.resize(1);
      MatureStockNames[i] = new char[strlen(text) + 1];
      strcpy(MatureStockNames[i], text);
      Ratio.resize(1);
      infile >> Ratio[i];
      i++;
      infile >> text;
    }
  } else
    handle.Unexpected("maturestocksandratios", text);

  if (!infile.good())
    handle.Failure("coefficients");
  Coefficient.resize(NoMatconst, keeper);
  Coefficient.Read(infile, TimeInfo, keeper);

  readWordAndVariable(infile, "minmatureage", MinMatureAge);
  keeper->ClearLast();
}

MaturityA::~MaturityA() {
}

void MaturityA::Precalc(const TimeClass* const TimeInfo) {
  this->Maturity::Precalc(TimeInfo);
  Coefficient.Update(TimeInfo);
  double my;
  int age, j;

  if (Coefficient.DidChange(TimeInfo)) {
    for (age = PrecalcMaturation.Minage(); age <= PrecalcMaturation.Maxage(); age++) {
      for (j = PrecalcMaturation.Minlength(age); j < PrecalcMaturation.Maxlength(age); j++) {
        if (age >= MinMatureAge) {
          my = exp(-Coefficient[0] - Coefficient[1] * LgrpDiv->Meanlength(j) - Coefficient[2] * age);
          PrecalcMaturation[age][j] = 1 / (1 + my);
        } else
          PrecalcMaturation[age][j] = 0.0;
      }
    }
  }
}

//Calculate the percentage that becomes Mature each timestep.
double MaturityA::MaturationProbability(int age, int length, int Growth,
  const TimeClass* const TimeInfo, const AreaClass* const Area, int area) {

  const double ratio =  PrecalcMaturation[age][length] *
    (Coefficient[1] * Growth * LgrpDiv->dl() +
    Coefficient[2] * TimeInfo->LengthOfCurrent() / TimeInfo->LengthOfYear());
  return (min(max(0.0, ratio), 1.0));
}

void MaturityA::Print(ofstream& outfile) const {
  Maturity::Print(outfile);
  outfile << "\tPrecalculated maturity.\n";
  BandmatrixPrint(PrecalcMaturation, outfile);
  outfile << endl;
}

int MaturityA::IsMaturationStep(int area, const TimeClass* const TimeInfo) {
  return 1;
}

// ********************************************************
// Functions for MaturityB
// ********************************************************
MaturityB::MaturityB(CommentStream& infile, const TimeClass* const TimeInfo,
  Keeper* const keeper, int minage, const IntVector& minabslength,
  const IntVector& size, const IntVector& tmpareas, const LengthGroupDivision*const lgrpdiv)
  : Maturity(tmpareas, minage, minabslength, size, lgrpdiv) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;
  infile >> text;
  keeper->AddString("maturity");
  if ((strcasecmp(text, "nameofmaturestocksandratio") == 0) || (strcasecmp(text, "maturestocksandratios") == 0)) {
    infile >> text;
    i = 0;
    while (!(strcasecmp(text, "maturitysteps") == 0) && infile.good()) {
      MatureStockNames.resize(1);
      MatureStockNames[i] = new char[strlen(text) + 1];
      strcpy(MatureStockNames[i], text);
      Ratio.resize(1);
      infile >> Ratio[i];
      i++;
      infile >> text;
    }
  } else
    handle.Unexpected("maturestocksandratios", text);

  if (!infile.good())
    handle.Failure("maturitysteps");
  infile >> ws;
  while (isdigit(infile.peek()) && !infile.eof()) {
    maturitystep.resize(1);
    infile >> maturitystep[maturitystep.Size() - 1] >> ws;
  }
  infile >> text;
  if (!(strcasecmp(text, "maturitylengths") == 0))
    handle.Unexpected("maturitylengths", text);
  while (maturitylength.Size() < maturitystep.Size() && !infile.eof()) {
    maturitylength.resize(1, keeper);
    maturitylength[maturitylength.Size() - 1].Read(infile, TimeInfo, keeper);
  }

  if (maturitylength.Size() != maturitystep.Size())
    handle.Message("Number of maturitysteps does not equal number of maturitylengths");
  keeper->ClearLast();
}

MaturityB::~MaturityB() {
}

void MaturityB::Print(ofstream& outfile) const {
  int i;
  Maturity::Print(outfile);
  outfile << "maturitysteps";
  for (i = 0; i < maturitystep.Size(); i++)
    outfile << sep << maturitystep[i];
  outfile << "\nmaturitylengths";
  for (i = 0; i < maturitylength.Size(); i++)
    outfile << sep << maturitylength[i];
}

//Calculate the percentage that becomes Mature each timestep.
//Need to check units on LengthOfCurrent
double MaturityB::MaturationProbability(int age, int length, int Growth,
  const TimeClass* const TimeInfo, const AreaClass* const Area, int area) {

  int i;
  for (i = 0; i < maturitylength.Size(); i++)
    if (TimeInfo->CurrentStep() == maturitystep[i])
      return (LgrpDiv->Meanlength(length) >= maturitylength[i]);
  return 0.0;
}

void MaturityB::Precalc(const TimeClass* const TimeInfo) {
  this->Maturity::Precalc(TimeInfo);
  maturitylength.Update(TimeInfo);
}

int MaturityB::IsMaturationStep(int area, const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < maturitystep.Size(); i++)
    if (maturitystep[i] == TimeInfo->CurrentStep())
      return 1;
  return 0;
}

// ********************************************************
// Functions for MaturityC
// ********************************************************
MaturityC::MaturityC(CommentStream& infile, const TimeClass* const TimeInfo,
  Keeper* const keeper, int minage, const IntVector& minabslength, const IntVector& size,
  const IntVector& tmpareas, const LengthGroupDivision* const lgrpdiv, int NoMatconst)
  : MaturityA(infile, TimeInfo, keeper, minage, minabslength, size, tmpareas, lgrpdiv, NoMatconst) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  infile >> text >> ws;
  if (strcasecmp(text, "maturitystep") != 0)
    handle.Unexpected("maturitystep", text);

  int i = 0;
  while (isdigit(infile.peek()) && !infile.eof()) {
    maturitystep.resize(1);
    infile >> maturitystep[i] >> ws;
    i++;
  }

  for (i = 0; i < maturitystep.Size(); i++)
    if (maturitystep[i] < 1 || maturitystep[i] > TimeInfo->StepsInYear())
      handle.Message("illegal maturity step in maturation type C");
}

void MaturityC::Precalc(const TimeClass* const TimeInfo) {
  this->Maturity::Precalc(TimeInfo);
  Coefficient.Update(TimeInfo);
  double my;
  int age, j;

  if (Coefficient.DidChange(TimeInfo)) {
    for (age = PrecalcMaturation.Minage(); age <= PrecalcMaturation.Maxage(); age++) {
      for (j = PrecalcMaturation.Minlength(age); j < PrecalcMaturation.Maxlength(age); j++) {
        if (age >= MinMatureAge) {
          my = exp(-Coefficient[0] * (LgrpDiv->Meanlength(j) - Coefficient[1]) -
            Coefficient[2] * (age - Coefficient[3]));
          PrecalcMaturation[age][j] = 1 / (1 + my);
        } else
          PrecalcMaturation[age][j] = 0.0;
      }
    }
  }
}

MaturityC::~MaturityC() {
}

double MaturityC::MaturationProbability(int age, int length, int Growth,
  const TimeClass* const TimeInfo, const AreaClass* const Area, int area) {

  if (this->IsMaturationStep(area, TimeInfo)) {
    const double ratio =  PrecalcMaturation[age][length] *
      (Coefficient[0] * Growth * LgrpDiv->dl() + Coefficient[2] *
      TimeInfo->LengthOfCurrent() / TimeInfo->LengthOfYear());
    return (min(max(0.0, ratio), 1.0));
  }
  return 0.0;
}

int MaturityC::IsMaturationStep(int area, const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < maturitystep.Size(); i++)
    if (maturitystep[i] == TimeInfo->CurrentStep())
      return 1;
  return 0;
}

// ********************************************************
// Functions for MaturityD
// ********************************************************
MaturityD::MaturityD(CommentStream& infile, const TimeClass* const TimeInfo,
  Keeper* const keeper, int minage, const IntVector& minabslength, const IntVector& size,
  const IntVector& tmpareas, const LengthGroupDivision* const lgrpdiv, int NoMatconst)
  : MaturityA(infile, TimeInfo, keeper, minage, minabslength, size, tmpareas, lgrpdiv, NoMatconst) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  infile >> text >> ws;
  if (strcasecmp(text, "maturitystep") != 0)
    handle.Unexpected("maturitystep", text);

  int i = 0;
  while (isdigit(infile.peek()) && !infile.eof()) {
    maturitystep.resize(1);
    infile >> maturitystep[i] >> ws;
    i++;
  }

  for (i = 0; i < maturitystep.Size(); i++)
    if (maturitystep[i] < 1 || maturitystep[i] > TimeInfo->StepsInYear())
      handle.Message("illegal maturity step in maturation type D");
}

void MaturityD::Precalc(const TimeClass* const TimeInfo) {
  this->Maturity::Precalc(TimeInfo);
  Coefficient.Update(TimeInfo);
  double my;
  int age, j;

  if (Coefficient.DidChange(TimeInfo)) {
    for (age = PrecalcMaturation.Minage(); age <= PrecalcMaturation.Maxage(); age++) {
      for (j = PrecalcMaturation.Minlength(age); j < PrecalcMaturation.Maxlength(age); j++){
        if (age >= MinMatureAge) {
          my = exp(-4.0 * Coefficient[0] * (LgrpDiv->Meanlength(j) -
            Coefficient[1]) - Coefficient[2] * (age - Coefficient[3]));
          PrecalcMaturation[age][j] = 1 / (1 + my);
        } else
          PrecalcMaturation[age][j] = 0.0;
      }
    }
  }
}

MaturityD::~MaturityD() {
}

double MaturityD::MaturationProbability(int age, int length, int Growth,
  const TimeClass* const TimeInfo, const AreaClass* const Area, int area) {

  if (this->IsMaturationStep(area, TimeInfo)) {
    const double ratio = PrecalcMaturation[age][length];
    return (min(max(0.0, ratio), 1.0));
  }
  return 0.0;
}

int MaturityD::IsMaturationStep(int area, const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < maturitystep.Size(); i++)
    if (maturitystep[i] == TimeInfo->CurrentStep())
      return 1;
  return 0;
}

