#include "otherfood.h"
#include "readfunc.h"
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
  int i = 0;
  int tmpint;

  keeper->addString("otherfood ");
  keeper->addString(givenname);

  infile >> text;
  IntVector tmpareas;
  if (strcasecmp(text, "livesonareas") == 0) {
    infile >> ws;
    char c = infile.peek();
    while (isdigit(c) && !infile.eof() && (i < Area->numAreas())) {
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
  infile >> text;
  if (strcasecmp(text, "lengths") == 0) {
    if (!readVector(infile, lengths))
      handle.Message("Failed to read lengths");
  } else
    handle.Unexpected("lengths", text);

  LengthGroupDivision LgrpDiv(lengths);
  prey = new LengthPrey(lengths, areas, Name());

  infile >> text >> ws;
  if (strcasecmp(text, "amounts") == 0) {
    infile >> text >> ws;
    subfile.open(text, ios::in);
    handle.checkIfFailure(subfile, text);
    handle.Open(text);

    if (!readAmounts(subcomment, areas, TimeInfo, Area, amount, keeper, givenname))
      handle.Message("Failed to read otherfood amounts");
    amount.Inform(keeper);

    handle.Close();
    subfile.close();
    subfile.clear();
  } else
    handle.Unexpected("amounts", text);

  keeper->clearLast();
  keeper->clearLast();

  prey->setCI(&LgrpDiv);
}

OtherFood::~OtherFood() {
  delete prey;
}

LengthPrey* OtherFood::returnPrey() const {
  return prey;
}

void OtherFood::checkEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {
  prey->checkConsumption(area, TimeInfo->numSubSteps());
}

void OtherFood::calcNumbers(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {
  PopInfo pop;
  //Warning: Choose the weight as 1.
  pop.W = 1.0;
  pop.N = amount[TimeInfo->CurrentTime()][AreaNr[area]] * Area->Size(area);
  PopInfoVector NumberInArea(1, pop);
  prey->SumUsingPopInfo(NumberInArea, area, TimeInfo->CurrentSubstep());
}

void OtherFood::Print(ofstream& outfile) const {
  outfile << "\nOtherfood " << Name() << endl;
  prey->Print(outfile);
  outfile << endl;
}

void OtherFood::Reset(const TimeClass* const TimeInfo) {
  if (TimeInfo->CurrentTime() == 1) {
    prey->Reset();
    handle.logMessage("Reset otherfood data for", this->Name());
  }
}
