#ifndef livesonareas_h
#define livesonareas_h

#include "intvector.h"

class LivesOnAreas;

class LivesOnAreas {
public:
  LivesOnAreas() {};
  virtual ~LivesOnAreas() {};
  LivesOnAreas(const IntVector& Areas);
  int IsInArea(int area) const;
  const IntVector& Areas() const;
protected:
  void LetLiveOnAreas(const IntVector& Areas);
  IntVector areas;
  IntVector AreaNr;
};

#endif
