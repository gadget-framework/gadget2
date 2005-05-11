#include "stockpreystdinfobylength.h"
#include "stockprey.h"
#include "areatime.h"
#include "gadget.h"

StockPreyStdInfoByLength::StockPreyStdInfoByLength(const StockPrey* p, const IntVector& Areas)
  : AbstrPreyStdInfoByLength(p, Areas), prey(p) {

  preyLgrpDiv = p->returnLengthGroupDiv();
}

StockPreyStdInfoByLength::~StockPreyStdInfoByLength() {
}

void StockPreyStdInfoByLength::Sum(const TimeClass* const TimeInfo, int area) {
  int l, inarea = this->areaNum(area);
  double timeratio;
  PopInfo nullpop;
  PopInfoVector PopBylength(BconbyLength.Ncol(inarea), nullpop);
  const DoubleVector& Bconsumption = prey->getConsumption(area);
  const AgeBandMatrix& Alk = prey->AlkeysPriorToEating(area);

  Alk.sumColumns(PopBylength);
  timeratio = 1.0 / TimeInfo->getTimeStepSize();
  for (l = 0; l < BconbyLength.Ncol(inarea); l++) {
    BconbyLength[inarea][l] = Bconsumption[l];

    if (isZero(PopBylength[l].W))
      NconbyLength[inarea][l] = 0.0;
    else
      NconbyLength[inarea][l] = BconbyLength[inarea][l] / PopBylength[l].W;

    if (isZero(PopBylength[l].N))
      MortbyLength[inarea][l] = 0.0;
    else if (NconbyLength[inarea][l] >= PopBylength[l].N)
      MortbyLength[inarea][l] = MaxMortality;
    else
      MortbyLength[inarea][l] = -log(1.0 - NconbyLength[inarea][l] / PopBylength[l].N) * timeratio;

  }
}
