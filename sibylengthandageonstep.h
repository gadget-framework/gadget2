#ifndef sibylengthandageonstep_h
#define sibylengthandageonstep_h

#include "agebandm.h"
#include "commentstream.h"
#include "intmatrix.h"
#include "doublematrixptrvector.h"
#include "sionstep.h"
#include "suitfunc.h"

class SIByLengthAndAgeOnStep;
class TimeClass;
class StockAggregator;
class LengthGroupDivision;

class SIByLengthAndAgeOnStep : public SIOnStep {
public:
  SIByLengthAndAgeOnStep(CommentStream& infile, const intvector& areas,
    const doublevector& lengths, const intmatrix& ages,
    const TimeClass* const TimeInfo, Keeper* const keeper,
    const charptrvector& lenindex, const charptrvector& ageindex,
    const char* arealabel, const char* datafilename);
  virtual ~SIByLengthAndAgeOnStep();
  virtual void Sum(const TimeClass* const TimeInfo);
  virtual void SetStocks(const Stockptrvector& Stocks);
  virtual double Regression() {return likelihood; };
  void calcIndex(const Agebandmatrix* alptr, FitType ftype);
  void PrintLikelihoodOnStep(ofstream& o, int print_type);
  virtual void PrintLikelihood(ofstream&, const TimeClass& time, const char*);
  virtual void PrintLikelihoodHeader(ofstream&, const char*);
  virtual void Reset(const Keeper* const keeper);
  virtual void LikelihoodPrint(ofstream& outfile) const;
  virtual void printMoreLikInfo(ofstream& outfile) const;
  virtual void printHeader(ofstream& o, const PrintInfo& print, const char* name);
  virtual void print(ofstream& o, const TimeClass& time, const PrintInfo&);
protected:
  void ReadSurveyData(CommentStream&, const char*, const charptrvector&, const charptrvector&, const TimeClass*);
  double calcLikPearson();
  double calcLikMultinomial();
  double calcLikGamma();
  double calcLikExperimental();
  double calcLikLog();
  StockAggregator* aggregator;
  LengthGroupDivision* LgrpDiv;
  intvector Areas;
  intmatrix Ages;
  doublematrixptrvector indexMatrix;
  doublematrixptrvector calc_index;  //for comparing with indexMatrix
  Formulavector b; //interpreted as power or intersection term according to fit type
  Formulavector q_y; //year dependent catchability factor
  doublevector q_l; //length dependent catchability factor
  TimeVariablevector parameters;
  int index;
  int funcnumber;
  double eps_ind;
  double mean_fact;
  double max_fact;
  double likelihood;
  SuitFunc* suitfunction;
  doublevector lik_val_on_step; //for print purposes
  doublevector max_val_on_step; //for print purposes
  doublevector b_vec;
  intvector l_index; //for print purposes
  intvector a_index; //for print purposes
  enum OptType { pearson = 0, multinomial, experimental, gamma, logfunc };
  OptType opttype;
  charptrvector stocknames;
  intvector mincol;
  intvector maxcol;
  int minrow;
  int maxrow;
};

#endif
