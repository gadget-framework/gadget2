#ifndef rectangle_h
#define rectangle_h

#include "gadget.h"
#include "commentstream.h"

/**
 * \class Rectangle
 * \brief This is the class used to store the co-ordinates and size of an area, used when calculating the migration within the model
 */
class Rectangle {
public:
  /**
   * \brief This is the default Rectangle constructor
   */
  Rectangle();
  /**
   * \brief This is the Rectangle constructor
   * \param infile is the CommentStream to read the rectangle parameters from
   */
  Rectangle(CommentStream& infile);
  /**
   * \brief This is the default Rectangle destructor
   */
  ~Rectangle() {};
  /**
   * \brief This will return the lower co-ordinate for the x axis of the rectangle
   * \return xLower
   */
  double getLowerX() { return xLower; };
  /**
   * \brief This will return the lower co-ordinate for the y axis of the rectangle
   * \return yLower
   */
  double getLowerY() { return yLower; };
  /**
   * \brief This will return the upper co-ordinate for the x axis of the rectangle
   * \return xUpper
   */
  double getUpperX() { return xUpper; };
  /**
   * \brief This will return the upper co-ordinate for the y axis of the rectangle
   * \return yUpper
   */
  double getUpperY() { return yUpper; };
  /**
   * \brief This will return the size of the rectangle
   * \return areaSize
   */
  double getArea() { return areaSize; };
private:
  /**
   * \brief This is the lower co-ordinate for the x axis of the rectangle
   */
  double xLower;
  /**
   * \brief This is the lower co-ordinate for the y axis of the rectangle
   */
  double yLower;
  /**
   * \brief This is the upper co-ordinate for the x axis of the rectangle
   */
  double xUpper;
  /**
   * \brief This is the upper co-ordinate for the y axis of the rectangle
   */
  double yUpper;
  /**
   * \brief This is the calculated size of the area of the rectangle
   */
  double areaSize;
};

#endif
