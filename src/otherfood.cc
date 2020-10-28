#include "otherfood.h"
#include "readfunc.h"
#include "readword.h"
#include "lengthprey.h"
#include "errorhandler.h"
#include "intvector.h"
#include "popinfo.h"
#include "popinfovector.h"
#include "gadget.h"
#include "global.h"

OtherFood::OtherFood(CommentStream& infile, const char* givenname,
  const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper)
  : BaseClass(givenname), prey(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  ifstream subfile;
  CommentStream subcomment(subfile);
  int tmpint = 0;
  IntVector tmpareas;
  char c;

  keeper->addString("otherfood");
  keeper->addString(givenname);

  infile >> text >> ws;
  if (strcasecmp(text, "livesonareas") != 0)
    handle.logFileUnexpected(LOGFAIL, "livesonareas", text);

  c = infile.peek();
  while (isdigit(c) && !infile.eof()) {
    infile >> tmpint >> ws;
    tmpareas.resize(1, Area->getInnerArea(tmpint));
    c = infile.peek();
  }
  this->storeAreas(tmpareas);

  prey = new LengthPrey(infile, this->getName(), areas, TimeInfo, keeper);

  infile >> text >> ws;
  if ((strcasecmp(text, "amount") != 0) && (strcasecmp(text, "amounts") != 0))
    handle.logFileUnexpected(LOGFAIL, "amount", text);

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

OtherFood::~OtherFood() {
  delete prey;
}

void OtherFood::checkEat(int area, const TimeClass* const TimeInfo) {
  if (this->isOtherFoodStepArea(area, TimeInfo))
    prey->checkConsumption(area, TimeInfo);
}

void OtherFood::calcNumbers(int area, const TimeClass* const TimeInfo) {
  if (this->isOtherFoodStepArea(area, TimeInfo))
    prey->Sum(tmpPopulation[this->areaNum(area)], area);
}

int OtherFood::isOtherFoodStepArea(int area, const TimeClass* const TimeInfo) {
  if (this->isInArea(area) == 0)
    return 0;
  if (amount[TimeInfo->getTime()][this->areaNum(area)] < 0.0)
    handle.logMessage(LOGWARN, "Warning in otherfood - negative amount to be consumed");
  if (isZero(amount[TimeInfo->getTime()][this->areaNum(area)]))
    return 0;
  return 1;
}

void OtherFood::Print(ofstream& outfile) const {
  outfile << "\nOtherfood " << this->getName() << endl;
  prey->Print(outfile);
  outfile << endl;
}

void OtherFood::Reset(const TimeClass* const TimeInfo) {
  int i;
  prey->Reset(TimeInfo);
  for (i = 0; i < tmpPopulation.Nrow(); i++)
    if (this->isOtherFoodStepArea(i, TimeInfo))
      tmpPopulation[this->areaNum(i)][0].N = amount[TimeInfo->getTime()][this->areaNum(i)];
}
