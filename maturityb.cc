#include "maturityb.h"
#include "stock.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "conversion.h"
#include "gadget.h"

MaturityB::MaturityB(CommentStream& infile, const TimeClass* const TimeInfo,
  Keeper* const keeper, int minage, const intvector& minabslength,
  const intvector& size, const intvector& tmpareas, const LengthGroupDivision*const lgrpdiv)
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
