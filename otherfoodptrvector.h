#ifndef otherfoodptrvector_h
#define otherfoodptrvector_h

class OtherFood;

class OtherFoodptrvector {
public:
  OtherFoodptrvector() { size = 0; v = 0; };
  OtherFoodptrvector(int sz);
  OtherFoodptrvector(int sz, OtherFood* initial);
  OtherFoodptrvector(const OtherFoodptrvector& initial);
  ~OtherFoodptrvector();
  void resize(int add, OtherFood* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return size; };
  OtherFood*& operator [] (int pos);
  OtherFood* const& operator [] (int pos) const;
protected:
  OtherFood** v;
  int size;
};

#ifdef GADGET_INLINE
#include "otherfoodptrvector.icc"
#endif

#endif
