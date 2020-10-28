#include "fleet.h"
#include "areatime.h"
#include "keeper.h"
#include "totalpredator.h"
#include "linearpredator.h"
#include "numberpredator.h"
#include "effortpredator.h"
#include "quotapredator.h"
#include "readfunc.h"
#include "readword.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

Fleet::Fleet(CommentStream& infile, const char* givenname, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper, FleetType ftype)
  : BaseClass(givenname), predator(0) {

  type = ftype;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  ifstream subfile;
  CommentStream subcomment(subfile);
  int tmpint = 0;
  Formula multscaler;
  IntVector tmpareas;

  keeper->addString("fleet");
  keeper->addString(this->getName());
  infile >> text >> ws;
  if (strcasecmp(text, "livesonareas") != 0)
    handle.logFileUnexpected(LOGFAIL, "livesonareas", text);

  char c = infile.peek();
  while (isdigit(c) && !infile.eof()) {
    infile >> tmpint >> ws;
    tmpareas.resize(1, Area->getInnerArea(tmpint));
    c = infile.peek();
  }
  this->storeAreas(tmpareas);

  infile >> ws;
  c = infile.peek();
  //JMB - removed the need to read in the fleet lengths
  if ((c == 'l') || (c == 'L')) {
    handle.logMessage(LOGWARN, "Warning in fleet - length data ignored");
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
    infile >> ws;
    c = infile.peek();
  }

  if ((c == 'm') || (c == 'M'))
    readWordAndVariable(infile, "multiplicative", multscaler);
  else
    multscaler.setValue(1.0);

  switch (type) {
    case TOTALFLEET:
      predator = new TotalPredator(infile, this->getName(), areas, TimeInfo, keeper, multscaler);
      break;
    case LINEARFLEET:
      predator = new LinearPredator(infile, this->getName(), areas, TimeInfo, keeper, multscaler);
      break;
    case NUMBERFLEET:
      predator = new NumberPredator(infile, this->getName(), areas, TimeInfo, keeper, multscaler);
      break;
    case EFFORTFLEET:
      predator = new EffortPredator(infile, this->getName(), areas, TimeInfo, keeper, multscaler);
      break;
    case QUOTAFLEET:
      predator = new QuotaPredator(infile, this->getName(), areas, TimeInfo, keeper, multscaler);
      break;
    default:
      handle.logMessage(LOGFAIL, "Error in fleet - unrecognised fleet type for", this->getName());
  }

  //the next entry in the file will be the name of the amounts datafile
  infile >> text >> ws;
  subfile.open(text, ios::binary);
  handle.checkIfFailure(subfile, text);
  handle.Open(text);
  readAmounts(subcomment, areas, TimeInfo, Area, amount, this->getName());
  amount.Inform(keeper);
  handle.Close();
  subfile.close();
  subfile.clear();

  //resize tmpPopulation, and set the weight to 1 since this will never change
  PopInfo tmppop;
  tmppop.W = 1.0;
  tmpPopulation.AddRows(Area->numAreas(), 1, tmppop);

  keeper->clearLast();
  keeper->clearLast();
}

Fleet::~Fleet() {
  delete predator;
}

void Fleet::calcEat(int area,
  const AreaClass* const Area, const TimeClass* const TimeInfo) {

  if (this->isFleetStepArea(area, TimeInfo))
    predator->Eat(area, Area, TimeInfo);
}

void Fleet::adjustEat(int area, const TimeClass* const TimeInfo) {
  if (this->isFleetStepArea(area, TimeInfo))
    predator->adjustConsumption(area, TimeInfo);
}

void Fleet::calcNumbers(int area, const TimeClass* const TimeInfo) {
  if (this->isFleetStepArea(area, TimeInfo))
    predator->Sum(tmpPopulation[this->areaNum(area)], area);
}

void Fleet::Reset(const TimeClass* const TimeInfo) {
  int i;
  predator->Reset(TimeInfo);
  for (i = 0; i < tmpPopulation.Nrow(); i++)
    if (this->isFleetStepArea(i, TimeInfo))
      tmpPopulation[this->areaNum(i)][0].N = amount[TimeInfo->getTime()][this->areaNum(i)];
}

void Fleet::Print(ofstream& outfile) const {
  outfile << "\nFleet - type ";
  predator->Print(outfile);
}

int Fleet::isFleetStepArea(int area, const TimeClass* const TimeInfo) {
  if ((this->isInArea(area) == 0) || isZero(predator->getMultScaler()))
    return 0;
  if (amount[TimeInfo->getTime()][this->areaNum(area)] < 0.0)
    handle.logMessage(LOGWARN, "Warning in fleet - negative amount consumed for", this->getName());
  if ((predator->getType() == QUOTAPREDATOR) && (TimeInfo->getStep() == 1))
    return 1;  //JMB need to check the first step
  if (isZero(amount[TimeInfo->getTime()][this->areaNum(area)]))
    return 0;
  return 1;
}
