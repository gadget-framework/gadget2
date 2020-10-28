#include "growthcalc.h"
#include "errorhandler.h"
#include "areatime.h"
#include "keeper.h"
#include "readfunc.h"
#include "readword.h"
#include "gadget.h"
#include "global.h"

// ********************************************************
// Functions for GrowthCalcBase
// ********************************************************
GrowthCalcBase::GrowthCalcBase(const IntVector& Areas) : LivesOnAreas(Areas) {
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
  if (strcasecmp(text, "growthparameters") != 0)
    handle.logFileUnexpected(LOGFAIL, "growthparameters", text);
  growthPar.read(infile, TimeInfo, keeper);
  keeper->clearLast();
}

void GrowthCalcA::calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& numGrow, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) {

  growthPar.Update(TimeInfo);
  double tempL = TimeInfo->getTimeStepSize() * growthPar[0] *
      (growthPar[2] * Area->getTemperature(area, TimeInfo->getTime()) + growthPar[3]);
  double tempW = TimeInfo->getTimeStepSize() * growthPar[4] *
      (growthPar[7] * Area->getTemperature(area, TimeInfo->getTime()) + growthPar[8]);

  int i;
  for (i = 0; i < Lgrowth.Size(); i++) {
    Lgrowth[i] = tempL * pow(LgrpDiv->meanLength(i), growthPar[1]) * Fphi[i];
    if (Lgrowth[i] < 0.0)
      Lgrowth[i] = 0.0;

    if (numGrow[i].W < verysmall || isZero(tempW))
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
  : GrowthCalcBase(Areas) {

  char datafilename[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  int i;
  for (i = 0; i < Areas.Size(); i++) {
    lgrowth.resize(new FormulaMatrix(TimeInfo->numTotalSteps() + 1, lenindex.Size(), 0.0));
    wgrowth.resize(new FormulaMatrix(TimeInfo->numTotalSteps() + 1, lenindex.Size(), 0.0));
  }

  keeper->addString("growthcalcB");

  readWordAndValue(infile, "lengthgrowthfile", datafilename);
  datafile.open(datafilename, ios::binary);
  handle.checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  readGrowthAmounts(subdata, TimeInfo, Area, lgrowth, lenindex, Areas);
  handle.Close();
  datafile.close();
  datafile.clear();

  readWordAndValue(infile, "weightgrowthfile", datafilename);
  datafile.open(datafilename, ios::binary);
  handle.checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  readGrowthAmounts(subdata, TimeInfo, Area, wgrowth, lenindex, Areas);
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

void GrowthCalcB::calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& numGrow, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) {

  int i, inarea = this->areaNum(area);
  for (i = 0; i < Lgrowth.Size(); i++) {
    Lgrowth[i] = (*lgrowth[inarea])[TimeInfo->getTime()][i];
    Wgrowth[i] = (*wgrowth[inarea])[TimeInfo->getTime()][i];
    if ((handle.getLogLevel() >= LOGWARN) && ((Lgrowth[i] < 0.0) || (Wgrowth[i] < 0.0)))
      handle.logMessage(LOGWARN, "Warning in growth calculation - negative growth parameter");
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
  refWeight.resize(LgrpDiv->numLengthGroups(), 0.0);

  int i, j;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text >> ws;
  if (strcasecmp(text, "wgrowthparameters") != 0)
    handle.logFileUnexpected(LOGFAIL, "wgrowthparameters", text);
  wgrowthPar.read(infile, TimeInfo, keeper);

  infile >> text >> ws;
  if (strcasecmp(text, "lgrowthparameters") != 0)
    handle.logFileUnexpected(LOGFAIL, "lgrowthparameters", text);
  lgrowthPar.read(infile, TimeInfo, keeper);

  //read information on reference weights.
  keeper->addString("referenceweights");
  ifstream subfile;
  subfile.open(refWeightFile, ios::binary);
  handle.checkIfFailure(subfile, refWeightFile);
  handle.Open(refWeightFile);
  CommentStream subcomment(subfile);
  DoubleMatrix tmpRefW;
  readRefWeights(subcomment, tmpRefW);
  handle.Close();
  subfile.close();
  subfile.clear();

  //Interpolate the reference weights. First there are some error checks.
  if (LgrpDiv->meanLength(0) < tmpRefW[0][0] ||
      LgrpDiv->meanLength(LgrpDiv->numLengthGroups() - 1) > tmpRefW[tmpRefW.Nrow() - 1][0])
    handle.logFileMessage(LOGFAIL, "lengths for reference weights must span the range of growth lengths");

  double ratio, tmplen;
  int pos = 0;
  for (j = 0; j < LgrpDiv->numLengthGroups(); j++) {
    tmplen = LgrpDiv->meanLength(j);
    for (i = pos; i < tmpRefW.Nrow() - 1; i++) {
      if (((tmplen > tmpRefW[i][0]) || (isEqual(tmplen, tmpRefW[i][0]))) &&
          ((tmplen < tmpRefW[i + 1][0]) || (isEqual(tmplen, tmpRefW[i + 1][0])))) {

        ratio = (tmplen - tmpRefW[i][0]) / (tmpRefW[i + 1][0] - tmpRefW[i][0]);
        refWeight[j] = tmpRefW[i][1] + ratio * (tmpRefW[i + 1][1] - tmpRefW[i][1]);
        pos = i;
      }
    }
  }
  keeper->clearLast();
  keeper->clearLast();
}

/* Von Bertalanffy growth function. dw/dt = a*w^n - b*w^m;
 * As a generalisation a and b are made temperature dependent so the
 * final form of the function is
 * dw/dt = a0*exp(a1*T)*((w/a2)^a4 - (w/a3)^a5)
 * For no temperature dependency a1 = 0 */
void GrowthCalcC::calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& numGrow, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) {

  wgrowthPar.Update(TimeInfo);
  lgrowthPar.Update(TimeInfo);

  //JMB - first some error checking
  if (handle.getLogLevel() >= LOGWARN) {
    if (isZero(wgrowthPar[2]) || isZero(wgrowthPar[3]))
      handle.logMessage(LOGWARN, "Warning in growth calculation - weight growth parameter is zero");
    if (isZero(lgrowthPar[6]) || isZero(lgrowthPar[7]))
      handle.logMessage(LOGWARN, "Warning in growth calculation - length growth parameter is zero");
    if (lgrowthPar[5] < 0.0)
      handle.logMessage(LOGWARN, "Warning in growth calculation - length growth parameter is negative");
  }

  int i;
  double fx;
  double ratio = lgrowthPar[0] + lgrowthPar[8] * (lgrowthPar[1] + lgrowthPar[2] * lgrowthPar[8]);
  double tempW = TimeInfo->getTimeStepSize() * wgrowthPar[0] *
      exp(wgrowthPar[1] * Area->getTemperature(area, TimeInfo->getTime()));

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
        fx = lgrowthPar[3] + (lgrowthPar[4] * (numGrow[i].W - ratio * refWeight[i]) / numGrow[i].W);
        if (fx > lgrowthPar[5])
          fx = lgrowthPar[5];
        if (fx < verysmall)
          Lgrowth[i] = 0.0;
        else
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
  refWeight.resize(LgrpDiv->numLengthGroups(), 0.0);

  int i, j;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text >> ws;
  if (strcasecmp(text, "wgrowthparameters") != 0)
    handle.logFileUnexpected(LOGFAIL, "wgrowthparameters", text);
  wgrowthPar.read(infile, TimeInfo, keeper);

  infile >> text >> ws;
  if (strcasecmp(text, "lgrowthparameters") != 0)
    handle.logFileUnexpected(LOGFAIL, "lgrowthparameters", text);
  lgrowthPar.read(infile, TimeInfo, keeper);

  //read information on reference weights.
  keeper->addString("referenceweights");
  ifstream subfile;
  subfile.open(refWeightFile, ios::binary);
  handle.checkIfFailure(subfile, refWeightFile);
  handle.Open(refWeightFile);
  CommentStream subcomment(subfile);
  DoubleMatrix tmpRefW;
  readRefWeights(subcomment, tmpRefW);
  handle.Close();
  subfile.close();
  subfile.clear();

  //Interpolate the reference weights. First there are some error checks.
  if (LgrpDiv->meanLength(0) < tmpRefW[0][0] ||
      LgrpDiv->meanLength(LgrpDiv->numLengthGroups() - 1) > tmpRefW[tmpRefW.Nrow() - 1][0])
    handle.logFileMessage(LOGFAIL, "lengths for reference weights must span the range of growth lengths");

  double ratio, tmplen;
  int pos = 0;
  for (j = 0; j < LgrpDiv->numLengthGroups(); j++) {
    tmplen = LgrpDiv->meanLength(j);
    for (i = pos; i < tmpRefW.Nrow() - 1; i++) {
      if (((tmplen > tmpRefW[i][0]) || (isEqual(tmplen, tmpRefW[i][0]))) &&
          ((tmplen < tmpRefW[i + 1][0]) || (isEqual(tmplen, tmpRefW[i + 1][0])))) {

        ratio = (tmplen - tmpRefW[i][0]) / (tmpRefW[i + 1][0] - tmpRefW[i][0]);
        refWeight[j] = tmpRefW[i][1] + ratio * (tmpRefW[i + 1][1] - tmpRefW[i][1]);
        pos = i;
      }
    }
  }
  keeper->clearLast();
  keeper->clearLast();
}

/* Growth function from Jones 1978.  Found from experiment in captivity.
 * Jones formula only applies to weight increase.  The length increase
 * part is derived from the weight increase part by assuming a formula
 * w = a*l^b. If the weight is below the curve no length increase takes place
 * but instead the weight increases until it reaches the curve. */
void GrowthCalcD::calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& numGrow, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) {

  wgrowthPar.Update(TimeInfo);
  lgrowthPar.Update(TimeInfo);

  //JMB - first some error checking
  if (handle.getLogLevel() >= LOGWARN) {
    if (isZero(wgrowthPar[0]))
      handle.logMessage(LOGWARN, "Warning in growth calculation - weight growth parameter is zero");
    if (isZero(lgrowthPar[6]) || isZero(lgrowthPar[7]))
      handle.logMessage(LOGWARN, "Warning in growth calculation - length growth parameter is zero");
    if (lgrowthPar[5] < 0.0)
      handle.logMessage(LOGWARN, "Warning in growth calculation - length growth parameter is negative");
  }

  int i;
  double ratio, fx;
  double tempC = TimeInfo->getTimeStepSize() / wgrowthPar[0];
  double tempW = TimeInfo->getTimeStepSize() * wgrowthPar[2] *
      exp(wgrowthPar[4] * Area->getTemperature(area, TimeInfo->getTime()) + wgrowthPar[5]);

  for (i = 0; i < Wgrowth.Size(); i++) {
    if (numGrow[i].W < verysmall) {
      Wgrowth[i] = 0.0;
      Lgrowth[i] = 0.0;
    } else {
      Wgrowth[i] = Fphi[i] * MaxCon[i] * tempC / (pow(numGrow[i].W, wgrowthPar[1])) -
        tempW * pow(numGrow[i].W, wgrowthPar[3]);

      if (Wgrowth[i] < verysmall) {
        Wgrowth[i] = 0.0;
        Lgrowth[i] = 0.0;
      } else {
        ratio = lgrowthPar[0] + Fphi[i] * (lgrowthPar[1] + lgrowthPar[2] * Fphi[i]);
        fx = lgrowthPar[3] + (lgrowthPar[4] * (numGrow[i].W - ratio * refWeight[i]) / numGrow[i].W);
        if (fx > lgrowthPar[5])
          fx = lgrowthPar[5];
        if (fx < verysmall)
          Lgrowth[i] = 0.0;
        else
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
  refWeight.resize(LgrpDiv->numLengthGroups(), 0.0);
  yearEffect.resize(TimeInfo->getLastYear() - TimeInfo->getFirstYear() + 1, keeper);
  stepEffect.resize(TimeInfo->numSteps(), keeper);
  areaEffect.resize(Areas.Size(), keeper);

  int i, j;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text >> ws;
  if (strcasecmp(text, "wgrowthparameters") != 0)
    handle.logFileUnexpected(LOGFAIL, "wgrowthparameters", text);
  wgrowthPar.read(infile, TimeInfo, keeper);

  infile >> text >> ws;
  if (strcasecmp(text, "lgrowthparameters") != 0)
    handle.logFileUnexpected(LOGFAIL, "lgrowthparameters", text);
  lgrowthPar.read(infile, TimeInfo, keeper);

  infile >> text >> ws;
  if (strcasecmp(text, "yeareffect") != 0)
    handle.logFileUnexpected(LOGFAIL, "yeareffect", text);
  for (i = 0; i < yearEffect.Size(); i++)
    if (!(infile >> yearEffect[i]))
      handle.logFileMessage(LOGFAIL, "invalid format of yeareffect vector");
  yearEffect.Inform(keeper);

  infile >> text >> ws;
  if (strcasecmp(text, "stepeffect") != 0)
    handle.logFileUnexpected(LOGFAIL, "stepeffect", text);
  for (i = 0; i < stepEffect.Size(); i++)
    if (!(infile >> stepEffect[i]))
      handle.logFileMessage(LOGFAIL, "invalid format of stepeffect vector");
  stepEffect.Inform(keeper);

  infile >> text >> ws;
  if (strcasecmp(text, "areaeffect") != 0)
    handle.logFileUnexpected(LOGFAIL, "areaeffect", text);
  for (i = 0; i < areaEffect.Size(); i++)
    if (!(infile >> areaEffect[i]))
      handle.logFileMessage(LOGFAIL, "invalid format of areaeffect vector");
  areaEffect.Inform(keeper);

  //read information on reference weights.
  keeper->addString("referenceweights");
  ifstream subfile;
  subfile.open(refWeightFile, ios::binary);
  handle.checkIfFailure(subfile, refWeightFile);
  handle.Open(refWeightFile);
  CommentStream subcomment(subfile);
  DoubleMatrix tmpRefW;
  readRefWeights(subcomment, tmpRefW);
  handle.Close();
  subfile.close();
  subfile.clear();

  //Interpolate the reference weights.
  if (LgrpDiv->meanLength(0) < tmpRefW[0][0] ||
      LgrpDiv->meanLength(LgrpDiv->numLengthGroups() - 1) > tmpRefW[tmpRefW.Nrow() - 1][0])
    handle.logFileMessage(LOGFAIL, "lengths for reference weights must span the range of growth lengths");

  double ratio, tmplen;
  int pos = 0;
  for (j = 0; j < LgrpDiv->numLengthGroups(); j++) {
    tmplen = LgrpDiv->meanLength(j);
    for (i = pos; i < tmpRefW.Nrow() - 1; i++) {
      if (((tmplen > tmpRefW[i][0]) || (isEqual(tmplen, tmpRefW[i][0]))) &&
          ((tmplen < tmpRefW[i + 1][0]) || (isEqual(tmplen, tmpRefW[i + 1][0])))) {

        ratio = (tmplen - tmpRefW[i][0]) / (tmpRefW[i + 1][0] - tmpRefW[i][0]);
        refWeight[j] = tmpRefW[i][1] + ratio * (tmpRefW[i + 1][1] - tmpRefW[i][1]);
        pos = i;
      }
    }
  }
  keeper->clearLast();
  keeper->clearLast();
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
void GrowthCalcE::calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& numGrow, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) {

  wgrowthPar.Update(TimeInfo);
  lgrowthPar.Update(TimeInfo);

  //JMB - first some error checking
  if (handle.getLogLevel() >= LOGWARN) {
    if (isZero(wgrowthPar[2]) || isZero(wgrowthPar[3]))
      handle.logMessage(LOGWARN, "Warning in growth calculation - weight growth parameter is zero");
    if (isZero(lgrowthPar[6]) || isZero(lgrowthPar[7]))
      handle.logMessage(LOGWARN, "Warning in growth calculation - length growth parameter is zero");
    if (lgrowthPar[5] < 0.0)
      handle.logMessage(LOGWARN, "Warning in growth calculation - length growth parameter is negative");
  }

  int i;
  double fx;
  double factor = yearEffect[TimeInfo->getYear() - TimeInfo->getFirstYear()] *
      stepEffect[TimeInfo->getStep() - 1] * areaEffect[this->areaNum(area)];
  double ratio = lgrowthPar[0] + lgrowthPar[8] * (lgrowthPar[1] + lgrowthPar[2] * lgrowthPar[8]);
  double tempW = factor * TimeInfo->getTimeStepSize() * wgrowthPar[0] *
      exp(wgrowthPar[1] * Area->getTemperature(area, TimeInfo->getTime()));

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
        fx = lgrowthPar[3] + (lgrowthPar[4] * (numGrow[i].W - ratio * refWeight[i]) / numGrow[i].W);
        if (fx > lgrowthPar[5])
          fx = lgrowthPar[5];
        if (fx < verysmall)
          Lgrowth[i] = 0.0;
        else
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
  : GrowthCalcBase(Areas), numGrowthConstants(2) {

  keeper->addString("growthcalcF");
  growthPar.resize(numGrowthConstants, keeper);

  int i;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text >> ws;
  if (strcasecmp(text, "growthparameters") != 0)
    handle.logFileUnexpected(LOGFAIL, "growthparameters", text);
  growthPar.read(infile, TimeInfo, keeper);

  for (i = 0; i < Areas.Size(); i++)
    wgrowth.resize(new FormulaMatrix(TimeInfo->numTotalSteps() + 1, lenindex.Size(), 0.0));

  ifstream datafile;
  CommentStream subdata(datafile);
  readWordAndValue(infile, "weightgrowthfile", text);
  datafile.open(text, ios::binary);
  handle.checkIfFailure(datafile, text);
  handle.Open(text);
  readGrowthAmounts(subdata, TimeInfo, Area, wgrowth, lenindex, Areas);
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

void GrowthCalcF::calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& numGrow, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) {

  growthPar.Update(TimeInfo);
  int i, t, inarea;
  t = TimeInfo->getTime();
  inarea = this->areaNum(area);
  double kval = 1.0 - exp(-growthPar[1] * TimeInfo->getTimeStepSize());

  for (i = 0; i < Lgrowth.Size(); i++) {
    Lgrowth[i] = (growthPar[0] - LgrpDiv->meanLength(i)) * kval;
    Wgrowth[i] = (*wgrowth[inarea])[t][i];
    if ((handle.getLogLevel() >= LOGWARN) && (Wgrowth[i] < 0.0))
      handle.logMessage(LOGWARN, "Warning in growth calculation - weight growth parameter is negative");
  }
}

// ********************************************************
// Functions for GrowthCalcG
// ********************************************************
GrowthCalcG::GrowthCalcG(CommentStream& infile, const IntVector& Areas,
  const TimeClass* const TimeInfo, Keeper* const keeper,
  const AreaClass* const Area, const CharPtrVector& lenindex)
  : GrowthCalcBase(Areas), numGrowthConstants(2) {

  keeper->addString("growthcalcG");
  growthPar.resize(numGrowthConstants, keeper);

  int i;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text >> ws;
  if (strcasecmp(text, "growthparameters") != 0)
    handle.logFileUnexpected(LOGFAIL, "growthparameters", text);
  growthPar.read(infile, TimeInfo, keeper);

  for (i = 0; i < Areas.Size(); i++)
    wgrowth.resize(new FormulaMatrix(TimeInfo->numTotalSteps() + 1, lenindex.Size(), 0.0));

  ifstream datafile;
  CommentStream subdata(datafile);
  readWordAndValue(infile, "weightgrowthfile", text);
  datafile.open(text, ios::binary);
  handle.checkIfFailure(datafile, text);
  handle.Open(text);
  readGrowthAmounts(subdata, TimeInfo, Area, wgrowth, lenindex, Areas);
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

void GrowthCalcG::calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& numGrow, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) {

  //written by kgf 24/10 00
  //Gives linear growth (growthPar[0] == 0) or
  //growth decreasing with length (growthPar[0] < 0)
  growthPar.Update(TimeInfo);
  int i, t, inarea;
  t = TimeInfo->getTime();
  inarea = this->areaNum(area);
  double kval = growthPar[1] * TimeInfo->getTimeStepSize();

  if ((handle.getLogLevel() >= LOGWARN) && (growthPar[0] > 0.0))
    handle.logMessage(LOGWARN, "Warning in growth calculation - growth parameter is positive");

  if (isZero(growthPar[0])) {
    for (i = 0; i < Lgrowth.Size(); i++) {
      Lgrowth[i] = kval;
      Wgrowth[i] = (*wgrowth[inarea])[t][i];
      if ((handle.getLogLevel() >= LOGWARN) && (Wgrowth[i] < 0.0))
        handle.logMessage(LOGWARN, "Warning in growth calculation - weight growth parameter is negative");
    }
  } else if (isEqual(growthPar[0], 1.0)) {
    for (i = 0; i < Lgrowth.Size(); i++) {
      Lgrowth[i] = kval * LgrpDiv->meanLength(i);
      Wgrowth[i] = (*wgrowth[inarea])[t][i];
      if ((handle.getLogLevel() >= LOGWARN) && (Wgrowth[i] < 0.0))
        handle.logMessage(LOGWARN, "Warning in growth calculation - weight growth parameter is negative");
    }
  } else {
    for (i = 0; i < Lgrowth.Size(); i++) {
      Lgrowth[i] = kval * pow(LgrpDiv->meanLength(i), growthPar[0]);
      Wgrowth[i] = (*wgrowth[inarea])[t][i];
      if ((handle.getLogLevel() >= LOGWARN) && (Wgrowth[i] < 0.0))
        handle.logMessage(LOGWARN, "Warning in growth calculation - weight growth parameter is negative");
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
  if (strcasecmp(text, "growthparameters") != 0)
    handle.logFileUnexpected(LOGFAIL, "growthparameters", text);
  growthPar.read(infile, TimeInfo, keeper);
  keeper->clearLast();
}

/* Simplified 2 parameter length based Von Bertalanffy growth function
 * compare with GrowthCalcC for the more complex weight based version */
void GrowthCalcH::calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& numGrow, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) {

  growthPar.Update(TimeInfo);
  //JMB - first some error checking
  if ((handle.getLogLevel() >= LOGWARN) && (growthPar.didChange(TimeInfo))) {
    if (isZero(growthPar[1]) || isZero(growthPar[2]))
      handle.logMessage(LOGWARN, "Warning in growth calculation - growth parameter is zero");
    if (LgrpDiv->maxLength() > growthPar[0])
      handle.logMessage(LOGWARN, "Warning in growth calculation - length greater than length infinity");
  }

  double mult = 1.0 - exp(-growthPar[1] * TimeInfo->getTimeStepSize());
  int i;
  for (i = 0; i < Lgrowth.Size(); i++)
    Lgrowth[i] = (growthPar[0] - LgrpDiv->meanLength(i)) * mult;
}

// ********************************************************
// Functions for GrowthCalcI
// ********************************************************
GrowthCalcI::GrowthCalcI(CommentStream& infile, const IntVector& Areas,
  const TimeClass* const TimeInfo, Keeper* const keeper)
  : GrowthCalcBase(Areas), numGrowthConstants(6) {

  keeper->addString("growthcalcI");
  growthPar.resize(numGrowthConstants, keeper);

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text >> ws;
  if (strcasecmp(text, "growthparameters") != 0)
    handle.logFileUnexpected(LOGFAIL, "growthparameters", text);
  growthPar.read(infile, TimeInfo, keeper);
  keeper->clearLast();
}

/* Simplified 4 parameter Jones growth function
 * compare with GrowthCalcD for the more complex version */
void GrowthCalcI::calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& numGrow, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) {

  growthPar.Update(TimeInfo);
  //JMB - first some error checking
  if ((handle.getLogLevel() >= LOGWARN) && (growthPar.didChange(TimeInfo))) {
    if (isZero(growthPar[0]) || isZero(growthPar[1]))
      handle.logMessage(LOGWARN, "Warning in growth calculation - weight growth parameter is zero");
    if (isZero(growthPar[4]) || isZero(growthPar[5]))
      handle.logMessage(LOGWARN, "Warning in growth calculation - length growth parameter is zero");
  }

  double tempC = TimeInfo->getTimeStepSize() * growthPar[0];
  double tempW = TimeInfo->getTimeStepSize() * growthPar[1] *
      exp(growthPar[3] * Area->getTemperature(area, TimeInfo->getTime()));

  int i;
  for (i = 0; i < Wgrowth.Size(); i++) {
    if (numGrow[i].W < verysmall) {
      Wgrowth[i] = 0.0;
      Lgrowth[i] = 0.0;
    } else {
      Wgrowth[i] = tempC * Fphi[i] * MaxCon[i] - tempW * pow(numGrow[i].W, growthPar[2]);

      if (Wgrowth[i] < verysmall) {
        Wgrowth[i] = 0.0;
        Lgrowth[i] = 0.0;
      } else {
        Lgrowth[i] = Wgrowth[i] / (growthPar[4] * growthPar[5] *
          pow(LgrpDiv->meanLength(i), growthPar[5] - 1.0));
      }
    }
  }
}

// ********************************************************
// Functions for GrowthCalcJ
// ********************************************************
GrowthCalcJ::GrowthCalcJ(CommentStream& infile, const IntVector& Areas,
  const TimeClass* const TimeInfo, Keeper* const keeper)
  : GrowthCalcBase(Areas), numGrowthConstants(5) {

  keeper->addString("GrowthCalcJ");
  growthPar.resize(numGrowthConstants, keeper);

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text >> ws;
  if (strcasecmp(text, "growthparameters") != 0)
    handle.logFileUnexpected(LOGFAIL, "growthparameters", text);
  growthPar.read(infile, TimeInfo, keeper);
  keeper->clearLast();
}

/* Simplified 2 parameter length based Von Bertalanffy growth function
 * compare with GrowthCalcC for the more complex weight based version
 * with a non-zero value for t0 (compare to GrowthCalcH for simpler version */
void GrowthCalcJ::calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& numGrow, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) {

  growthPar.Update(TimeInfo);
  //JMB - first some error checking
  if ((handle.getLogLevel() >= LOGWARN) && (growthPar.didChange(TimeInfo))) {
    if (isZero(growthPar[1]) || isZero(growthPar[2]))
      handle.logMessage(LOGWARN, "Warning in growth calculation - growth parameter is zero");
    if (LgrpDiv->maxLength() > growthPar[0])
      handle.logMessage(LOGWARN, "Warning in growth calculation - length greater than length infinity");
  }

  double mult = 1.0 - exp(-growthPar[1] * TimeInfo->getTimeStepSize());
  int i;
  for (i = 0; i < Lgrowth.Size(); i++)
    Lgrowth[i] = (growthPar[0] - LgrpDiv->meanLength(i)) * mult;
}

// ********************************************************
// Functions for GrowthCalcK
// ********************************************************
GrowthCalcK::GrowthCalcK(CommentStream& infile, const IntVector& Areas,
  const TimeClass* const TimeInfo, Keeper* const keeper)
  : GrowthCalcBase(Areas), numGrowthConstants(5) {

  keeper->addString("GrowthCalcK");
  growthPar.resize(numGrowthConstants, keeper);

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text >> ws;
  if (strcasecmp(text, "growthparameters") != 0)
    handle.logFileUnexpected(LOGFAIL, "growthparameters", text);
  growthPar.read(infile, TimeInfo, keeper);
  keeper->clearLast();
}

/* Simplified length based Gompertz growth function */
void GrowthCalcK::calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
  const PopInfoVector& numGrow, const AreaClass* const Area,
  const TimeClass* const TimeInfo, const DoubleVector& Fphi,
  const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) {

  growthPar.Update(TimeInfo);
  //JMB - first some error checking
  if ((handle.getLogLevel() >= LOGWARN) && (growthPar.didChange(TimeInfo))) {
    if (isZero(growthPar[1]) || isZero(growthPar[2]))
      handle.logMessage(LOGWARN, "Warning in growth calculation - growth parameter is zero");
    if (LgrpDiv->maxLength() > growthPar[0])
      handle.logMessage(LOGWARN, "Warning in growth calculation - length greater than length infinity");
  }

  double mult = 1.0 - exp(-growthPar[1] * TimeInfo->getTimeStepSize());
  int i;
  for (i = 0; i < Lgrowth.Size(); i++)
    Lgrowth[i] = (growthPar[0] - LgrpDiv->meanLength(i)) * mult;
}
