#include "livesonareas.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

LivesOnAreas::LivesOnAreas(const IntVector& Areas) : areas(Areas) {
  int i, maxim = 0;
  for (i = 0; i < areas.Size(); i++)
    if (areas[i] > maxim)
      maxim = areas[i];
  areaConvert.resize(maxim + 1, -1);
  for (i = 0; i < areas.Size(); i++)
    areaConvert[areas[i]] = i;
}

int LivesOnAreas::isInArea(int area) const {
  if (0 <= area && area < areaConvert.Size())
    return (areaConvert[area] >= 0);
  return 0;
}

int LivesOnAreas::areaNum(int area) const {
  if (0 <= area && area < areaConvert.Size())
    return areaConvert[area];
  handle.logMessage(LOGFAIL, "Error in livesonareas - failed to match area", area);
  return -1;
}

void LivesOnAreas::storeAreas(const IntVector& Areas) {

  while (areas.Size() > 0)
    areas.Delete(0);
  while (areaConvert.Size() > 0)
    areaConvert.Delete(0);

  if (Areas.Size() == 0)
    handle.logMessage(LOGFAIL, "Error in livesonareas - found no areas");

  int i, maxim = 0;
  areas.resize(Areas.Size());
  for (i = 0; i < areas.Size(); i++) {
    areas[i] = Areas[i];
    if (areas[i] > maxim)
      maxim = areas[i];
  }

  areaConvert.resize(maxim + 1, -1);
  for (i = 0; i < areas.Size(); i++)
    areaConvert[areas[i]] = i;
}
