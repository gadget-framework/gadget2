#include "growthcalc.h"
#include "errorhandler.h"
#include "areatime.h"
#include "keeper.h"
#include "readfunc.h"
#include "readword.h"
#include "conversion.h"
#include "gadget.h"

// ********************************************************
// Functions for GrowthCalcBase
// ********************************************************
GrowthCalcBase::GrowthCalcBase(const IntVector& Areas) : LivesOnAreas(Areas) {
}

GrowthCalcBase::~GrowthCalcBase() {
}

// ********************************************************
// Functions for GrowthCalcA
// ********************************************************
GrowthCalcA::GrowthCalcA(CommentStream& infile,
  const IntVector& Areas, Keeper* const keeper)
  : GrowthCalcBase(Areas), NumberOfGrowthConstants(9) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  growthPar.resize(NumberOfGrowthConstants, keeper);
  //If Growthpar is an empty vector then only make size of vector
  //in Growthpar == NumberOfGrowthConstants else ...
  //Maybe should make a new function resize which demands an emtpy vector.
  keeper->AddString("growthcalcA");
  infile >> text;
  if (strcasecmp(text, "growthparameters") == 0) {
    if (!(infile >> growthPar))
      handle.Message("Incorrect format of growthpar vector");
    growthPar.Inform(keeper);
  } else
    handle.Unexpected("growthparameters", text);
  keeper->ClearLast();
}

GrowthCalcA::~GrowthCalcA() {
}

void GrowthCalcA::GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& GrEatNumber, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) const {

  const double stepsize = TimeInfo->LengthOfCurrent() / TimeInfo->LengthOfYear();
  const double Temperature = Area->Temperature(area, TimeInfo->CurrentTime());

  int i;
  double tempL, tempW;
  tempL = stepsize * growthPar[0] * (growthPar[2] * Temperature + growthPar[3]);
  tempW = stepsize * growthPar[4] * (growthPar[7] * Temperature + growthPar[8]);

  for (i = 0; i < Lgrowth.Size(); i++) {
    Lgrowth[i] = tempL * pow(LgrpDiv->Meanlength(i), growthPar[1]) * Fphi[i];
    if (Lgrowth[i] < 0)
      Lgrowth[i] = 0;

    if (GrEatNumber[i].W <= rathersmall)
      Wgrowth[i] = 0;
    else {
      Wgrowth[i] = tempW * pow(GrEatNumber[i].W, growthPar[5]) * (Fphi[i] - growthPar[6]);
    }
  }
}

// ********************************************************
// Functions for GrowthCalcB
// ********************************************************
GrowthCalcB::GrowthCalcB(CommentStream& infile, const IntVector& Areas,
  const TimeClass* const TimeInfo, Keeper* const keeper,
  const AreaClass* const Area, const CharPtrVector& lenindex)
  : GrowthCalcBase(Areas), lgrowth(Areas.Size()), wgrowth(Areas.Size()) {

  ErrorHandler handle;
  char datafilename[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  int i;
  Formula tempF;   //value of tempF is initiated to 0.0
  for (i = 0; i < Areas.Size(); i++) {
    lgrowth[i] = new FormulaMatrix(TimeInfo->TotalNoSteps() + 1, lenindex.Size(), tempF);
    wgrowth[i] = new FormulaMatrix(TimeInfo->TotalNoSteps() + 1, lenindex.Size(), tempF);
  }

  keeper->AddString("growthcalcB");

  readWordAndValue(infile, "lengthgrowthfile", datafilename);
  datafile.open(datafilename);
  checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  i = readGrowthAmounts(subdata, TimeInfo, Area, lgrowth, lenindex, keeper);
  handle.Close();
  datafile.close();
  datafile.clear();

  readWordAndValue(infile, "weightgrowthfile", datafilename);
  datafile.open(datafilename);
  checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  i = readGrowthAmounts(subdata, TimeInfo, Area, wgrowth, lenindex, keeper);
  handle.Close();
  datafile.close();
  datafile.clear();

  for (i = 0; i < Areas.Size(); i++) {
    (*lgrowth[i]).Inform(keeper);
    (*wgrowth[i]).Inform(keeper);
  }

  keeper->ClearLast();
}

GrowthCalcB::~GrowthCalcB() {
  int a;
  for (a = 0; a < lgrowth.Size(); a++) {
    delete lgrowth[a];
    delete wgrowth[a];
  }
}

void GrowthCalcB::GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& GrEatNumber, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) const {

  int i;
  const int inarea = AreaNr[area];
  DoubleVector l((*lgrowth[inarea])[TimeInfo->CurrentTime()].Size());
  DoubleVector w((*wgrowth[inarea])[TimeInfo->CurrentTime()].Size());

  //Initialize l and w with values from vector lgrowth[][] and wgrowth[][].
  for (i = 0; i < l.Size(); i++)
    l[i] = (*lgrowth[inarea])[TimeInfo->CurrentTime()][i];
  for (i = 0; i < w.Size(); i++)
    w[i] = (*wgrowth[inarea])[TimeInfo->CurrentTime()][i];

  for (i = 0; i < Lgrowth.Size(); i++) {
    if ((l[i] < 0.0) || (w[i] < 0.0))
      cerr << "Warning - negative growth parameter\n";

    Lgrowth[i] = l[i];
    Wgrowth[i] = w[i];
  }
}

// ********************************************************
// Functions for GrowthCalcC
// ********************************************************
GrowthCalcC::GrowthCalcC(CommentStream& infile, const IntVector& Areas,
  const LengthGroupDivision* const LgrpDiv, Keeper* const keeper, const char* refWeight)
  : GrowthCalcBase(Areas), NumberOfWGrowthConstants(6),  NumberOfLGrowthConstants(9) {

  int i, j, pos;
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  //If Wgrowthpar is an empty vector then only make size of vector
  //in Wgrowthpar == NumberOfGrowthConstants else ...
  //Maybe should make a new function resize wich demands an emtpy vector.
  wgrowthPar.resize(NumberOfWGrowthConstants, keeper);
  lgrowthPar.resize(NumberOfLGrowthConstants, keeper);
  Wref.resize(LgrpDiv->NoLengthGroups());

  keeper->AddString("growthcalcC");
  infile >> text;
  if (strcasecmp(text, "wgrowthparameters") == 0) {
    if (!(infile >> wgrowthPar))
      handle.Message("Incorrect format of wgrowthpar vector");
    wgrowthPar.Inform(keeper);
  } else
    handle.Unexpected("wgrowthparameters", text);

  infile >> text;
  if (strcasecmp(text, "lgrowthparameters") == 0) {
    if (!(infile >> lgrowthPar))
      handle.Message("Incorrect format of lgrowthpar vector");
    lgrowthPar.Inform(keeper);
  } else
    handle.Unexpected("lgrowthparameters", text);
  keeper->ClearLast();

  //Read information on reference weights.
  keeper->AddString("referenceweights");
  //JMB - changed since filename is passed as refWeight
  ifstream subfile(refWeight, ios::in);
  checkIfFailure(subfile, refWeight);
  handle.Open(refWeight);
  CommentStream subcomment(subfile);

  //Read information on length increase.
  DoubleMatrix tmpRefW;
  if (!read2ColVector(subcomment, tmpRefW))
    handle.Message("Wrong format for reference weights");

  //Interpolate the reference weights. First there are some error checks.
  for (i = 0; i < tmpRefW.Nrow() - 1; i++)
    if ((tmpRefW[i + 1][0] - tmpRefW[i][0]) <= 0)
      handle.Message("Lengths in must be strictly increasing");

  if (LgrpDiv->Meanlength(0) < tmpRefW[0][0] ||
      LgrpDiv->Meanlength(LgrpDiv->NoLengthGroups() - 1) > tmpRefW[tmpRefW.Nrow() - 1][0])
    handle.Message("Lengths must span the range of growth lengths");

  handle.Close();
  subfile.close();
  subfile.clear();

  double ratio;
  pos = 0;
  for (j = pos; j < LgrpDiv->NoLengthGroups(); j++)
    for (i = pos; i < tmpRefW.Nrow() - 1; i++)
      if (LgrpDiv->Meanlength(j) >= tmpRefW[i][0] && LgrpDiv->Meanlength(j) <= tmpRefW[i + 1][0]) {
        ratio = (LgrpDiv->Meanlength(j) - tmpRefW[i][0]) / (tmpRefW[i + 1][0] - tmpRefW[i][0]);
        Wref[j] = tmpRefW[i][1] + ratio * (tmpRefW[i + 1][1] - tmpRefW[i][1]);
        pos = i;
      }

  keeper->ClearLast();
}

GrowthCalcC::~GrowthCalcC() {
}

/* Von Bertalanffy growth function. dw/dt = a*w^n - b*w^m;
 * As a generalisation a and b are made temperature dependent so the
 * final form of the function is
 * dw/dt = a0*exp(a1*T)*((w/a2)^a4 - (w/a3)^a5)
 * For no temperature dependency a1 = 0 */
void GrowthCalcC::GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& GrEatNumber, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) const {

  const double stepsize =  TimeInfo->LengthOfCurrent() / TimeInfo->LengthOfYear();
  const double Temperature = Area->Temperature(area, TimeInfo->CurrentTime());
  const double ratio = lgrowthPar[0] + lgrowthPar[8] * (lgrowthPar[1] + lgrowthPar[2] * lgrowthPar[8]);
  const double tempW = stepsize * wgrowthPar[0] * exp(wgrowthPar[1] * Temperature);

  int i;
  double x, fx;

  //JMB - first some error checking
  if (isZero(wgrowthPar[2]) || isZero(wgrowthPar[3]))
    cerr << "Warning - weight growth parameter is zero\n";
  if (isZero(lgrowthPar[6]) || isZero(lgrowthPar[7]))
    cerr << "Warning - length growth parameter is zero\n";
  if (lgrowthPar[5] < 0)
    cerr << "Warning - length growth parameter is negative\n";

  for (i = 0; i < Wgrowth.Size(); i++) {
    if (isZero(GrEatNumber[i].W)) {
      Wgrowth[i] = 0.0;
      Lgrowth[i] = 0.0;
    } else {
      Wgrowth[i] = tempW * (pow(GrEatNumber[i].W / wgrowthPar[2], wgrowthPar[4]) -
        pow(GrEatNumber[i].W / wgrowthPar[3], wgrowthPar[5]));

      //Here after the code is similar as for GrowthCalcD except an extra
      //parameter Lgrowthpar[8] comes instead of Fphi[i]
      if (Wgrowth[i] <= 0) {
        Wgrowth[i] = 0.0;
        Lgrowth[i] = 0.0;
      } else {
        x = (GrEatNumber[i].W - ratio * Wref[i]) / GrEatNumber[i].W;
        fx = lgrowthPar[3] + lgrowthPar[4] * x;
        if (fx < 0)
          fx = 0.0;
        if (fx > lgrowthPar[5])
          fx = lgrowthPar[5];
        Lgrowth[i] = fx * Wgrowth[i] / (lgrowthPar[6] * lgrowthPar[7] *
          pow(LgrpDiv->Meanlength(i), lgrowthPar[7] - 1.0));
      }
    }
  }
}

// ********************************************************
// Functions for GrowthCalcD
// ********************************************************
GrowthCalcD::GrowthCalcD(CommentStream& infile, const IntVector& Areas,
  const LengthGroupDivision* const LgrpDiv, Keeper* const keeper, const char* refWeight)
  : GrowthCalcBase(Areas), NumberOfWGrowthConstants(6), NumberOfLGrowthConstants(8) {

  int i, j, pos;
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  //If Wgrowthpar is an empty vector then only make size of vector
  //in Wgrowthpar == NumberOfGrowthConstants else ...
  //Maybe should make a new function resize wich demands an emtpy vector.
  wgrowthPar.resize(NumberOfWGrowthConstants, keeper);
  lgrowthPar.resize(NumberOfLGrowthConstants, keeper);
  Wref.resize(LgrpDiv->NoLengthGroups());

  keeper->AddString("growthcalcD");
  infile >> text;
  if (strcasecmp(text, "wgrowthparameters") == 0) {
    if (!(infile >> wgrowthPar))
      handle.Message("Incorrect format of wgrowthpar vector");
    wgrowthPar.Inform(keeper);
  } else
    handle.Unexpected("wgrowthparameters", text);

  infile >> text;
  if (strcasecmp(text, "lgrowthparameters") == 0) {
    if (!(infile >> lgrowthPar))
      handle.Message("Incorrect format of lgrowthpar vector");
    lgrowthPar.Inform(keeper);
  } else
    handle.Unexpected("lgrowthparameters", text);
  keeper->ClearLast();

  //Read information on reference weights.
  keeper->AddString("referenceweights");
  //JMB - changed since filename is passed as refWeight
  ifstream subfile(refWeight, ios::in);
  checkIfFailure(subfile, refWeight);
  handle.Open(refWeight);
  CommentStream subcomment(subfile);

  //Read information on length increase.
  DoubleMatrix tmpRefW;
  if (!read2ColVector(subcomment, tmpRefW))
    handle.Message("Wrong format for reference weights");

  //Interpolate the reference weights. First there are some error checks.
  for (i = 0; i < tmpRefW.Nrow() - 1; i++)
    if ((tmpRefW[i + 1][0] - tmpRefW[i][0]) <= 0)
      handle.Message("Lengths must be strictly increasing");

  if (LgrpDiv->Meanlength(0) < tmpRefW[0][0] ||
      LgrpDiv->Meanlength(LgrpDiv->NoLengthGroups() - 1) > tmpRefW[tmpRefW.Nrow() - 1][0])
    handle.Message("Lengths must span the range of growth lengths");

  handle.Close();
  subfile.close();
  subfile.clear();

  double ratio;
  pos = 0;
  for (j = pos; j < LgrpDiv->NoLengthGroups(); j++)
    for (i = pos; i < tmpRefW.Nrow() - 1; i++)
      if (LgrpDiv->Meanlength(j) >= tmpRefW[i][0] && LgrpDiv->Meanlength(j) <= tmpRefW[i + 1][0]) {
        ratio = (LgrpDiv->Meanlength(j) - tmpRefW[i][0]) / (tmpRefW[i + 1][0] - tmpRefW[i][0]);
        Wref[j] = tmpRefW[i][1] + ratio * (tmpRefW[i + 1][1] - tmpRefW[i][1]);
        pos = i;
      }

  keeper->ClearLast();
}

GrowthCalcD::~GrowthCalcD() {
}

/* Growth function from Jones 1978.  Found from experiment in captivity.
 * Jones formula only applies to weight increase.  The length increase
 * part is derived from the weight increase part by assuming a formula
 * w = a*l^b. If the weight is below the curve no length increase takes place
 * but instead the weight increases until it reaches the curve. */
void GrowthCalcD::GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& GrEatNumber, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) const {

  const double stepsize =  TimeInfo->LengthOfCurrent() / TimeInfo->LengthOfYear();
  const double Temperature = Area->Temperature(area, TimeInfo->CurrentTime());
  const double tempW = stepsize * exp(wgrowthPar[4] * Temperature + wgrowthPar[5]) * TimeInfo->NrOfSubsteps();

  int i;
  double ratio, fx, x;

  //JMB - first some error checking
  if (isZero(wgrowthPar[0]))
    cerr << "Warning - weight growth parameter is zero\n";
  if (isZero(lgrowthPar[6]) || isZero(lgrowthPar[7]))
    cerr << "Warning - length growth parameter is zero\n";
  if (lgrowthPar[5] < 0)
    cerr << "Warning - length growth parameter is negative\n";

  for (i = 0; i < Wgrowth.Size(); i++) {
    if (isZero(GrEatNumber[i].W)) {
      Wgrowth[i] = 0.0;
      Lgrowth[i] = 0.0;
    } else {
      Wgrowth[i] = Fphi[i] * MaxCon[i] * tempW /
        (wgrowthPar[0] * pow(GrEatNumber[i].W, wgrowthPar[1])) -
        wgrowthPar[2] * pow(GrEatNumber[i].W, wgrowthPar[3]);

      if (Wgrowth[i] <= 0) {
        Wgrowth[i] = 0.0;
        Lgrowth[i] = 0.0;
      } else {
        ratio = lgrowthPar[0] + Fphi[i] * (lgrowthPar[1] + lgrowthPar[2] * Fphi[i]);
        x = (GrEatNumber[i].W - ratio * Wref[i]) / GrEatNumber[i].W;
        fx = lgrowthPar[3] + lgrowthPar[4] * x;
        if (fx < 0)
          fx = 0.0;
        if (fx > lgrowthPar[5])
          fx = lgrowthPar[5];
        Lgrowth[i] = fx * Wgrowth[i] / (lgrowthPar[6] * lgrowthPar[7] *
          pow(LgrpDiv->Meanlength(i), lgrowthPar[7] - 1.0));
      }
    }
  }
}

// ********************************************************
// Functions for GrowthCalcE
// ********************************************************
GrowthCalcE::GrowthCalcE(CommentStream& infile, const IntVector& Areas,
  const TimeClass* const TimeInfo, const LengthGroupDivision* const LgrpDiv,
  Keeper* const keeper, const char* refWeight)
  : GrowthCalcBase(Areas), NumberOfWGrowthConstants(6), NumberOfLGrowthConstants(9) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j, pos;
  //If Wgrowthpar is an empty vector then only make size of vector
  //in Wgrowthpar == NumberOfGrowthConstants else ...
  //Maybe should make a new function resize wich demands an emtpy vector.
  wgrowthPar.resize(NumberOfWGrowthConstants, keeper);
  lgrowthPar.resize(NumberOfLGrowthConstants, keeper);
  Wref.resize(LgrpDiv->NoLengthGroups());

  yearEffect.resize(TimeInfo->LastYear() - TimeInfo->FirstYear() + 1, keeper);
  stepEffect.resize(TimeInfo->StepsInYear(), keeper);
  areaEffect.resize(Areas.Size(), keeper);
  keeper->AddString("growthcalcE");
  infile >> text;
  if (strcasecmp(text, "wgrowthparameters") == 0) {
    if (!(infile >> wgrowthPar))
      handle.Message("Incorrect format of wgrowthpar vector");
    wgrowthPar.Inform(keeper);
  } else
    handle.Unexpected("wgrowthparameters", text);

  infile >> text;
  if (strcasecmp(text, "lgrowthparameters") == 0) {
    if (!(infile >> lgrowthPar))
      handle.Message("Incorrect format of lgrowthpar vector");
    lgrowthPar.Inform(keeper);
  } else
    handle.Unexpected("lgrowthparameters", text);

  infile >> text;
  if (strcasecmp(text, "yeareffect") == 0) {
    if (!(infile >> yearEffect))
      handle.Message("Incorrect format of yeareffect vector");
    yearEffect.Inform(keeper);
  } else
    handle.Unexpected("yeareffect", text);

  infile >> text;
  if (strcasecmp(text, "stepeffect") == 0) {
    if (!(infile >> stepEffect))
      handle.Message("Incorrect format of stepeffect vector");
    stepEffect.Inform(keeper);
  } else
    handle.Unexpected("stepeffect", text);

  infile >> text;
  if (strcasecmp(text, "areaeffect") == 0) {
    if (!(infile >> areaEffect))
      handle.Message("Incorrect format of areaeffect vector");
    areaEffect.Inform(keeper);
  } else
    handle.Unexpected("areaeffect", text);
  keeper->ClearLast();

  //Read information on reference weights.
  keeper->AddString("referenceweights");
  //JMB - changed since filename is passed as refWeight
  ifstream subfile(refWeight, ios::in);
  checkIfFailure(subfile, refWeight);
  handle.Open(refWeight);
  CommentStream subcomment(subfile);

  //Read information on length increase.
  DoubleMatrix tmpRefW;
  if (!read2ColVector(subcomment, tmpRefW))
    handle.Message("Wrong format for reference weights");

  //Some error checks.
  for (i = 0; i < tmpRefW.Nrow() - 1; i++)
    if ((tmpRefW[i + 1][0] - tmpRefW[i][0]) <= 0)
      handle.Message("Lengths must be strictly increasing");

  if (LgrpDiv->Meanlength(0) < tmpRefW[0][0] ||
      LgrpDiv->Meanlength(LgrpDiv->NoLengthGroups() - 1) > tmpRefW[tmpRefW.Nrow() - 1][0])
    handle.Message("Lengths must span the range of growth lengths");

  handle.Close();
  subfile.close();
  subfile.clear();

  //Interpolate the reference weights.
  double ratio;
  pos = 0;
  for (j = pos; j < LgrpDiv->NoLengthGroups(); j++)
    for (i = pos; i < tmpRefW.Nrow() - 1; i++)
      if (LgrpDiv->Meanlength(j) >= tmpRefW[i][0] &&
          LgrpDiv->Meanlength(j) <= tmpRefW[i + 1][0]) {
        ratio = (LgrpDiv->Meanlength(j) - tmpRefW[i][0]) / (tmpRefW[i + 1][0] - tmpRefW[i][0]);
        Wref[j] = tmpRefW[i][1] + ratio * (tmpRefW[i + 1][1] - tmpRefW[i][1]);
        pos = i;
      }

  keeper->ClearLast();
}

GrowthCalcE::~GrowthCalcE(){
}

/* Growthfunction to be tested for capelin.
 * The weight growth here is given by the formula
 * dw/dt = a0*factor(year)*factor(area)*factor(Step)*w^a1 - a2*w^a3
 * This is Von Bertanlanffy equation with possibility to make the growth
 * year, area and Step dependent. 3 vectors were added to the class
 *       YearEffect
 *       AreaEffect
 *       StepEffect
 * Length increase is upgraded in the same way as earlier. */
void GrowthCalcE::GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& GrEatNumber, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) const {

  const double stepsize =  TimeInfo->LengthOfCurrent() / TimeInfo->LengthOfYear();
  const double Temperature = Area->Temperature(area, TimeInfo->CurrentTime());
  const double factor = yearEffect[TimeInfo->CurrentYear() - TimeInfo->FirstYear()]
                        * stepEffect[TimeInfo->CurrentStep() - 1]
                        * areaEffect[AreaNr[area]];
  const double ratio = lgrowthPar[0] + lgrowthPar[8] * (lgrowthPar[1] + lgrowthPar[2] * lgrowthPar[8]);
  const double tempW = factor * stepsize * wgrowthPar[0] * exp(wgrowthPar[1] * Temperature);

  int i;
  double x, fx;

  //JMB - first some error checking
  if (isZero(wgrowthPar[2]) || isZero(wgrowthPar[3]))
    cerr << "Warning - weight growth parameter is zero\n";
  if (isZero(lgrowthPar[6]) || isZero(lgrowthPar[7]))
    cerr << "Warning - length growth parameter is zero\n";
  if (lgrowthPar[5] < 0)
    cerr << "Warning - length growth parameter is negative\n";

  for (i = 0; i < Wgrowth.Size(); i++) {
    if (isZero(GrEatNumber[i].W) || isZero(tempW)) {
      Wgrowth[i] = 0.0;
      Lgrowth[i] = 0.0;
    } else {
      Wgrowth[i] = tempW * (pow(GrEatNumber[i].W / wgrowthPar[2], wgrowthPar[4]) -
        pow(GrEatNumber[i].W / wgrowthPar[3], wgrowthPar[5]));

      if (Wgrowth[i] <= 0) {
        Wgrowth[i] = 0.0;
        Lgrowth[i] = 0.0;
      } else {
        x = (GrEatNumber[i].W - ratio * Wref[i]) / GrEatNumber[i].W;
        fx = lgrowthPar[3] + lgrowthPar[4] * x;
        if (fx < 0)
          fx = 0.0;
        if (fx > lgrowthPar[5])
          fx = lgrowthPar[5];
        Lgrowth[i] = fx * Wgrowth[i] / (lgrowthPar[6] * lgrowthPar[7] *
          pow(LgrpDiv->Meanlength(i), lgrowthPar[7] - 1.0));
      }
    }
  }
}

// ********************************************************
// Functions for GrowthCalcF
// ********************************************************
/* The Norwegian Growthfunction Equations 6 and 7 on page 7 in
 * Description of a multispecies model for the Barents Sea.
 * parameter # 0 corresponds to C4 in the equation etc. */
GrowthCalcF::GrowthCalcF(CommentStream& infile, const IntVector& Areas,
  const TimeClass* const TimeInfo, Keeper* const keeper,
  const AreaClass* const Area, const CharPtrVector& lenindex)
  : GrowthCalcBase(Areas), NumberOfGrowthConstants(1), wgrowth(Areas.Size()) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  keeper->AddString("growthcalcF");

  infile >> text >> ws;
  growthPar.resize(NumberOfGrowthConstants, keeper);
  if (strcasecmp(text, "growthparameters") == 0) {
    if (!(infile >> growthPar))
      handle.Message("Incorrect format for growth parameter");
    growthPar.Inform(keeper);
  } else
    handle.Unexpected("growthparameters", text);

  infile >> text >> ws;
  k_values.resize(TimeInfo->LastYear() - TimeInfo->FirstYear() + 1, keeper);
  if (strcasecmp(text, "kvalues") == 0) {
    if (!(infile >> k_values))
      handle.Message("Incorrect format for growth k values");
    k_values.Inform(keeper);
  } else
    handle.Unexpected("kvalues", text);

  int i;
  Formula tempF;   //value of tempF is initiated to 0.0
  for (i = 0; i < Areas.Size(); i++)
    wgrowth[i] = new FormulaMatrix(TimeInfo->TotalNoSteps() + 1, lenindex.Size(), tempF);

  ifstream datafile;
  CommentStream subdata(datafile);
  readWordAndValue(infile, "weightgrowthfile", text);
  datafile.open(text);
  checkIfFailure(datafile, text);
  handle.Open(text);
  i = readGrowthAmounts(subdata, TimeInfo, Area, wgrowth, lenindex, keeper);
  handle.Close();
  datafile.close();
  datafile.clear();

  for (i = 0; i < Areas.Size(); i++)
    (*wgrowth[i]).Inform(keeper);

  keeper->ClearLast();
}

GrowthCalcF::~GrowthCalcF() {
  int a;
  for (a = 0; a < wgrowth.Size(); a++)
    delete wgrowth[a];
}

void GrowthCalcF::GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& GrEatNumber, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) const {

  const double stepsize =  TimeInfo->LengthOfCurrent() / TimeInfo->LengthOfYear();
  const int inarea = AreaNr[area];
  double kval = k_values[TimeInfo->CurrentYear() - TimeInfo->FirstYear()] * stepsize;

  int i;
  DoubleVector w((*wgrowth[inarea])[TimeInfo->CurrentTime()].Size());
  for (i = 0; i < w.Size(); i++)
    w[i] = (*wgrowth[inarea])[TimeInfo->CurrentTime()][i];

  for (i = 0; i < Lgrowth.Size(); i++) {
    if (w[i] < 0.0)
      cerr << "Warning - negative weight growth parameter\n";

    Lgrowth[i] = (growthPar[0] - LgrpDiv->Meanlength(i)) * (1.0 - exp(-kval));
    Wgrowth[i] = w[i];
  }
}

// ********************************************************
// Functions for GrowthCalcG
// ********************************************************
GrowthCalcG::GrowthCalcG(CommentStream& infile, const IntVector& Areas,
  const TimeClass* const TimeInfo, Keeper* const keeper,
  const AreaClass* const Area, const CharPtrVector& lenindex)
  : GrowthCalcBase(Areas), NumberOfGrowthConstants(1), wgrowth(Areas.Size()) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  keeper->AddString("growthcalcG");

  infile >> text >> ws;
  growthPar.resize(NumberOfGrowthConstants, keeper);
  if (strcasecmp(text, "growthparameters") == 0) {
    if (!(infile >> growthPar))
      handle.Message("Incorrect format for growth parameter");
    growthPar.Inform(keeper);
  } else
    handle.Unexpected("growthparameters", text);

  infile >> text >> ws;
  k_values.resize(TimeInfo->LastYear() - TimeInfo->FirstYear() + 1, keeper);
  if (strcasecmp(text, "kvalues") == 0) {
    if (!(infile >> k_values))
      handle.Message("Incorrect format for growth k values");
    k_values.Inform(keeper);
  } else
    handle.Unexpected("kvalues", text);

  int i;
  Formula tempF;   //value of tempF is initiated to 0.0
  for (i = 0; i < Areas.Size(); i++)
    wgrowth[i] = new FormulaMatrix(TimeInfo->TotalNoSteps() + 1, lenindex.Size(), tempF);

  ifstream datafile;
  CommentStream subdata(datafile);
  readWordAndValue(infile, "weightgrowthfile", text);
  datafile.open(text);
  checkIfFailure(datafile, text);
  handle.Open(text);
  i = readGrowthAmounts(subdata, TimeInfo, Area, wgrowth, lenindex, keeper);
  handle.Close();
  datafile.close();
  datafile.clear();

  for (i = 0; i < Areas.Size(); i++)
    (*wgrowth[i]).Inform(keeper);

  keeper->ClearLast();
}

GrowthCalcG::~GrowthCalcG() {
  int a;
  for (a = 0; a < wgrowth.Size(); a++)
    delete wgrowth[a];
}

void GrowthCalcG::GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& GrEatNumber, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) const {

  //written by kgf 24/10 00
  //Gives linear growth (Growthpar[0] == 0) or
  //growth decreasing with length (Growthpar[0] < 0).
  const double stepsize =  TimeInfo->LengthOfCurrent() / TimeInfo->LengthOfYear();
  const int inarea = AreaNr[area];
  double kval = k_values[TimeInfo->CurrentYear() - TimeInfo->FirstYear()] * stepsize;

  int i;
  DoubleVector w((*wgrowth[inarea])[TimeInfo->CurrentTime()].Size());
  for (i = 0; i < w.Size(); i++)
    w[i] = (*wgrowth[inarea])[TimeInfo->CurrentTime()][i];

  if (growthPar[0] > 0)
    cerr << "Warning - growth parameter is positive\n";

  if (isZero(growthPar[0])) {
    for (i = 0; i < Lgrowth.Size(); i++) {
      if (w[i] < 0.0)
        cerr << "Warning - negative weight growth parameter\n";

      Lgrowth[i] = kval;
      Wgrowth[i] = w[i];
    }
  } else {
    for (i = 0; i < Lgrowth.Size(); i++) {
      if (w[i] < 0.0)
        cerr << "Warning - negative weight growth parameter\n";

      Lgrowth[i] = kval * pow(LgrpDiv->Meanlength(i), growthPar[0]);
      Wgrowth[i] = w[i];
    }
  }
}

// ********************************************************
// Functions for GrowthCalcH
// ********************************************************
GrowthCalcH::GrowthCalcH(CommentStream& infile, const IntVector& Areas,
  const LengthGroupDivision* const LgrpDiv, Keeper* const keeper)
  : GrowthCalcBase(Areas), NumberOfGrowthConstants(4) {

  int i, j, pos;
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  growthPar.resize(NumberOfGrowthConstants, keeper);

  keeper->AddString("growthcalcH");
  infile >> text;
  //parameters are linf, k and a and b for the weight
  if (strcasecmp(text, "growthparameters") == 0) {
    if (!(infile >> growthPar))
      handle.Message("Incorrect format of growthpar vector");
    growthPar.Inform(keeper);
  } else
    handle.Unexpected("growthparameters", text);

  keeper->ClearLast();
}

GrowthCalcH::~GrowthCalcH() {
}

/* Simplified 2 parameter length based Von Bertalanffy growth function
 * compare with GrowthCalcC for the more complex weight based version */
void GrowthCalcH::GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& GrEatNumber, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) const {

  const double mult = 1.0 - exp(-growthPar[1] * TimeInfo->LengthOfCurrent() / TimeInfo->LengthOfYear());

  //JMB - first some error checking
  if (isZero(growthPar[1]) || isZero(growthPar[2]))
    cerr << "Warning - growth parameter is zero\n";
  if (LgrpDiv->Maxlength(LgrpDiv->NoLengthGroups() - 1) > growthPar[0])
    cerr << "Warning - length greater than length infinity\n";

  int i;
  for (i = 0; i < Wgrowth.Size(); i++) {
    Lgrowth[i] = (growthPar[0] - LgrpDiv->Meanlength(i)) * mult;
    Wgrowth[i] = growthPar[2] * (pow(LgrpDiv->Meanlength(i) + Lgrowth[i], growthPar[3])
      - pow(LgrpDiv->Meanlength(i), growthPar[3]));
  }
}
