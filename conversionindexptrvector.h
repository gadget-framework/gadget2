#ifndef conversionindexptrvector_h
#define conversionindexptrvector_h

class ConversionIndex;

class ConversionIndexptrvector {
public:
  ConversionIndexptrvector() { size = 0; v = 0; };
  ConversionIndexptrvector(int sz);
  ConversionIndexptrvector(int sz, ConversionIndex* initial);
  ConversionIndexptrvector(const ConversionIndexptrvector& initial);
  ~ConversionIndexptrvector() { delete[] v; };
  void resize(int add, ConversionIndex* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return(size); };
  ConversionIndex*& operator [] (int pos);
  ConversionIndex* const& operator [] (int pos) const;
protected:
  ConversionIndex** v;
  int size;
};

#ifdef INLINE_VECTORS
#include "conversionindexptrvector.icc"
#endif

#endif
