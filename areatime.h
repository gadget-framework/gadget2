#ifndef areatime_h
#define areatime_h

#include "doublematrix.h"
#include "doubleindexvector.h"
#include "commentstream.h"

class AreaClass;
class TimeClass;

/**
 * \class AreaClass
 * \brief This is the class used to store information about the areas used for the current model
 */
class AreaClass {
public:
  /**
   * \brief This is the AreaClass constructor
   * \param infile is the CommentStream to read the area data from
   * \param TimeInfo is the TimeClass for the current model
   */
  AreaClass(CommentStream& infile, const TimeClass* const TimeInfo);
  /**
   * \brief This is the default AreaClass destructor
   */
  ~AreaClass() {};
  /**
   * \brief This will return the number of areas for the current model
   * \return number
   */
  int NoAreas() const { return OuterAreas.Size(); };
  /**
   * \brief This will return the size of an area
   * \param area is the identifier for the required area
   * \return size
   */
  double Size(int area) const { return size[area]; };
  /**
   * \brief This will return the temperature of an area on a timestep
   * \param area is the identifier for the required area
   * \param time is the identifier for the required timestep
   * \return size
   */
  double Temperature(int area, int time) const { return temperature[time][area]; };
  /**
   * \brief This will return the internal identifier of an area
   * \param area is the identifier for the required area
   * \return innerarea
   */
  int InnerArea(int area) const;
  /**
   * \brief This will return the external identifier of an area
   * \param area is the identifier for the required area
   * \return outerarea
   */
  int OuterArea(int area) const { return OuterAreas[area]; };
protected:
  /**
   * \brief This is the IntVector of the areas in the current model
   */
  IntVector OuterAreas;
  /**
   * \brief This is the DoubleVector of the size of the areas in the current model
   */
  DoubleVector size;
  /**
   * \brief This is the DoubleMatrix of the temperature, for each timestep, of the areas in the current model
   */
  DoubleMatrix temperature;
};

/**
 * \class TimeClass
 * \brief This is the class used to store information about the timesteps used for the current model
 */
class TimeClass {
public:
  /**
   * \brief This is the TimeClass constructor
   * \param infile is the CommentStream to read the time data from
   */
  TimeClass(CommentStream& infile);
  /**
   * \brief This is the TimeClass destructor
   */
  ~TimeClass() {};
  /**
   * \brief This will return the current substep of the model simulation
   * \return currentsubstep
   */
  int CurrentSubstep() const { return currentsubstep; };
  /**
   * \brief This will return the current step of the model simulation
   * \return currentstep
   */
  int CurrentStep() const { return currentstep; };
  /**
   * \brief This will return the current year of the model simulation
   * \return currentyear
   */
  int CurrentYear() const { return currentyear; };
  /**
   * \brief This will return the total number of timesteps that have taken place in the simulation from the start of the model simulation until the current timestep
   * \return number of timesteps taken from the start of the simulation
   */
  int CurrentTime() const { return this->CalcSteps(currentyear, currentstep); };
  /**
   * \brief This will return the first step of the model simulation
   * \return firststep
   */
  int FirstStep() const { return firststep; };
  /**
   * \brief This will return the first year of the model simulation
   * \return firstyear
   */
  int FirstYear() const { return firstyear; };
  /**
   * \brief This will return the last step of the model simulation
   * \return laststep
   */
  int LastStep() const { return laststep; };
  /**
   * \brief This will return the last year of the model simulation
   * \return lastyear
   */
  int LastYear() const { return lastyear; };
  /**
   * \brief This will return the length of the current step of the model simulation
   * \return length of step
   */
  double LengthOfCurrent() const { return timesteps[currentstep]; };
  /**
   * \brief This will return the length of each year of the model simulation
   * \return lengthofyear
   */
  double LengthOfYear() const { return lengthofyear; };
  /**
   * \brief This will return the length of the specified step of the model simulation
   * \param step is the specified timestep 
   * \return length of step
   */
  double LengthOfStep(int step) const { return timesteps[step]; };
  /**
   * \brief This will return the total number of timesteps that have taken place in the simulation from the start of the model simulation until a specifed year and step
   * \param year is the specified year 
   * \param step is the specified step 
   * \return number of timesteps taken
   */
  int CalcSteps(int year, int step) const {
    return (notimesteps * (year - firstyear) + step - firststep + 1); };
  /**
   * \brief This will return the total number of timesteps in the model simulation
   * \return total number of timesteps
   */
  int TotalNoSteps() const {
    return (notimesteps * (lastyear - firstyear) + laststep - firststep + 1); };
  /**
   * \brief This will return the number of steps in each year of the model simulation
   * \return notimesteps
   */
  int StepsInYear() const { return notimesteps; };
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
  int IsWithinPeriod(int year, int step) const;
  /**
   * \brief This is the function that resets the timestep to the beginning of the model simulation
   */
  void ResetToBeginning() { currentyear = firstyear; currentstep = firststep; currentsubstep = 1; };
  /**
   * \brief This will return the number of substeps in the current timestep of the model simulation
   * \return notimesteps
   */
  int NrOfSubsteps() const;
  /**
   * \brief This is the function that increases the substep within the current timestep
   */
  void IncrementSubstep() { currentsubstep++; };
  /**
   * \brief This is the function that will check to see if the length of the current timestep has changed from the previous timestep
   * \return 1 if the length of the timestep has changed, 0 otherwise
   */
  int SizeOfStepDidChange() const;
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
  int notimesteps;
  /**
   * \brief This is the length of a year in the model simulation (should be 12)
   */
  double lengthofyear;
  /**
   * \brief This is the DoubleIndexVector of timesteps in each year
   */
  DoubleIndexVector timesteps;
  /**
   * \brief This is the DoubleIndexVector of substeps in each step
   */
  DoubleIndexVector nrofsubsteps;
  /**
   * \brief This is the current substep of the model simulation
   */
  int currentsubstep;
};

#endif
