#ifndef prey_h
#define prey_h

#include "popinfovector.h"
#include "conversionindex.h"
#include "commentstream.h"
#include "charptrvector.h"
#include "hasname.h"
#include "livesonareas.h"
#include "agebandmatrix.h"
#include "keeper.h"
#include "gadget.h"

enum PreyType { STOCKPREYTYPE = 1, LENGTHPREYTYPE };

/**
 * \class Prey
 * \brief This is the base class used to model the consumption of a prey
 * \note This will be overridden by the derived classes that actually calculate the consumption
 */
class Prey : public HasName, public LivesOnAreas {
public:
  Prey(CommentStream& infile, const IntVector& areas, const char* givenname, Keeper* const keeper);
  /**
   * \brief This is the Prey constructor for a prey with a given length group
   * \param lengths is the DoubleVector of length groups of the prey
   * \param areas is the IntVector of areas that the prey lives on
   * \param givenname is the name of the prey
   */
  Prey(const DoubleVector& lengths, const IntVector& areas, const char* givenname);
  /**
   * \brief This is the default Prey destructor
   */
  virtual ~Prey() = 0;
  virtual void Sum(const AgeBandMatrix& Alkeys, int area, int NrofSubstep) {};
  virtual void Subtract(AgeBandMatrix& Alkeys, int area);
  void addBiomassConsumption(int area, const DoubleIndexVector& predconsumption);
  void addNumbersConsumption(int area, const DoubleIndexVector& predconsumption);
  virtual void setCI(const LengthGroupDivision* const GivenLDiv);
  virtual void Print(ofstream& outfile) const;
  double getBiomass(int area, int length) const { return biomass[this->areaNum(area)][length]; };
  double getNumber(int area, int length) const { return Number[this->areaNum(area)][length].N; };
  double Biomass(int area) const { return total[this->areaNum(area)]; };
  int TooMuchConsumption(int area) const { return tooMuchConsumption[this->areaNum(area)]; };
  virtual void checkConsumption(int area, int numsubsteps);
  double Ratio(int area, int length) const { return ratio[this->areaNum(area)][length]; };
  double Length(int j) const { return LgrpDiv->meanLength(j); };
  int numLengthGroups() const { return LgrpDiv->numLengthGroups(); };
  const DoubleVector& Bconsumption(int area) const { return consumption[this->areaNum(area)]; };
  const DoubleVector& OverConsumption(int area) const { return overconsumption[this->areaNum(area)]; };
  const LengthGroupDivision* returnLengthGroupDiv() const { return LgrpDiv; };
  virtual void Reset();
  const PopInfoVector& getNumberPriorToEating(int area) const { return numberPriorToEating[this->areaNum(area)]; };
  /**
   * \brief This will check if there is any prey to consume on a given area
   * \param area is the area that the prey will be consumed on
   * \return 1 if there is prey to consume, 0 otherwise
   */
  int isPreyArea(int area);
  /**
   * \brief This will return the type of prey class
   * \return type
   */
  PreyType Type() { return type; };
protected:
  void InitialiseObjects();
  ConversionIndex* CI;
  LengthGroupDivision* LgrpDiv;
  PopInfoMatrix Number;
  PopInfoMatrix numberPriorToEating;
  DoubleMatrix biomass;
  DoubleMatrix ratio;
  DoubleMatrix consumption;
  IntVector tooMuchConsumption;      //set if any lengthgr is overconsumed in area
  DoubleVector total;
  DoubleMatrix overconsumption;
  DoubleMatrix overcons;             //overconsumption of prey in subinterval
  DoubleMatrix cons;                 //consumption of prey in subinterval
  /**
   * \brief This denotes what type of prey class has been created
   */
  PreyType type;
};

#endif
