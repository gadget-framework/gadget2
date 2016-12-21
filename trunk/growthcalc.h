#ifndef growthcalc_h
#define growthcalc_h

#include "areatime.h"
#include "conversionindex.h"
#include "formulamatrixptrvector.h"
#include "commentstream.h"
#include "popinfovector.h"
#include "livesonareas.h"
#include "modelvariablevector.h"
#include "keeper.h"

/**
 * \class GrowthCalcBase
 * \brief This is the base class used to calculate the growth of a stock within the model
 * \note This will always be overridden by the derived classes that actually calculate the growth
 */
class GrowthCalcBase : protected LivesOnAreas {
public:
  /**
   * \brief This is the default GrowthCalcBase constructor
   * \param Areas is the IntVector of areas that the growth calculation can take place on
   */
  GrowthCalcBase(const IntVector& Areas);
  /**
   * \brief This is the default GrowthCalcBase destructor
   */
  ~GrowthCalcBase() {};
  /**
   * \brief This is the function that calculates the growth
   * \param area is the area that the growth is being calculated on
   * \param Lgrowth is the DoubleVector of the mean length increase for each length group of the stock
   * \param Wgrowth is the DoubleVector of the mean weight increase for each length group of the stock
   * \param numGrow is the PopInfoVector of the current population of the stock
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param Fphi is the DoubleVector of the feeding level of the stock
   * \param MaxCon is the DoubleVector of the maximum consumption of the stock
   * \param LgrpDiv is the LengthGroupDivision of the stock
   * \note This will be overridden by the derived classes that actually calculate the growth
   */
  virtual void calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
    const PopInfoVector& numGrow, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const DoubleVector& Fphi,
    const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv) = 0;
  /**
   * \brief This will return the power term of the length - weight relationship
   * \return 0 (will be overridden in derived classes)
   */
  virtual double getPower() { return 0.0; };
  /**
   * \brief This will return the multiplier of the length - weight relationship
   * \return 0 (will be overridden in derived classes)
   */
  virtual double getMult() { return 0.0; };
};

/**
 * \class GrowthCalcA
 * \brief This is the class used to calculate the growth of a stock using the MULTSPEC growth function
 */
class GrowthCalcA : public GrowthCalcBase {
public:
  /**
   * \brief This is the default GrowthCalcA constructor
   * \param infile is the CommentStream to read the growth parameters from
   * \param Areas is the IntVector of areas that the growth calculation can take place on
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  GrowthCalcA(CommentStream& infile, const IntVector& Areas,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the default GrowthCalcA destructor
   */
  ~GrowthCalcA() {};
  /**
   * \brief This is the function that calculates the growth using the MULTSPEC function
   * \param area is the area that the growth is being calculated on
   * \param Lgrowth is the DoubleVector of the mean length increase for each length group of the stock
   * \param Wgrowth is the DoubleVector of the mean weight increase for each length group of the stock
   * \param numGrow is the PopInfoVector of the current population of the stock
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param Fphi is the DoubleVector of the feeding level of the stock
   * \param MaxCon is the DoubleVector of the maximum consumption of the stock
   * \param LgrpDiv is the LengthGroupDivision of the stock
   */
  virtual void calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
    const PopInfoVector& numGrow, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const DoubleVector& Fphi,
    const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv);
protected:
  /**
   * \brief This is the number of growth parameters (set to 9)
   */
  int numGrowthConstants;
  /**
   * \brief This is the ModelVariableVector of growth parameters
   */
  ModelVariableVector growthPar;
};

/**
 * \class GrowthCalcB
 * \brief This is the class used to calculate the growth when reading the growth directly from a file
 */
class GrowthCalcB : public GrowthCalcBase {
public:
  /**
   * \brief This is the default GrowthCalcB constructor
   * \param infile is the CommentStream to read the growth parameters from
   * \param Areas is the IntVector of areas that the growth calculation can take place on
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param Area is the AreaClass for the current model
   * \param lenindex is the CharPtrVector of the lengths used for the growth of the stock
   */
  GrowthCalcB(CommentStream& infile, const IntVector& Areas, const TimeClass* const TimeInfo,
    Keeper* const keeper, const AreaClass* const Area, const CharPtrVector& lenindex);
  /**
   * \brief This is the default GrowthCalcB destructor
   */
  ~GrowthCalcB();
  /**
   * \brief This is the function that calculates the growth read from file
   * \param area is the area that the growth is being calculated on
   * \param Lgrowth is the DoubleVector of the mean length increase for each length group of the stock
   * \param Wgrowth is the DoubleVector of the mean weight increase for each length group of the stock
   * \param numGrow is the PopInfoVector of the current population of the stock
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param Fphi is the DoubleVector of the feeding level of the stock
   * \param MaxCon is the DoubleVector of the maximum consumption of the stock
   * \param LgrpDiv is the LengthGroupDivision of the stock
   */
  virtual void calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
    const PopInfoVector& numGrow, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const DoubleVector& Fphi,
    const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv);
protected:
  /**
   * \brief This is the FormulaMatrixPtrVector of increase in length for each length group
   */
  FormulaMatrixPtrVector lgrowth;
  /**
   * \brief This is the FormulaMatrixPtrVector of increase in weight for each length group
   */
  FormulaMatrixPtrVector wgrowth;
};

/**
 * \class GrowthCalcC
 * \brief This is the class used to calculate the growth of a stock using a weight-based Von Bertalanffy growth function
 */
class GrowthCalcC : public GrowthCalcBase {
public:
  /**
   * \brief This is the default GrowthCalcC constructor
   * \param infile is the CommentStream to read the growth parameters from
   * \param Areas is the IntVector of areas that the growth calculation can take place on
   * \param TimeInfo is the TimeClass for the current model
   * \param LgrpDiv is the LengthGroupDivision of the stock
   * \param keeper is the Keeper for the current model
   * \param refWeightFile is the name of the file containing the reference weight information for the stock
   */
  GrowthCalcC(CommentStream& infile, const IntVector& Areas, const TimeClass* const TimeInfo,
    const LengthGroupDivision* const LgrpDiv, Keeper* const keeper, const char* refWeightFile);
  /**
   * \brief This is the default GrowthCalcC destructor
   */
  ~GrowthCalcC() {};
  /**
   * \brief This is the function that calculates the growth using a weight-based Von Bertalanffy function
   * \param area is the area that the growth is being calculated on
   * \param Lgrowth is the DoubleVector of the mean length increase for each length group of the stock
   * \param Wgrowth is the DoubleVector of the mean weight increase for each length group of the stock
   * \param numGrow is the PopInfoVector of the current population of the stock
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param Fphi is the DoubleVector of the feeding level of the stock
   * \param MaxCon is the DoubleVector of the maximum consumption of the stock
   * \param LgrpDiv is the LengthGroupDivision of the stock
   */
  virtual void calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
    const PopInfoVector& numGrow, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const DoubleVector& Fphi,
    const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv);
  /**
   * \brief This will return the power term of the length - weight relationship
   * \return lgrowthPar[7]
   */
  virtual double getPower() { return lgrowthPar[7]; };
  /**
   * \brief This will return the multiplier of the length - weight relationship
   * \return lgrowthPar[6]
   */
  virtual double getMult() { return lgrowthPar[6]; };
protected:
  /**
   * \brief This is the number of weight growth parameters (set to 6)
   */
  int numWeightGrowthConstants;
  /**
   * \brief This is the number of length growth parameters (set to 9)
   */
  int numLengthGrowthConstants;
  /**
   * \brief This is the ModelVariableVector of weight growth parameters
   */
  ModelVariableVector wgrowthPar;
  /**
   * \brief This is the ModelVariableVector of length growth parameters
   */
  ModelVariableVector lgrowthPar;
  /**
   * \brief This is the DoubleVector of the reference weight values
   */
  DoubleVector refWeight;
};

/**
 * \class GrowthCalcD
 * \brief This is the class used to calculate the growth of a stock using the Jones growth function
 */
class GrowthCalcD : public GrowthCalcBase {
public:
  /**
   * \brief This is the default GrowthCalcD constructor
   * \param infile is the CommentStream to read the growth parameters from
   * \param Areas is the IntVector of areas that the growth calculation can take place on
   * \param TimeInfo is the TimeClass for the current model
   * \param LgrpDiv is the LengthGroupDivision of the stock
   * \param keeper is the Keeper for the current model
   * \param refWeightFile is the name of the file containing the reference weight information for the stock
   */
  GrowthCalcD(CommentStream& infile, const IntVector& Areas, const TimeClass* const TimeInfo,
    const LengthGroupDivision* const LgrpDiv, Keeper* const keeper, const char* refWeightFile);
  /**
   * \brief This is the default GrowthCalcD destructor
   */
  ~GrowthCalcD() {};
  /**
   * \brief This is the function that calculates the growth using the Jones growth function
   * \param area is the area that the growth is being calculated on
   * \param Lgrowth is the DoubleVector of the mean length increase for each length group of the stock
   * \param Wgrowth is the DoubleVector of the mean weight increase for each length group of the stock
   * \param numGrow is PopInfoVector of the the current population of the stock
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param Fphi is the DoubleVector of the feeding level of the stock
   * \param MaxCon is the DoubleVector of the maximum consumption of the stock
   * \param LgrpDiv is the LengthGroupDivision of the stock
   */
  virtual void calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
    const PopInfoVector& numGrow, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const DoubleVector& Fphi,
    const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv);
  /**
   * \brief This will return the power term of the length - weight relationship
   * \return lgrowthPar[7]
   */
  virtual double getPower() { return lgrowthPar[7]; };
  /**
   * \brief This will return the multiplier of the length - weight relationship
   * \return lgrowthPar[6]
   */
  virtual double getMult() { return lgrowthPar[6]; };
protected:
  /**
   * \brief This is the number of weight growth parameters (set to 6)
   */
  int numWeightGrowthConstants;
  /**
   * \brief This is the number of length growth parameters (set to 9)
   */
  int numLengthGrowthConstants;
  /**
   * \brief This is the ModelVariableVector of weight growth parameters
   */
  ModelVariableVector wgrowthPar;
  /**
   * \brief This is the ModelVariableVector of length growth parameters
   */
  ModelVariableVector lgrowthPar;
  /**
   * \brief This is the DoubleVector of the reference weight values
   */
  DoubleVector refWeight;
};

/**
 * \class GrowthCalcE
 * \brief This is the class used to calculate the growth of a stock using an extended form of the Von Bertalanffy function
 */
class GrowthCalcE : public GrowthCalcBase {
public:
  /**
   * \brief This is the default GrowthCalcE constructor
   * \param infile is the CommentStream to read the growth parameters from
   * \param Areas is the IntVector of areas that the growth calculation can take place on
   * \param TimeInfo is the TimeClass for the current model
   * \param LgrpDiv is the LengthGroupDivision of the stock
   * \param keeper is the Keeper for the current model
   * \param refWeightFile is the name of the file containing the reference weight information for the stock
   */
  GrowthCalcE(CommentStream& infile, const IntVector& Areas, const TimeClass* const TimeInfo,
    const LengthGroupDivision* const LgrpDiv, Keeper* const keeper, const char* refWeightFile);
  /**
   * \brief This is the default GrowthCalcE destructor
   */
  ~GrowthCalcE() {};
  /**
   * \brief This is the function that calculates the growth using an extended Von Bertalanffy growth function
   * \param area is the area that the growth is being calculated on
   * \param Lgrowth is the DoubleVector of the mean length increase for each length group of the stock
   * \param Wgrowth is the DoubleVector of the mean weight increase for each length group of the stock
   * \param numGrow is the PopInfoVector of the current population of the stock
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param Fphi is the DoubleVector of the feeding level of the stock
   * \param MaxCon is the DoubleVector of the maximum consumption of the stock
   * \param LgrpDiv is the LengthGroupDivision of the stock
   */
  virtual void calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
    const PopInfoVector& numGrow, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const DoubleVector& Fphi,
    const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv);
  /**
   * \brief This will return the power term of the length - weight relationship
   * \return lgrowthPar[7]
   */
  virtual double getPower() { return lgrowthPar[7]; };
  /**
   * \brief This will return the multiplier of the length - weight relationship
   * \return lgrowthPar[6]
   */
  virtual double getMult() { return lgrowthPar[6]; };
protected:
  /**
   * \brief This is the number of weight growth parameters (set to 6)
   */
  int numWeightGrowthConstants;
  /**
   * \brief This is the number of length growth parameters (set to 9)
   */
  int numLengthGrowthConstants;
  /**
   * \brief This is the ModelVariableVector of weight growth parameters
   */
  ModelVariableVector wgrowthPar;
  /**
   * \brief This is the ModelVariableVector of length growth parameters
   */
  ModelVariableVector lgrowthPar;
  /**
   * \brief This is the DoubleVector of the reference weight values
   */
  DoubleVector refWeight;
  /**
   * \brief This is the FormulaVector of parameters for the year effect
   */
  FormulaVector yearEffect;
  /**
   * \brief This is the FormulaVector of parameters for the step effect
   */
  FormulaVector stepEffect;
  /**
   * \brief This is the FormulaVector of parameters for the area effect
   */
  FormulaVector areaEffect;
};

/**
 * \class GrowthCalcF
 * \brief This is the class used to calculate the growth of a stock using a length based Von Bertalanffy growth function
 */
class GrowthCalcF : public GrowthCalcBase {
public:
  /**
   * \brief This is the default GrowthCalcF constructor
   * \param infile is the CommentStream to read the growth parameters from
   * \param Areas is the IntVector of areas that the growth calculation can take place on
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param Area is the AreaClass for the current model
   * \param lenindex is the CharPtrVector of the lengths used for the growth of the stock
   */
  GrowthCalcF(CommentStream& infile, const IntVector& Areas, const TimeClass* const TimeInfo,
    Keeper* const keeper, const AreaClass* const Area, const CharPtrVector& lenindex);
  /**
   * \brief This is the default GrowthCalcF destructor
   */
  ~GrowthCalcF();
  /**
   * \brief This is the function that calculates the growth using a length based Von Bertalanffy growth function
   * \param area is the area that the growth is being calculated on
   * \param Lgrowth is the DoubleVector of the mean length increase for each length group of the stock
   * \param Wgrowth is the DoubleVector of the mean weight increase for each length group of the stock
   * \param numGrow is the PopInfoVector of the current population of the stock
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param Fphi is the DoubleVector of the feeding level of the stock
   * \param MaxCon is the DoubleVector of the maximum consumption of the stock
   * \param LgrpDiv is the LengthGroupDivision of the stock
   */
  virtual void calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
    const PopInfoVector& numGrow, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const DoubleVector& Fphi,
    const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv);
protected:
  /**
   * \brief This is the number of growth parameters (set to 2)
   */
  int numGrowthConstants;
  /**
   * \brief This is the ModelVariableVector of growth parameters
   */
  ModelVariableVector growthPar;
  /**
   * \brief This is the FormulaMatrixPtrVector of increase in weight for each length group
   */
  FormulaMatrixPtrVector wgrowth;
};

/**
 * \class GrowthCalcG
 * \brief This is the class used to calculate the growth of a stock using a power growth function
 */
class GrowthCalcG : public GrowthCalcBase {
public:
  /**
   * \brief This is the default GrowthCalcG constructor
   * \param infile is the CommentStream to read the growth parameters from
   * \param Areas is the IntVector of areas that the growth calculation can take place on
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param Area is the AreaClass for the current model
   * \param lenindex is the CharPtrVector of the lengths used for the growth of the stock
   */
  GrowthCalcG(CommentStream& infile, const IntVector& Areas, const TimeClass* const TimeInfo,
    Keeper* const keeper, const AreaClass* const Area, const CharPtrVector& lenindex);
  /**
   * \brief This is the default GrowthCalcG destructor
   */
  ~GrowthCalcG();
  /**
   * \brief This is the function that calculates the growth using a power growth function
   * \param area is the area that the growth is being calculated on
   * \param Lgrowth is the DoubleVector of the mean length increase for each length group of the stock
   * \param Wgrowth is the DoubleVector of the mean weight increase for each length group of the stock
   * \param numGrow is PopInfoVector of the the current population of the stock
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param Fphi is the DoubleVector of the feeding level of the stock
   * \param MaxCon is the DoubleVector of the maximum consumption of the stock
   * \param LgrpDiv is the LengthGroupDivision of the stock
   */
  virtual void calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
    const PopInfoVector& numGrow, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const DoubleVector& Fphi,
    const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv);
protected:
  /**
   * \brief This is the number of growth parameters (set to 2)
   */
  int numGrowthConstants;
  /**
   * \brief This is the ModelVariableVector of growth parameters
   */
  ModelVariableVector growthPar;
  /**
   * \brief This is the FormulaMatrixPtrVector of increase in weight for each length group
   */
  FormulaMatrixPtrVector wgrowth;
};

/**
 * \class GrowthCalcH
 * \brief This is the class used to calculate the growth of a stock using a simple length based Von Bertalanffy growth function
 */
class GrowthCalcH : public GrowthCalcBase {
public:
  /**
   * \brief This is the default GrowthCalcH constructor
   * \param infile is the CommentStream to read the growth parameters from
   * \param Areas is the IntVector of areas that the growth calculation can take place on
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  GrowthCalcH(CommentStream& infile, const IntVector& Areas,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the default GrowthCalcH destructor
   */
  ~GrowthCalcH() {};
  /**
   * \brief This is the function that calculates the growth using a simple length based Von Bertalanffy growth function
   * \param area is the area that the growth is being calculated on
   * \param Lgrowth is the DoubleVector of the mean length increase for each length group of the stock
   * \param Wgrowth is the DoubleVector of the mean weight increase for each length group of the stock
   * \param numGrow is the PopInfoVector of the current population of the stock
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param Fphi is the DoubleVector of the feeding level of the stock
   * \param MaxCon is the DoubleVector of the maximum consumption of the stock
   * \param LgrpDiv is the LengthGroupDivision of the stock
   */
  virtual void calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
    const PopInfoVector& numGrow, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const DoubleVector& Fphi,
    const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv);
  /**
   * \brief This will return the power term of the length - weight relationship
   * \return growthPar[3]
   */
  virtual double getPower() { return growthPar[3]; };
  /**
   * \brief This will return the multiplier of the length - weight relationship
   * \return growthPar[2]
   */
  virtual double getMult() { return growthPar[2]; };
protected:
  /**
   * \brief This is the number of growth parameters (set to 4)
   */
  int numGrowthConstants;
  /**
   * \brief This is the ModelVariableVector of growth parameters
   */
  ModelVariableVector growthPar;
};

/**
 * \class GrowthCalcI
 * \brief This is the class used to calculate the growth of a stock using a simple Jones growth function
 */
class GrowthCalcI : public GrowthCalcBase {
public:
  /**
   * \brief This is the default GrowthCalcI constructor
   * \param infile is the CommentStream to read the growth parameters from
   * \param Areas is the IntVector of areas that the growth calculation can take place on
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  GrowthCalcI(CommentStream& infile, const IntVector& Areas,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the default GrowthCalcI destructor
   */
  ~GrowthCalcI() {};
  /**
   * \brief This is the function that calculates the growth using a simple Jones growth function
   * \param area is the area that the growth is being calculated on
   * \param Lgrowth is the DoubleVector of the mean length increase for each length group of the stock
   * \param Wgrowth is the DoubleVector of the mean weight increase for each length group of the stock
   * \param numGrow is the PopInfoVector of the current population of the stock
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param Fphi is the DoubleVector of the feeding level of the stock
   * \param MaxCon is the DoubleVector of the maximum consumption of the stock
   * \param LgrpDiv is the LengthGroupDivision of the stock
   */
  virtual void calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
    const PopInfoVector& numGrow, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const DoubleVector& Fphi,
    const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv);
  /**
   * \brief This will return the power term of the length - weight relationship
   * \return growthPar[4]
   */
  virtual double getPower() { return growthPar[4]; };
  /**
   * \brief This will return the multiplier of the length - weight relationship
   * \return growthPar[5]
   */
  virtual double getMult() { return growthPar[5]; };
protected:
  /**
   * \brief This is the number of growth parameters (set to 6)
   */
  int numGrowthConstants;
  /**
   * \brief This is the ModelVariableVector of growth parameters
   */
  ModelVariableVector growthPar;
};

/**
 * \class GrowthCalcJ
 * \brief This is the class used to calculate the growth of a stock using a simple length based Von Bertalanffy growth  function with a non-zero value for t0
 */
class GrowthCalcJ : public GrowthCalcBase {
public:
  /**
   * \brief This is the default GrowthCalcJ constructor
   * \param infile is the CommentStream to read the growth parameters from
   * \param Areas is the IntVector of areas that the growth calculation can take place on
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  GrowthCalcJ(CommentStream& infile, const IntVector& Areas,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the default GrowthCalcJ destructor
   */
  ~GrowthCalcJ() {};
  /**
   * \brief This is the function that calculates the growth using a simple length based Von Bertalanffy growth function with a non-zero value for t0
   * \param area is the area that the growth is being calculated on
   * \param Lgrowth is the DoubleVector of the mean length increase for each length group of the stock
   * \param Wgrowth is the DoubleVector of the mean weight increase for each length group of the stock
   * \param numGrow is the PopInfoVector of the current population of the stock
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param Fphi is the DoubleVector of the feeding level of the stock
   * \param MaxCon is the DoubleVector of the maximum consumption of the stock
   * \param LgrpDiv is the LengthGroupDivision of the stock
   */
  virtual void calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
    const PopInfoVector& numGrow, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const DoubleVector& Fphi,
    const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv);
  /**
   * \brief This will return the power term of the length - weight relationship
   * \return growthPar[4]
   */
  virtual double getPower() { return growthPar[4]; };
  /**
   * \brief This will return the multiplier of the length - weight relationship
   * \return growthPar[3]
   */
  virtual double getMult() { return growthPar[3]; };
protected:
  /**
   * \brief This is the number of growth parameters (set to 5)
   */
  int numGrowthConstants;
  /**
   * \brief This is the ModelVariableVector of growth parameters
   */
  ModelVariableVector growthPar;
};

/**
 * \class GrowthCalcK
 * \brief This is the class used to calculate the growth of a stock using a simple length based Gompertz function
 */
class GrowthCalcK : public GrowthCalcBase {
public:
  /**
   * \brief This is the default GrowthCalcK constructor
   * \param infile is the CommentStream to read the growth parameters from
   * \param Areas is the IntVector of areas that the growth calculation can take place on
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  GrowthCalcK(CommentStream& infile, const IntVector& Areas,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the default GrowthCalcK destructor
   */
  ~GrowthCalcK() {};
  /**
   * \brief This is the function that calculates the growth using a simple length based Gompertz function
   * \param area is the area that the growth is being calculated on
   * \param Lgrowth is the DoubleVector of the mean length increase for each length group of the stock
   * \param Wgrowth is the DoubleVector of the mean weight increase for each length group of the stock
   * \param numGrow is the PopInfoVector of the current population of the stock
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param Fphi is the DoubleVector of the feeding level of the stock
   * \param MaxCon is the DoubleVector of the maximum consumption of the stock
   * \param LgrpDiv is the LengthGroupDivision of the stock
   */
  virtual void calcGrowth(int area, DoubleVector& Lgrowth, DoubleVector& Wgrowth,
    const PopInfoVector& numGrow, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const DoubleVector& Fphi,
    const DoubleVector& MaxCon, const LengthGroupDivision* const LgrpDiv);
  /**
   * \brief This will return the power term of the length - weight relationship
   * \return growthPar[4]
   */
  virtual double getPower() { return growthPar[4]; };
  /**
   * \brief This will return the multiplier of the length - weight relationship
   * \return growthPar[3]
   */
  virtual double getMult() { return growthPar[3]; };
protected:
  /**
   * \brief This is the number of growth parameters (set to 5)
   */
  int numGrowthConstants;
  /**
   * \brief This is the ModelVariableVector of growth parameters
   */
  ModelVariableVector growthPar;
};

#endif
