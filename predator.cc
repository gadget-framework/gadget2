#include "mathfunc.h"
#include "keeper.h"
#include "prey.h"
#include "suits.h"
#include "predator.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "gadget.h"

extern ErrorHandler handle;

Predator::Predator(const char* givenname, const IntVector& Areas)
  : HasName(givenname), LivesOnAreas(Areas), Suitable(0) {
}

Predator::~Predator() {
  int i;
  for (i = 0; i < Suitable->numFuncPreys(); i++)
    delete Suitable->FuncPrey(i);
  delete Suitable;
}

void Predator::setPrey(PreyPtrVector& preyvec, Keeper* const keeper) {

  int i, j;
  int found = 0;

  preys.resize(this->numPreys(), 0);
  for (i = 0; i < preyvec.Size(); i++) {
    found = 0;
    for (j = 0; j < this->numPreys(); j++) {
      if (strcasecmp(this->Preyname(j), preyvec[i]->getName()) == 0) {
        if (found == 0) {
          preys[j] = preyvec[i];
          found++;
        } else
          handle.logFailure("Error in predator - repeated suitability values for prey", preyvec[i]->getName());

      }
    }
  }

  found = 0;
  for (i = 0; i < preys.Size(); i++) {
    //If we find a prey that we have read the suitability for, but not
    //received a pointer to, we issue a warning and delete it through the
    //virtual function DeleteParametersForPrey.
    if (preys[i] == 0) {
      found++;
      handle.logWarning("Warning in predator - failed to match prey", this->Preyname(i));
      this->DeleteParametersForPrey(i, keeper);
      //This function allows derived classes to delete the information they keep.
      //Since we have deleted element no. i from the vectors, we must take
      //care not to miss the element that is now no. i
      if (found != preys.Size())
        i--;
    }
  }
  //Now we can resize objects according to the size of this->numPreys().
  this->resizeObjects();
}

int Predator::doesEat(const char* preyname) const {
  int found = 0;
  int i;
  for (i = 0; i < this->numPreys(); i++)
    if (strcasecmp(this->Preyname(i), preyname) == 0)
      found = 1;
  return found;
}

void Predator::Print(ofstream& outfile) const {
  int i;
  outfile << "\tName" << sep << this->getName()
    << "\n\tNames of preys:";
  for (i = 0; i < preys.Size(); i++)
    outfile << sep << this->Preyname(i);
  outfile << endl;
  for (i = 0; i < this->numPreys(); i++) {
    outfile << "\tSuitability for " << this->Preyname(i) << endl;
    this->Suitability(i).Print(outfile);
  }
}

void Predator::Reset(const TimeClass* const TimeInfo) {
  Suitable->Reset(this, TimeInfo);
}

void Predator::DeleteParametersForPrey(int p, Keeper* const keeper) {
  /* This class has set a data invariant for the protected variables
   * that derived classes are to keep.
   * This function bases its action on that data invariant.
   * Derived classes that need to delete information when a prey is
   * deleted should do so in this function.
   * Explanation of parameters: int p is the number of the element
   * of this->Preyname() that keeps the name of the prey that is to be deleted.*/
  if (preys.Size() > 0)
    preys.Delete(p);
  Suitable->DeletePrey(p, keeper);
}

void Predator::resizeObjects() {
}

void Predator::setSuitability(const Suits* const S, Keeper* const keeper) {
  if (Suitable != 0)
    handle.logFailure("Error in predator - repeated suitability values");
  Suitable = new Suits(*S, keeper);
}

int Predator::readSuitabilityMatrix(CommentStream& infile,
  const char* FinalString, const TimeClass* const TimeInfo, Keeper* const keeper) {

  // the suitability matrix has each line either
  //    1. nameofprey nameoffunction vectorofdouble
  // or 2. nameofprey nameoffilecontainingmatrix constant

  Suitable = new Suits();
  SuitFuncPtrVector suitf;
  int i, j;
  char preyname[MaxStrLength];
  char text[MaxStrLength];
  strncpy(preyname, "", MaxStrLength);
  strncpy(text, "", MaxStrLength);
  keeper->addString("suitabilityfor");

  infile >> preyname >> ws;
  while (!(strcasecmp(preyname, FinalString) == 0) && infile.good()) {
    keeper->addString(preyname);
    infile >> text >> ws;

    if (strcasecmp(text, "function") == 0) {
      infile >> text;
      if (readSuitFunction(suitf, infile, text, TimeInfo, keeper) == 1)
        Suitable->addPrey(preyname, suitf[suitf.Size() - 1]);
      else
        handle.Message("Error in suitability - unrecognised suitability function");

    } else if (strcasecmp(text, "suitfile") == 0) {
      infile >> text;

      DoubleMatrix dm;
      ifstream subfile;
      CommentStream subcomment(subfile);
      subfile.open(text, ios::in);
      handle.checkIfFailure(subfile, text);
      handle.Open(text);
      i = 0;
      while (!subfile.eof()) {
        DoubleVector dv;
        if (!readVectorInLine(subcomment, dv))
          handle.Message("Error in suitability matrix - failed to read data");

        dm.AddRows(1, dv.Size());
        for (j = 0; j < dv.Size(); j++)
          dm[i][j] = dv[j];
        subfile >> ws;
        i++;
      }
      handle.logMessage("Read suitability matrix data file - number of entries", i);
      handle.Close();
      subfile.close();
      subfile.clear();

      Formula multiplication;
      if (!(infile >> multiplication))
        handle.Message("Incorrect format of suitability multiplication factor");
      multiplication.Inform(keeper);
      if (multiplication <= 0)
        handle.Message("Incorrect format of suitability multiplication factor");
      Suitable->addPrey(preyname, multiplication, dm, keeper);

    } else
      handle.Message("Error in suitability - unrecognised format", text);

    infile >> preyname >> ws;
    keeper->clearLast();
  }

  if (!infile.good())
    handle.Failure();

  keeper->clearLast();
  return 1;
}
