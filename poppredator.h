#ifndef poppredator_h
#define poppredator_h

#include "areatime.h"
#include "conversionindex.h"
#include "commentstream.h"
#include "agebandmatrix.h"
#include "predator.h"

class PopPredator;
class Keeper;
class Prey;
class AgeBandMatrix;

class PopPredator : public Predator {
public:
  PopPredator(const char* givenname, const IntVector& areas,
    const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv);
  virtual ~PopPredator();
  virtual void Print(ofstream& outfile) const;
  virtual const BandMatrix& Consumption(int area, const char* preyname) const;
  virtual const DoubleVector& Consumption(int area) const;
  virtual const DoubleVector& OverConsumption(int area) const;
  virtual const LengthGroupDivision* ReturnLengthGroupDiv() const;
  virtual int NoLengthGroups() const;
  virtual double Length(int i) const;
  virtual void Reset(const TimeClass* const TimeInfo);
  virtual const double consumedBiomass(int prey_nr, int area_nr) const;
  void Multiply(AgeBandMatrix& stock_alkeys, const DoubleVector& ratio); //kgf 3/8 98
protected:
  virtual void DeleteParametersForPrey(int prey, Keeper* const keeper);
  virtual void ResizeObjects();
  LengthGroupDivision* LgrpDiv;
  ConversionIndex* CI;
  //storage variables for intermediate calculations.
  PopInfoMatrix Prednumber;     //[area][predlength]
  DoubleMatrix overconsumption; //[area][predlength]
  BandMatrixMatrix consumption; //[area][prey][predlength][preylength]
  DoubleMatrix totalconsumption;//[area][predlength]
  DoubleMatrix overcons; //[area][predlength] per substep
  BandMatrixMatrix cons; //[area][prey][predlength][preylength] per substep
  DoubleMatrix totalcons;//[area][predlength] per substep
};

#endif
