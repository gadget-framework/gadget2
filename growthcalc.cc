#include "growthcalc.h"
#include "errorhandler.h"
#include "areatime.h"
#include "keeper.h"
#include "readfunc.h"
#include "readword.h"
#include "gadget.h"

extern ErrorHandler handle;

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
GrowthCalcA::GrowthCalcA(CommentStream& infile, const IntVector& Areas,
  const TimeClass* const TimeInfo, Keeper* const keeper)
  : GrowthCalcBase(Areas), numGrowthConstants(9) {

  keeper->addString("growthcalcA");
  growthPar.resize(numGrowthConstants, keeper);

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text >> ws;
  if (strcasecmp(text, "growthparameters") == 0)
    growthPar.read(infile, TimeInfo, keeper);
  else
    handle.Unexpected("growthparameters", text);
  keeper->clearLast();
}

GrowthCalcA::~GrowthCalcA() {
}

void GrowthCalcA::GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& numGrow, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) {

  growthPar.Update(TimeInfo);
  double stepsize = TimeInfo->LengthOfCurrent() / TimeInfo->LengthOfYear();
  double Temperature = Area->Temperature(area, TimeInfo->CurrentTime());
  double tempL = stepsize * growthPar[0] * (growthPar[2] * Temperature + growthPar[3]);
  double tempW = stepsize * growthPar[4] * (growthPar[7] * Temperature + growthPar[8]);

  int i;
  for (i = 0; i < Lgrowth.Size(); i++) {
    Lgrowth[i] = tempL * pow(LgrpDiv->meanLength(i), growthPar[1]) * Fphi[i];
    if (Lgrowth[i] < 0)
      Lgrowth[i] = 0;

    if (numGrow[i].W < verysmall)
      Wgrowth[i] = 0.0;
    else
      Wgrowth[i] = tempW * pow(numGrow[i].W, growthPar[5]) * (Fphi[i] - growthPar[6]);
  }
}

// ********************************************************
// Functions for GrowthCalcB
// ********************************************************
GrowthCalcB::GrowthCalcB(CommentStream& infile, const IntVector& Areas,
  const TimeClass* const TimeInfo, Keeper* const keeper,
  const AreaClass* const Area, const CharPtrVector& lenindex)
  : GrowthCalcBase(Areas), lgrowth(Areas.Size()), wgrowth(Areas.Size()) {

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

  keeper->addString("growthcalcB");

  readWordAndValue(infile, "lengthgrowthfile", datafilename);
  datafile.open(datafilename, ios::in);
  handle.checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  i = readGrowthAmounts(subdata, TimeInfo, Area, lgrowth, lenindex, keeper);
  handle.Close();
  datafile.close();
  datafile.clear();

  readWordAndValue(infile, "weightgrowthfile", datafilename);
  datafile.open(datafilename, ios::in);
  handle.checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  i = readGrowthAmounts(subdata, TimeInfo, Area, wgrowth, lenindex, keeper);
  handle.Close();
  datafile.close();
  datafile.clear();

  for (i = 0; i < Areas.Size(); i++) {
    (*lgrowth[i]).Inform(keeper);
    (*wgrowth[i]).Inform(keeper);
  }

  keeper->clearLast();
}

GrowthCalcB::~GrowthCalcB() {
  int a;
  for (a = 0; a < lgrowth.Size(); a++) {
    delete lgrowth[a];
    delete wgrowth[a];
  }
}

void GrowthCalcB::GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& numGrow, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) {

  int i;
  int inarea = AreaNr[area];
  DoubleVector l((*lgrowth[inarea])[TimeInfo->CurrentTime()].Size());
  DoubleVector w((*wgrowth[inarea])[TimeInfo->CurrentTime()].Size());

  //Initialise l and w with values from vector lgrowth[][] and wgrowth[][].
  for (i = 0; i < l.Size(); i++)
    l[i] = (*lgrowth[inarea])[TimeInfo->CurrentTime()][i];
  for (i = 0; i < w.Size(); i++)
    w[i] = (*wgrowth[inarea])[TimeInfo->CurrentTime()][i];

  for (i = 0; i < Lgrowth.Size(); i++) {
    if ((l[i] < 0.0) || (w[i] < 0.0))
      handle.logWarning("Warning in growth calculation - negative growth parameter");

    Lgrowth[i] = l[i];
    Wgrowth[i] = w[i];
  }
}

// ********************************************************
// Functions for GrowthCalcC
// ********************************************************
GrowthCalcC::GrowthCalcC(CommentStream& infile, const IntVector& Areas,
  const TimeClass* const TimeInfo, const LengthGroupDivision* const LgrpDiv,
  Keeper* const keeper, const char* refWeightFile)
  : GrowthCalcBase(Areas), numWeightGrowthConstants(6),  numLengthGrowthConstants(9) {

  keeper->addString("growthcalcC");
  wgrowthPar.resize(numWeightGrowthConstants, keeper);
  lgrowthPar.resize(numLengthGrowthConstants, keeper);
  refWeight.resize(LgrpDiv->NoLengthGroups());

  int i, j;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text >> ws;
  if (strcasecmp(text, "wgrowthparameters") == 0)
    wgrowthPar.read(infile, TimeInfo, keeper);
  else
    handle.Unexpected("wgrowthparameters", text);

  infile >> text >> ws;
  if (strcasecmp(text, "lgrowthparameters") == 0)
    lgrowthPar.read(infile, TimeInfo, keeper);
  else
    handle.Unexpected("lgrowthparameters", text);

  //read information on reference weights.
  keeper->addString("referenceweights");
  //JMB - changed since filename is passed as refWeight
  ifstream subfile;
  subfile.open(refWeightFile, ios::in);
  handle.checkIfFailure(subfile, refWeightFile);
  handle.Open(refWeightFile);
  CommentStream subcomment(subfile);

  //read information on length increase.
  DoubleMatrix tmpRefW;
  if (!readRefWeights(subcomment, tmpRefW))
    handle.Message("Wrong format for reference weights");
  handle.Close();
  subfile.close();
  subfile.clear();

  //Interpolate the reference weights. First there are some error checks.
  if (LgrpDiv->meanLength(0) < tmpRefW[0][0] ||
      LgrpDiv->meanLength(LgrpDiv->NoLengthGroups() - 1) > tmpRefW[tmpRefW.Nrow() - 1][0])
    handle.Message("Lengths for reference weights must span the range of growth lengths");

  double ratio;
  int pos = 0;
  for (j = pos; j < LgrpDiv->NoLengthGroups(); j++)
    for (i = pos; i < tmpRefW.Nrow() - 1; i++)
      if (LgrpDiv->meanLength(j) >= tmpRefW[i][0] && LgrpDiv->meanLength(j) <= tmpRefW[i + 1][0]) {
        ratio = (LgrpDiv->meanLength(j) - tmpRefW[i][0]) / (tmpRefW[i + 1][0] - tmpRefW[i][0]);
        refWeight[j] = tmpRefW[i][1] + ratio * (tmpRefW[i + 1][1] - tmpRefW[i][1]);
        pos = i;
      }

  keeper->clearLast();
  keeper->clearLast();
}

GrowthCalcC::~GrowthCalcC() {
}

/* Von Bertalanffy growth function. dw/dt = a*w^n - b*w^m;
 * As a generalisation a and b are made temperature dependent so the
 * final form of the function is
 * dw/dt = a0*exp(a1*T)*((w/a2)^a4 - (w/a3)^a5)
 * For no temperature dependency a1 = 0 */
void GrowthCalcC::GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& numGrow, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) {

  wgrowthPar.Update(TimeInfo);
  lgrowthPar.Update(TimeInfo);
  double stepsize = TimeInfo->LengthOfCurrent() / TimeInfo->LengthOfYear();
  double Temperature = Area->Temperature(area, TimeInfo->CurrentTime());
  double ratio = lgrowthPar[0] + lgrowthPar[8] * (lgrowthPar[1] + lgrowthPar[2] * lgrowthPar[8]);
  double tempW = stepsize * wgrowthPar[0] * exp(wgrowthPar[1] * Temperature);

  int i;
  double x, fx;

  //JMB - first some error checking
  if (isZero(wgrowthPar[2]) || isZero(wgrowthPar[3]))
    handle.logWarning("Warning in growth calculation - weight growth parameter is zero");
  if (isZero(lgrowthPar[6]) || isZero(lgrowthPar[7]))
    handle.logWarning("Warning in growth calculation - length growth parameter is zero");
  if (lgrowthPar[5] < 0)
    handle.logWarning("Warning in growth calculation - length growth parameter is negative");

  for (i = 0; i < Wgrowth.Size(); i++) {
    if (numGrow[i].W < verysmall || isZero(tempW)) {
      Wgrowth[i] = 0.0;
      Lgrowth[i] = 0.0;
    } else {
      Wgrowth[i] = tempW * (pow(numGrow[i].W / wgrowthPar[2], wgrowthPar[4]) -
        pow(numGrow[i].W / wgrowthPar[3], wgrowthPar[5]));

      //Here after the code is similar as for GrowthCalcD except an extra
      //parameter Lgrowthpar[8] comes instead of Fphi[i]
      if (Wgrowth[i] < verysmall) {
        Wgrowth[i] = 0.0;
        Lgrowth[i] = 0.0;
      } else {
        x = (numGrow[i].W - ratio * refWeight[i]) / numGrow[i].W;
        fx = lgrowthPar[3] + lgrowthPar[4] * x;
        if (fx < verysmall)
          fx = 0.0;
        if (fx > lgrowthPar[5])
          fx = lgrowthPar[5];
        Lgrowth[i] = fx * Wgrowth[i] / (lgrowthPar[6] * lgrowthPar[7] *
          pow(LgrpDiv->meanLength(i), lgrowthPar[7] - 1.0));
      }
    }
  }
}

// ********************************************************
// Functions for GrowthCalcD
// ********************************************************
GrowthCalcD::GrowthCalcD(CommentStream& infile, const IntVector& Areas,
  const TimeClass* const TimeInfo, const LengthGroupDivision* const LgrpDiv,
  Keeper* const keeper, const char* refWeightFile)
  : GrowthCalcBase(Areas), numWeightGrowthConstants(6), numLengthGrowthConstants(8) {

  keeper->addString("growthcalcD");
  wgrowthPar.resize(numWeightGrowthConstants, keeper);
  lgrowthPar.resize(numLengthGrowthConstants, keeper);
  refWeight.resize(LgrpDiv->NoLengthGroups());

  int i, j;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text >> ws;
  if (strcasecmp(text, "wgrowthparameters") == 0)
    wgrowthPar.read(infile, TimeInfo, keeper);
  else
    handle.Unexpected("wgrowthparameters", text);

  infile >> text >> ws;
  if (strcasecmp(text, "lgrowthparameters") == 0)
    lgrowthPar.read(infile, TimeInfo, keeper);
  else
    handle.Unexpected("lgrowthparameters", text);

  //read information on reference weights.
  keeper->addString("referenceweights");
  //JMB - changed since filename is passed as refWeight
  ifstream subfile;
  subfile.open(refWeightFile, ios::in);
  handle.checkIfFailure(subfile, refWeightFile);
  handle.Open(refWeightFile);
  CommentStream subcomment(subfile);

  //read information on length increase.
  DoubleMatrix tmpRefW;
  if (!readRefWeights(subcomment, tmpRefW))
    handle.Message("Wrong format for reference weights");
  handle.Close();
  subfile.close();
  subfile.clear();

  //Interpolate the reference weights. First there are some error checks.
  if (LgrpDiv->meanLength(0) < tmpRefW[0][0] ||
      LgrpDiv->meanLength(LgrpDiv->NoLengthGroups() - 1) > tmpRefW[tmpRefW.Nrow() - 1][0])
    handle.Message("Lengths for reference weights must span the range of growth lengths");

  double ratio;
  int pos = 0;
  for (j = pos; j < LgrpDiv->NoLengthGroups(); j++)
    for (i = pos; i < tmpRefW.Nrow() - 1; i++)
      if (LgrpDiv->meanLength(j) >= tmpRefW[i][0] && LgrpDiv->meanLength(j) <= tmpRefW[i + 1][0]) {
        ratio = (LgrpDiv->meanLength(j) - tmpRefW[i][0]) / (tmpRefW[i + 1][0] - tmpRefW[i][0]);
        refWeight[j] = tmpRefW[i][1] + ratio * (tmpRefW[i + 1][1] - tmpRefW[i][1]);
        pos = i;
      }

  keeper->clearLast();
  keeper->clearLast();
}

GrowthCalcD::~GrowthCalcD() {
}

/* Growth function from Jones 1978.  Found from experiment in captivity.
 * Jones formula only applies to weight increase.  The length increase
 * part is derived from the weight increase part by assuming a formula
 * w = a*l^b. If the weight is below the curve no length increase takes place
 * but instead the weight increases until it reaches the curve. */
void GrowthCalcD::GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& numGrow, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) {

  wgrowthPar.Update(TimeInfo);
  lgrowthPar.Update(TimeInfo);
  double stepsize = TimeInfo->LengthOfCurrent() / TimeInfo->LengthOfYear();
  double Temperature = Area->Temperature(area, TimeInfo->CurrentTime());
  double tempW = stepsize * exp(wgrowthPar[4] * Temperature + wgrowthPar[5]) * TimeInfo->NrOfSubsteps();

  int i;
  double ratio, fx, x;

  //JMB - first some error checking
  if (isZero(wgrowthPar[0]))
    handle.logWarning("Warning in growth calculation - weight growth parameter is zero");
  if (isZero(lgrowthPar[6]) || isZero(lgrowthPar[7]))
    handle.logWarning("Warning in growth calculation - length growth parameter is zero");
  if (lgrowthPar[5] < 0)
    handle.logWarning("Warning in growth calculation - length growth parameter is negative");

  for (i = 0; i < Wgrowth.Size(); i++) {
    if (numGrow[i].W < verysmall || isZero(tempW)) {
      Wgrowth[i] = 0.0;
      Lgrowth[i] = 0.0;
    } else {
      Wgrowth[i] = Fphi[i] * MaxCon[i] * tempW /
        (wgrowthPar[0] * pow(numGrow[i].W, wgrowthPar[1])) -
        wgrowthPar[2] * pow(numGrow[i].W, wgrowthPar[3]);

      if (Wgrowth[i] < verysmall) {
        Wgrowth[i] = 0.0;
        Lgrowth[i] = 0.0;
      } else {
        ratio = lgrowthPar[0] + Fphi[i] * (lgrowthPar[1] + lgrowthPar[2] * Fphi[i]);
        x = (numGrow[i].W - ratio * refWeight[i]) / numGrow[i].W;
        fx = lgrowthPar[3] + lgrowthPar[4] * x;
        if (fx < verysmall)
          fx = 0.0;
        if (fx > lgrowthPar[5])
          fx = lgrowthPar[5];
        Lgrowth[i] = fx * Wgrowth[i] / (lgrowthPar[6] * lgrowthPar[7] *
          pow(LgrpDiv->meanLength(i), lgrowthPar[7] - 1.0));
      }
    }
  }
}

// ********************************************************
// Functions for GrowthCalcE
// ********************************************************
GrowthCalcE::GrowthCalcE(CommentStream& infile, const IntVector& Areas,
  const TimeClass* const TimeInfo, const LengthGroupDivision* const LgrpDiv,
  Keeper* const keeper, const char* refWeightFile)
  : GrowthCalcBase(Areas), numWeightGrowthConstants(6), numLengthGrowthConstants(9) {

  keeper->addString("growthcalcE");
  wgrowthPar.resize(numWeightGrowthConstants, keeper);
  lgrowthPar.resize(numLengthGrowthConstants, keeper);
  refWeight.resize(LgrpDiv->NoLengthGroups());
  yearEffect.resize(TimeInfo->LastYear() - TimeInfo->FirstYear() + 1, keeper);
  stepEffect.resize(TimeInfo->StepsInYear(), keeper);
  areaEffect.resize(Areas.Size(), keeper);

  int i, j;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text >> ws;
  if (strcasecmp(text, "wgrowthparameters") == 0)
    wgrowthPar.read(infile, TimeInfo, keeper);
  else
    handle.Unexpected("wgrowthparameters", text);

  infile >> text >> ws;
  if (strcasecmp(text, "lgrowthparameters") == 0)
    lgrowthPar.read(infile, TimeInfo, keeper);
  else
    handle.Unexpected("lgrowthparameters", text);

  infile >> text >> ws;
  if (strcasecmp(text, "yeareffect") == 0) {
    if (!(infile >> yearEffect))
      handle.Message("Incorrect format of yeareffect vector");
    yearEffect.Inform(keeper);
  } else
    handle.Unexpected("yeareffect", text);

  infile >> text >> ws;
  if (strcasecmp(text, "stepeffect") == 0) {
    if (!(infile >> stepEffect))
      handle.Message("Incorrect format of stepeffect vector");
    stepEffect.Inform(keeper);
  } else
    handle.Unexpected("stepeffect", text);

  infile >> text >> ws;
  if (strcasecmp(text, "areaeffect") == 0) {
    if (!(infile >> areaEffect))
      handle.Message("Incorrect format of areaeffect vector");
    areaEffect.Inform(keeper);
  } else
    handle.Unexpected("areaeffect", text);

  //read information on reference weights.
  keeper->addString("referenceweights");
  //JMB - changed since filename is passed as refWeight
  ifstream subfile;
  subfile.open(refWeightFile, ios::in);
  handle.checkIfFailure(subfile, refWeightFile);
  handle.Open(refWeightFile);
  CommentStream subcomment(subfile);

  //read information on length increase.
  DoubleMatrix tmpRefW;
  if (!readRefWeights(subcomment, tmpRefW))
    handle.Message("Wrong format for reference weights");
  handle.Close();
  subfile.close();
  subfile.clear();

  //Interpolate the reference weights.
  if (LgrpDiv->meanLength(0) < tmpRefW[0][0] ||
      LgrpDiv->meanLength(LgrpDiv->NoLengthGroups() - 1) > tmpRefW[tmpRefW.Nrow() - 1][0])
    handle.Message("Lengths for reference weights must span the range of growth lengths");

  double ratio;
  int pos = 0;
  for (j = pos; j < LgrpDiv->NoLengthGroups(); j++)
    for (i = pos; i < tmpRefW.Nrow() - 1; i++)
      if (LgrpDiv->meanLength(j) >= tmpRefW[i][0] &&
          LgrpDiv->meanLength(j) <= tmpRefW[i + 1][0]) {
        ratio = (LgrpDiv->meanLength(j) - tmpRefW[i][0]) / (tmpRefW[i + 1][0] - tmpRefW[i][0]);
        refWeight[j] = tmpRefW[i][1] + ratio * (tmpRefW[i + 1][1] - tmpRefW[i][1]);
        pos = i;
      }

  keeper->clearLast();
  keeper->clearLast();
}

GrowthCalcE::~GrowthCalcE() {
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
  const PopInfoVector& numGrow, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) {

  wgrowthPar.Update(TimeInfo);
  lgrowthPar.Update(TimeInfo);
  double stepsize = TimeInfo->LengthOfCurrent() / TimeInfo->LengthOfYear();
  double Temperature = Area->Temperature(area, TimeInfo->CurrentTime());
  double factor = yearEffect[TimeInfo->CurrentYear() - TimeInfo->FirstYear()]
                    * stepEffect[TimeInfo->CurrentStep() - 1] * areaEffect[AreaNr[area]];
  double ratio = lgrowthPar[0] + lgrowthPar[8] * (lgrowthPar[1] + lgrowthPar[2] * lgrowthPar[8]);
  double tempW = factor * stepsize * wgrowthPar[0] * exp(wgrowthPar[1] * Temperature);

  int i;
  double x, fx;

  //JMB - first some error checking
  if (isZero(wgrowthPar[2]) || isZero(wgrowthPar[3]))
    handle.logWarning("Warning in growth calculation - weight growth parameter is zero");
  if (isZero(lgrowthPar[6]) || isZero(lgrowthPar[7]))
    handle.logWarning("Warning in growth calculation - length growth parameter is zero");
  if (lgrowthPar[5] < 0)
    handle.logWarning("Warning in growth calculation - length growth parameter is negative");

  for (i = 0; i < Wgrowth.Size(); i++) {
    if (numGrow[i].W < verysmall || isZero(tempW)) {
      Wgrowth[i] = 0.0;
      Lgrowth[i] = 0.0;
    } else {
      Wgrowth[i] = tempW * (pow(numGrow[i].W / wgrowthPar[2], wgrowthPar[4]) -
        pow(numGrow[i].W / wgrowthPar[3], wgrowthPar[5]));

      if (Wgrowth[i] < verysmall) {
        Wgrowth[i] = 0.0;
        Lgrowth[i] = 0.0;
      } else {
        x = (numGrow[i].W - ratio * refWeight[i]) / numGrow[i].W;
        fx = lgrowthPar[3] + lgrowthPar[4] * x;
        if (fx < verysmall)
          fx = 0.0;
        if (fx > lgrowthPar[5])
          fx = lgrowthPar[5];
        Lgrowth[i] = fx * Wgrowth[i] / (lgrowthPar[6] * lgrowthPar[7] *
          pow(LgrpDiv->meanLength(i), lgrowthPar[7] - 1.0));
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
  : GrowthCalcBase(Areas), numGrowthConstants(2), wgrowth(Areas.Size()) {


  keeper->addString("growthcalcF");
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text >> ws;
  if (strcasecmp(text, "growthparameters") == 0)
    growthPar.read(infile, TimeInfo, keeper);
  else
    handle.Unexpected("growthparameters", text);

  ifstream datafile;
  int i;
  Formula tempF;   //value of tempF is initiated to 0.0
  for (i = 0; i < Areas.Size(); i++)
    wgrowth[i] = new FormulaMatrix(TimeInfo->TotalNoSteps() + 1, lenindex.Size(), tempF);

  CommentStream subdata(datafile);
  readWordAndValue(infile, "weightgrowthfile", text);
  datafile.open(text, ios::in);
  handle.checkIfFailure(datafile, text);
  handle.Open(text);
  i = readGrowthAmounts(subdata, TimeInfo, Area, wgrowth, lenindex, keeper);
  handle.Close();
  datafile.close();
  datafile.clear();

  for (i = 0; i < Areas.Size(); i++)
    (*wgrowth[i]).Inform(keeper);

  keeper->clearLast();
}

GrowthCalcF::~GrowthCalcF() {
  int a;
  for (a = 0; a < wgrowth.Size(); a++)
    delete wgrowth[a];
}

void GrowthCalcF::GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& numGrow, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) {

  growthPar.Update(TimeInfo);
  int inarea = AreaNr[area];
  double kval = growthPar[1] * TimeInfo->LengthOfCurrent() / TimeInfo->LengthOfYear();

  int i;
  DoubleVector w((*wgrowth[inarea])[TimeInfo->CurrentTime()].Size());
  for (i = 0; i < w.Size(); i++)
    w[i] = (*wgrowth[inarea])[TimeInfo->CurrentTime()][i];

  for (i = 0; i < Lgrowth.Size(); i++) {
    if (w[i] < 0.0)
      handle.logWarning("Warning in growth calculation - weight growth parameter is negative");

    Lgrowth[i] = (growthPar[0] - LgrpDiv->meanLength(i)) * (1.0 - exp(-kval));
    Wgrowth[i] = w[i];
  }
}

// ********************************************************
// Functions for GrowthCalcG
// ********************************************************
GrowthCalcG::GrowthCalcG(CommentStream& infile, const IntVector& Areas,
  const TimeClass* const TimeInfo, Keeper* const keeper,
  const AreaClass* const Area, const CharPtrVector& lenindex)
  : GrowthCalcBase(Areas), numGrowthConstants(2), wgrowth(Areas.Size()) {


  keeper->addString("growthcalcG");
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text >> ws;
  if (strcasecmp(text, "growthparameters") == 0)
    growthPar.read(infile, TimeInfo, keeper);
  else
    handle.Unexpected("growthparameters", text);

  int i;
  Formula tempF;   //value of tempF is initiated to 0.0
  for (i = 0; i < Areas.Size(); i++)
    wgrowth[i] = new FormulaMatrix(TimeInfo->TotalNoSteps() + 1, lenindex.Size(), tempF);

  ifstream datafile;
  CommentStream subdata(datafile);
  readWordAndValue(infile, "weightgrowthfile", text);
  datafile.open(text, ios::in);
  handle.checkIfFailure(datafile, text);
  handle.Open(text);
  i = readGrowthAmounts(subdata, TimeInfo, Area, wgrowth, lenindex, keeper);
  handle.Close();
  datafile.close();
  datafile.clear();

  for (i = 0; i < Areas.Size(); i++)
    (*wgrowth[i]).Inform(keeper);

  keeper->clearLast();
}

GrowthCalcG::~GrowthCalcG() {
  int a;
  for (a = 0; a < wgrowth.Size(); a++)
    delete wgrowth[a];
}

void GrowthCalcG::GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& numGrow, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) {

  //written by kgf 24/10 00
  //Gives linear growth (Growthpar[0] == 0) or
  //growth decreasing with length (Growthpar[0] < 0).
  growthPar.Update(TimeInfo);
  int inarea = AreaNr[area];
  double kval = growthPar[1] * TimeInfo->LengthOfCurrent() / TimeInfo->LengthOfYear();

  int i;
  DoubleVector w((*wgrowth[inarea])[TimeInfo->CurrentTime()].Size());
  for (i = 0; i < w.Size(); i++)
    w[i] = (*wgrowth[inarea])[TimeInfo->CurrentTime()][i];

  if (growthPar[0] > 0)
    handle.logWarning("Warning in growth calculation - growth parameter is positive");

  if (isZero(growthPar[0])) {
    for (i = 0; i < Lgrowth.Size(); i++) {
      if (w[i] < 0.0)
        handle.logWarning("Warning in growth calculation - weight growth parameter is negative");

      Lgrowth[i] = kval;
      Wgrowth[i] = w[i];
    }
  } else {
    for (i = 0; i < Lgrowth.Size(); i++) {
      if (w[i] < 0.0)
        handle.logWarning("Warning in growth calculation - weight growth parameter is negative");

      Lgrowth[i] = kval * pow(LgrpDiv->meanLength(i), growthPar[0]);
      Wgrowth[i] = w[i];
    }
  }
}

// ********************************************************
// Functions for GrowthCalcH
// ********************************************************
GrowthCalcH::GrowthCalcH(CommentStream& infile, const IntVector& Areas,
  const TimeClass* const TimeInfo, Keeper* const keeper)
  : GrowthCalcBase(Areas), numGrowthConstants(4) {

  keeper->addString("growthcalcH");
  growthPar.resize(numGrowthConstants, keeper);

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text >> ws;
  if (strcasecmp(text, "growthparameters") == 0)
    growthPar.read(infile, TimeInfo, keeper);
  else
    handle.Unexpected("growthparameters", text);
  keeper->clearLast();
}

GrowthCalcH::~GrowthCalcH() {
}

/* Simplified 2 parameter length based Von Bertalanffy growth function
 * compare with GrowthCalcC for the more complex weight based version */
void GrowthCalcH::GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& numGrow, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) {

  growthPar.Update(TimeInfo);
  double mult = 1.0 - exp(-growthPar[1] * TimeInfo->LengthOfCurrent() / TimeInfo->LengthOfYear());

  //JMB - first some error checking
  if (isZero(growthPar[1]) || isZero(growthPar[2]))
    handle.logWarning("Warning in growth calculation - growth parameter is zero");
  if (LgrpDiv->maxLength() > growthPar[0])
    handle.logWarning("Warning in growth calculation - length greater than length infinity");

  int i;
  for (i = 0; i < Wgrowth.Size(); i++) {
    Lgrowth[i] = (growthPar[0] - LgrpDiv->meanLength(i)) * mult;
    Wgrowth[i] = growthPar[2] * (pow(LgrpDiv->meanLength(i) + Lgrowth[i], growthPar[3])
      - pow(LgrpDiv->meanLength(i), growthPar[3]));
  }
}
