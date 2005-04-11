#ifndef poppredator_h
#define poppredator_h

#include "areatime.h"
#include "conversionindex.h"
#include "commentstream.h"
#include "agebandmatrix.h"
#include "predator.h"
#include "keeper.h"

class PopPredator : public Predator {
public:
  PopPredator(const char* givenname, const IntVector& areas,
    const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv);
  virtual ~PopPredator();
  virtual void Print(ofstream& outfile) const;
  virtual const BandMatrix& Consumption(int area, const char* preyname) const;
  virtual const DoubleVector& Consumption(int area) const { return totalconsumption[this->areaNum(area)]; };
  virtual const DoubleVector& OverConsumption(int area) const { return overconsumption[this->areaNum(area)]; };
  virtual double getTotalOverConsumption(int area) const;
  virtual const LengthGroupDivision* returnLengthGroupDiv() const { return LgrpDiv; };
  virtual int numLengthGroups() const { return LgrpDiv->numLengthGroups(); };
  virtual double Length(int i) const { return LgrpDiv->meanLength(i); };
  virtual void Reset(const TimeClass* const TimeInfo);
  virtual const double getConsumptionBiomass(int prey, int area) const;
  void setPrey(PreyPtrVector& preyvec, Keeper* const keeper);
protected:
  LengthGroupDivision* LgrpDiv;
  ConversionIndex* CI;
  //storage variables for intermediate calculations.
  PopInfoMatrix prednumber;     //[area][predlength]
  DoubleMatrix overconsumption; //[area][predlength]
  BandMatrixMatrix consumption; //[area][prey][predlength][preylength]
  DoubleMatrix totalconsumption;//[area][predlength]
  DoubleMatrix overcons; //[area][predlength] per substep
  BandMatrixMatrix cons; //[area][prey][predlength][preylength] per substep
  DoubleMatrix totalcons;//[area][predlength] per substep
};

#endif
