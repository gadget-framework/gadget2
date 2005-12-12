#include "otherfood.h"
#include "readfunc.h"
#include "readword.h"
#include "lengthprey.h"
#include "errorhandler.h"
#include "intvector.h"
#include "popinfo.h"
#include "popinfovector.h"
#include "gadget.h"

extern ErrorHandler handle;

OtherFood::OtherFood(CommentStream& infile, const char* givenname,
  const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper)
  : BaseClass(givenname), prey(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  ifstream subfile;
  CommentStream subcomment(subfile);
  char c;
  int i, tmpint;

  keeper->addString("otherfood");
  keeper->addString(givenname);

  infile >> text;
  IntVector tmpareas;
  if (strcasecmp(text, "livesonareas") == 0) {
    infile >> ws;
    i = 0;
    c = infile.peek();
    while (isdigit(c) && !infile.eof() && (i < Area->numAreas())) {
      infile >> tmpint >> ws;
      tmpareas.resize(1, Area->getInnerArea(tmpint));
      c = infile.peek();
      i++;
    }
    this->storeAreas(tmpareas);
  } else
    handle.logFileUnexpected(LOGFAIL, "livesonareas", text);

  //read the length information
  DoubleVector lengths(2, 0.0);
  infile >> text >> ws;
  if (strcasecmp(text, "lengths") == 0) {
    for (i = 0; i < 2; i++)
      infile >> lengths[i] >> ws;
  } else
    handle.logFileUnexpected(LOGFAIL, "lengths", text);

  //read the energy content of this prey
  double energy;
  infile >> ws;
  c = infile.peek();
  if ((c == 'e') || (c == 'E'))
    readWordAndVariable(infile, "energycontent", energy);
  else
    energy = 1.0;

  prey = new LengthPrey(lengths, areas, energy, this->getName());

  infile >> text >> ws;
  if ((strcasecmp(text, "amount") == 0) || (strcasecmp(text, "amounts") == 0)) {
    infile >> text >> ws;
    subfile.open(text, ios::in);
    handle.checkIfFailure(subfile, text);
    handle.Open(text);
    readAmounts(subcomment, areas, TimeInfo, Area, amount, this->getName());
    amount.Inform(keeper);
    handle.Close();
    subfile.close();
    subfile.clear();
  } else
    handle.logFileUnexpected(LOGFAIL, "amount", text);

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

LengthPrey* OtherFood::getPrey() const {
  return prey;
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

void OtherFood::Reset(const TimeClass* const TimeInfo, const AreaClass* const Area) {
  int i;
  prey->Reset();
  for (i = 0; i < tmpPopulation.Nrow(); i++)
    if (this->isOtherFoodStepArea(i, TimeInfo))
      tmpPopulation[this->areaNum(i)][0].N = amount[TimeInfo->getTime()][this->areaNum(i)] * Area->getSize(i);
}
