#ifndef stockpredator_h
#define stockpredator_h

#include "commentstream.h"
#include "poppredator.h"

class StockPredator;
class Keeper;
class LengthGroupDivision;
class Prey;
class ConversionIndex;
class AreaClass;

class StockPredator : public PopPredator {
public:
  StockPredator(CommentStream& infile, const char* givenname, const intvector& areas,
    const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv,
    int minage, int maxage, const TimeClass* const TimeInfo, Keeper* const keeper);
  virtual ~StockPredator();
  virtual void Sum(const Agebandmatrix& Alkeys, int area);
  virtual void Eat(int area, double LengthOfStep, double Temperature,
    double Areasize, int CurrentSubstep, int NrOfSubsteps);
  virtual const popinfovector& NumberPriortoEating(int area, const char* preyname) const;
  virtual void AdjustConsumption(int area, int NrOfSubsteps, int CurrentSubstep);
  virtual void Print(ofstream& outfile) const;
  const bandmatrix& Alproportion(int area) const;
  const doublevector& FPhi(int area) const;
  const doublevector& MaxConByLength(int area) const;
  virtual void Reset(const TimeClass* const TimeInfo);
protected:
  virtual void ResizeObjects();
  virtual void CalcMaximumConsumption(double Temperature, int area, int CurrentSubstep,
    int NrOfSubsteps, double LengthOfStep);
  double MaxConsumption(double Length, const Formulavector &Maxconsumption, double Temperature);
  Formulavector maxConsumption;
  Formula halfFeedingValue;
  doublematrix Phi;  //[area][predLengthgroup]
  doublematrix fphi; //[area][predLengthgroup]
  doublematrix fphI; //[area][predLengthgroup]  fphi per substep
  bandmatrixvector Alprop;     //[area][age][length group]
  doublematrix MaxconByLength; //[area][length group]
  Agebandmatrixvector Alkeys;  //[area][age][length group]
};

#endif
