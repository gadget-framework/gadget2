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
  GrowthCalcBase(const IntVector& areas);
  ~GrowthCalcBase();
  virtual void GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
    const PopInfoVector& GrEatNumber, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const DoubleVector& Fphi,
    const DoubleVector& Consumption, const LengthGroupDivision* const LgrpDiv) const = 0;
  virtual double getPower() { return 0.0; };
  virtual double getMult() { return 0.0; };
};

//The Multspecies Growthfunction.
class GrowthCalcA : public GrowthCalcBase {
public:
  GrowthCalcA(CommentStream& infile, const IntVector& areas, Keeper* const keeper);
  ~GrowthCalcA();
  virtual void GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
    const PopInfoVector& GrEatNumber, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const DoubleVector& Fphi,
    const DoubleVector& Consumption, const LengthGroupDivision* const LgrpDiv) const;
protected:
  int NumberOfGrowthConstants;
  FormulaVector growthPar;
};

//Read growth from file.
class GrowthCalcB : public GrowthCalcBase {
public:
  GrowthCalcB(CommentStream& infile, const IntVector& areas, const TimeClass* const TimeInfo,
    Keeper* const keeper, const AreaClass* const Area, const CharPtrVector& lenindex);
  ~GrowthCalcB();
  virtual void GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
    const PopInfoVector& GrEatNumber, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const DoubleVector& Fphi,
    const DoubleVector& Consumption, const LengthGroupDivision* const LgrpDiv) const;
protected:
  FormulaMatrixPtrVector lgrowth; //[area][timestep][length group]
  FormulaMatrixPtrVector wgrowth; //[area][timestep][length group]
};

//Von Bertanlanfy growth function.
class GrowthCalcC : public GrowthCalcBase {
public:
  GrowthCalcC(CommentStream& infile, const IntVector& areas,
    const LengthGroupDivision* const LgrpDiv, Keeper* const keeper, const char* refWeight);
  ~GrowthCalcC();
  virtual void GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
    const PopInfoVector& GrEatNumber, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const DoubleVector& Fphi,
    const DoubleVector& Consumption, const LengthGroupDivision* const LgrpDiv) const;
  virtual double getPower() { return lgrowthPar[7]; };
  virtual double getMult() { return lgrowthPar[6]; };
protected:
  int NumberOfWGrowthConstants;
  int NumberOfLGrowthConstants;
  FormulaVector wgrowthPar;
  FormulaVector lgrowthPar;
  DoubleVector Wref;
};

//GrowthFunction based on R. Jones.
class GrowthCalcD : public GrowthCalcBase {
public:
  GrowthCalcD(CommentStream& infile, const IntVector& areas,
    const LengthGroupDivision* const LgrpDiv, Keeper* const keeper, const char* refWeight);
  ~GrowthCalcD();
  virtual void GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
    const PopInfoVector& GrEatNumber, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const DoubleVector& Fphi,
    const DoubleVector& Consumption, const LengthGroupDivision* const LgrpDiv) const;
  virtual double getPower() { return lgrowthPar[7]; };
  virtual double getMult() { return lgrowthPar[6]; };
protected:
  int NumberOfWGrowthConstants;
  int NumberOfLGrowthConstants;
  FormulaVector wgrowthPar;
  FormulaVector lgrowthPar;
  DoubleVector Wref;
};

//GrowthFunction to be tested for capelin.
class GrowthCalcE : public GrowthCalcBase {
public:
  GrowthCalcE(CommentStream& infile, const IntVector& areas, const TimeClass* const TimeInfo,
    const LengthGroupDivision* const LgrpDiv, Keeper* const keeper, const char* refWeight);
  ~GrowthCalcE();
  virtual void GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
    const PopInfoVector& GrEatNumber, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const DoubleVector& Fphi,
    const DoubleVector& Consumption, const LengthGroupDivision* const LgrpDiv) const;
  virtual double getPower() { return lgrowthPar[7]; };
  virtual double getMult() { return lgrowthPar[6]; };
protected:
  int NumberOfWGrowthConstants;
  int NumberOfLGrowthConstants;
  DoubleVector Wref;
  FormulaVector wgrowthPar;
  FormulaVector lgrowthPar;
  FormulaVector yearEffect;
  FormulaVector stepEffect;
  FormulaVector areaEffect;
};

class GrowthCalcF : public GrowthCalcBase {
public:
  GrowthCalcF(CommentStream& infile, const IntVector& areas, const TimeClass* const TimeInfo,
    Keeper* const keeper, const AreaClass* const Area, const CharPtrVector& lenindex);
  ~GrowthCalcF();
  virtual void GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
    const PopInfoVector& GrEatNumber, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const DoubleVector& Fphi,
    const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) const;
protected:
  int NumberOfGrowthConstants;
  FormulaVector growthPar;
  FormulaVector k_values;
  FormulaMatrixPtrVector wgrowth; //[area][timestep][length group]
};

class GrowthCalcG : public GrowthCalcBase {
public:
  GrowthCalcG(CommentStream& infile, const IntVector& areas, const TimeClass* const TimeInfo,
    Keeper* const keeper, const AreaClass* const Area, const CharPtrVector& lenindex);
  ~GrowthCalcG();
  virtual void GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
    const PopInfoVector& GrEatNumber, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const DoubleVector& Fphi,
    const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) const;
protected:
  int NumberOfGrowthConstants;
  FormulaVector growthPar;
  FormulaVector k_values;
  FormulaMatrixPtrVector wgrowth; //[area][timestep][length group]
};

//simplified Von Bertanlanfy growth function.
class GrowthCalcH : public GrowthCalcBase {
public:
  GrowthCalcH(CommentStream& infile, const IntVector& areas,
    const LengthGroupDivision* const LgrpDiv, Keeper* const keeper);
  ~GrowthCalcH();
  virtual void GrowthCalc(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
    const PopInfoVector& GrEatNumber, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const DoubleVector& Fphi,
    const DoubleVector& Consumption, const LengthGroupDivision* const LgrpDiv) const;
  virtual double getPower() { return growthPar[3]; };
  virtual double getMult() { return growthPar[2]; };
protected:
  int NumberOfGrowthConstants;
  FormulaVector growthPar;
};
#endif
