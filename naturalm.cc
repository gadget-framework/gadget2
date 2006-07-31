#include "naturalm.h"
#include "readfunc.h"
#include "areatime.h"
#include "readword.h"
#include "keeper.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

NaturalMortality::NaturalMortality(CommentStream& infile, int minage, int numage,
  const TimeClass* const TimeInfo, Keeper* const keeper) {

  keeper->addString("naturalmortality");
  proportion.resize(numage, 0.0);
  mortality.setsize(numage);
  mortality.read(infile, TimeInfo, keeper);
  keeper->clearLast();
}

NaturalMortality::~NaturalMortality() {
}

void NaturalMortality::Reset(const TimeClass* const TimeInfo) {
  mortality.Update(TimeInfo);
  if (mortality.didChange(TimeInfo) || TimeInfo->didStepSizeChange()) {
    int i;
    for (i = 0; i < proportion.Size(); i++) {
      if (mortality[i] > verysmall)
        proportion[i] = exp(-mortality[i] * TimeInfo->getTimeStepSize());
      else
        proportion[i] = 1.0; //use mortality rate 0
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
