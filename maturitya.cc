#include "maturitya.h"
#include "stock.h"
#include "readfunc.h"
#include "errorhandler.h"
#include "conversion.h"
#include "intvector.h"
#include "print.h"
#include "readword.h"
#include "gadget.h"

MaturityA::MaturityA(CommentStream& infile, const TimeClass* const TimeInfo,
  Keeper* const keeper, int minage, const intvector& minabslength, const intvector& size,
  const intvector& tmpareas, const LengthGroupDivision* const lgrpdiv, int NoMatconst)
  : Maturity(tmpareas, minage, minabslength, size, lgrpdiv), PrecalcMaturation(minabslength, size, minage) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;
  keeper->AddString("maturity");
  infile >> text;
  if (strcasecmp(text, "nameofmaturestocksandratio") != 0)
    handle.Unexpected("nameofmaturestocksandratio", text);
  else {
    infile >> text;
    i = 0;
    while (strcasecmp(text, "coefficients") != 0 && infile.good()) {
      NameOfMatureStocks.resize(1);
      NameOfMatureStocks[i] = new char[strlen(text) + 1];
      strcpy(NameOfMatureStocks[i], text);
      Ratio.resize(1);
      infile >> Ratio[i];
      i++;
      infile >> text;
    }
  }
  if (!infile.good())
    handle.Failure("coefficients");
  Coefficient.resize(NoMatconst, keeper);
  Coefficient.Read(infile, TimeInfo, keeper);

  ReadWordAndVariable(infile, "minmatureage", MinMatureAge);
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
