#include "rectangle.h"
#include "mathfunc.h"

Rectangle::Rectangle() {
  xLower = yLower = xUpper = yUpper = 0.0;
  areaSize = 0.0;
}

//JMB - we need to do some checks here ...
//should we convert these into something?  km?
Rectangle::Rectangle(CommentStream& infile) {
  infile >> yLower >> xLower >> yUpper >> xUpper >> ws;
//  infile >> xLower >> yLower >> xUpper >> yUpper >> ws;

  double R = 6371.0;
  double yLowerSin = sin(yLower*pivalue/180.0);
  double yUpperSin = sin(yUpper*pivalue/180.0);
  areaSize = ((pivalue/180.0)*R*R)*fabs(yLowerSin - yUpperSin)*fabs(xLower - xUpper);
}

/*
Equatorial radius (km)	        6378.1
Polar radius (km)               6356.8
Volumetric mean radius (km)     6371.0
Core radius (km)                3485
 = 2*pi*R^2 |sin(lat1)-sin(lat2)| |lon1-lon2|/360
 = (pi/180)R^2 |sin(lat1)-sin(lat2)| |lon1-lon2|
*/
