#ifndef predstdinfobylength_h
#define predstdinfobylength_h

#include "livesonareas.h"
#include "bandmatrix.h"

class PredStdInfoByLength;
class AbstrPreyStdInfoByLength;
class PopPredator;
class Prey;
class StockPrey;
class TimeClass;

class PredStdInfoByLength : protected LivesOnAreas {
public:
  PredStdInfoByLength(const PopPredator* predator, const Prey* prey,
    const intvector& areas);
  PredStdInfoByLength(const PopPredator* predator, const StockPrey* prey,
    const intvector& areas);
  virtual ~PredStdInfoByLength();
  const bandmatrix& NconsumptionByLength(int area) const;
  const bandmatrix& BconsumptionByLength(int area) const;
  const bandmatrix& MortalityByLength(int area) const;
  virtual void Sum(const TimeClass* const TimeInfo, int area);
protected:
  void AdjustObjects();
private:
  AbstrPreyStdInfoByLength* preyinfo;
  const PopPredator* predator;
  const Prey* prey;
  bandmatrixvector MortbyLength; //[area][pred_l][prey_l]
  bandmatrixvector NconbyLength;
  bandmatrixvector BconbyLength;
};

#endif
