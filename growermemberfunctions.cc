#include "grower.h"
#include "mathfunc.h"

//Uses the length increase in InterpLGrowth and mean weight change in
//InterpWGrowth to calculate lgrowth and wgrowth.
void Grower::GrowthImplement(int area, const popinfovector& NumberInArea,
  const LengthGroupDivision* const Lengths) {

  int inarea = AreaNr[area];
  int lgroup, j;
  double meanw, dw;

  if (Version() == 1) {
    double growth, alpha;
    double tempdl, temppower, tempmeanlength, temppart3;

    tempdl = 1.0 / Lengths->dl();
    temppower = power * Lengths->dl();
    for (lgroup = 0; lgroup < Lengths->NoLengthGroups(); lgroup++) {
      part3 = 1.0;
      growth = InterpLgrowth[inarea][lgroup] * tempdl;
      if (growth >= maxlengthgroupgrowth)
        growth = double(maxlengthgroupgrowth) - 0.1;  //If too much growth

      alpha = beta * growth / (maxlengthgroupgrowth - growth);
      for (j = 0; j < maxlengthgroupgrowth; j++)
        part3 = part3 * (alpha + beta + double(j));

      temppart3 = 1 / part3;
      part4[0] = 1.0;
      part4[1] = alpha;
      if (maxlengthgroupgrowth > 1) {
        for (j = 2; j <= maxlengthgroupgrowth; j++)
          part4[j] = part4[j - 1] * (j - 1 + alpha);
      }

      tempmeanlength = temppower / Lengths->Meanlength(lgroup);
      meanw = 0.0;
      for (j = 0; j <=  maxlengthgroupgrowth; j++) {
        (*lgrowth[inarea])[j][lgroup] = part1[j] * part2[j] * temppart3 * part4[j];
        (*wgrowth[inarea])[j][lgroup] = NumberInArea[lgroup].W * tempmeanlength * j;

        meanw += (*wgrowth[inarea])[j][lgroup] * (*lgrowth[inarea])[j][lgroup];
      }
      dw = InterpWgrowth[inarea][lgroup] - meanw;
      for (j = 0; j < lgrowth[inarea]->Nrow(); j++)
        (*wgrowth[inarea])[j][lgroup] += dw;
    }

  } else if (Version() == 2) {
    int n1, n2, n;
    double variance;
    double tempvardl, tempmeandl, temppower, tempmeanlength;

    tempvardl = 1.0 / (Lengths->dl() * GrIPar->VarResolution());
    tempmeandl = 1.0 / (Lengths->dl() * GrIPar->MeanResolution());
    temppower = power * Lengths->dl();
    for (lgroup = 0; lgroup < Lengths->NoLengthGroups(); lgroup++) {
      variance = MeanVarianceParameters[0] + MeanVarianceParameters[1] * InterpLgrowth[inarea][lgroup];
      if (variance < 0)
        variance = 0.0;

      n1 = (int) floor((InterpLgrowth[inarea][lgroup] * tempmeandl) + 0.5);
      n2 = (int) floor((variance * tempvardl) + 0.5);
      n1 = min(n1, GrIPar->NMean() - 1);
      n2 = min(n2, GrIPar->NVar() - 1);
      n = n1 * GrIPar->NVar() + n2;
      meanw = 0.0;

      tempmeanlength = temppower / Lengths->Meanlength(lgroup);
      for (j = 0; j < lgrowth[inarea]->Nrow(); j++) {
        (*lgrowth[inarea])[j][lgroup] = GrIPar->Distribution()[n][j];
        (*wgrowth[inarea])[j][lgroup] = NumberInArea[lgroup].W * tempmeanlength * (j + GrIPar->MinLengthgroupGrowth());
        meanw += (*wgrowth[inarea])[j][lgroup] * (*lgrowth[inarea])[j][lgroup];
      }
      dw = InterpWgrowth[inarea][lgroup] - meanw;
      for (j = 0; j < lgrowth[inarea]->Nrow(); j++)
        (*wgrowth[inarea])[j][lgroup] += dw;
    }
  }
}

//Uses only the length increase in InterpLGrowth to calculate lgrowth.
void Grower::GrowthImplement(int area, const LengthGroupDivision* const Lengths) {

  int inarea = AreaNr[area];
  int lgroup, j;

  if (Version() == 1) {
    double growth, alpha;
    double tempdl, temppart3;

    tempdl = 1.0 / Lengths->dl();
    for (lgroup = 0; lgroup < Lengths->NoLengthGroups(); lgroup++) {
      part3 = 1.0;
      growth = InterpLgrowth[inarea][lgroup] * tempdl;
      if (growth >= maxlengthgroupgrowth)
        growth = double(maxlengthgroupgrowth) - 0.1;  //If too much growth
      alpha = beta * growth / (maxlengthgroupgrowth - growth);
      for (j = 0; j < maxlengthgroupgrowth; j++)
        part3 = part3 * (alpha + beta + double(j));

      temppart3 = 1 / part3;
      part4[0] = 1.0;
      part4[1] = alpha;
      if (maxlengthgroupgrowth > 1) {
        for (j = 2; j <= maxlengthgroupgrowth; j++)
          part4[j] = part4[j - 1] * (j - 1 + alpha);
      }
      for (j = 0; j <= maxlengthgroupgrowth; j++)
        (*lgrowth[inarea])[j][lgroup] = part1[j] * part2[j] * temppart3 * part4[j];
    }

  } else if (Version() == 2) {
    //New version adapted to Norwegian change (must test)
    int n1, n2, n;
    double variance, tempvardl, tempmeandl;

    tempvardl = 1.0 / (Lengths->dl() * GrIPar->VarResolution());
    tempmeandl = 1.0 / (Lengths->dl() * GrIPar->MeanResolution());
    for (lgroup = 0; lgroup < Lengths->NoLengthGroups(); lgroup++) {
      variance = MeanVarianceParameters[0] + MeanVarianceParameters[1] * InterpLgrowth[inarea][lgroup];
      if (variance < 0)
        variance = 0.0;

      n1 = (int) floor((InterpLgrowth[inarea][lgroup] * tempmeandl) + 0.5);
      n2 = (int) floor((variance * tempvardl) + 0.5);
      n1 = min(n1, GrIPar->NMean() - 1);
      n2 = min(n2, GrIPar->NVar() - 1);
      n = n1 * GrIPar->NVar() + n2;
      for (j = 0; j < lgrowth[inarea]->Nrow(); j++)
        (*lgrowth[inarea])[j][lgroup] = GrIPar->Distribution()[n][j];
    }
  }
}
