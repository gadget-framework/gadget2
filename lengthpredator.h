#ifndef lengthpredator_h
#define lengthpredator_h

#include "poppredator.h"

class LengthPredator : public PopPredator {
public:
  LengthPredator(const char* givenname, const IntVector& areas,
    const LengthGroupDivision* const OtherLgrpDiv,
    const LengthGroupDivision* const GivenLgrpDiv,
    double Multiplicative);
  virtual ~LengthPredator();
  virtual void Sum(const PopInfoVector& NumberInarea, int area);
  double Scaler(int area) const { return scaler[AreaNr[area]]; };
  virtual double getFlevel(int area, const TimeClass* const TimeInfo)
    { return -1.0; }; //meaningfull only for mortality model
protected:
  DoubleVector scaler;
  double Multiplicative;
};

#endif
