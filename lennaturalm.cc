#include "lennaturalm.h"
#include "readfunc.h"
#include "areatime.h"
#include "errorhandler.h"
#include "keeper.h"
#include "gadget.h"

//Example of  format of infile: 0.15#1 0.13#2 0.05#3 45 130
LenNaturalM::LenNaturalM(CommentStream& infile, const LengthGroupDivision* lenp, Keeper* const keeper)
  : parammort(3), xparammort(2, 0.0), natmort(lenp->NoLengthGroups(), 0.0) {

  ErrorHandler handle;
  keeper->AddString("lennaturalm");
  lengroup = new LengthGroupDivision(*lenp);
  //AJ 25.10.00 Adding error check for reading parammort
  if (!(infile >> parammort))
    handle.Message("Incorrect format of parammort vector when reading lennaturalm");
  parammort.Inform(keeper);
  ReadVector(infile, xparammort);
  this->NatCalc();
  keeper->ClearLast();
}

//calculates natural mortality natmort
//(based on a combination of  a/(b + len) and constant mort.)
void LenNaturalM::NatCalc() {
  int i;
  for (i = 0; i < lengroup->NoLengthGroups(); i++)
    if (lengroup->Meanlength(i) < xparammort[0])
      natmort[i] = Hyperbola(lengroup->Minlength(0), xparammort[0],
        parammort[0], parammort[2], lengroup->Meanlength(i));
    else if (lengroup->Meanlength(i) <= xparammort[1])
      natmort[i] = parammort[2];
    else
      natmort[i] = Hyperbola(xparammort[1], lengroup->Maxlength(lengroup->NoLengthGroups() - 1),
        parammort[2], parammort[1], lengroup->Meanlength(i));
}

//hyperbola (or horizontal line returned)
double LenNaturalM::Hyperbola(double start, double end, double startm, double endm, double len) {
  if (startm == endm || start == end)
    return startm;
  else {
    double a = startm * endm * (start - end);
    double b = (start * startm - end * endm);
    if (b == -len)
      return 0.0;
    else
      return a / (b + len * (endm - startm));
  }
}

void LenNaturalM::Print(ofstream& outfile) {
  int i;
  outfile << "Natural mortality\n";
  for (i = 0; i < natmort.Size(); i++)
    outfile << sep << natmort[i];
  outfile << endl << endl;
}

LenNaturalM::~LenNaturalM() {
  delete lengroup;
}
