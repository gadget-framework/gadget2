#ifndef doubleindexvector_h
#define doubleindexvector_h

class doubleindexvector {
public:
  doubleindexvector() { minpos = 0; size = 0; v = 0; };
  doubleindexvector(int sz, int minpos);
  doubleindexvector(int sz, int minpos, double initial);
  doubleindexvector(const doubleindexvector& initial);
  ~doubleindexvector() { delete[] v; };
  void resize(int addsize, int newminpos, double value);
  double& operator [] (int pos);
  const double& operator [] (int pos) const;
  int Mincol() const { return(minpos); };
  int Maxcol() const { return(minpos + size); };
  int Size() const { return(size); };
  void Delete(int pos);
protected:
  int minpos;
  int size;
  double* v;
};

#endif
