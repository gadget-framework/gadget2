#include "stockpreystdinfobylength.h"
#include "stockprey.h"
#include "areatime.h"
#include "agebandm.h"
#include "gadget.h"

StockPreyStdInfoByLength::StockPreyStdInfoByLength(const StockPrey* p, const intvector& Areas)
  : AbstrPreyStdInfoByLength(p, Areas), prey(p) {

  PreyLgrpDiv = p->ReturnLengthGroupDiv();
}

StockPreyStdInfoByLength::~StockPreyStdInfoByLength() {
}

void StockPreyStdInfoByLength::Sum(const TimeClass* const TimeInfo, int area) {
  const Agebandmatrix& Alk = prey->AlkeysPriorToEating(area);
  const int inarea = AreaNr[area];
  popinfo nullpop;

  popinfovector PopBylength(BconbyLength.Ncol(inarea), nullpop);
  Alk.Colsum(PopBylength);
  const doublevector& Bconsumption = prey->Bconsumption(area);
  assert(BconbyLength.Ncol() == Bconsumption.Size());
  int l;
  double timeratio;

  timeratio = TimeInfo->LengthOfYear() / TimeInfo->LengthOfCurrent();
  for (l = 0; l < BconbyLength.Ncol(inarea); l++) {
    BconbyLength[inarea][l] = Bconsumption[l];
    NconbyLength[inarea][l] = (iszero(PopBylength[l].W) ? 0.0 : BconbyLength[inarea][l] / PopBylength[l].W);
    if (iszero(PopBylength[l].N))
      MortbyLength[inarea][l] = 0.0;
    else {
      if (NconbyLength[inarea][l] >= PopBylength[l].N)
        MortbyLength[inarea][l] = MAX_MORTALITY;
      else
        MortbyLength[inarea][l] = -log(1 - NconbyLength[inarea][l] / PopBylength[l].N) * timeratio;
    }
  }
}
