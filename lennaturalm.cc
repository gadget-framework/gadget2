#include "lennaturalm.h"
#include "readfunc.h"
#include "areatime.h"
#include "errorhandler.h"
#include "keeper.h"
#include "gadget.h"

extern ErrorHandler handle;

//Example of  format of infile: 0.15#1 0.13#2 0.05#3 45 130
LenNaturalM::LenNaturalM(CommentStream& infile, const LengthGroupDivision* lenp, Keeper* const keeper)
  : parammort(3), xparammort(2, 0.0), natmort(lenp->numLengthGroups(), 0.0) {

  keeper->addString("lennaturalm");
  lengroup = new LengthGroupDivision(*lenp);
  //AJ 25.10.00 Adding error check for reading parammort
  if (!(infile >> parammort))
    handle.Message("Incorrect format of parammort vector when reading lennaturalm");
  parammort.Inform(keeper);

  if (!readVector(infile, xparammort))
    handle.Message("Incorrect format of xparammort vector when reading lennaturalm");

  this->NatCalc();
  keeper->clearLast();
}

//calculates natural mortality natmort
//(based on a combination of  a/(b + len) and constant mort.)
void LenNaturalM::NatCalc() {
  int i;
  for (i = 0; i < lengroup->numLengthGroups(); i++)
    if (lengroup->meanLength(i) < xparammort[0])
      natmort[i] = Hyperbola(lengroup->minLength(), xparammort[0],
        parammort[0], parammort[2], lengroup->meanLength(i));
    else if (lengroup->meanLength(i) <= xparammort[1])
      natmort[i] = parammort[2];
    else
      natmort[i] = Hyperbola(xparammort[1], lengroup->maxLength(),
        parammort[2], parammort[1], lengroup->meanLength(i));
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
