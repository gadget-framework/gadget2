#ifndef livesonareas_h
#define livesonareas_h

#include "intvector.h"

class LivesOnAreas;

class LivesOnAreas {
public:
  LivesOnAreas() {};
  virtual ~LivesOnAreas() {};
  LivesOnAreas(const intvector& Areas);
  int IsInArea(int area) const;
  const intvector& Areas() const;
protected:
  void LetLiveOnAreas(const intvector& Areas);
  intvector areas;
  intvector AreaNr;
};

#endif
