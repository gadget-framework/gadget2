#ifndef sibylengthandageonstep_h
#define sibylengthandageonstep_h

#include "doublematrixptrvector.h"
#include "sionstep.h"
#include "suitfunc.h"

enum OptType { PEARSONOPTTYPE = 1, MULTINOMIALOPTTYPE, GAMMAOPTTYPE, LOGFUNCOPTTYPE };

class SIByLengthAndAgeOnStep : public SIOnStep {
public:
  SIByLengthAndAgeOnStep(CommentStream& infile, const IntMatrix& areas,
    const DoubleVector& lengths, const IntMatrix& ages,
    const TimeClass* const TimeInfo, Keeper* const keeper,
    const CharPtrVector& lenindex, const CharPtrVector& ageindex,
    const CharPtrVector& areaindex, const char* datafilename, const char* name);
  virtual ~SIByLengthAndAgeOnStep();
  virtual void Sum(const TimeClass* const TimeInfo);
  virtual void setStocks(const StockPtrVector& Stocks);
  virtual double Regression() {return likelihood; };
  void calcIndex(const AgeBandMatrix* alptr, const TimeClass* const TimeInfo);
  virtual void PrintLikelihood(ofstream&, const TimeClass& time, const char* name);
  virtual void PrintLikelihoodHeader(ofstream&, const char* name);
  virtual void Reset(const Keeper* const keeper);
  virtual void LikelihoodPrint(ofstream& outfile);
  OptType getOptType() { return opttype; };
protected:
  void readSurveyData(CommentStream& infile, const CharPtrVector& areaindex,
    const CharPtrVector& lenindex, const CharPtrVector& ageindex,
    const TimeClass* TimeInfo, const char* name);
  double calcLikPearson();
  double calcLikMultinomial();
  double calcLikGamma();
  double calcLikLog();
  /**
   * \brief This is the StockAggregator used to collect information about the relevant stocks for the survey index data
   */
  StockAggregator* aggregator;
  /**
   * \brief This is the IntMatrix used to store age information
   */
  IntMatrix Ages;
  /**
   * \brief This is the LengthGroupDivision used to store length information
   */
  LengthGroupDivision* LgrpDiv;
  DoubleMatrixPtrVector indexMatrix;
  DoubleMatrixPtrVector calc_index;  //for comparing with indexMatrix
  DoubleVector q_l; //length dependent catchability factor
  TimeVariableVector parameters;
  int index;
  double epsilon;
  double likelihood;
  SuitFunc* suitfunction;
  DoubleVector lik_val_on_step; //for print purposes
  DoubleVector max_val_on_step; //for print purposes
  IntVector l_index; //for print purposes
  IntVector a_index; //for print purposes
  OptType opttype;
  CharPtrVector stocknames;
  IntVector mincol;
  IntVector maxcol;
  int minrow;
  int maxrow;
  int stocktype;
};

#endif
