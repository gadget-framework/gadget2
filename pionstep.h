#ifndef pionstep_h
#define pionstep_h

#include "areatime.h"
#include "commentstream.h"
#include "charptrvector.h"
#include "predatorptrvector.h"
#include "preyptrvector.h"
#include "sionstep.h"

class PIOnStep;
class PredatorAggregator;

class PIOnStep : public SIOnStep {
public:
  /**
   * \brief This is the PIOnStep constructor
   * \param infile is the CommentStream to read the predator survey index data from
   * \param areas is the IntMatrix of areas that the predator survey index can take place on
   * \param predatorlengths is the DoubleVector of predator lengths
   * \param preylengths is the DoubleVector of prey lengths
   * \param TimeInfo is the TimeClass for the current model
   * \param biomass is a flag to determine whether the predator survey index calculation should be based on the biomass of the stock, or the number of the stock
   * \param areaindex is the CharPtrVector of area identifier text strings
   * \param preylenindex is the CharPtrVector of length identifier text strings for the preys
   * \param predlenindex is the CharPtrVector of length identifier text strings for the predators
   * \param datafilename is the name of the file containing the predator survey index data
   * \param name is the name of the PIOnStep likelihood component
   */
  PIOnStep(CommentStream& infile, const IntMatrix& areas,
    const DoubleVector& predatorlengths, const DoubleVector& preylengths,
    const TimeClass* const TimeInfo, int biomass, const CharPtrVector& areaindex,
    const CharPtrVector& preylenindex, const CharPtrVector& predlenindex,
    const char* datafilename, const char* name);
  /**
   * \brief This is the default PIOnStep destructor
   */
  ~PIOnStep();
  /**
   * \brief This function will sum the predator survey index data
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Sum(const TimeClass* const TimeInfo);
  /**
   * \brief This will select the stocks required to calculate the predator survey index likelihood score
   * \param Stocks is the StockPtrVector of all the available stocks
   * \note This is a dummy function to ensure compatability with other classes and doesnt do anything
   */
  virtual void setStocks(const StockPtrVector& Stocks) {};
  /**
   * \brief This will select the predators and preys required to calculate the predator survey index likelihood score
   * \param predators is the PredatorPtrVector of all the available predators
   * \param preys is the PreyPtrVector of all the available preys
   */
  virtual void setPredatorsAndPreys(const PredatorPtrVector& predators, const PreyPtrVector& preys);
protected:
  /**
   * \brief This function will read the predator survey index data from file
   * \param infile is the CommentStream to read the predator survey index data from
   * \param areaindex is the CharPtrVector of area identifier text strings
   * \param predlenindex is the CharPtrVector of length identifier text strings for the predators
   * \param preylenindex is the CharPtrVector of length identifier text strings for the preys
   * \param TimeInfo is the TimeClass for the current model
   * \param name is the name of the PIOnStep likelihood component
   */
  void readPredatorData(CommentStream& infile, const CharPtrVector& areaindex,
    const CharPtrVector& predlenindex, const CharPtrVector& preylenindex,
    const TimeClass* TimeInfo, const char* name);
  /**
   * \brief This is the LengthGroupDivision of the predators
   */
  LengthGroupDivision* predLgrpDiv;
    /**
   * \brief This is the LengthGroupDivision of the preys
   */
  LengthGroupDivision* preyLgrpDiv;
  /**
   * \param This is the flag used to denote whether the predator index calculation should be based on the biomass of the stock, or the number of the stock
   */
  int Biomass;
  /**
   * \brief This is the PredatorAggregator used to collect information about the predation
   */
  PredatorAggregator* aggregator;
};

#endif
