#ifndef printerptrvector_h
#define printerptrvector_h

class Printer;

class Printerptrvector {
public:
  Printerptrvector() { size = 0; v = 0; };
  Printerptrvector(int sz);
  Printerptrvector(int sz, Printer* initial);
  Printerptrvector(const Printerptrvector& initial);
  ~Printerptrvector();
  void resize(int add, Printer* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return size; };
  Printer*& operator [] (int pos);
  Printer* const& operator [] (int pos) const;
protected:
  Printer** v;
  int size;
};

#ifdef GADGET_INLINE
#include "printerptrvector.icc"
#endif

#endif
