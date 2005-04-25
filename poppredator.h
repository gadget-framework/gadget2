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
  PopPredator(const char* givenname, const IntVector& areas,
    const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv);
  PopPredator(const char* givenname, const IntVector& areas);
  virtual ~PopPredator();
  virtual void Print(ofstream& outfile) const;
  virtual const BandMatrix& getConsumption(int area, const char* preyname) const;
  virtual const DoubleVector& getOverConsumption(int area) const { return overconsumption[this->areaNum(area)]; };
  virtual double getTotalOverConsumption(int area) const;
  virtual const LengthGroupDivision* returnLengthGroupDiv() const { return LgrpDiv; };
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
