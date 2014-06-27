#ifndef migrationarea_h
#define migrationarea_h

#include "rectangleptrvector.h"
#include "commentstream.h"
#include "gadget.h"

/**
 * \class MigrationArea
 * \brief This is the class used to store details of the areas used when calculating the migration within the model
 */
class MigrationArea {
public:
  /**
   * \brief This is the MigrationArea constructor
   * \param infile is the CommentStream to read the migration area parameters from
   * \param name is the name of the migration area
   * \param id is the internal identifer of the migration area
   */
  MigrationArea(CommentStream& infile, char* name, int id);
  /**
   * \brief This is the default MigrationArea destructor
   */
  ~MigrationArea();
  /**
   * \brief This will return the size of the migration area
   * \return areaSize
   */
  double getArea() { return areaSize; };
  /**
   * \brief This will return the internal identifier of the migration area
   * \return areaid
   */
  int getAreaID() { return areaid; };
  /**
   * \brief This will return the number of rectangles that make up the the migration area
   * \return number of rectangles
   */
  int getNumRectangles() { return rectangles.Size(); };
  /**
   * \brief This will return the RectanglePtrVector of rectangles that make up the the migration area
   * \return rectangles, a RectanglePtrVector of the rectangles that make up the migration area
   */
  RectanglePtrVector& getRectangles() { return rectangles; };
private:
  /**
   * \brief This is the name of the migration area
   */
  char* areaName;
  /**
   * \brief This is the internal identifier for the migration area
   */
  int areaid;
  /**
   * \brief This is the calculated size of the area of the migration area
   */
  double areaSize;
  /**
   * \brief This is the RectanglePtrVector of the rectangles that make up the the migration area
   */
  RectanglePtrVector rectangles;
};

#endif
