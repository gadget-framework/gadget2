#ifndef sibylengthandageonstep_h
#define sibylengthandageonstep_h

#include "doublematrixptrvector.h"
#include "sionstep.h"
#include "suitfunc.h"

class SIByLengthAndAgeOnStep;
class StockAggregator;

class SIByLengthAndAgeOnStep : public SIOnStep {
public:
  SIByLengthAndAgeOnStep(CommentStream& infile, const IntVector& areas,
    const DoubleVector& lengths, const IntMatrix& ages,
    const TimeClass* const TimeInfo, Keeper* const keeper,
    const CharPtrVector& lenindex, const CharPtrVector& ageindex,
    const char* arealabel, const char* datafilename);
  virtual ~SIByLengthAndAgeOnStep();
  virtual void Sum(const TimeClass* const TimeInfo);
  virtual void SetStocks(const StockPtrVector& Stocks);
  virtual double Regression() {return likelihood; };
  void calcIndex(const AgeBandMatrix* alptr, FitType ftype);
  void PrintLikelihoodOnStep(ofstream& o, int print_type);
  virtual void PrintLikelihood(ofstream&, const TimeClass& time, const char*);
  virtual void PrintLikelihoodHeader(ofstream&, const char*);
  virtual void Reset(const Keeper* const keeper);
  virtual void LikelihoodPrint(ofstream& outfile);
  virtual void CommandLinePrint(ofstream& o, const TimeClass& time, const PrintInfo&);
protected:
  void ReadSurveyData(CommentStream&, const char*, const CharPtrVector&, const CharPtrVector&, const TimeClass*);
  double calcLikPearson();
  double calcLikMultinomial();
  double calcLikGamma();
  double calcLikExperimental();
  double calcLikLog();
  StockAggregator* aggregator;
  LengthGroupDivision* LgrpDiv;
  IntVector Areas;
  IntMatrix Ages;
  DoubleMatrixPtrVector indexMatrix;
  DoubleMatrixPtrVector calc_index;  //for comparing with indexMatrix
  FormulaVector b; //interpreted as power or intersection term according to fit type
  FormulaVector q_y; //year dependent catchability factor
  DoubleVector q_l; //length dependent catchability factor
  TimeVariableVector parameters;
  int index;
  int funcnumber;
  double eps_ind;
  double mean_fact;
  double max_fact;
  double likelihood;
  SuitFunc* suitfunction;
  DoubleVector lik_val_on_step; //for print purposes
  DoubleVector max_val_on_step; //for print purposes
  DoubleVector b_vec;
  IntVector l_index; //for print purposes
  IntVector a_index; //for print purposes
  enum OptType { pearson = 0, multinomial, experimental, gamma, logfunc };
  OptType opttype;
  CharPtrVector stocknames;
  IntVector mincol;
  IntVector maxcol;
  int minrow;
  int maxrow;
};

#endif
