#include "naturalm.h"
#include "readfunc.h"
#include "areatime.h"
#include "keeper.h"
#include "errorhandler.h"
#include "doubleindexvector.h"
#include "gadget.h"

extern ErrorHandler handle;

//A simple beginning, only one mortality for each yearclass.
NaturalM::NaturalM(CommentStream& infile, int minage, int maxage, const TimeClass* const TimeInfo,
  Keeper* const keeper) : mortality(maxage - minage + 1, minage), proportion(maxage - minage + 1, 0.0) {

  keeper->addString("naturalm");
  mortality.read(infile, TimeInfo, keeper);
  this->Reset(TimeInfo);
  keeper->clearLast();
}

void NaturalM::Reset(const TimeClass* const TimeInfo) {
  mortality.Update(TimeInfo);
  double timeratio;
  int age, shift;

  if (mortality.DidChange(TimeInfo) || TimeInfo->SizeOfStepDidChange()) {
    shift = mortality.Mincol();
    timeratio = TimeInfo->LengthOfCurrent() / TimeInfo->LengthOfYear();
    for (age = mortality.Mincol(); age < mortality.Maxcol(); age++)
      if (mortality[age] > verysmall)
        proportion[age - shift] = exp(-mortality[age] * timeratio);
      else
        proportion[age - shift] = 1.0; //i.e. use mortality rate 0
  }
}

const DoubleVector& NaturalM::ProportionSurviving(const TimeClass* const TimeInfo) const {
  return proportion;
}

void NaturalM::Print(ofstream& outfile) {
  int i;
  outfile << "Natural mortality\n\t";
  for (i = mortality.Mincol(); i < mortality.Maxcol(); i++)
    outfile << mortality[i] << sep;
  outfile << endl;
}

const DoubleIndexVector& NaturalM::getMortality() const {
  DoubleIndexVector* tmpvec = new DoubleIndexVector(mortality.Size(), mortality.Mincol());
  int i;
  for (i = tmpvec->Mincol(); i < tmpvec->Maxcol(); i++)
    (*tmpvec)[i] = mortality[i];
  return *tmpvec;
}
