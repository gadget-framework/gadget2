#ifndef tagptrvector_h
#define tagptrvector_h

#include "tags.h"

class Tags;

class Tagptrvector {
 public:
  Tagptrvector() { size = 0; v = 0; teljari = 0; index = 0;};
  Tagptrvector(int sz);
  Tagptrvector(int sz, Tags* initial);
  Tagptrvector(const Tagptrvector& initial);
  ~Tagptrvector() { delete[] v; };
  void resize(int add, Tags* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return size; };
  Tags*& operator [] (int pos);
  Tags* const& operator [] (int pos) const;
  void UpdateTags(const TimeClass* const TimeInfo);
  void DeleteTags(const TimeClass* const TimeInfo);
  void SetTaggedStocks(const charptrvector Names);
 protected:
  Tags** v;
  int size;
  int index;
  charptrvector tagstocknames;
  int teljari;
};

#ifdef INLINE_VECTORS
#include "tagptrvector.icc"
#endif

#endif
