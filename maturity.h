#ifndef maturity_h
#define maturity_h

#include "keeper.h"
#include "agebandmatrix.h"
#include "livesonareas.h"
#include "conversionindexptrvector.h"
#include "charptrvector.h"
#include "commentstream.h"
#include "agebandmatrixratioptrvector.h"
#include "timevariablevector.h"

class Maturity;

/**
 * \class Maturity
 * \brief This is the base class used to calculate the maturity of a stock within the model
 * \note This will always be overridden by the derived classes that actually calculate the maturity
 */
class Maturity: protected LivesOnAreas {
public:
  /**
   * \brief This is the default Maturity constructor
   */
  Maturity();
  /**
   * \brief This is the Maturity constructor
   * \param areas is the list of areas that the maturity calculation can take place on
   * \param minage is the minimum age of the stock that can mature
   * \param minabslength is the list of minimum lengths for the stock that can mature
   * \param size is the list of number of age groups for the stock, used to resize the AgeBandMatrixPtrVector
   * \param LgrpDiv is the LengthGroupDivision for the stock
   */
  Maturity(const IntVector& areas, int minage, const IntVector& minabslength,
    const IntVector& size, const LengthGroupDivision* const LgrpDiv);
  /**
   * \brief This is the default Maturity destructor
   */
  virtual ~Maturity();
  /**
   * \brief This will select the stocks required for the maturation process
   * \param stockvec is the StockPtrVector listing all the available stocks
   */
  void SetStock(StockPtrVector& stockvec);
  /**
   * \brief This function will print the maturation information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This will do the maturation calculations that only need to be performed once for the current model run
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Precalc(const TimeClass* const TimeInfo);
  /**
   * \brief This will check if the maturation process will take place on the current timestep
   * \param area is the area that the maturation is being calculated on
   * \param TimeInfo is the TimeClass for the current model
   * \return 0 (will be overridden in derived classes)
   */
  virtual int IsMaturationStep(int area, const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This will calculate the probability of maturation for a given age and length of the immature stock
   * \param age is the age of the age-length cell that the maturation is being calculated on
   * \param length is the length of the age-length cell that the maturation is being calculated on
   * \param growth is the length of the age-length cell that the mature stock will grow into
   * \param TimeInfo is the TimeClass for the current model
   * \param Area is the AreaClass for the current model
   * \param area is the area that the maturation is being calculated on
   * \return 0 (will be overridden in derived classes)
   */
  virtual double MaturationProbability(int age, int length, int growth,
    const TimeClass* const TimeInfo, const AreaClass* const Area, int area) = 0;
  /**
   * \brief This will store the calculated mature stock
   * \param area is the area that the maturation is being calculated on
   * \param age is the age of the calculated mature stock
   * \param length is the length of the calculated mature stock
   * \param number is the number of the calculated mature stock
   * \param weight is the weight of the calculated mature stock
   * \param TimeInfo is the TimeClass for the current model
   */
  void PutInStorage(int area, int age, int length, double number,
    double weight, const TimeClass* const TimeInfo);
  /**
   * \brief This will store the calculated mature tagged stock
   * \param area is the area that the maturation is being calculated on
   * \param age is the age of the calculated mature stock
   * \param length is the length of the calculated mature stock
   * \param number is the number of the calculated mature stock
   * \param TimeInfo is the TimeClass for the current model
   * \param id is the tagging identifier of the mature stock
   */
  void PutInStorage(int area, int age, int length, double number,
    const TimeClass* const TimeInfo, int id);
  /**
   * \brief This will move the calculated mature stock into the mature stock age-length cells
   * \param area is the area that the maturation is being calculated on
   * \param TimeInfo is the TimeClass for the current model
   */
  void Move(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This will calculate the number of stock the immature stock can mature into
   * \return number of mature stocks
   */
  int NoOfMatureStocks() { return MatureStocks.Size(); };
  /**
   * \brief This will calculate the mature stocks the immature stock can mature into
   * \return mature stocks
   */
  const StockPtrVector& GetMatureStocks();
  /**
   * \brief This will add a tagging experiment to the maturation calculations
   * \param tagname is the name of the tagging experiment
   */
  void AddTag(const char* tagname);
  /**
   * \brief This will remove a tagging experiment from the maturation calculations
   * \param tagname is the name of the tagging experiment
   */
  void DeleteTag(const char* tagname);
protected:
  /**
   * \brief This is the StockPtrVector of the mature stocks
   */
  StockPtrVector MatureStocks;
  /**
   * \brief This is the CharPtrVector of the names of the mature stocks
   */
  CharPtrVector MatureStockNames;
  /**
   * \brief This is the vector of the ratio of the immature stock to mature into each mature stock
   */
  DoubleVector Ratio;
  /**
   * \brief This is the ConversionIndexPtrVector used to convert from the immature stock lengths to the mature stock lengths
   */
  ConversionIndexPtrVector CI;
  /**
   * \brief This is the LengthGroupDivision of the stock
   */
  LengthGroupDivision* LgrpDiv;
private:
  /**
   * \brief This is the AgeBandMatrixPtrVector used to store the calculated mature stocks
   */
  AgeBandMatrixPtrVector Storage;
  /**
   * \brief This is the AgeBandMatrixRatioPtrVector used to store the calculated mature tagged stocks
   */
  AgeBandMatrixRatioPtrVector TagStorage;
};

/**
 * \class MaturityA
 * \brief This is the class used to calculate the maturity based on 'continuous' maturity function
 */
class MaturityA : public Maturity {
public:
  /**
   * \brief This is the MaturityA constructor
   * \param infile is the CommentStream to read the maturation parameters from
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param minage is the minimum age of the stock that can mature
   * \param minabslength is the list of minimum lengths for the stock that can mature
   * \param size is the list of number of age groups for the stock, used to resize the AgeBandMatrixPtrVector
   * \param areas is the list of areas that the maturity calculation can take place on
   * \param LgrpDiv is the LengthGroupDivision for the stock
   * \param NoMatconst is the number of maturation parameters (3)
   */
  MaturityA(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper,
    int minage, const IntVector& minabslength, const IntVector& size, const IntVector& areas,
    const LengthGroupDivision* const LgrpDiv, int NoMatconst);
  /**
   * \brief This is the default MaturityA destructor
   */
  virtual ~MaturityA() {};
  /**
   * \brief This will do the maturation calculations that only need to be performed once for the current model run
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Precalc(const TimeClass* const TimeInfo);
  /**
   * \brief This will check if the maturation process will take place on the current timestep
   * \param area is the area that the maturation is being calculated on
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the maturation process will take place, 0 otherwise
   */
  virtual int IsMaturationStep(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This will calculate the probability of maturation for a given age and length of the immature stock
   * \param age is the age of the age-length cell that the maturation is being calculated on
   * \param length is the length of the age-length cell that the maturation is being calculated on
   * \param growth is the length of the age-length cell that the mature stock will grow into
   * \param TimeInfo is the TimeClass for the current model
   * \param Area is the AreaClass for the current model
   * \param area is the area that the maturation is being calculated on
   * \return maturation probability
   */
  virtual double MaturationProbability(int age, int length, int growth,
    const TimeClass* const TimeInfo, const AreaClass* const Area, int area);
  /**
   * \brief This function will print the maturation information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
protected:
  /**
   * \brief This is the BandMatrix of maturation parameters
   */
  BandMatrix PrecalcMaturation;
  /**
   * \brief This is the TimeVariableVector of maturation parameters
   */
  TimeVariableVector Coefficient;
  /**
   * \brief This is the minimum age when maturation will take place
   */
  int MinMatureAge;
};

/**
 * \class MaturityB
 * \brief This is the class used to calculate the maturity based on 'fixedlength' maturity function
 */
class MaturityB : public Maturity {
public:
  /**
   * \brief This is the MaturityB constructor
   * \param infile is the CommentStream to read the maturation parameters from
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param minage is the minimum age of the stock that can mature
   * \param minabslength is the list of minimum lengths for the stock that can mature
   * \param size is the list of number of age groups for the stock, used to resize the AgeBandMatrixPtrVector
   * \param areas is the list of areas that the maturity calculation can take place on
   * \param LgrpDiv is the LengthGroupDivision for the stock
   */
  MaturityB(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper,
    int minage, const IntVector& minabslength, const IntVector& size,
    const IntVector& areas, const LengthGroupDivision* const LgrpDiv);
  /**
   * \brief This is the default MaturityB destructor
   */
  virtual ~MaturityB() {};
  /**
   * \brief This will do the maturation calculations that only need to be performed once for the current model run
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Precalc(const TimeClass* const TimeInfo);
  /**
   * \brief This will check if the maturation process will take place on the current timestep
   * \param area is the area that the maturation is being calculated on
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the maturation process will take place, 0 otherwise
   */
  virtual int IsMaturationStep(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This will calculate the probability of maturation for a given age and length of the immature stock
   * \param age is the age of the age-length cell that the maturation is being calculated on
   * \param length is the length of the age-length cell that the maturation is being calculated on
   * \param growth is the length of the age-length cell that the mature stock will grow into
   * \param TimeInfo is the TimeClass for the current model
   * \param Area is the AreaClass for the current model
   * \param area is the area that the maturation is being calculated on
   * \return maturation probability
   */
  virtual double MaturationProbability(int age, int length, int growth,
    const TimeClass* const TimeInfo, const AreaClass* const Area, int area);
  /**
   * \brief This function will print the maturation information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
protected:
  /**
   * \brief This is the vector of timesteps when maturation will take place
   */
  IntVector maturitystep;
  /**
   * \brief This is the TimeVariableVector of lengths when maturation will take place
   */
  TimeVariableVector maturitylength;
};

/**
 * \class MaturityC
 * \brief This is the class used to calculate the maturity based on 'ageandlength' maturity function
 */
class MaturityC : public MaturityA {
public:
  /**
   * \brief This is the MaturityC constructor
   * \param infile is the CommentStream to read the maturation parameters from
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param minage is the minimum age of the stock that can mature
   * \param minabslength is the list of minimum lengths for the stock that can mature
   * \param size is the list of number of age groups for the stock, used to resize the AgeBandMatrixPtrVector
   * \param areas is the list of areas that the maturity calculation can take place on
   * \param LgrpDiv is the LengthGroupDivision for the stock
   * \param NoMatconst is the number of maturation parameters (4)
   */
  MaturityC(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper,
    int minage, const IntVector& minabslength, const IntVector& size,
    const IntVector& areas, const LengthGroupDivision* const LgrpDiv, int NoMatconst);
  /**
   * \brief This is the default MaturityC destructor
   */
  virtual ~MaturityC() {};
  /**
   * \brief This will do the maturation calculations that only need to be performed once for the current model run
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Precalc(const TimeClass* const TimeInfo);
  /**
   * \brief This will check if the maturation process will take place on the current timestep
   * \param area is the area that the maturation is being calculated on
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the maturation process will take place, 0 otherwise
   */
  virtual int IsMaturationStep(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This will calculate the probability of maturation for a given age and length of the immature stock
   * \param age is the age of the age-length cell that the maturation is being calculated on
   * \param length is the length of the age-length cell that the maturation is being calculated on
   * \param growth is the length of the age-length cell that the mature stock will grow into
   * \param TimeInfo is the TimeClass for the current model
   * \param Area is the AreaClass for the current model
   * \param area is the area that the maturation is being calculated on
   * \return maturation probability
   */
  virtual double MaturationProbability(int age, int length, int growth,
    const TimeClass* const TimeInfo, const AreaClass* const Area, int area);
protected:
  /**
   * \brief This is the vector of timesteps when maturation will take place
   */
  IntVector maturitystep;
};

/**
 * \class MaturityD
 * \brief This is the class used to calculate the maturity based on 'constant' maturity function
 */
class MaturityD : public MaturityA {
public:
  /**
   * \brief This is the MaturityD constructor
   * \param infile is the CommentStream to read the maturation parameters from
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param minage is the minimum age of the stock that can mature
   * \param minabslength is the list of minimum lengths for the stock that can mature
   * \param size is the list of number of age groups for the stock, used to resize the AgeBandMatrixPtrVector
   * \param areas is the list of areas that the maturity calculation can take place on
   * \param LgrpDiv is the LengthGroupDivision for the stock
   * \param NoMatconst is the number of maturation parameters (4)
   */
  MaturityD(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper,
    int minage, const IntVector& minabslength, const IntVector& size,
    const IntVector& areas, const LengthGroupDivision* const LgrpDiv, int NoMatconst);
  /**
   * \brief This is the default MaturityD destructor
   */
  virtual ~MaturityD() {};
  /**
   * \brief This will do the maturation calculations that only need to be performed once for the current model run
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Precalc(const TimeClass* const TimeInfo);
  /**
   * \brief This will check if the maturation process will take place on the current timestep
   * \param area is the area that the maturation is being calculated on
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the maturation process will take place, 0 otherwise
   */
  virtual int IsMaturationStep(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This will calculate the probability of maturation for a given age and length of the immature stock
   * \param age is the age of the age-length cell that the maturation is being calculated on
   * \param length is the length of the age-length cell that the maturation is being calculated on
   * \param growth is the length of the age-length cell that the mature stock will grow into
   * \param TimeInfo is the TimeClass for the current model
   * \param Area is the AreaClass for the current model
   * \param area is the area that the maturation is being calculated on
   * \return maturation probability
   */
  virtual double MaturationProbability(int age, int length, int growth,
    const TimeClass* const TimeInfo, const AreaClass* const Area, int area);
protected:
  /**
   * \brief This is the vector of timesteps when maturation will take place
   */
  IntVector maturitystep;
};

#endif
