#ifndef prey_h
#define prey_h

#include "popinfovector.h"
#include "conversion.h"
#include "commentstream.h"
#include "charptrvector.h"
#include "hasname.h"
#include "livesonareas.h"
#include "agebandmatrix.h"
#include "gadget.h"

class Prey;
class Keeper;

enum PreyType { PREYTYPE, MORTPREYTYPE, STOCKPREYTYPE, LENGTHPREYTYPE };

class Prey : public HasName, public LivesOnAreas {
public:
  Prey(CommentStream& infile, const IntVector& areas, const char* givenname, Keeper* const keeper);
  Prey(const DoubleVector& lengths, const IntVector& areas, const char* givenname);
  virtual ~Prey() = 0;
  virtual PreyType preyType() const { return PREYTYPE; };
  virtual void Sum(const AgeBandMatrix& Alkeys, int area, int NrofSubstep) {};
  virtual void Subtract(AgeBandMatrix& Alkeys, int area);
  void AddConsumption(int area, const DoubleIndexVector& predconsumption);
  virtual void SetCI(const LengthGroupDivision* const GivenLDiv);
  virtual void Print(ofstream& outfile) const;
  double Biomass(int area, int length) const { return biomass[AreaNr[area]][length]; };
  double Biomass(int area) const { return total[AreaNr[area]]; };
  int TooMuchConsumption(int area) const { return tooMuchConsumption[AreaNr[area]]; };
  virtual void CheckConsumption(int area, int NrOfSubsteps);
  double Ratio(int area, int length) const { return ratio[AreaNr[area]][length]; };
  double Length(int j) const { return LgrpDiv->Meanlength(j); };
  int NoLengthGroups() const { return LgrpDiv->NoLengthGroups(); };
  const DoubleVector& Bconsumption(int area) const;
  const DoubleVector& OverConsumption(int area) const;
  const LengthGroupDivision* ReturnLengthGroupDiv() const { return LgrpDiv; };
  virtual void Reset();
  const PopInfoVector& NumberPriortoEating(int area) const;
  //The following functions are added 3/8 98 by kgf
  void Multiply(AgeBandMatrix& stock_alkeys, const DoubleVector& rat);
  virtual void calcZ(int area, const DoubleVector& nat_m) {};
  virtual void calcMeanN(int area) {};
  virtual void setCannibalism(int area, const DoubleVector& cann) {};
  virtual void setAgeMatrix(int pred_no, int area, const DoubleVector& agegroupno) {};
  virtual void setConsumption(int area, int pred_no, const BandMatrix& consum) {};
  const DoubleVector& getCons(int area) const { return cons[AreaNr[area]]; };
protected:
  void InitializeObjects();
  ConversionIndex* CI;
  LengthGroupDivision* LgrpDiv;
  PopInfoMatrix Number;              //Number and weight of prey.
  PopInfoMatrix numberPriortoEating; //Number and weight of prey
  DoubleMatrix biomass;              //Biomass of prey in Area.
  DoubleMatrix ratio;                //Ration eaten hopefully < 1.
  DoubleMatrix consumption;          //Consumption of prey.
  IntVector tooMuchConsumption;      //Set if any lengthgr is overconsumed in area
  DoubleVector total;                //if zero the amount of prey in the area is zero.
  DoubleMatrix overconsumption;      //[area][lengthgroup].
  DoubleMatrix overcons;             //overconsumption of prey in subinterval
  DoubleMatrix cons;                 //consumption of prey in subinterval
};

#endif
