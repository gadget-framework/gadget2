#ifndef surveyindices_h
#define surveyindices_h

#include "likelihood.h"
#include "commentstream.h"
#include "sionstep.h"

class SurveyIndices : public Likelihood {
public:
  /**
   * \brief This is the SurveyIndices constructor
   * \param infile is the CommentStream to read the SurveyIndices data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param weight is the weight for the likelihood component
   * \param name is the name for the likelihood component
   */
  SurveyIndices(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper, double weight, const char* name);
  /**
   * \brief This is the default SurveyIndices destructor
   */
  virtual ~SurveyIndices();
  /**
   * \brief This function will calculate the likelihood score for the SurveyIndices component
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void addLikelihood(const TimeClass* const TimeInfo);
  /**
   * \brief This will select the stocks required to calculate the SurveyIndices likelihood score
   * \param Stocks is the StockPtrVector of all the available stocks
   */
  void setStocks(StockPtrVector& Stocks);
  /**
   * \brief This function will reset the SurveyIndices likelihood information
   * \param keeper is the Keeper for the current model
   */
  virtual void Reset(const Keeper* const keeper);
  /**
   * \brief This function will print the summary SurveyIndices likelihood information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This function will print information from each SurveyIndices likelihood calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   */
  virtual void LikelihoodPrint(ofstream& outfile);
  /**
   * \brief This function will print summary information from each SurveyIndices likelihood calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   */
  virtual void SummaryPrint(ofstream& outfile) {
    SI->SummaryPrint(outfile, weight); };
  virtual void PrintLikelihood(ofstream& outfile, const TimeClass& time) {
    SI->PrintLikelihood(outfile, time, surveyname); };
  virtual void PrintLikelihoodHeader(ofstream& outfile) {
    SI->PrintLikelihoodHeader(outfile, surveyname); };
protected:
  SIOnStep* SI;
  /**
   * \brief This is the IntMatrix used to store area information
   */
  IntMatrix areas;
  /**
   * \brief This is the CharPtrVector of the names of the stocks that will be used to calculate the likelihood score
   */
  CharPtrVector stocknames;
  /**
   * \brief This is the name of the SurveyIndices likelihood component
   */
  char* surveyname;
};

#endif
