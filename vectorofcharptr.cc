#include "vectorofcharptr.h"
#include "gadget.h"

void vectorofcharptr::set(int id, char* value) {
  if (v[id] != NULL) {
    delete[] v[id];
    v[id] = NULL;
  }
  if (value != NULL) {
    v[id] = new char[strlen(value) + 1];
    strcpy(v[id], value);
  } else
    v[id] = NULL;
}

vectorofcharptr::vectorofcharptr(int sz) {
  int i;
  size = (sz > 0 ? sz : 0);
  if (size > 0) {
    v = new char*[size];
    for (i = 0; i < size; i++)
      v[i] = NULL;

  } else
    v = 0;
}

vectorofcharptr::vectorofcharptr(int sz, char* value) {
  int i;
  size = (sz > 0 ? sz : 0);
  if (size > 0) {
    v = new char*[size];
    for (i = 0; i < size; i++) {
      v[i] = NULL;
      set(i, value);
    }
  } else
    v = 0;
}

vectorofcharptr::vectorofcharptr(const vectorofcharptr& initial) {
  int i;
  size = initial.size;
  if (size > 0) {
    v = new char*[size];
    for (i = 0; i < size; i++) {
      v[i] = NULL;
      set(i, initial.v[i]);
    }
  } else
    v = 0;
}

vectorofcharptr::~vectorofcharptr() {
  int i;
  if (v != 0) {
    for (i = 0; i < size; i++) {
      if (v[i] != NULL) {
        delete[] v[i];
        v[i] = NULL;
      }
    }
    delete[] v;
  }
}

void vectorofcharptr::resize(int addsize, char* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      set(i, value);

}

void vectorofcharptr::resize(int addsize) {
  int i;
  if (size == 0) {
    size = addsize;
    v = new char*[size];
    for (i = 0; i < size; i++)
      v[i] = NULL;

  } else if (addsize > 0) {
    char** vnew = new char*[addsize + size];
    for (i = 0; i < size; i++) {
      if (v[i] == NULL) {
        vnew[i] = NULL;
      } else {
        vnew[i] = new char[strlen(v[i]) + 1];
        strcpy(vnew[i], v[i]);
        delete[] v[i];
      }
    }
    delete[] v;
    v = vnew;
    for (i = size; i < (addsize + size); i++)
      v[i] = NULL;

    size += addsize;
  }
}

void vectorofcharptr::Delete(int pos) {
  int i;
  if (size == 1) {
    if (v[0] != NULL)
      delete[] v[0];

    delete[] v;
    v = 0;
    size = 0;

  } else {
    char** vnew = new char*[size - 1];
    for (i = 0; i < pos; i++) {
      if (v[i] == NULL) {
        vnew[i] = NULL;
      } else {
        vnew[i] = new char[strlen(v[i]) + 1];
        strcpy(vnew[i], v[i]);
        delete[] v[i];
      }
    }
    for (i = pos; i < size - 1; i++) {
      if (v[i] == NULL) {
        vnew[i] = NULL;
      } else {
        vnew[i] = new char[strlen(v[i + 1]) + 1];
        strcpy(vnew[i], v[i + 1]);
        delete[] v[i + 1];
      }
    }
    delete[] v;
    v = vnew;
    size--;
  }
}

char* const& vectorofcharptr::operator [] (int pos) const {
  return v[pos];
}
