#include "livesonareas.h"
#include "gadget.h"

LivesOnAreas::LivesOnAreas(const intvector& Areas) : areas(Areas) {
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
    return(AreaNr[area] >= 0);
  else
    return 0;
}

//Function added 25-9 2001 due to problems in fleetpreyaggregator HB
const intvector& LivesOnAreas::Areas() const {
  return areas;
}

void LivesOnAreas::LetLiveOnAreas(const intvector& Areas) {
  assert(Areas.Size() > 0);
  while (areas.Size() > 0)
    areas.Delete(0);
  areas.resize(Areas.Size());
  int i;
  int maxim = 0;
  for (i = 0; i < areas.Size(); i++)
    areas[i] = Areas[i];
  while (AreaNr.Size() > 0)
    AreaNr.Delete(0);
  for (i = 0; i < areas.Size(); i++)
    if (areas[i] > maxim)
      maxim = areas[i];
  AreaNr.resize(maxim + 1, -1);
  for (i = 0; i < areas.Size(); i++)
    AreaNr[areas[i]] = i;
}
