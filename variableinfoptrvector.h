#ifndef variableinfoptrvector_h
#define variableinfoptrvector_h

class VariableInfo;

class VariableInfoptrvector {
public:
  VariableInfoptrvector() { size = 0; v = 0; };
  VariableInfoptrvector(int sz);
  VariableInfoptrvector(int sz, VariableInfo* initial);
  VariableInfoptrvector(const VariableInfoptrvector& initial);
  ~VariableInfoptrvector();
  void resize(int add, VariableInfo* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return size; };
  VariableInfo*& operator [] (int pos);
  VariableInfo* const& operator [] (int pos) const;
protected:
  VariableInfo** v;
  int size;
};

#ifdef GADGET_INLINE
#include "variableinfoptrvector.icc"
#endif

#endif
