#include "conversion.h"
#include "checkconversion.h"
#include "mathfunc.h"
#include "gadget.h"

void ErrorPrintLengthGroupDivision(const LengthGroupDivision* lgrpdiv) {
  int i;
  if (lgrpdiv->dl() != 0) {
    cerr << "Length group with minimum length " << lgrpdiv->Minlength(0) << ", maximum length "
      << lgrpdiv->Maxlength(lgrpdiv->NoLengthGroups() - 1) << " and stepsize " << lgrpdiv->dl();

  } else {
    cerr << "Length group with lengths ";
    for (i = 0; i < lgrpdiv->NoLengthGroups(); i++)
      cerr << lgrpdiv->Minlength(i) << ", ";

    cerr << lgrpdiv->Maxlength(lgrpdiv->NoLengthGroups() - 1);
  }
  cerr << endl;
}

void ErrorPrintLengthGroupDivision(const LengthGroupDivision* finer,
  const LengthGroupDivision* coarser, const char* finername, const char* coarsername) {

  cerr << "Error: Length group divisions for " << finername
    << " is not finer than the length group division for " << coarsername
    << "\nThe length group division for " << finername << " is:\n";
  ErrorPrintLengthGroupDivision(finer);
  cerr << "The length group division for " << coarsername << " is:\n";
  ErrorPrintLengthGroupDivision(coarser);
}

/* returns -1 if algorithm fails. Should never happen, but Murphys law ...
 * returns 0 if finer is not finer than coarser and -1 and 2 did not happen.
 * returns 1 if finer is indeed finer than coarser.
 * returns 2 if comparison failed -- e.g. the intersection is empty,...
 * If finer is not finer than coarser, BogusLengthGroup will have
 * changed after the function has returned, and keep the number of a
 * length group in coarser that is not wholly contained in a single
 * length group in coarser.*/

int LengthGroupIsFiner(const LengthGroupDivision* finer,
  const LengthGroupDivision* coarser, int& BogusLengthGroup) {

  if (coarser->NoLengthGroups() == 0 || finer->NoLengthGroups() == 0)
    return 2;
  int c = 0;
  const int cmax = coarser->NoLengthGroups() - 1;
  double allowederror = (coarser->Maxlength(cmax) - coarser->Minlength(0)) *
    rathersmall / coarser->NoLengthGroups();

  int f = 0;
  int fmin = 0;
  int fmax = finer->NoLengthGroups() - 1;
  double minlength = max(coarser->Minlength(0), finer->Minlength(0));
  double maxlength = min(coarser->Maxlength(cmax), finer->Maxlength(fmax));

  //check to see if the intersection is empty
  if (minlength - allowederror >= maxlength)
    return 2;

  if (minlength - allowederror > finer->Minlength(0))
    if (absolute(minlength -
       finer->Minlength(finer->NoLengthGroup(minlength))) > allowederror) {
      BogusLengthGroup = finer->NoLengthGroup(minlength);
      return 0;
    }

  if (maxlength + allowederror < finer->Maxlength(fmax))
    if (absolute(maxlength -
       finer->Minlength(finer->NoLengthGroup(maxlength))) > allowederror) {
      BogusLengthGroup = finer->NoLengthGroup(maxlength);
      return 0;
    }

  fmin = finer->NoLengthGroup(minlength);
  fmax = finer->NoLengthGroup(maxlength);

  //if fmax is finer is not in the intersection of the length groups
  if (absolute(maxlength - finer->Minlength(fmax)) < allowederror)
    fmax--;

  //fmin is the first length group in finer that is wholly contained
  //in the intersection of finer and coarser.
  //fmax is the last length group in finer in the intersection
  for (f = fmin; f <= fmax; f++) {
    c = coarser->NoLengthGroup(finer->Minlength(f));
    if (c < 0)
      return -1;
    if (!(coarser->Minlength(c) - allowederror <= finer->Minlength(f)
        && finer->Maxlength(f) <= coarser->Maxlength(c) + allowederror)) {
      BogusLengthGroup = f;
      return 0;
    }
  }
  return 1;
}

void CheckLengthGroupIsFiner(const LengthGroupDivision* finer,
  const LengthGroupDivision* coarser, const char* finername, const char* coarsername) {

  int BogusLengthGroup = 0;
  int isfiner = LengthGroupIsFiner(finer, coarser, BogusLengthGroup);
  switch(isfiner) {
    case -1:
      cerr << "Error: the algorithm in lengthgroupisfiner does not work.\n"
        << "The length group division for " << finername << " is:\n";
      ErrorPrintLengthGroupDivision(finer);
      cerr << "The length group division for " << coarsername << " is:\n";
      ErrorPrintLengthGroupDivision(coarser);
      exit(EXIT_FAILURE);
    case 0:
      cerr << "Error in length group " << BogusLengthGroup << " for " << finername << endl;
      ErrorPrintLengthGroupDivision(finer, coarser, finername, coarsername);
      exit(EXIT_FAILURE);
    case 1:
      return;
    case 2:
      cerr << "Warning - empty intersection when checking length groups\n";
      return;
    default:
      cerr << "Error when comparing length group divisions.\n"
        << "Unrecognized return code " << isfiner << " from IsLengthGroupFiner.\n"
        << "Please find someone to blame for this.\n";
      ErrorPrintLengthGroupDivision(finer, coarser, finername, coarsername);
      exit(EXIT_FAILURE);
    }
}
