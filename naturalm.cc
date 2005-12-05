#include "naturalm.h"
#include "readfunc.h"
#include "areatime.h"
#include "keeper.h"
#include "gadget.h"

NaturalMortality::NaturalMortality(CommentStream& infile, int minage, int maxage,
  const TimeClass* const TimeInfo, Keeper* const keeper) {

  keeper->addString("naturalmortality");
  proportion.resize(maxage - minage + 1, 0.0);
  mortality.setsize(maxage - minage + 1);
  mortality.read(infile, TimeInfo, keeper);
  keeper->clearLast();
}

void NaturalMortality::Reset(const TimeClass* const TimeInfo) {
  mortality.Update(TimeInfo);
  if (mortality.didChange(TimeInfo) || TimeInfo->didStepSizeChange()) {
    int i;
    for (i = 0; i < mortality.Size(); i++) {
      if (mortality[i] > verysmall)
        proportion[i] = exp(-mortality[i] * TimeInfo->getTimeStepSize());
      else
        proportion[i] = 1.0; //i.e. use mortality rate 0
    }
  }
}

void NaturalMortality::Print(ofstream& outfile) {
  int i;
  outfile << "Natural mortality\n\t";
  for (i = 0; i < mortality.Size(); i++)
    outfile << mortality[i] << sep;
  outfile << endl;
}
