#ifndef stockpreyptrvector_h
#define stockpreyptrvector_h

class StockPrey;

class StockPreyptrvector {
public:
  StockPreyptrvector() { size = 0; v = 0; };
  StockPreyptrvector(int sz);
  StockPreyptrvector(int sz, StockPrey* initial);
  StockPreyptrvector(const StockPreyptrvector& initial);
  ~StockPreyptrvector() { delete[] v; };
  void resize(int add, StockPrey* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return(size); };
  StockPrey*& operator [] (int pos);
  StockPrey* const& operator [] (int pos) const;
protected:
  StockPrey** v;
  int size;
};

#ifdef INLINE_VECTORS
#include "stockpreyptrvector.icc"
#endif

#endif
