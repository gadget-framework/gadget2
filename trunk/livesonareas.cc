#include "livesonareas.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

LivesOnAreas::LivesOnAreas(const IntVector& Areas) : areas(Areas) {
  int i, j, maxim = 0;
  if (areas.Size() == 0)
    handle.logMessage(LOGFAIL, "Error in input files - found no areas");

  for (i = 0; i < areas.Size(); i++)
    if (areas[i] > maxim)
      maxim = areas[i];

  for (i = 0; i < areas.Size(); i++)
    for (j = 0; j < areas.Size(); j++)
      if ((areas[i] == areas[j]) && (i != j))
        handle.logMessage(LOGFAIL, "Error in input files - repeated area", i);

  areaConvert.Reset();
  areaConvert.resize(maxim + 1, -1);
  for (i = 0; i < areas.Size(); i++)
    if ((areas[i] >= 0) && (areas[i] <= maxim))
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
  return -1;
}

void LivesOnAreas::storeAreas(const IntVector& Areas) {
  int i, j, maxim = 0;
  if (Areas.Size() == 0)
    handle.logMessage(LOGFAIL, "Error in input files - found no areas");

  areas.Reset();
  areas.resize(Areas.Size(), -1);
  for (i = 0; i < areas.Size(); i++) {
    areas[i] = Areas[i];
    if (areas[i] > maxim)
      maxim = areas[i];
  }

  for (i = 0; i < areas.Size(); i++)
    for (j = 0; j < areas.Size(); j++)
      if ((areas[i] == areas[j]) && (i != j))
        handle.logMessage(LOGFAIL, "Error in input files - repeated area", i);

  areaConvert.Reset();
  areaConvert.resize(maxim + 1, -1);
  for (i = 0; i < areas.Size(); i++)
    if ((areas[i] >= 0) && (areas[i] <= maxim))
      areaConvert[areas[i]] = i;
}
