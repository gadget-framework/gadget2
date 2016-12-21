#ifndef areatime_h
#define areatime_h

#include "doublematrix.h"
#include "formulavector.h"
#include "commentstream.h"
#include "keeper.h"

class AreaClass;
class TimeClass;

/**
 * \class AreaClass
 * \brief This is the class used to store information about the areas used for the current model
 *
 * This class keeps an index of the areas in the model.  The areas are read in with a numerical identifier, which is stored along with an internal identifier for the areas, which is a simple index starting from 0.
 */
class AreaClass {
public:
  /**
   * \brief This is the AreaClass constructor
   * \param infile is the CommentStream to read the area data from
   * \param keeper is the Keeper for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  AreaClass(CommentStream& infile, Keeper* const keeper, const TimeClass* const TimeInfo);
  /**
   * \brief This is the default AreaClass destructor
   */
  ~AreaClass() {};
  /**
   * \brief This will return the number of areas for the current model
   * \return number
   */
  int numAreas() const { return modelAreas.Size(); };
  /**
   * \brief This will return the size of an area
   * \param area is the identifier for the required area
   * \return size
   */
  double getSize(int area) const { return size[area]; };
  /**
   * \brief This will return the temperature of an area on a timestep
   * \param area is the identifier for the required area
   * \param time is the identifier for the required timestep
   * \return temperature
   */
  double getTemperature(int area, int time) const { return temperature[time][area]; };
  /**
   * \brief This will return the internal identifier of an area
   * \param area is the identifier for the required area
   * \return innerarea
   */
  int getInnerArea(int area) const;
  /**
   * \brief This will return the external identifier of an area
   * \param area is the identifier for the required area
   * \return outerarea
   */
  int getModelArea(int area) const { return modelAreas[area]; };
  /**
   * \brief This will return the the IntVector of all the areas in the current model
   * \return modelAreas
   */
  const IntVector& getAllModelAreas() const { return modelAreas; };
protected:
  /**
   * \brief This is the IntVector of the areas in the current model
   */
  IntVector modelAreas;
  /**
   * \brief This is the FormulaVector of the size of the areas in the current model
   */
  FormulaVector size;
  /**
   * \brief This is the DoubleMatrix of the temperature, for each timestep, of the areas in the current model
   */
  DoubleMatrix temperature;
};

/**
 * \class TimeClass
 * \brief This is the class used to store information about the timesteps used for the current model
 *
 * This class keeps an index of the timesteps in the model.  A step is a subdivision of a year, and is usually defined in terms of months, and is the same in each year of the model.  A check is made to ensure that the total length of all the steps adds up to 12.
 */
class TimeClass {
public:
  /**
   * \brief This is the TimeClass constructor
   * \param infile is the CommentStream to read the time data from
   * \param maxratio is the maximum ratio of a stock that will be consumed on any given timestep
   */
  TimeClass(CommentStream& infile, double maxratio);
  /**
   * \brief This is the TimeClass destructor
   */
  ~TimeClass() {};
  /**
   * \brief This will return the current substep of the model simulation
   * \return currentsubstep
   */
  int getSubStep() const { return currentsubstep; };
  /**
   * \brief This will return the current step of the model simulation
   * \return currentstep
   */
  int getStep() const { return currentstep; };
  /**
   * \brief This will return the current year of the model simulation
   * \return currentyear
   */
  int getYear() const { return currentyear; };
  /**
   * \brief This will return the total number of timesteps that have taken place in the simulation from the start of the model simulation until the current timestep
   * \return number of timesteps taken from the start of the simulation
   */
  int getTime() const { return this->calcSteps(currentyear, currentstep); };
  /**
   * \brief This will return the first step of the model simulation
   * \return firststep
   */
  int getFirstStep() const { return firststep; };
  /**
   * \brief This will return the first year of the model simulation
   * \return firstyear
   */
  int getFirstYear() const { return firstyear; };
  /**
   * \brief This will return the last step of the model simulation
   * \return laststep
   */
  int getLastStep() const { return laststep; };
  /**
   * \brief This will return the last year of the model simulation
   * \return lastyear
   */
  int getLastYear() const { return lastyear; };
  /**
   * \brief This will return the length of the current step of the model simulation as a proportion of the whole year
   * \return proportion of year
   */
  double getTimeStepSize() const { return (timesteps[currentstep] * lengthofyear); };
  /**
   * \brief This will return the length of the current step of the model simulation
   * \return length of step
   */
  double getTimeStepLength() const { return timesteps[currentstep]; };
  /**
   * \brief This will return the total number of timesteps that have taken place in the simulation from the start of the model simulation until a specifed year and step
   * \param year is the specified year
   * \param step is the specified step
   * \return number of timesteps taken
   */
  int calcSteps(int year, int step) const {
    return (numtimesteps * (year - firstyear) + step - firststep + 1); };
  /**
   * \brief This will return the total number of timesteps in the model simulation
   * \return total number of timesteps
   */
  int numTotalSteps() const {
    return (numtimesteps * (lastyear - firstyear) + laststep - firststep + 1); };
  /**
   * \brief This will return the number of steps in each year of the model simulation
   * \return numtimesteps
   */
  int numSteps() const { return numtimesteps; };
  /**
   * \brief This is the function that increases the timestep for the model simulation
   */
  void IncrementTime();
  /**
   * \brief This is the function that will check to see if specified year and step are within the time period covered by the model simulation
   * \param year is the specified year
   * \param step is the specified step
   * \return 1 if the timestep is within the model period, 0 otherwise
   */
  int isWithinPeriod(int year, int step) const;
  /**
   * \brief This is the function that resets the timestep to the beginning of the model simulation
   */
  void Reset();
  /**
   * \brief This will return the number of substeps in the current timestep of the model simulation
   * \return number of substeps
   */
  int numSubSteps() const { return numsubsteps[currentstep - 1]; };
  /**
   * \brief This is the function that increases the substep within the current timestep
   */
  void IncrementSubstep() { currentsubstep++; };
  /**
   * \brief This is the function that will check to see if the length of the current timestep has changed from the previous timestep
   * \return 1 if the length of the timestep has changed, 0 otherwise
   */
  int didStepSizeChange() const;
  /**
   * \brief This function will return the maximum ratio of any stock that can be consumed on the current substep
   * \return maximum ratio of the stock that can be consumed on the current substep
   */
  double getMaxRatioConsumed() const;
protected:
  /**
   * \brief This is the current step of the model simulation
   */
  int currentstep;
  /**
   * \brief This is the current year of the model simulation
   */
  int currentyear;
  /**
   * \brief This is the first year in the model simulation
   */
  int firstyear;
  /**
   * \brief This is the first step in the model simulation
   */
  int firststep;
  /**
   * \brief This is the last year in the model simulation
   */
  int lastyear;
  /**
   * \brief This is the last step in the model simulation
   */
  int laststep;
  /**
   * \brief This is the number of steps in a year in the model simulation
   */
  int numtimesteps;
  /**
   * \brief This is the length of a year in the model simulation (should be 12)
   * \note This is stored as 1/length of year to save processing time
   */
  double lengthofyear;
  /**
   * \brief This is the maximum ratio of stock that can be consumed in any given timestep
   * \note This value will enforce a limit on the consumption of a stock which should prevent a stock from collapsing on any given timestep.  If the calculated consumption is over this ratio, then the consumption is limited to this value and the rest is treated as "overconsumption", which will lead to understocking.
   */
  double maxratioconsumed;
  /**
   * \brief This is the DoubleVector of timesteps in each year
   */
  DoubleVector timesteps;
  /**
   * \brief This is the IntVector of substeps in each step
   */
  IntVector numsubsteps;
  /**
   * \brief This is the current substep of the model simulation
   */
  int currentsubstep;
};

#endif
