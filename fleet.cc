#include "fleet.h"
#include "areatime.h"
#include "keeper.h"
#include "totalpredator.h"
#include "linearpredator.h"
#include "numberpredator.h"
#include "readfunc.h"
#include "readword.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

Fleet::Fleet(CommentStream& infile, const char* givenname, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper, FleetType ftype)
  : BaseClass(givenname), predator(0) {

  type = ftype;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  ifstream subfile;
  CommentStream subcomment(subfile);
  int i, tmpint;
  Formula multscaler;
  IntVector tmpareas;
  char c;

  keeper->addString("fleet");
  keeper->addString(this->getName());

  infile >> text >> ws;
  if (strcasecmp(text, "livesonareas") == 0) {
    i = 0;
    c = infile.peek();
    while (isdigit(c) && !infile.eof() && (i < Area->numAreas())) {
      tmpareas.resize(1);
      infile >> tmpint >> ws;
      tmpareas[i] = Area->InnerArea(tmpint);
      c = infile.peek();
      i++;
    }
    this->LetLiveOnAreas(tmpareas);
  } else
    handle.logFileUnexpected(LOGFAIL, "livesonareas", text);

  infile >> ws;
  c = infile.peek();
  //JMB - removed the need to read in the fleet lengths
  if ((c == 'l') || (c == 'L')) {
    handle.logFileMessage(LOGWARN, "Warning in fleet - length data ignored");
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
    infile >> ws;
    c = infile.peek();
  }

  if ((c == 'm') || (c == 'M'))
    readWordAndFormula(infile, "multiplicative", multscaler);
  else
    multscaler.setValue(1.0);

  infile >> text >> ws;
  if (!(strcasecmp(text, "suitability") == 0))
    handle.logFileUnexpected(LOGFAIL, "suitability", text);

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
    default:
      handle.logFileMessage(LOGFAIL, "Error in fleet - unrecognised fleet type for", this->getName());
    }

  //the next entry in the file will be the name of the amounts datafile
  infile >> text >> ws;
  subfile.open(text, ios::in);
  handle.checkIfFailure(subfile, text);
  handle.Open(text);

  if (!readAmounts(subcomment, areas, TimeInfo, Area, amount, keeper, this->getName()))
    handle.logFileMessage(LOGFAIL, "Error in fleet - failed to read fleet amounts");
  amount.Inform(keeper);

  handle.Close();
  subfile.close();
  subfile.clear();

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

void Fleet::calcNumbers(int area,
  const AreaClass* const Area, const TimeClass* const TimeInfo) {

  if (this->isFleetStepArea(area, TimeInfo)) {
    PopInfo pop;
    pop.N = amount[TimeInfo->getTime()][this->areaNum(area)];
    pop.W = 1.0;
    PopInfoVector NumberInArea(1, pop);
    predator->Sum(NumberInArea, area);
  }
}

void Fleet::Reset(const TimeClass* const TimeInfo) {
  predator->Reset(TimeInfo);
}

void Fleet::Print(ofstream& outfile) const {
  outfile << "\nFleet - type ";
  predator->Print(outfile);
}

LengthPredator* Fleet::returnPredator() const {
  return predator;
}

int Fleet::isFleetStepArea(int area, const TimeClass* const TimeInfo) {
  if (isZero(predator->multScaler()))
    return 0;
  if ((handle.getLogLevel() >= LOGWARN) && (amount[TimeInfo->getTime()][this->areaNum(area)] < 0))
    handle.logMessage(LOGWARN, "Warning in fleet - negative amount consumed");
  if (isZero(amount[TimeInfo->getTime()][this->areaNum(area)]))
    return 0;
  return 1;
}

double Fleet::Amount(int area, const TimeClass* const TimeInfo) const {
  return amount[TimeInfo->getTime()][this->areaNum(area)];
}
