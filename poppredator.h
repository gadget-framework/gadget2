#ifndef poppredator_h
#define poppredator_h

#include "areatime.h"
#include "conversionindex.h"
#include "commentstream.h"
#include "agebandmatrix.h"
#include "bandmatrixptrmatrix.h"
#include "predator.h"
#include "keeper.h"

class PopPredator : public Predator {
public:
  /**
   * \brief This is the PopPredator constructor to create a predator with a specified length group
   * \param givenname is the name of the predator
   * \param area is the IntVector of the areas that the predator will live on
   * \param OtherLgrpDiv is the LengthGroupDivision of the predator
   * \param GivenLgrpDiv is the LengthGroupDivision that the predation will be calculated on
   * \note this constructor is used when creating a dynamic predator (ie. StockPredator class)
   */
  PopPredator(const char* givenname, const IntVector& areas,
    const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv);
  /**
   * \brief This is the PopPredator constructor to create a predator without a specified length group
   * \param givenname is the name of the predator
   * \param area is the IntVector of the areas that the predator will live on
   * \note this constructor is used when creating a non-dynamic predator (ie. LengthPredator class)
   */
  PopPredator(const char* givenname, const IntVector& areas);
  /**
   * \brief This is the default PopPredator destructor
   */
  virtual ~PopPredator();
  virtual void Print(ofstream& outfile) const;
  virtual const BandMatrix& getConsumption(int area, const char* preyname) const;
  virtual const DoubleVector& getOverConsumption(int area) const { return overconsumption[this->areaNum(area)]; };
  virtual double getTotalOverConsumption(int area) const;
  virtual const LengthGroupDivision* getLengthGroupDiv() const { return LgrpDiv; };
  virtual int numLengthGroups() const { return LgrpDiv->numLengthGroups(); };
  virtual double meanLength(int i) const { return LgrpDiv->meanLength(i); };
  virtual void Reset(const TimeClass* const TimeInfo);
  virtual const double getConsumptionBiomass(int prey, int area) const;
  void setPrey(PreyPtrVector& preyvec, Keeper* const keeper);
protected:
  /**
   * \brief This is the LengthGroupDivision used to store length information
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the ConversionIndexPtrVector used to convert to the lengths groups for the length-based predator
   */
  ConversionIndex* CI;
  /**
   * \brief This is the PopInfoMatrix used to store information on the number of predators for the current timestep
   * \note the indices for this object are [area][predator length]
   */
  PopInfoMatrix prednumber;
  /**
   * \brief This is the DoubleMatrix used to store information on the overconsumption by the predators on the current timestep
   * \note the indices for this object are [area][predator length]
   */
  DoubleMatrix overconsumption;
  /**
   * \brief This is the BandMatrixPtrMatrix used to store information on the consumption by the predators on the current timestep
   * \note the indices for this object are [area][prey][predator length][prey length]
   */
  BandMatrixPtrMatrix consumption;
  /**
   * \brief This is the DoubleMatrix used to store information on the total consumption by the predators on the current timestep
   * \note the indices for this object are [area][predator length]
   */
  DoubleMatrix totalconsumption;
  /**
   * \brief This is the DoubleMatrix used to store information on the overconsumption by the predators on the current substep of the current timestep
   * \note the indices for this object are [area][predator length]
   */
  DoubleMatrix overcons;
  /**
   * \brief This is the BandMatrixPtrMatrix used to store information on the consumption by the predators on the current substep of the current timestep
   * \note the indices for this object are [area][prey][predator length][prey length]
   */
  BandMatrixPtrMatrix cons;
  /**
   * \brief This is the DoubleMatrix used to store information on the total consumption by the predators on the current substep of the current timestep
   * \note the indices for this object are [area][predator length]
   */
  DoubleMatrix totalcons;
};

#endif
