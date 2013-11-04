#include "grower.h"
#include "mathfunc.h"
#include "errorhandler.h"
#include "global.h"

//Uses the length increase in interpLengthGrowth and mean weight change in
//interpWeightGrowth to calculate lgrowth and wgrowth.
void Grower::implementGrowth(int area, const PopInfoVector& NumberInArea,
  const LengthGroupDivision* const Lengths) {

  int lgroup, j, inarea = this->areaNum(area);
  double meanw, tmppart3, tmpweight;
  double tmpMult = growthcalc->getMult();
  double tmpPower = growthcalc->getPower();
  double tmpDl = 1.0 / Lengths->dl();  //JMB no need to check zero here

  for (lgroup = 0; lgroup < Lengths->numLengthGroups(); lgroup++) {
    part3 = 1.0;
    growth = interpLengthGrowth[inarea][lgroup] * tmpDl;
    if (growth >= maxlengthgroupgrowth)
      growth = double(maxlengthgroupgrowth) - 0.1;
    if (growth < verysmall)
      growth = 0.0;
    alpha = beta * growth / (maxlengthgroupgrowth - growth);
    for (j = 0; j < maxlengthgroupgrowth; j++)
      part3 *= (alpha + beta + double(j));

    tmppart3 = 1.0 / part3;
    part4[1] = alpha;
    if (maxlengthgroupgrowth > 1)
      for (j = 2; j <= maxlengthgroupgrowth; j++)
        part4[j] = part4[j - 1] * (j - 1 + alpha);

    for (j = 0; j <= maxlengthgroupgrowth; j++)
      (*lgrowth[inarea])[j][lgroup] = part1[j] * part2[j] * tmppart3 * part4[j];

    switch (functionnumber) {
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 9:
        meanw = 0.0;
        tmpweight = (NumberInArea[lgroup].W * tmpPower * Lengths->dl()) / Lengths->meanLength(lgroup);
        for (j = 0; j <= maxlengthgroupgrowth; j++) {
          (*wgrowth[inarea])[j][lgroup] = tmpweight * j;
          meanw += (*wgrowth[inarea])[j][lgroup] * (*lgrowth[inarea])[j][lgroup];
        }

        tmpweight = interpWeightGrowth[inarea][lgroup] - meanw;
        for (j = 0; j <= maxlengthgroupgrowth; j++)
          (*wgrowth[inarea])[j][lgroup] += tmpweight;
        break;
      case 8:
      case 10:
      case 11:
        if (lgroup != Lengths->numLengthGroups())
          for (j = 1; j <= maxlengthgroupgrowth; j++)
            (*wgrowth[inarea])[j][lgroup] = tmpMult * (pow(Lengths->meanLength(lgroup + j), tmpPower) - pow(Lengths->meanLength(lgroup), tmpPower));
        break;
      default:
        handle.logMessage(LOGFAIL, "Error in grower - unrecognised growth function", functionnumber);
        break;
    }
  }
}

//Uses only the length increase in interpLengthGrowth to calculate lgrowth.
void Grower::implementGrowth(int area, const LengthGroupDivision* const Lengths) {

  int lgroup, j, inarea = this->areaNum(area);
  double tmppart3;
  double tmpDl = 1.0 / Lengths->dl();  //JMB no need to check zero here

  for (lgroup = 0; lgroup < Lengths->numLengthGroups(); lgroup++) {
    part3 = 1.0;
    growth = interpLengthGrowth[inarea][lgroup] * tmpDl;
    if (growth >= maxlengthgroupgrowth)
      growth = double(maxlengthgroupgrowth) - 0.1;
    if (growth < verysmall)
      growth = 0.0;
    alpha = beta * growth / (maxlengthgroupgrowth - growth);
    for (j = 0; j < maxlengthgroupgrowth; j++)
      part3 *= (alpha + beta + double(j));

    tmppart3 = 1.0 / part3;
    part4[1] = alpha;
    if (maxlengthgroupgrowth > 1)
      for (j = 2; j <= maxlengthgroupgrowth; j++)
        part4[j] = part4[j - 1] * (j - 1 + alpha);

    for (j = 0; j <= maxlengthgroupgrowth; j++)
      (*lgrowth[inarea])[j][lgroup] = part1[j] * part2[j] * tmppart3 * part4[j];
  }
}
