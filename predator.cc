#include "mathfunc.h"
#include "keeper.h"
#include "prey.h"
#include "conversion.h"
#include "print.h"
#include "suits.h"
#include "predator.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "gadget.h"

Predator::Predator(const char* givenname, const intvector& Areas)
  : HasName(givenname), LivesOnAreas(Areas), Suitable(0) {
}

Predator::~Predator() {
  int i;
  for (i = 0; i < Suitable->NoFuncPreys(); i++)
    delete Suitable->FuncPrey(i);
  delete Suitable;
}

void Predator::SetPrey(Preyptrvector& preyvec, Keeper* const keeper) {
  assert(preys.Size() == 0);
  //Resize the vector preys and initialize pointers to 0.
  preys.resize(this->NoPreys(), 0);
  int i, j;
  int found = 0;

  for (i = 0; i < preyvec.Size(); i++) {
    found = 0;
    for (j = 0; j < this->NoPreys(); j++) {
      if (strcasecmp(this->Preyname(j), preyvec[i]->Name()) == 0) {
        if (found == 0) {
          preys[j] = preyvec[i];
          found = 1;
        } else {
          cerr << "Error: It seems as if the predator " << this->Name()
            << "\nhas read suitability for "
            << (const char*)(preyvec[i]->Name()) << " twice.\n";
          exit(EXIT_FAILURE);
        }
      }
    }
  }

  for (i = 0; i < preys.Size(); i++)
    //If we find a prey that we have read the suitability for, but not
    //received a pointer to, we issue a warning and delete it through the
    //virtual function DeleteParametersForPrey.
    if (preys[i] == 0) {
      cerr << "Warning: The predator " << this->Name()  << " read suitability for "
        << this->Preyname(i) << "\nwhich was not found in the input files.\n";
      this->DeleteParametersForPrey(i, keeper);
      //This function allows derived classes to delete the information they keep.
      //Since we have deleted element no. i from the vectors, we must take
      //care not to miss the element that is now no. i
      i--;
    }
  //Now we can resize objects according to the size of this->NoPreys().
  this->ResizeObjects();
}

int Predator::DoesEat(const char* preyname) const {
  int found = 0;
  int i;
  for (i = 0; i < this->NoPreys(); i++)
    if (strcasecmp(this->Preyname(i), preyname) == 0)
      found = 1;
  return found;
}

void Predator::Print(ofstream& outfile) const {
  int i = 0;
  outfile << "Predator\n\tname" << sep << this->Name()
    << "\n\tRead names of preys:";
  for (i = 0; i < this->NoPreys(); i++)
    outfile << sep << this->Preyname(i);
  outfile << "\n\tNames of preys (through pointers):";
  for (i = 0; i < preys.Size(); i++)
    outfile << sep << (const char*)(this->Preys(i)->Name());
  outfile << endl;
  for (i = 0; i < this->NoPreys(); i++) {
    outfile << "\tSuitability for " << this->Preyname(i) << endl;
    BandmatrixPrint(this->Suitability(i), outfile);
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

void Predator::ResizeObjects() {
}

void Predator::SetSuitability(const Suits* const S, Keeper* const keeper) {
  if (Suitable != 0) {
    cerr << "Error in predator " << this->Name()
      << "\nTrying to set suitability twice\n";
    exit(EXIT_FAILURE);
  }

  Suitable = new Suits(*S, keeper);
}

int Predator::ReadSuitabilityMatrix(CommentStream& infile,
  const char* FinalString, const TimeClass* const TimeInfo, Keeper* const keeper) {

  // the suitability matrix has each line either
  //    1. nameofprey nameoffunction vectorofdouble
  // or 2. nameofprey nameoffilecontainingmatrix constant

  Suitable = new Suits();
  SuitfuncPtrvector suitf;
  ErrorHandler handle;
  int i, j;
  char preyname[MaxStrLength];
  char text[MaxStrLength];
  strncpy(preyname, "", MaxStrLength);
  strncpy(text, "", MaxStrLength);

  if (!infile.good())
    handle.Failure("suitability");

  keeper->AddString("suitabilityfor");
  //The next line is a cheap trick so we can call ClearLastAddString later.
  keeper->AddString("FakeString");

  i = 0;
  infile >> preyname >> ws;
  while (!(strcasecmp(preyname, FinalString) == 0) && infile.good()) {
    keeper->ClearLastAddString(preyname);
    infile >> text >> ws;

    if (strcasecmp(text, "function") == 0) {
      infile >> text;
      if (readSuitFunction(suitf, infile, text, TimeInfo, keeper) == 1) {
        i = suitf.Size();
        Suitable->AddPrey(preyname, suitf[i - 1]);

      } else
        handle.Message("Error in suitability - unknown functionname");

    } else if (strcasecmp(text, "suitfile") == 0) {
      infile >> text;
      ifstream subfile(text);
      CommentStream subcomment(subfile);
      CheckIfFailure(subfile, text);
      handle.Open(text);
      doublematrix dm;
      Formula multiplication;
      while (!subfile.eof()) {
        doublevector dv;
        if (!ReadVectorInLine(subcomment, dv))
          handle.Message("Error in suitability matrix - failed to read data");

        dm.AddRows(1, dv.Size());
        for (j = 0; j < dv.Size(); j++)
          dm[i][j] = dv[j];
        subfile >> ws;
        i++;
      }
      handle.Close();
      subfile.close();
      subfile.clear();
      if (!(infile >> multiplication))
        handle.Message("Incorrect format of suitability multiplication factor");
      multiplication.Inform(keeper);
      if (multiplication <= 0)
        handle.Message("Incorrect format of suitability multiplication factor");
      Suitable->AddPrey(preyname, multiplication, dm, keeper);

    } else
      handle.Message("Error in suitability - unknown format");

    infile >> preyname >> ws;
    if (infile.fail())
      handle.Failure("suitabilities");
  }
  keeper->ClearLast();
  keeper->ClearLast();
  return 1;
}
