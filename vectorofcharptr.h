#ifndef vectorofcharptr_h
#define vectorofcharptr_h

/* Class vectorofcharptr implements a vector of char* where values
 * can be added and deleted dynamically. When new char* values are
 * set then new memory is allocated and when they are deleted then
 * all memory is returned */

class vectorofcharptr {
public:
  vectorofcharptr() { size = 0; v = 0; };
  vectorofcharptr(int sz);
  vectorofcharptr(int sz, char* initial);
  vectorofcharptr(const vectorofcharptr& initial);
  ~vectorofcharptr();
  void set(int id, char* value);
  void resize(int add, char* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return size; };
  char* const& operator [] (int pos) const;
private:
  char** v;        // v[0..size-1], if size = 0 then v = 0.
  int size;
};

#endif
