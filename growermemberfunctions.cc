#include "grower.h"
#include "mathfunc.h"
#include "errorhandler.h"

extern ErrorHandler handle;

//Uses the length increase in InterpLGrowth and mean weight change in
//InterpWGrowth to calculate lgrowth and wgrowth.
void Grower::GrowthImplement(int area, const PopInfoVector& NumberInArea,
  const LengthGroupDivision* const Lengths) {

  int lgroup, j;
  double meanw, dw;
  double growth, alpha, tmppart3, tmpMeanLength;

  if (isZero(Lengths->dl()))
    handle.logFailure("Error in growth - recieved illegal value for length step");

  int inarea = AreaNr[area];
  int type = getGrowthType();
  double tmpMult = getMultValue();
  double tmpPower = getPowerValue();
  double tmpDl = 1.0 / Lengths->dl();

  for (lgroup = 0; lgroup < Lengths->NoLengthGroups(); lgroup++) {
    meanw = 0.0;
    part3 = 1.0;
    tmpMeanLength = (tmpPower * Lengths->dl()) / Lengths->meanLength(lgroup);
    growth = InterpLgrowth[inarea][lgroup] * tmpDl;
    if (growth >= maxlengthgroupgrowth)
      growth = double(maxlengthgroupgrowth) - 0.1;

    //JMB - check for negative growth
    if (growth < verysmall)
      growth = 0.0;

    alpha = beta * growth / (maxlengthgroupgrowth - growth);
    for (j = 0; j < maxlengthgroupgrowth; j++)
      part3 = part3 * (alpha + beta + double(j));

    tmppart3 = 1 / part3;
    part4[0] = 1.0;
    part4[1] = alpha;
    if (maxlengthgroupgrowth > 1)
      for (j = 2; j <= maxlengthgroupgrowth; j++)
        part4[j] = part4[j - 1] * (j - 1 + alpha);

    if (type == 8) {
      for (j = 0; j <= maxlengthgroupgrowth; j++) {
        (*lgrowth[inarea])[j][lgroup] = part1[j] * part2[j] * tmppart3 * part4[j];
        (*wgrowth[inarea])[j][lgroup] = tmpMult * (pow(Lengths->meanLength(lgroup + j), tmpPower) - pow(Lengths->meanLength(lgroup), tmpPower));
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
}

//Uses only the length increase in InterpLGrowth to calculate lgrowth.
void Grower::GrowthImplement(int area, const LengthGroupDivision* const Lengths) {

  int lgroup, j;
  double growth, alpha, tmpPart3;

  int inarea = AreaNr[area];
  double tmpDl = 1.0 / Lengths->dl();

  for (lgroup = 0; lgroup < Lengths->NoLengthGroups(); lgroup++) {
    part3 = 1.0;
    growth = InterpLgrowth[inarea][lgroup] * tmpDl;
    if (growth >= maxlengthgroupgrowth)
      growth = double(maxlengthgroupgrowth) - 0.1;
    if (growth < verysmall)
      growth = 0.0;
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
}
