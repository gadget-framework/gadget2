#include "popstatistics.h"
#include "popinfo.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"
#include "agebandmatrix.h"

void PopStatistics::calcStatistics(const PopInfoIndexVector& pop,
  const LengthGroupDivision* const lgrpdiv, int calcweight) {

  PopInfo sum;
  int i, offset; 
  double tmp;

  if (pop.Size() != lgrpdiv->numLengthGroups())
    handle.logMessage(LOGFAIL, "Error in popstatistics - length groups dont match population");

  offset = pop.minCol();
  meanlength = meanweight = totalnumber = sdevlength = 0.0;
  for (i = offset; i < pop.maxCol(); i++) {
    if ((handle.getLogLevel() >= LOGWARN) && calcweight)
      if ((isZero(pop[i].W)) && (!(isZero(pop[i].N))))
        handle.logMessage(LOGWARN, "Warning in popstatistics - non-zero population has zero mean weight");

    if (calcweight)
      sum += pop[i];
    totalnumber += pop[i].N;
    meanlength += pop[i].N * lgrpdiv->meanLength(i - offset);
  }

  if (totalnumber > verysmall) {
    if (calcweight)
      meanweight = sum.W;
    meanlength /= totalnumber;
    for (i = offset; i < pop.maxCol(); i++) {
      tmp = meanlength - lgrpdiv->meanLength(i - offset);
      sdevlength += pop[i].N * tmp * tmp;
    }
    sdevlength = sqrt(sdevlength / totalnumber);
  } else  {
    //JMB reset back to 0
    meanlength = 0.0;
    totalnumber = 0.0;
  }
}

void PopStatistics::calcStatistics(const AgeBandMatrix& agelenum, int lengr) {

  PopInfo sum;
  int age;
  double weig, totn, weigt;
  
// loop over the ages to find specific length group else go to next age

  meanweight = totalnumber= 0.0;
  for (age = agelenum.minAge(); age <= agelenum.maxAge(); age++) {
    weig <= 0.0;
    totn <=0.0;
    weigt <= 0.0;

    if ((handle.getLogLevel() >= LOGWARN))
      if ((isZero((agelenum[age][lengr]).W)) && (!((agelenum[age][lengr]).N)))
        handle.logMessage(LOGWARN, "Warning in popstatistics - non-zero population has zero mean weight");

    sum += agelenum[age][lengr];
    weig = sum.W;
    
    totn += (agelenum[age][lengr]).N;

    weigt= weig*totn;

  }
  meanweight += weigt;
  totalnumber += totn;
  meanweight /= totalnumber;
}




