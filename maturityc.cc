#include "maturityc.h"
#include "stock.h"
#include "readfunc.h"
#include "errorhandler.h"
#include "conversion.h"
#include "intvector.h"
#include "gadget.h"

MaturityC::MaturityC(CommentStream& infile, const TimeClass* const TimeInfo,
  Keeper* const keeper, int minage, const intvector& minabslength, const intvector& size,
  const intvector& tmpareas, const LengthGroupDivision* const lgrpdiv, int NoMatconst)
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
      TimeInfo->LengthOfCurrent()) / TimeInfo->LengthOfYear();
    return(min(max(0.0, ratio), 1.0));
    //return 0 if ratio < 0, 1 if ratio > 1, else ratio.
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
