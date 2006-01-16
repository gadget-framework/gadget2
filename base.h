#ifndef base_h
#define base_h

#include "areatime.h"
#include "hasname.h"
#include "livesonareas.h"

/**
 * \class BaseClass
 * \brief This is the base class for any object that can be dynamically modelled within Gadget
 * \note This will always be overridden by the derived classes (either Stock, Fleet or OtherFood)
 */
class BaseClass : public HasName, public LivesOnAreas {
public:
  /**
   * \brief This is the default BaseClass constructor
   */
  BaseClass() {};
  /**
   * \brief This is the BaseClass constructor for an object with a name
   * \param givenname is a text string containing the name of the object to be created
   */
  BaseClass(const char* givenname) : HasName(givenname) {};
  /**
   * \brief This is the BaseClass constructor for an object with a name and an area
   * \param givenname is a text string containing the name of the object to be created
   * \param Areas is the IntVector of internal areas to be used
   */
  BaseClass(const char* givenname, const IntVector& Areas) : HasName(givenname), LivesOnAreas(Areas) {};
  /**
   * \brief This is the default BaseClass destructor
   */
  virtual ~BaseClass() {};
  /**
   * \brief This function will update the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void calcNumbers(int area, const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This function will calculate the modelled consumption for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void calcEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This function will check the modelled consumption for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void checkEat(int area, const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This function will adjust the modelled consumption for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void adjustEat(int area, const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This function will reduce the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void reducePop(int area, const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This function will calculate the growth of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Grow(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This function will calculate any transition of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updateAgePart1(int area, const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This function will calculate the age increase of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updateAgePart2(int area, const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This function will implement the transiton of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updateAgePart3(int area, const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This function will calculate the spawning of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updatePopulationPart1(int area, const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This function will calculate add the newly matured stock into the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updatePopulationPart2(int area, const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This function will calculate add the new recruits into the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updatePopulationPart3(int area, const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This function will calculate calculate the straying of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updatePopulationPart4(int area, const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This function will calculate add the strayed stock into the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updatePopulationPart5(int area, const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This function will implement the migration of the model population for the model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Migrate(const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This function will reset the model population
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Reset(const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This function will print the model population
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const = 0;
};

#endif
