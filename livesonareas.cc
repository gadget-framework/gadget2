#include "livesonareas.h"
#include "gadget.h"

LivesOnAreas::LivesOnAreas(const IntVector& Areas) : areas(Areas) {
  int i;
  int maxim = 0;
  for (i = 0; i < areas.Size(); i++)
    if (areas[i] > maxim)
      maxim = areas[i];
  AreaNr.resize(maxim + 1, -1);
  for (i = 0; i < areas.Size(); i++)
    AreaNr[areas[i]] = i;
}

int LivesOnAreas::IsInArea(int area) const {
  if (0 <= area && area < AreaNr.Size())
    return (AreaNr[area] >= 0);
  else
    return 0;
}

void LivesOnAreas::LetLiveOnAreas(const IntVector& Areas) {

  while (areas.Size() > 0)
    areas.Delete(0);
  while (AreaNr.Size() > 0)
    AreaNr.Delete(0);

  int i;
  int maxim = 0;
  areas.resize(Areas.Size());
  for (i = 0; i < areas.Size(); i++) {
    areas[i] = Areas[i];
    if (areas[i] > maxim)
      maxim = areas[i];
  }

  AreaNr.resize(maxim + 1, -1);
  for (i = 0; i < areas.Size(); i++)
    AreaNr[areas[i]] = i;
}
