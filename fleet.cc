#include "fleet.h"
#include "areatime.h"
#include "keeper.h"
#include "totalpredator.h"
#include "linearpredator.h"
#include "mortpredator.h"
#include "readfunc.h"
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
  int i = 0;
  int tmpint;
  double multscaler;
  int readamount = 0; //mortalityfleet need no amounts
  keeper->addString("fleet");
  keeper->addString(givenname);

  infile >> text >> ws;
  IntVector tmpareas;
  if (strcasecmp(text, "livesonareas") == 0) {
    char c = infile.peek();
    while (isdigit(c) && !infile.eof() && (i < Area->NoAreas())) {
      tmpareas.resize(1);
      infile >> tmpint >> ws;
      if ((tmpareas[i] = Area->InnerArea(tmpint)) == -1)
        handle.UndefinedArea(tmpint);
      c = infile.peek();
      i++;
    }
    this->LetLiveOnAreas(tmpareas);
  } else
    handle.Unexpected("livesonareas", text);

  DoubleVector lengths(2, 0.0);
  infile >> text >> ws;
  if (strcasecmp(text, "lengths") == 0) {
    if (!readVector(infile, lengths))
      handle.Message("Failed to read lengths");
  } else
    handle.Unexpected("lengths", text);

  LengthGroupDivision LgrpDiv(lengths);

  infile >> text >> ws;
  if (strcasecmp(text, "multiplicative") == 0) {
    infile >> multscaler >> ws;
    infile >> text >> ws;
  } else
    multscaler = 1.0;

  if (multscaler < 0)
    handle.Warning("negative value for multiplicative");

  if (!(strcasecmp(text, "suitability") == 0))
    handle.Unexpected("suitability", text);

  switch(type) {
    case TOTALFLEET:
      predator = new TotalPredator(infile, givenname, areas, &LgrpDiv,
        &LgrpDiv, TimeInfo, keeper, multscaler);
      readamount = 1;
      break;
    case LINEARFLEET:
      predator = new LinearPredator(infile, givenname, areas, &LgrpDiv,
        &LgrpDiv, TimeInfo, keeper, multscaler);
      readamount = 1;
      break;
    case MORTALITYFLEET:
      predator = new MortPredator(infile, givenname, areas, &LgrpDiv,
        &LgrpDiv, TimeInfo, keeper);
      readamount = 1; //should be 0
      break;
    default:
      handle.Message("Unrecognized type of fleet");
    }

  //the next entry in the file will be the name of the amounts datafile
  infile >> text >> ws;
  subfile.open(text, ios::in);
  handle.checkIfFailure(subfile, text);
  handle.Open(text);

  if (readamount != 0) {
    if (!readAmounts(subcomment, areas, TimeInfo, Area, amount, keeper, givenname))
      handle.Message("Failed to read fleet amounts");
    amount.Inform(keeper);
  }

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

  predator->Eat(area, TimeInfo->LengthOfCurrent()/TimeInfo->LengthOfYear(),
    Area->Temperature(area, TimeInfo->CurrentTime()),
    Area->Size(area), TimeInfo->CurrentSubstep(), TimeInfo->NrOfSubsteps());
}

void Fleet::adjustEat(int area,
  const AreaClass* const Area, const TimeClass* const TimeInfo) {

  predator->adjustConsumption(area, TimeInfo->NrOfSubsteps(), TimeInfo->CurrentSubstep());
}

void Fleet::ThirdUpdate(int area,
  const AreaClass* const Area, const TimeClass* const TimeInfo) {

  //Calls calcCHat in mortalityfleet,
  //to calculate the modelled catches for printing [AJ&MNAA 13.05.01]
  //This should not be called when optimizing, we should look into adding information
  //to keeper about what kind of run we are doing [AJ&MNAA 13.05.01]
  if (type == MORTALITYFLEET)
    ((MortPredator*)predator)->calcCHat(area, TimeInfo);
}

void Fleet::CalcNumbers(int area,
  const AreaClass* const Area, const TimeClass* const TimeInfo) {

  PopInfo pop;
  pop.N = amount[TimeInfo->CurrentTime()][AreaNr[area]];
  pop.W = 1.0;
  PopInfoVector NumberInArea(1, pop);
  predator->Sum(NumberInArea, area);
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

double Fleet::OverConsumption(int area) const {
  return predator->OverConsumption(area)[0];
}

double Fleet::Amount(int area, const TimeClass* const TimeInfo) const {
  return amount[TimeInfo->CurrentTime()][AreaNr[area]];
}
