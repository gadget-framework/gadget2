#ifndef poppredator_h
#define poppredator_h

#include "commentstream.h"
#include "agebandm.h"
#include "predator.h"

class PopPredator;
class Keeper;
class LengthGroupDivision;
class Prey;
class ConversionIndex;
class AreaClass;
class Agebandmatrix;

class PopPredator : public Predator {
 public:
  PopPredator(const char* givenname, const intvector& areas,
    const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv);
  virtual ~PopPredator();
  virtual void Print(ofstream& outfile) const;
  virtual const bandmatrix& Consumption(int area, const char* preyname) const;
  virtual const doublevector& Consumption(int area) const;
  virtual const doublevector& OverConsumption(int area) const;
  virtual const LengthGroupDivision* ReturnLengthGroupDiv() const;
  virtual int NoLengthGroups() const;
  virtual double Length(int i) const;
  virtual void Reset(const TimeClass* const TimeInfo);
  virtual const double consumedBiomass(int prey_nr, int area_nr) const;
  void Multiply(Agebandmatrix& stock_alkeys, const doublevector& ratio); //kgf 3/8 98
protected:
  virtual void DeleteParametersForPrey(int prey, Keeper* const keeper);
  virtual void ResizeObjects();
  LengthGroupDivision* LgrpDiv;
  ConversionIndex* CI;
  //storage variables for intermediate calculations.
  popinfomatrix Prednumber;     //[area][predlength]
  doublematrix overconsumption; //[area][predlength]
  bandmatrixmatrix consumption; //[area][prey][predlength][preylength]
  doublematrix totalconsumption;//[area][predlength]
  doublematrix overcons; //[area][predlength] per substep
  bandmatrixmatrix cons; //[area][prey][predlength][preylength] per substep
  doublematrix totalcons;//[area][predlength] per substep
};

#endif
