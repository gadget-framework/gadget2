#include "grower.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "readword.h"
#include "keeper.h"
#include "areatime.h"
#include "growthcalc.h"
#include "gadget.h"

extern ErrorHandler handle;

Grower::Grower(CommentStream& infile, const LengthGroupDivision* const OtherLgrpDiv,
  const LengthGroupDivision* const GivenLgrpDiv, const IntVector& Areas,
  const TimeClass* const TimeInfo, Keeper* const keeper, const char* refWeight,
  const AreaClass* const Area, const CharPtrVector& lenindex)
  : LivesOnAreas(Areas), LgrpDiv(0), CI(0), growthcalc(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;

  keeper->addString("grower");
  fixedweights = 0;
  LgrpDiv = new LengthGroupDivision(*GivenLgrpDiv);
  CI = new ConversionIndex(OtherLgrpDiv, LgrpDiv, 1);

  functionname = new char[MaxStrLength];
  strncpy(functionname, "", MaxStrLength);
  readWordAndValue(infile, "growthfunction", functionname);

  if (strcasecmp(functionname, "multspec") == 0)
    functionnumber = 1;
  else if (strcasecmp(functionname, "fromfile") == 0)
    functionnumber = 2;
  else if (strcasecmp(functionname, "weightvb") == 0)
    functionnumber = 3;
  else if (strcasecmp(functionname, "weightjones") == 0)
    functionnumber = 4;
  else if (strcasecmp(functionname, "weightvbexpanded") == 0)
    functionnumber = 5;
  else if (strcasecmp(functionname, "lengthvb") == 0)
    functionnumber = 6;
  else if (strcasecmp(functionname, "lengthpower") == 0)
    functionnumber = 7;
  else if (strcasecmp(functionname, "lengthvbsimple") == 0)
    functionnumber = 8;
  else
    handle.Message("Error in stock file - unrecognised growth function", functionname);

  switch(functionnumber) {
    case 1:
      growthcalc = new GrowthCalcA(infile, areas, TimeInfo, keeper);
      break;
    case 2:
      growthcalc = new GrowthCalcB(infile, areas, TimeInfo, keeper, Area, lenindex);
      break;
    case 3:
      growthcalc = new GrowthCalcC(infile, areas, TimeInfo, LgrpDiv, keeper, refWeight);
      break;
    case 4:
      growthcalc = new GrowthCalcD(infile, areas, TimeInfo, LgrpDiv, keeper, refWeight);
      break;
    case 5:
      growthcalc = new GrowthCalcE(infile, areas, TimeInfo, LgrpDiv, keeper, refWeight);
      break;
    case 6:
      growthcalc = new GrowthCalcF(infile, areas, TimeInfo, keeper, Area, lenindex);
      fixedweights = 1;
      break;
    case 7:
      growthcalc = new GrowthCalcG(infile, areas, TimeInfo, keeper, Area, lenindex);
      fixedweights = 1;
      break;
    case 8:
      growthcalc = new GrowthCalcH(infile, areas, TimeInfo, keeper);
      break;
    default:
      handle.Message("Error in stock file - unrecognised growth function", functionname);
      break;
  }

  int rows = 0; //Number of rows in wgrowth and lgrowth
  infile >> ws >>  text;
  if ((strcasecmp(text, "beta") == 0)) {
    //Beta binomial growth distribution code is used
    infile >> beta;
    beta.Inform(keeper);
    readWordAndVariable(infile, "maxlengthgroupgrowth", maxlengthgroupgrowth);

    //Finished reading from input files.
    rows = maxlengthgroupgrowth + 1;
    part1.resize(rows, 0.0);
    part2.resize(rows, 0.0);
    part4.resize(rows, 0.0);

  } else if ((strcasecmp(text, "meanvarianceparameters") == 0)) {
    handle.Warning("The mean variance parameters implementation of the growth is no longer supported\nUse the beta-binomial distribution implementation of the growth instead");

  } else
    handle.Unexpected("beta", text);

  //Finished reading from input files.
  const int noareas = areas.Size();
  const int len = LgrpDiv->NoLengthGroups();
  const int otherlen = OtherLgrpDiv->NoLengthGroups();
  PopInfo nullpop;
  numGrow.AddRows(noareas, len, nullpop);
  keeper->clearLast();

  //setting storage spaces for Growth.
  calcLengthGrowth.AddRows(noareas, len, 0.0);
  calcWeightGrowth.AddRows(noareas, len, 0.0);
  InterpLgrowth.AddRows(noareas, otherlen, 0.0);
  InterpWgrowth.AddRows(noareas, otherlen, 0.0);
  lgrowth.resize(noareas);
  wgrowth.resize(noareas);
  for (i = 0; i < noareas; i++) {
    lgrowth[i] = new DoubleMatrix(rows, otherlen, 0.0);
    wgrowth[i] = new DoubleMatrix(rows, otherlen, 0.0);
  }
  Fphi.resize(len, 0.0);
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
  delete[] functionname;
}

void Grower::Print(ofstream& outfile) const {
  int i, j, area;

  outfile << "\nGrower\n\tMean lengths of length groups:\n\t";
  for (i = 0; i < LgrpDiv->NoLengthGroups(); i++)
    outfile << sep << LgrpDiv->meanLength(i);

  outfile << "\n\tLength increase:\n";
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tInternal area " << areas[area] << endl << TAB;
    for (i = 0; i < calcLengthGrowth.Ncol(area); i++)
      outfile << sep << calcLengthGrowth[area][i];
    outfile << endl;
  }

  outfile << "\tWeight increase:\n";
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tInternal area " << areas[area] << endl << TAB;
    for (i = 0; i < calcWeightGrowth.Ncol(area); i++)
      outfile << sep << calcWeightGrowth[area][i];
    outfile << endl;
  }

  outfile << "\tDistributed length increase:\n";
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tInternal area " << areas[area] << endl;
    for (i = 0; i < lgrowth[area]->Nrow(); i++) {
      outfile << TAB;
      for (j = 0; j < lgrowth[area]->Ncol(i); j++)
        outfile << sep << (*lgrowth[area])[i][j];
      outfile << endl;
    }
  }

  outfile << "\tDistributed weight increase:\n";
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tInternal area " << areas[area] << endl;
    for (i = 0; i < wgrowth[area]->Nrow(); i++) {
      outfile << TAB;
      for (j = 0; j < wgrowth[area]->Ncol(i); j++)
        outfile << sep << (*wgrowth[area])[i][j];
      outfile << endl;
    }
  }
}

//The following function is just a copy of Prey::Sum
void Grower::Sum(const PopInfoVector& NumberInArea, int area) {
  int inarea = AreaNr[area];
  int i;
  for (i = 0; i < numGrow[inarea].Size(); i++)
    numGrow[inarea][i].N = 0.0;
  numGrow[inarea].Sum(&NumberInArea, *CI);
}

void Grower::GrowthCalc(int area,
  const AreaClass* const Area, const TimeClass* const TimeInfo) {

  DoubleVector dummy(calcLengthGrowth[AreaNr[area]].Size(), 0.0);
  //Let the feeding level and consumption equal 0.
  this->GrowthCalc(area, Area, TimeInfo, dummy, dummy);
}

void Grower::GrowthCalc(int area,
  const AreaClass* const Area, const TimeClass* const TimeInfo,
  const DoubleVector& FPhi, const DoubleVector& MaxCon) {

  int inarea = AreaNr[area];
  growthcalc->GrowthCalc(area, calcLengthGrowth[inarea], calcWeightGrowth[inarea],
    numGrow[inarea], Area, TimeInfo, FPhi, MaxCon, LgrpDiv);

  CI->interpolateLengths(InterpLgrowth[inarea], calcLengthGrowth[inarea]);
  CI->interpolateLengths(InterpWgrowth[inarea], calcWeightGrowth[inarea]);
}

const DoubleMatrix& Grower::LengthIncrease(int area) const {
  return *lgrowth[AreaNr[area]];
}

const DoubleMatrix& Grower::WeightIncrease(int area) const {
  return *wgrowth[AreaNr[area]];
}

const DoubleVector& Grower::getWeight(int area) const {
  return InterpWgrowth[AreaNr[area]];
}

double Grower::getPowerValue() {
  return growthcalc->getPower();
}

double Grower::getMultValue() {
  return growthcalc->getMult();
}

void Grower::Reset() {
  int i, j, area;
  double factorialx, tmppart, tmpmax;

  for (area = 0; area < areas.Size(); area++) {
    for (i = 0; i < LgrpDiv->NoLengthGroups(); i++) {
      calcLengthGrowth[area][i] = 0.0;
      calcWeightGrowth[area][i] = 0.0;
      numGrow[area][i].N = 0.0;
      numGrow[area][i].W = 0.0;
    }

    for (i = 0; i < wgrowth[area]->Nrow(); i++) {
      for (j = 0; j < wgrowth[area]->Ncol(i); j++) {
        (*wgrowth[area])[i][j] = 0.0;
        (*lgrowth[area])[i][j] = 0.0;
      }
    }

    for (i = 0; i < InterpLgrowth.Ncol(); i++) {
      InterpLgrowth[area][i] = 0.0;
      InterpWgrowth[area][i] = 0.0;
    }
  }

  tmpmax = double(maxlengthgroupgrowth) * 1.0;
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

  handle.logMessage("Reset grower data");
}
