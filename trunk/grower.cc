#include "grower.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "readword.h"
#include "keeper.h"
#include "areatime.h"
#include "growthcalc.h"
#include "gadget.h"
#include "global.h"

Grower::Grower(CommentStream& infile, const LengthGroupDivision* const OtherLgrpDiv,
  const LengthGroupDivision* const GivenLgrpDiv, const IntVector& Areas,
  const TimeClass* const TimeInfo, Keeper* const keeper, const char* refWeight,
  const char* givenname, const AreaClass* const Area, const CharPtrVector& lenindex)
  : HasName(givenname), LivesOnAreas(Areas), LgrpDiv(0), CI(0), growthcalc(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;

  keeper->addString("grower");
  fixedweights = 0;
  functionnumber = 0;
  LgrpDiv = new LengthGroupDivision(*GivenLgrpDiv);
  if (LgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in grower - failed to create length group");
  CI = new ConversionIndex(OtherLgrpDiv, LgrpDiv, 1);
  if (CI->Error())
    handle.logMessage(LOGFAIL, "Error in grower - error when checking length structure");

  readWordAndValue(infile, "growthfunction", text);
  if (strcasecmp(text, "multspec") == 0) {
    functionnumber = 1;
    growthcalc = new GrowthCalcA(infile, areas, TimeInfo, keeper);
  } else if (strcasecmp(text, "fromfile") == 0) {
    functionnumber = 2;
    growthcalc = new GrowthCalcB(infile, areas, TimeInfo, keeper, Area, lenindex);
  } else if (strcasecmp(text, "weightvb") == 0) {
    functionnumber = 3;
    growthcalc = new GrowthCalcC(infile, areas, TimeInfo, LgrpDiv, keeper, refWeight);
  } else if (strcasecmp(text, "weightjones") == 0) {
    functionnumber = 4;
    growthcalc = new GrowthCalcD(infile, areas, TimeInfo, LgrpDiv, keeper, refWeight);
  } else if (strcasecmp(text, "weightvbexpanded") == 0) {
    functionnumber = 5;
    growthcalc = new GrowthCalcE(infile, areas, TimeInfo, LgrpDiv, keeper, refWeight);
  } else if (strcasecmp(text, "lengthvb") == 0) {
    functionnumber = 6;
    fixedweights = 1;
    growthcalc = new GrowthCalcF(infile, areas, TimeInfo, keeper, Area, lenindex);
  } else if (strcasecmp(text, "lengthpower") == 0) {
    functionnumber = 7;
    fixedweights = 1;
    growthcalc = new GrowthCalcG(infile, areas, TimeInfo, keeper, Area, lenindex);
  } else if (strcasecmp(text, "lengthvbsimple") == 0) {
    functionnumber = 8;
    growthcalc = new GrowthCalcH(infile, areas, TimeInfo, keeper);
  } else if (strcasecmp(text, "weightjonessimple") == 0) {
    functionnumber = 9;
    growthcalc = new GrowthCalcI(infile, areas, TimeInfo, keeper);
  } else if (strcasecmp(text, "lengthvpsimplet0") == 0) {
    functionnumber = 10;
    growthcalc = new GrowthCalcJ(infile, areas, TimeInfo, keeper);
  } else if (strcasecmp(text, "lengthgompertz") == 0) {
    functionnumber = 11;
    growthcalc = new GrowthCalcK(infile, areas, TimeInfo, keeper);
  } else {
    handle.logFileMessage(LOGFAIL, "unrecognised growth function", text);
  }

  infile >> ws >> text;
  if (strcasecmp(text, "beta") == 0) {
    //Beta binomial growth distribution code is used
    infile >> beta;
    beta.Inform(keeper);
    readWordAndVariable(infile, "maxlengthgroupgrowth", maxlengthgroupgrowth);

    if (maxlengthgroupgrowth > LgrpDiv->numLengthGroups()) {
      handle.logMessage(LOGWARN, "Warning in grower - maxlengthgroupgrowth is greater than number of length groups");
      maxlengthgroupgrowth = LgrpDiv->numLengthGroups();
    }

    part1.resize(maxlengthgroupgrowth + 1, 0.0);
    part2.resize(maxlengthgroupgrowth + 1, 0.0);
    part4.resize(maxlengthgroupgrowth + 1, 0.0);

  } else if (strcasecmp(text, "meanvarianceparameters") == 0) {
    handle.logFileMessage(LOGFAIL, "\nThe mean variance parameters implementation of the growth is no longer supported\nUse the beta-binomial distribution implementation of the growth instead");

  } else
    handle.logFileUnexpected(LOGFAIL, "beta", text);

  //Finished reading from input files.
  keeper->clearLast();
  int noareas = areas.Size();
  int len = LgrpDiv->numLengthGroups();
  int otherlen = OtherLgrpDiv->numLengthGroups();
  PopInfo nullpop;
  numGrow.AddRows(noareas, len, nullpop);

  //setting storage spaces for growth
  calcLengthGrowth.AddRows(noareas, len, 0.0);
  calcWeightGrowth.AddRows(noareas, len, 0.0);
  interpLengthGrowth.AddRows(noareas, otherlen, 0.0);
  interpWeightGrowth.AddRows(noareas, otherlen, 0.0);
  dummyfphi.resize(len, 0.0);
  for (i = 0; i < noareas; i++) {
    lgrowth.resize(new DoubleMatrix(maxlengthgroupgrowth + 1, otherlen, 0.0));
    wgrowth.resize(new DoubleMatrix(maxlengthgroupgrowth + 1, otherlen, 0.0));
  }
}

Grower::~Grower() {
  int i;
  for (i = 0; i < lgrowth.Size(); i++) {
    delete lgrowth[i];
    delete wgrowth[i];
  }
  delete CI;
  delete LgrpDiv;
  delete growthcalc;
}

void Grower::Print(ofstream& outfile) const {
  int i, j, area;

  outfile << "\nGrower\n\t";
  LgrpDiv->Print(outfile);
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tLength increase on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < calcLengthGrowth.Ncol(area); i++)
      outfile << sep << calcLengthGrowth[area][i];
    outfile << "\n\tWeight increase on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < calcWeightGrowth.Ncol(area); i++)
      outfile << sep << calcWeightGrowth[area][i];
    outfile << "\n\tDistributed length increase on internal area " << areas[area] << ":\n";
    for (i = 0; i < lgrowth[area]->Nrow(); i++) {
      outfile << TAB;
      for (j = 0; j < lgrowth[area]->Ncol(i); j++)
        outfile << sep << (*lgrowth[area])[i][j];
      outfile << endl;
    }
    outfile << "\tDistributed weight increase on internal area " << areas[area] << ":\n";
    for (i = 0; i < wgrowth[area]->Nrow(); i++) {
      outfile << TAB;
      for (j = 0; j < wgrowth[area]->Ncol(i); j++)
        outfile << sep << (*wgrowth[area])[i][j];
      outfile << endl;
    }
  }
}

void Grower::Sum(const PopInfoVector& NumberInArea, int area) {
  numGrow[this->areaNum(area)].Sum(&NumberInArea, *CI);
}

void Grower::calcGrowth(int area,
  const AreaClass* const Area, const TimeClass* const TimeInfo) {

  this->calcGrowth(area, Area, TimeInfo, dummyfphi, dummyfphi);
}

void Grower::calcGrowth(int area,
  const AreaClass* const Area, const TimeClass* const TimeInfo,
  const DoubleVector& FPhi, const DoubleVector& MaxCon) {

  int inarea = this->areaNum(area);
  growthcalc->calcGrowth(area, calcLengthGrowth[inarea], calcWeightGrowth[inarea],
    numGrow[inarea], Area, TimeInfo, FPhi, MaxCon, LgrpDiv);

  CI->interpolateLengths(interpLengthGrowth[inarea], calcLengthGrowth[inarea]);
  switch (functionnumber) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 9:
      CI->interpolateLengths(interpWeightGrowth[inarea], calcWeightGrowth[inarea]);
      break;
    case 8:
    case 10:
    case 11:
      break;
    default:
      handle.logMessage(LOGFAIL, "Error in grower - unrecognised growth function", functionnumber);
      break;
  }
}

void Grower::Reset() {
  int i, j, area;
  double factorialx, tmppart, tmpmax;

  calcLengthGrowth.setToZero();
  calcWeightGrowth.setToZero();
  interpLengthGrowth.setToZero();
  for (area = 0; area < areas.Size(); area++) {
    (*lgrowth[area]).setToZero();
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      numGrow[area][i].setToZero();
  }

  switch (functionnumber) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 9:
      interpWeightGrowth.setToZero();
      for (area = 0; area < areas.Size(); area++)
        (*wgrowth[area]).setToZero();
      break;
    case 8:
    case 10:
    case 11:
      break;
    default:
      handle.logMessage(LOGFAIL, "Error in grower - unrecognised growth function", functionnumber);
      break;
  }

  tmpmax = double(maxlengthgroupgrowth);
  part1[0] = 1.0;
  part1[1] = tmpmax;
  factorialx = 1.0;
  tmppart = tmpmax;
  if (maxlengthgroupgrowth > 1) {
    for (i = 2; i < maxlengthgroupgrowth + 1; i++) {
      tmppart *= (tmpmax - i + 1);
      factorialx *= double(i);
      part1[i] = (tmppart / factorialx);
    }
  }

  part2[maxlengthgroupgrowth] = 1.0;
  part2[maxlengthgroupgrowth - 1] = beta;
  if (maxlengthgroupgrowth > 1)
    for (i = maxlengthgroupgrowth - 2; i >= 0; i--)
      part2[i] = part2[i + 1] * (beta + tmpmax - i - 1);

  //JMB this will never change so we can set it once
  part4[0] = 1.0;

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset grower data for stock", this->getName());
}
