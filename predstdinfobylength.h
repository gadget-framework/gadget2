#ifndef predstdinfobylength_h
#define predstdinfobylength_h

#include "areatime.h"
#include "lengthgroup.h"
#include "livesonareas.h"
#include "bandmatrix.h"

class PredStdInfoByLength;
class AbstrPreyStdInfoByLength;
class PopPredator;
class Prey;
class StockPrey;

class PredStdInfoByLength : protected LivesOnAreas {
public:
  PredStdInfoByLength(const PopPredator* predator, const Prey* prey,
    const IntVector& areas);
  PredStdInfoByLength(const PopPredator* predator, const StockPrey* prey,
    const IntVector& areas);
  virtual ~PredStdInfoByLength();
  const BandMatrix& NconsumptionByLength(int area) const;
  const BandMatrix& BconsumptionByLength(int area) const;
  const BandMatrix& MortalityByLength(int area) const;
  virtual void Sum(const TimeClass* const TimeInfo, int area);
  const LengthGroupDivision* ReturnPredLengthGroupDiv() const { return PredLgrpDiv; };
  const LengthGroupDivision* ReturnPreyLengthGroupDiv() const { return PreyLgrpDiv; };
protected:
  void AdjustObjects();
private:
  AbstrPreyStdInfoByLength* preyinfo;
  const PopPredator* predator;
  const Prey* prey;
  BandMatrixVector MortbyLength; //[area][pred_l][prey_l]
  BandMatrixVector NconbyLength;
  BandMatrixVector BconbyLength;
  const LengthGroupDivision* PredLgrpDiv;
  const LengthGroupDivision* PreyLgrpDiv;
};

#endif
