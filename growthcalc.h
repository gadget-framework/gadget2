#ifndef growthcalc_h
#define growthcalc_h

#include "formulamatrixptrvector.h"
#include "commentstream.h"
#include "popinfovector.h"
#include "livesonareas.h"

class AreaClass;
class TimeClass;
class LengthGroupDivision;
class Keeper;

class GrowthCalcBase : protected LivesOnAreas {
public:
  GrowthCalcBase(const intvector& areas);
  ~GrowthCalcBase();
  virtual void GrowthCalc(int area, doublevector& Lgrowth, doublevector& Wgrowth,
    const popinfovector& GrEatNumber, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const doublevector& Fphi,
    const doublevector& Consumption, const LengthGroupDivision* const LgrpDiv) const = 0;
  virtual double getPower() { return 0.0; };
  virtual double getMult() { return 0.0; };
};

//The Multspecies Growthfunction.
class GrowthCalcA : public GrowthCalcBase {
public:
  GrowthCalcA(CommentStream& infile, const intvector& areas, Keeper* const keeper);
  ~GrowthCalcA();
  virtual void GrowthCalc(int area, doublevector& Lgrowth, doublevector& Wgrowth,
    const popinfovector& GrEatNumber, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const doublevector& Fphi,
    const doublevector& Consumption, const LengthGroupDivision* const LgrpDiv) const;
protected:
  int NumberOfGrowthConstants;
  Formulavector growthPar;
};

//Read growth from file.
class GrowthCalcB : public GrowthCalcBase {
public:
  GrowthCalcB(CommentStream& infile, const intvector& areas, const TimeClass* const TimeInfo,
    Keeper* const keeper, const AreaClass* const Area, const charptrvector& lenindex);
  ~GrowthCalcB();
  virtual void GrowthCalc(int area, doublevector& Lgrowth, doublevector& Wgrowth,
    const popinfovector& GrEatNumber, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const doublevector& Fphi,
    const doublevector& Consumption, const LengthGroupDivision* const LgrpDiv) const;
protected:
  Formulamatrixptrvector lgrowth; //[area][timestep][length group]
  Formulamatrixptrvector wgrowth; //[area][timestep][length group]
};

//Von Bertanlanfy growth function.
class GrowthCalcC : public GrowthCalcBase {
public:
  GrowthCalcC(CommentStream& infile, const intvector& areas,
    const LengthGroupDivision* const LgrpDiv, Keeper* const keeper, const char* refWeight);
  ~GrowthCalcC();
  virtual void GrowthCalc(int area, doublevector& Lgrowth, doublevector& Wgrowth,
    const popinfovector& GrEatNumber, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const doublevector& Fphi,
    const doublevector& Consumption, const LengthGroupDivision* const LgrpDiv) const;
  virtual double getPower() { return lgrowthPar[7]; };
  virtual double getMult() { return lgrowthPar[6]; };
protected:
  int NumberOfWGrowthConstants;
  int NumberOfLGrowthConstants;
  Formulavector wgrowthPar;
  Formulavector lgrowthPar;
  doublevector Wref;
};

//GrowthFunction based on R. Jones.
class GrowthCalcD : public GrowthCalcBase {
public:
  GrowthCalcD(CommentStream& infile, const intvector& areas,
    const LengthGroupDivision* const LgrpDiv, Keeper* const keeper, const char* refWeight);
  ~GrowthCalcD();
  virtual void GrowthCalc(int area, doublevector& Lgrowth, doublevector& Wgrowth,
    const popinfovector& GrEatNumber, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const doublevector& Fphi,
    const doublevector& Consumption, const LengthGroupDivision* const LgrpDiv) const;
  virtual double getPower() { return lgrowthPar[7]; };
  virtual double getMult() { return lgrowthPar[6]; };
protected:
  int NumberOfWGrowthConstants;
  int NumberOfLGrowthConstants;
  Formulavector wgrowthPar;
  Formulavector lgrowthPar;
  doublevector Wref;
};

//GrowthFunction to be tested for capelin.
class GrowthCalcE : public GrowthCalcBase {
public:
  GrowthCalcE(CommentStream& infile, const intvector& areas, const TimeClass* const TimeInfo,
    const LengthGroupDivision* const LgrpDiv, Keeper* const keeper, const char* refWeight);
  ~GrowthCalcE();
  virtual void GrowthCalc(int area, doublevector& Lgrowth, doublevector& Wgrowth,
    const popinfovector& GrEatNumber, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const doublevector& Fphi,
    const doublevector& Consumption, const LengthGroupDivision* const LgrpDiv) const;
  virtual double getPower() { return lgrowthPar[7]; };
  virtual double getMult() { return lgrowthPar[6]; };
protected:
  int NumberOfWGrowthConstants;
  int NumberOfLGrowthConstants;
  doublevector Wref;
  Formulavector wgrowthPar;
  Formulavector lgrowthPar;
  Formulavector yearEffect;
  Formulavector stepEffect;
  Formulavector areaEffect;
};

class GrowthCalcF : public GrowthCalcBase {
public:
  GrowthCalcF(CommentStream& infile, const intvector& areas, const TimeClass* const TimeInfo,
    Keeper* const keeper, const AreaClass* const Area, const charptrvector& lenindex);
  ~GrowthCalcF();
  virtual void GrowthCalc(int area, doublevector& Lgrowth, doublevector& Wgrowth,
    const popinfovector& GrEatNumber, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const doublevector& Fphi,
    const doublevector& MaxCon, const LengthGroupDivision* const LgrpDiv) const;
protected:
  int NumberOfGrowthConstants;
  Formulavector growthPar;
  Formulavector k_values;
  Formulamatrixptrvector wgrowth; //[area][timestep][length group]
};

class GrowthCalcG : public GrowthCalcBase {
public:
  GrowthCalcG(CommentStream& infile, const intvector& areas, const TimeClass* const TimeInfo,
    Keeper* const keeper, const AreaClass* const Area, const charptrvector& lenindex);
  ~GrowthCalcG();
  virtual void GrowthCalc(int area, doublevector& Lgrowth, doublevector& Wgrowth,
    const popinfovector& GrEatNumber, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const doublevector& Fphi,
    const doublevector& MaxCon, const LengthGroupDivision* const LgrpDiv) const;
protected:
  int NumberOfGrowthConstants;
  Formulavector growthPar;
  Formulavector k_values;
  Formulamatrixptrvector wgrowth; //[area][timestep][length group]
};

//simplified Von Bertanlanfy growth function.
class GrowthCalcH : public GrowthCalcBase {
public:
  GrowthCalcH(CommentStream& infile, const intvector& areas,
    const LengthGroupDivision* const LgrpDiv, Keeper* const keeper);
  ~GrowthCalcH();
  virtual void GrowthCalc(int area, doublevector& Lgrowth, doublevector& Wgrowth,
    const popinfovector& GrEatNumber, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const doublevector& Fphi,
    const doublevector& Consumption, const LengthGroupDivision* const LgrpDiv) const;
  virtual double getPower() { return growthPar[3]; };
  virtual double getMult() { return growthPar[2]; };
protected:
  int NumberOfGrowthConstants;
  Formulavector growthPar;
};
#endif
