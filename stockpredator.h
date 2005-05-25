#ifndef stockpredator_h
#define stockpredator_h

#include "areatime.h"
#include "lengthgroup.h"
#include "commentstream.h"
#include "agebandmatrixptrvector.h"
#include "poppredator.h"

class StockPredator : public PopPredator {
public:
  StockPredator(CommentStream& infile, const char* givenname, const IntVector& areas,
    const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv,
    int minage, int maxage, const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the default StockPredator destructor
   */
  virtual ~StockPredator() {};
  /**
   * \brief This will calculate the amount the predator consumes on a given area
   * \param area is the area that the prey consumption is being calculated on
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Eat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void Sum(const AgeBandMatrix& Alkeys, int area);
  /**
   * \brief This will adjust the amount the predator consumes on a given area, to take oversconsumption into consideration
   * \param area is the area that the prey consumption is being calculated on
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void adjustConsumption(int area, const TimeClass* const TimeInfo);
  virtual const PopInfoVector& getNumberPriorToEating(int area, const char* preyname) const;
  virtual void Print(ofstream& outfile) const;
  const BandMatrix& Alproportion(int area) const { return Alprop[this->areaNum(area)]; };
  const DoubleVector& FPhi(int area) const { return fphi[this->areaNum(area)]; };
  const DoubleVector& maxConByLength(int area) const { return maxconbylength[this->areaNum(area)]; };
  virtual void Reset(const TimeClass* const TimeInfo);
protected:
  virtual void calcMaxConsumption(double Temperature, int inarea, const TimeClass* const TimeInfo);
  FormulaVector maxcons;
  Formula halfFeedingValue;
  DoubleMatrix Phi;  //[area][len]
  DoubleMatrix fphi; //[area][len]
  DoubleMatrix subfphi; //[area][len]  fphi per substep
  DoubleMatrix maxconbylength; //[area][len]
  BandMatrixPtrVector Alprop;     //[area][age][len]
  AgeBandMatrixPtrVector Alkeys;  //[area][age][len]
};

#endif
