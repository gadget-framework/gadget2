#ifndef logsionstep_h
#define logsionstep_h

#include "agebandm.h"
#include "commentstream.h"
#include "intmatrix.h"
#include "doublematrixptrvector.h"
#include "sionstep.h"
#include "suitfunc.h"

class LogSIOnStep;
class TimeClass;
class StockAggregator;
class LengthGroupDivision;

/*  LogSIOnStep
 *
 *  Purpose:    Take log of the difference between
 *  modelled and observed survey indices by length and age.
 *
 *  Usage: Constructor reads survey indices from file.
 */

class LogSIOnStep : public SIOnStep {
public:
  LogSIOnStep(CommentStream& infile, const intvector& areas,
    const doublevector& lengths, const intmatrix& ages,
    const TimeClass* const TimeInfo, Keeper* const keeper,
    const charptrvector& lenindex, const charptrvector& ageindex,
    const char* arealabel, const char* datafilename);
  virtual ~LogSIOnStep();
  virtual void Sum(const TimeClass* const TimeInfo);
  virtual void SetStocks(const Stockptrvector& Stocks);
  virtual double Regression() { return likelihood; };
  void calcIndex(const Agebandmatrix* alptr, FitType ftype);
  double LogLik();
  void PrintLikelihoodOnStep(int print_type);
  virtual void Reset(const Keeper* const keeper);
  virtual void LikelihoodPrint(ofstream& outfile) const;
  virtual void printMoreLikInfo(ofstream& outfile) const;
protected:
  void ReadLogSurveyData(CommentStream&, const char*, const charptrvector&, const charptrvector&, const TimeClass*);
  StockAggregator* aggregator;
  LengthGroupDivision* LgrpDiv;
  intvector Areas;
  intmatrix Ages;
  doublematrixptrvector indexMatrix;
  doublematrixptrvector calc_index; //for comparing with indexMatrix
  Formulavector b;   //interpreted as power or constant term according to fit type
  Formulavector q_y; //year dependent catchability factor
  doublevector q_l;  //length dependent catchability factor
  TimeVariablevector parameters;
  int index;
  int funcnumber;
  double eps_ind;
  double mean_fact; //not in use, for consistency purpose only
  double max_fact;  //not in use, for consistency purpose only
  double likelihood;
  SuitFunc* suitfunction;
  doublevector lik_val_on_step; //for print purposes
  doublevector max_val_on_step; //for print purposes
  doublevector b_vec;
  intvector l_index;            //for print purposes
  intvector a_index;            //for print purposes
  enum OptType { pearson = 0, multinomial };
  OptType opttype;
  intvector mincol;
  intvector maxcol;
  int minrow;
  int maxrow;
};

#endif
