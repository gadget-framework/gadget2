#include "grower.h"
#include "mathfunc.h"

//Uses the length increase in InterpLGrowth and mean weight change in
//InterpWGrowth to calculate lgrowth and wgrowth.
void Grower::GrowthImplement(int area, const PopInfoVector& NumberInArea,
  const LengthGroupDivision* const Lengths) {

  int inarea = AreaNr[area];
  int type = getGrowthType();
  int lgroup, j;
  double meanw, dw;

  if (Version() == 1) {
    double growth, alpha, tmppart3;
    double tmpDl, tmpPower, tmpMeanLength, tmpMult;

    tmpMult = getMultValue();
    tmpPower = getPowerValue();
    tmpDl = 1.0 / Lengths->dl();
    for (lgroup = 0; lgroup < Lengths->NoLengthGroups(); lgroup++) {
      meanw = 0.0;
      part3 = 1.0;
      tmpMeanLength = (tmpPower * Lengths->dl()) / Lengths->Meanlength(lgroup);
      growth = InterpLgrowth[inarea][lgroup] * tmpDl;
      if (growth >= maxlengthgroupgrowth)
        growth = double(maxlengthgroupgrowth) - 0.1;

      alpha = beta * growth / (maxlengthgroupgrowth - growth);
      for (j = 0; j < maxlengthgroupgrowth; j++)
        part3 = part3 * (alpha + beta + double(j));

      tmppart3 = 1 / part3;
      part4[0] = 1.0;
      part4[1] = alpha;
      if (maxlengthgroupgrowth > 1) {
        for (j = 2; j <= maxlengthgroupgrowth; j++)
          part4[j] = part4[j - 1] * (j - 1 + alpha);
      }

      if (type == 8) {
        for (j = 0; j <= maxlengthgroupgrowth; j++) {
          (*lgrowth[inarea])[j][lgroup] = part1[j] * part2[j] * tmppart3 * part4[j];
          (*wgrowth[inarea])[j][lgroup] = tmpMult * (pow(LgrpDiv->Meanlength(lgroup + j), tmpPower)
            - pow(LgrpDiv->Meanlength(lgroup), tmpPower));
        }

      } else {
        for (j = 0; j <= maxlengthgroupgrowth; j++) {
          (*lgrowth[inarea])[j][lgroup] = part1[j] * part2[j] * tmppart3 * part4[j];
          (*wgrowth[inarea])[j][lgroup] = NumberInArea[lgroup].W * tmpMeanLength * j;
          meanw += (*wgrowth[inarea])[j][lgroup] * (*lgrowth[inarea])[j][lgroup];
        }

        dw = InterpWgrowth[inarea][lgroup] - meanw;
        for (j = 0; j < lgrowth[inarea]->Nrow(); j++)
          (*wgrowth[inarea])[j][lgroup] += dw;
      }
    }

  } else if (Version() == 2) {
    int n1, n2, n;
    double variance, tmpVarDl, tmpMeanDl, tmpPower, tmpMeanLength;

    tmpVarDl = 1.0 / (Lengths->dl() * GrIPar->VarResolution());
    tmpMeanDl = 1.0 / (Lengths->dl() * GrIPar->MeanResolution());
    tmpPower = power * Lengths->dl();
    for (lgroup = 0; lgroup < Lengths->NoLengthGroups(); lgroup++) {
      variance = MeanVarianceParameters[0] + MeanVarianceParameters[1] * InterpLgrowth[inarea][lgroup];
      if (variance < 0)
        variance = 0.0;

      n1 = (int) floor((InterpLgrowth[inarea][lgroup] * tmpMeanDl) + 0.5);
      n2 = (int) floor((variance * tmpVarDl) + 0.5);
      n1 = min(n1, GrIPar->NMean() - 1);
      n2 = min(n2, GrIPar->NVar() - 1);
      n = n1 * GrIPar->NVar() + n2;
      meanw = 0.0;

      tmpMeanLength = tmpPower / Lengths->Meanlength(lgroup);
      for (j = 0; j < lgrowth[inarea]->Nrow(); j++) {
        (*lgrowth[inarea])[j][lgroup] = GrIPar->Distribution()[n][j];
        (*wgrowth[inarea])[j][lgroup] = NumberInArea[lgroup].W * tmpMeanLength * (j + GrIPar->MinLengthgroupGrowth());
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
    double tmpDl, tmpPart3;

    tmpDl = 1.0 / Lengths->dl();
    for (lgroup = 0; lgroup < Lengths->NoLengthGroups(); lgroup++) {
      part3 = 1.0;
      growth = InterpLgrowth[inarea][lgroup] * tmpDl;
      if (growth >= maxlengthgroupgrowth)
        growth = double(maxlengthgroupgrowth) - 0.1;
      alpha = beta * growth / (maxlengthgroupgrowth - growth);
      for (j = 0; j < maxlengthgroupgrowth; j++)
        part3 = part3 * (alpha + beta + double(j));

      tmpPart3 = 1 / part3;
      part4[0] = 1.0;
      part4[1] = alpha;
      if (maxlengthgroupgrowth > 1) {
        for (j = 2; j <= maxlengthgroupgrowth; j++)
          part4[j] = part4[j - 1] * (j - 1 + alpha);
      }
      for (j = 0; j <= maxlengthgroupgrowth; j++)
        (*lgrowth[inarea])[j][lgroup] = part1[j] * part2[j] * tmpPart3 * part4[j];
    }

  } else if (Version() == 2) {
    int n1, n2, n;
    double variance, tmpVarDl, tmpMeanDl;

    tmpVarDl = 1.0 / (Lengths->dl() * GrIPar->VarResolution());
    tmpMeanDl = 1.0 / (Lengths->dl() * GrIPar->MeanResolution());
    for (lgroup = 0; lgroup < Lengths->NoLengthGroups(); lgroup++) {
      variance = MeanVarianceParameters[0] + MeanVarianceParameters[1] * InterpLgrowth[inarea][lgroup];
      if (variance < 0)
        variance = 0.0;

      n1 = (int) floor((InterpLgrowth[inarea][lgroup] * tmpMeanDl) + 0.5);
      n2 = (int) floor((variance * tmpVarDl) + 0.5);
      n1 = min(n1, GrIPar->NMean() - 1);
      n2 = min(n2, GrIPar->NVar() - 1);
      n = n1 * GrIPar->NVar() + n2;
      for (j = 0; j < lgrowth[inarea]->Nrow(); j++)
        (*lgrowth[inarea])[j][lgroup] = GrIPar->Distribution()[n][j];
    }
  }
}
