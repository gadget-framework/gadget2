#include "lengthpredator.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

LengthPredator::LengthPredator(const char* givenname, const IntVector& Areas,const TimeClass* const TimeInfo,
			       Keeper* const keeper,Formula multscaler) : PopPredator(givenname, Areas) {

  keeper->addString("scale");
  multi = multscaler;
  multi.Inform(keeper);
  keeper->clearLast();
  timeMultiplier.resize(TimeInfo->numTotalSteps()+1,1.0);  //+1 is in line with amount in total fleet
}

void LengthPredator::Sum(const PopInfoVector& NumberInArea, int area) {
  prednumber[this->areaNum(area)].Sum(&NumberInArea, *CI);
}

void LengthPredator::Reset(const TimeClass* const TimeInfo) {
  PopPredator::Reset(TimeInfo);
  if (multi < 0.0)
    handle.logMessage(LOGWARN, "Warning in lengthpredator - negative value for multiplicative");
}

// New HB seems to work look better at the timing.  
void LengthPredator::setTimeMultiplier(const TimeClass* const TimeInfo,int quotastep,double value){
  if((TimeInfo->getTime()+quotastep) <= (TimeInfo->numTotalSteps())) // not go beyond last step
    timeMultiplier[TimeInfo->getTime()+quotastep] = value;
}
