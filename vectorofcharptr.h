#ifndef vectorofcharptr_h
#define vectorofcharptr_h

/**
 * \class VectorOfCharPtr
 * \brief This class implements a dynamic vector of char arrays
 */
class VectorOfCharPtr {
public:
  /**
   * \brief This is the default VectorOfCharPtr constructor
   */
  VectorOfCharPtr() { size = 0; v = 0; };
  /**
   * \brief This is the VectorOfCharPtr constructor for a specified size
   * \param sz is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  VectorOfCharPtr(int sz);
  /**
   * \brief This is the VectorOfCharPtr constructor for a specified size with an initial value
   * \param sz is the size of the vector to be created
   * \param initial is the initial value for all the entries of the vector
   */
  VectorOfCharPtr(int sz, char* initial);
  /**
   * \brief This is the VectorOfCharPtr constructor that create a copy of an existing VectorOfCharPtr
   * \param initial is the VectorOfCharPtr to copy
   */
  VectorOfCharPtr(const VectorOfCharPtr& initial);
  /**
   * \brief This is the VectorOfCharPtr destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~VectorOfCharPtr();
  /**
   * \brief This will set an entry of the VectorOfCharPtr vector to a specified value
   * \param id is the element of the vector to be set
   * \param value is the value that will be entered into the vector
   */
  void set(int id, char* value);
  /**
   * \brief This will add new entries to the vector
   * \param add is the number of new entries to the vector
   * \param value is the value that will be entered for the new entries
   */
  void resize(int add, char* value);
  /**
   * \brief This will add new empty entries to the vector
   * \param add is the number of new entries to the vector
   * \note The new elements of the vector will be created, and set to zero
   */
  void resize(int add);
  /**
   * \brief This will delete an entry from the vector
   * \param pos is the element of the vector to be deleted
   * \note This will free the memory allocated to the deleted element of the vector
   */
  void Delete(int pos);
  /**
   * \brief This will return the size of the vector
   * \return the size of the vector
   */
  int Size() const { return size; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  char* const& operator [] (int pos) const;
private:
  /**
   * \brief This is the vector of char arrays
   */
  char** v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#endif
