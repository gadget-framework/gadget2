#ifndef prey_h
#define prey_h

#include "popinfovector.h"
#include "commentstream.h"
#include "charptrvector.h"
#include "hasname.h"
#include "livesonareas.h"
#include "predatorptrvector.h"
#include "agebandm.h"

class Prey;
class ConversionIndex;
class LengthGroupDivision;
class Keeper;

enum prey_type { PREY_TYPE, MORTPREY_TYPE, STOCKPREY_TYPE, LENGTHPREY_TYPE };

class Prey : public HasName, public LivesOnAreas {
public:
  Prey(CommentStream& infile, const intvector& areas, const char* givenname, Keeper* const keeper);
  Prey(const doublevector& lengths, const intvector& areas, const char* givenname);
  virtual ~Prey() = 0;
  virtual prey_type preyType() const { return PREY_TYPE; };
  virtual void Sum(const Agebandmatrix& Alkeys, int area, int NrofSubstep) {};
  virtual void Subtract(Agebandmatrix& Alkeys, int area);
  void AddConsumption(int area, const doubleindexvector& predconsumption);
  virtual void SetCI(const LengthGroupDivision* const GivenLDiv);
  virtual void Print(ofstream& outfile) const;
  double Biomass(int area, int length) const { return biomass[AreaNr[area]][length]; };
  double Biomass(int area) const { return total[AreaNr[area]]; };
  int TooMuchConsumption(int area) const { return tooMuchConsumption[AreaNr[area]]; };
  virtual void CheckConsumption(int area, int NrOfSubsteps);
  double Ratio(int area, int length) const { return ratio[AreaNr[area]][length]; };
  double Length(int j) const { return LgrpDiv->Meanlength(j); };
  int NoLengthGroups() const { return LgrpDiv->NoLengthGroups(); };
  const doublevector& Bconsumption(int area) const;
  const doublevector& OverConsumption(int area) const;
  const LengthGroupDivision* ReturnLengthGroupDiv() const { return LgrpDiv; };
  virtual void Reset();
  const popinfovector& NumberPriortoEating(int area) const;
  //The following functions are added 3/8 98 by kgf
  void Multiply(Agebandmatrix& stock_alkeys, const doublevector& rat);
  virtual void calcZ(int area, const doublevector& nat_m) {};
  virtual void calcMeanN(int area) {};
  virtual void setCannibalism(int area, const doublevector& cann) {};
  virtual void setAgeMatrix(int pred_no, int area, const doublevector& agegroupno) {};
  virtual void setConsumption(int area, int pred_no, const bandmatrix& consum) {};
  const doublevector& getCons(int area) const { return cons[AreaNr[area]]; };
protected:
  void InitializeObjects();
  ConversionIndex* CI;
  LengthGroupDivision* LgrpDiv;
  popinfomatrix Number;              //Number and weight of prey.
  popinfomatrix numberPriortoEating; //Number and weight of prey
  doublematrix biomass;              //Biomass of prey in Area.
  doublematrix ratio;                //Ration eaten hopefully < 1.
  doublematrix consumption;          //Consumption of prey.
  intvector tooMuchConsumption;      //Set if any lengthgr is overconsumed in area
  doublevector total;                //if zero the amount of prey in the area is zero.
  doublematrix overconsumption;      //[area][lengthgroup].
  doublematrix overcons;             //overconsumption of prey in subinterval
  doublematrix cons;                 //consumption of prey in subinterval
};

#endif
