#ifndef stockptrvector_h
#define stockptrvector_h

class Stock;

class Stockptrvector {
public:
  Stockptrvector() { size = 0; v = 0; };
  Stockptrvector(int sz);
  Stockptrvector(int sz, Stock* initial);
  Stockptrvector(const Stockptrvector& initial);
  ~Stockptrvector();
  void resize(int add, Stock* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return size; };
  Stock*& operator [] (int pos);
  Stock* const& operator [] (int pos) const;
  Stockptrvector& operator = (const Stockptrvector& d);
protected:
  Stock** v;
  int size;
};

#ifdef GADGET_INLINE
#include "stockptrvector.icc"
#endif

#endif
