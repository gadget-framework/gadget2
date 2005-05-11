#include "naturalm.h"
#include "readfunc.h"
#include "areatime.h"
#include "keeper.h"
#include "doubleindexvector.h"
#include "gadget.h"

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

  if (mortality.didChange(TimeInfo) || TimeInfo->didStepSizeChange()) {
    shift = mortality.minCol();
    timeratio = TimeInfo->getTimeStepSize();
    for (age = mortality.minCol(); age < mortality.maxCol(); age++)
      if (mortality[age] > verysmall)
        proportion[age - shift] = exp(-mortality[age] * timeratio);
      else
        proportion[age - shift] = 1.0; //i.e. use mortality rate 0
  }
}

void NaturalM::Print(ofstream& outfile) {
  int i;
  outfile << "Natural mortality\n\t";
  for (i = mortality.minCol(); i < mortality.maxCol(); i++)
    outfile << mortality[i] << sep;
  outfile << endl;
}
