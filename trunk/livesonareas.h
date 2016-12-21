#ifndef livesonareas_h
#define livesonareas_h

#include "intvector.h"

/**
 * \class LivesOnAreas
 * \brief This is the class used to store information about the areas an object is defined on for the current model
 */
class LivesOnAreas {
public:
  /**
   * \brief This is the default LivesOnAreas constructor
   */
  LivesOnAreas() {};
  /**
   * \brief This is the default LivesOnAreas destructor
   */
  virtual ~LivesOnAreas() {};
  /**
   * \brief This is the LivesOnAreas constructor for a specified vector of areas
   * \param Areas is the IntVector of internal areas to be used
   */
  LivesOnAreas(const IntVector& Areas);
  /**
   * \brief This function will check whether the object is defined on a specified area
   * \param area is an integer to denote the internal area of interest
   * \return 1 if the object is defined on the area, 0 otherwise
   */
  int isInArea(int area) const;
  /**
   * \brief This function will return the internal areas that the object is defined on
   * \return areas
   */
  IntVector& getAreas() { return areas; };
  /**
   * \brief This function will return the area associated with an internal area
   * \param area is an integer to denote the internal area of interest
   * \return the associated area number
   */
  int areaNum(int area) const;
protected:
  /**
   * \brief This function will store the internal areas to be used
   * \param Areas is the IntVector of internal areas to be used
   */
  void storeAreas(const IntVector& Areas);
  /**
   * \brief This is the IntVector of internal areas that the object is defined on
   */
  IntVector areas;
private:
  /**
   * \brief This is the IntVector used to convert to the internal areas
   */
  IntVector areaConvert;
};

#endif
