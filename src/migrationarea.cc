#include "migrationarea.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "rectangle.h"
#include "gadget.h"
#include "global.h"

MigrationArea::MigrationArea(CommentStream& infile, char* name, int id) {

  areaid = id;
  areaName = new char[strlen(name) + 1];
  strcpy(areaName, name);

  // read rectangles
  infile >> ws;
  if (countColumns(infile) != 4)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 4");
  while (!infile.eof() && !infile.fail())
    rectangles.resize(new Rectangle(infile));

  int i;
  areaSize = 0.0;
  for (i = 0; i < rectangles.Size(); i++)
    areaSize += rectangles[i]->getArea();

  handle.logMessage(LOGMESSAGE, "Read migration area file - number of rectangles", rectangles.Size());
}

MigrationArea::~MigrationArea() {
  int i;
  delete[] areaName;
  for (i = 0; i < rectangles.Size(); i++)
    delete rectangles[i];
}
