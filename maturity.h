#ifndef maturity_h
#define maturity_h

#include "keeper.h"
#include "agebandmatrix.h"
#include "livesonareas.h"
#include "conversionindexptrvector.h"
#include "charptrvector.h"
#include "commentstream.h"
#include "agebandmatrixptrvector.h"
#include "agebandmatrixratioptrvector.h"
#include "timevariablevector.h"

/**
 * \class Maturity
 * \brief This is the base class used to calculate the maturity of a stock within the model
 *
 * This class moves fish from an immature stock to matures stocks within the model.  The proportion of those fish in the immature stock that mature is calulated according to the different maturity functions of the derived classes, and then that propotion is moved into the mature stocks.  The length and age groups of the mature stocks are checked, and any fish that haven't yet reached the minimum length or age of the mature stock remain in the immature stock.
 *
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
   * \param areas is the IntVector of areas that the maturity calculation can take place on
   * \param minage is the minimum age of the stock that can mature
   * \param minabslength is the IntVector of minimum lengths for the stock that can mature
   * \param size is the IntVector of the number of age groups for the stock, used to resize the AgeBandMatrixPtrVector
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
   * \param stockvec is the StockPtrVector of all the available stocks
   */
  virtual void setStock(StockPtrVector& stockvec);
  /**
   * \brief This function will print the maturation information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This will reset the maturation information for the current model run
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Reset(const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This will check if the maturation process will take place on the current timestep
   * \param TimeInfo is the TimeClass for the current model
   * \return 0 (will be overridden in derived classes)
   */
  virtual int isMaturationStep(const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This will calculate the probability of maturation for a given age and length of the immature stock
   * \param age is the age of the age-length cell that the maturation is being calculated on
   * \param length is the length of the age-length cell that the maturation is being calculated on
   * \param growth is the length of the age-length cell that the mature stock will grow into
   * \param TimeInfo is the TimeClass for the current model
   * \param weight is the weight of the fish of the age-length cell that are maturing
   * \return 0 (will be overridden in derived classes)
   */
  virtual double MaturationProbability(int age, int length, int growth,
    const TimeClass* const TimeInfo, double weight) = 0;
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
   * \brief This will calculate the mature stocks the immature stock can mature into
   * \return mature stocks
   */
  const StockPtrVector& getMatureStocks();
  /**
   * \brief This will add a tagging experiment to the maturation calculations
   * \param tagname is the name of the tagging experiment
   */
  void addMaturityTag(const char* tagname);
  /**
   * \brief This will remove a tagging experiment from the maturation calculations
   * \param tagname is the name of the tagging experiment
   */
  void deleteMaturityTag(const char* tagname);
protected:
  /**
   * \brief This is the StockPtrVector of the mature stocks
   */
  StockPtrVector matureStocks;
  /**
   * \brief This is the CharPtrVector of the names of the mature stocks
   */
  CharPtrVector matureStockNames;
  /**
   * \brief This is the DoubleVector of the ratio of the immature stock to mature into each mature stock
   */
  DoubleVector matureRatio;
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
  AgeBandMatrixRatioPtrVector tagStorage;
};

/**
 * \class MaturityA
 * \brief This is the class used to calculate the maturity based on the 'continuous' maturity function
 */
class MaturityA : public Maturity {
public:
  /**
   * \brief This is the MaturityA constructor
   * \param infile is the CommentStream to read the maturation parameters from
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param minage is the minimum age of the stock that can mature
   * \param minabslength is the IntVector of minimum lengths for the stock that can mature
   * \param size is the IntVector of the number of age groups for the stock, used to resize the AgeBandMatrixPtrVector
   * \param areas is the IntVector of areas that the maturity calculation can take place on
   * \param LgrpDiv is the LengthGroupDivision for the stock
   */
  MaturityA(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper,
    int minage, const IntVector& minabslength, const IntVector& size, const IntVector& areas,
    const LengthGroupDivision* const LgrpDiv);
  /**
   * \brief This is the default MaturityA destructor
   */
  virtual ~MaturityA() {};
  /**
   * \brief This will select the stocks required for the maturation process
   * \param stockvec is the StockPtrVector of all the available stocks
   */
  virtual void setStock(StockPtrVector& stockvec);
  /**
   * \brief This will reset the maturation information for the current model run
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Reset(const TimeClass* const TimeInfo);
  /**
   * \brief This will check if the maturation process will take place on the current timestep
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the maturation process will take place, 0 otherwise
   */
  virtual int isMaturationStep(const TimeClass* const TimeInfo);
  /**
   * \brief This will calculate the probability of maturation for a given age and length of the immature stock
   * \param age is the age of the age-length cell that the maturation is being calculated on
   * \param length is the length of the age-length cell that the maturation is being calculated on
   * \param growth is the length of the age-length cell that the mature stock will grow into
   * \param TimeInfo is the TimeClass for the current model
   * \param weight is the weight of the fish of the age-length cell that are maturing
   * \return maturation probability
   */
  virtual double MaturationProbability(int age, int length, int growth,
    const TimeClass* const TimeInfo, double weight);
  /**
   * \brief This function will print the maturation information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
protected:
  /**
   * \brief This is the BandMatrix of maturation parameters
   */
  BandMatrix preCalcMaturation;
  /**
   * \brief This is the TimeVariableVector of maturation parameters
   */
  TimeVariableVector maturityParameters;
  /**
   * \brief This is the minimum age when maturation will take place
   */
  int minMatureAge;
  /**
   * \brief This is the minimum length group division when maturation will take place
   */
  int minMatureLength;
};

/**
 * \class MaturityB
 * \brief This is the class used to calculate the maturity based on the 'fixedlength' maturity function
 */
class MaturityB : public Maturity {
public:
  /**
   * \brief This is the MaturityB constructor
   * \param infile is the CommentStream to read the maturation parameters from
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param minage is the minimum age of the stock that can mature
   * \param minabslength is the IntVector of minimum lengths for the stock that can mature
   * \param size is the IntVector of the number of age groups for the stock, used to resize the AgeBandMatrixPtrVector
   * \param areas is the IntVector of areas that the maturity calculation can take place on
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
   * \brief This will select the stocks required for the maturation process
   * \param stockvec is the StockPtrVector of all the available stocks
   */
  virtual void setStock(StockPtrVector& stockvec);
  /**
   * \brief This will reset the maturation information for the current model run
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Reset(const TimeClass* const TimeInfo);
  /**
   * \brief This will check if the maturation process will take place on the current timestep
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the maturation process will take place, 0 otherwise
   */
  virtual int isMaturationStep(const TimeClass* const TimeInfo);
  /**
   * \brief This will calculate the probability of maturation for a given age and length of the immature stock
   * \param age is the age of the age-length cell that the maturation is being calculated on
   * \param length is the length of the age-length cell that the maturation is being calculated on
   * \param growth is the length of the age-length cell that the mature stock will grow into
   * \param TimeInfo is the TimeClass for the current model
   * \param weight is the weight of the fish of the age-length cell that are maturing
   * \return maturation probability
   */
  virtual double MaturationProbability(int age, int length, int growth,
    const TimeClass* const TimeInfo, double weight);
  /**
   * \brief This function will print the maturation information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
protected:
  /**
   * \brief This is the IntVector of timesteps when maturation will take place
   */
  IntVector maturitystep;
  /**
   * \brief This is the TimeVariableVector of lengths when maturation will take place
   */
  TimeVariableVector maturitylength;
};

/**
 * \class MaturityC
 * \brief This is the class used to calculate the maturity based on the 'constant' maturity function
 */
class MaturityC : public Maturity {
public:
  /**
   * \brief This is the MaturityC constructor
   * \param infile is the CommentStream to read the maturation parameters from
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param minage is the minimum age of the stock that can mature
   * \param minabslength is the IntVector of minimum lengths for the stock that can mature
   * \param size is the IntVector of the number of age groups for the stock, used to resize the AgeBandMatrixPtrVector
   * \param areas is the IntVector of areas that the maturity calculation can take place on
   * \param LgrpDiv is the LengthGroupDivision for the stock
   * \param numMatConst is the number of maturation parameters (4)
   */
  MaturityC(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper,
    int minage, const IntVector& minabslength, const IntVector& size,
    const IntVector& areas, const LengthGroupDivision* const LgrpDiv, int numMatConst);
  /**
   * \brief This is the default MaturityC destructor
   */
  virtual ~MaturityC() {};
  /**
   * \brief This will select the stocks required for the maturation process
   * \param stockvec is the StockPtrVector of all the available stocks
   */
  virtual void setStock(StockPtrVector& stockvec);
  /**
   * \brief This will reset the maturation information for the current model run
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Reset(const TimeClass* const TimeInfo);
  /**
   * \brief This will check if the maturation process will take place on the current timestep
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the maturation process will take place, 0 otherwise
   */
  virtual int isMaturationStep(const TimeClass* const TimeInfo);
  /**
   * \brief This will calculate the probability of maturation for a given age and length of the immature stock
   * \param age is the age of the age-length cell that the maturation is being calculated on
   * \param length is the length of the age-length cell that the maturation is being calculated on
   * \param growth is the length of the age-length cell that the mature stock will grow into
   * \param TimeInfo is the TimeClass for the current model
   * \param weight is the weight of the fish of the age-length cell that are maturing
   * \return maturation probability
   */
  virtual double MaturationProbability(int age, int length, int growth,
    const TimeClass* const TimeInfo, double weight);
  /**
   * \brief This function will print the maturation information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
protected:
  /**
   * \brief This is the BandMatrix of maturation parameters
   */
  BandMatrix preCalcMaturation;
  /**
   * \brief This is the TimeVariableVector of maturation parameters
   */
  TimeVariableVector maturityParameters;
  /**
   * \brief This is the minimum age when maturation will take place
   */
  int minMatureAge;
  /**
   * \brief This is the minimum length group division when maturation will take place
   */
  int minMatureLength;
  /**
   * \brief This is the IntVector of timesteps when maturation will take place
   */
  IntVector maturitystep;
};

/**
 * \class MaturityD
 * \brief This is the class used to calculate the maturity based on the 'constantweight' maturity function
 */
class MaturityD : public MaturityC {
public:
  /**
   * \brief This is the MaturityD constructor
   * \param infile is the CommentStream to read the maturation parameters from
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param minage is the minimum age of the stock that can mature
   * \param minabslength is the IntVector of minimum lengths for the stock that can mature
   * \param size is the IntVector of the number of age groups for the stock, used to resize the AgeBandMatrixPtrVector
   * \param areas is the IntVector of areas that the maturity calculation can take place on
   * \param LgrpDiv is the LengthGroupDivision for the stock
   * \param numMatConst is the number of maturation parameters (6)
   * \param refWeightFile is the name of the file to read the reference weight information from
   */
  MaturityD(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper,
    int minage, const IntVector& minabslength, const IntVector& size, const IntVector& areas,
    const LengthGroupDivision* const LgrpDiv, int numMatConst, const char* refWeightFile);
  /**
   * \brief This is the default MaturityD destructor
   */
  virtual ~MaturityD() {};
  /**
   * \brief This will select the stocks required for the maturation process
   * \param stockvec is the StockPtrVector of all the available stocks
   */
  virtual void setStock(StockPtrVector& stockvec);
  /**
   * \brief This will reset the maturation information for the current model run
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Reset(const TimeClass* const TimeInfo);
  /**
   * \brief This will check if the maturation process will take place on the current timestep
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the maturation process will take place, 0 otherwise
   */
  virtual int isMaturationStep(const TimeClass* const TimeInfo);
  /**
   * \brief This will calculate the probability of maturation for a given age and length of the immature stock
   * \param age is the age of the age-length cell that the maturation is being calculated on
   * \param length is the length of the age-length cell that the maturation is being calculated on
   * \param growth is the length of the age-length cell that the mature stock will grow into
   * \param TimeInfo is the TimeClass for the current model
   * \param weight is the weight of the fish of the age-length cell that are maturing
   * \return maturation probability
   */
  virtual double MaturationProbability(int age, int length, int growth,
    const TimeClass* const TimeInfo, double weight);
protected:
  /**
   * \brief This is the DoubleVector of reference weight information
   */
  DoubleVector refWeight;
};

#endif
