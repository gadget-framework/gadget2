#ifndef poppredatorptrvector_h
#define poppredatorptrvector_h

class PopPredator;

class PopPredatorptrvector {
public:
  PopPredatorptrvector() { size = 0; v = 0; };
  PopPredatorptrvector(int sz);
  PopPredatorptrvector(int sz, PopPredator* initial);
  PopPredatorptrvector(const PopPredatorptrvector& initial);
  ~PopPredatorptrvector();
  void resize(int add, PopPredator* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return size; };
  PopPredator*& operator [] (int pos);
  PopPredator* const& operator [] (int pos) const;
protected:
  PopPredator** v;
  int size;
};

#ifdef GADGET_INLINE
#include "poppredatorptrvector.icc"
#endif

#endif
