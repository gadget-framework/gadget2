#ifndef mortpredator_h
#define mortpredator_h

#include "lengthpredator.h"
#include "conversionindex.h"
#include "commentstream.h"
#include "formulamatrix.h"
#include "readfunc.h"
#include "mortprey.h"
#include "gadget.h"

class MortPredator;
class Keeper;

class MortPredator : public LengthPredator {
public:
  MortPredator(CommentStream& infile, const char* givenname, const IntVector& areas,
    const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  virtual ~MortPredator();
  virtual void Eat(int area, double LengthOfStep, double Temperature, double Areasize,
    int CurrentSubstep, int NrOfSubsteps);
  virtual void AdjustConsumption(int area, int NrOfSubsteps, int CurrentSubstep) {};
  virtual void Print(ofstream& infile) const;
  virtual const PopInfoVector& NumberPriortoEating(int area, const char* preyname) const;
  void calcFlevel();
  void InitializeCHat(int area, int prey, const AgeBandMatrix& mean_n);
  void calcCHat(int area, const TimeClass* const TimeInfo);
  void Multiply(AgeBandMatrix& stock_alkeys, const DoubleVector& ratio);
  virtual const double consumedBiomass(int prey_nr, int area_nr) const;
  virtual void Reset(const TimeClass* const TimeInfo);
  virtual double getFlevel(int area, const TimeClass* const TimeInfo);
  friend ostream& printc_hat(ostream& o, const MortPredator& pred,
    AreaClass area, int indent);
  int minPreyAge(int prey, int area) const {
    return ((MortPrey*)Preys(prey))->getMeanN(area).Minage();
  };
  int maxPreyAge(int prey, int area) const {
    return ((MortPrey*)Preys(prey))->getMeanN(area).Maxage();
  };
protected:
  int pres_time_step;
  int no_of_time_steps;
  int* year0; //absolute, e.g. 1988   year0[area]
  int* year; //absolute   year[area]
  FormulaVector q1; //used to calculate the fishing mortality
  FormulaVector q2; //level together with effort. Values for each area
  DoubleMatrix effort; //[area][TimeInfo->TotalNoSteps()]
  DoubleMatrix f_level; //[area][TimeInfo->TotalNoSteps()], calculated from calculateFlevel().
  FormulaMatrix f_lev; //[area][TimeInfo->TotalNoSteps()], read from file.
  BandMatrixMatrix c_hat; //[area][prey][age][prey_l] //Modelled catch
  int calc_f_lev; //1 if calcFlevel() is used, 0 if f_lev is read from file.
  int initialisedCHat;
};

#endif
