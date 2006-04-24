#ifndef optinfoptrvector_h
#define optinfoptrvector_h

class OptInfo;

/**
 * \class OptInfoPtrVector
 * \brief This class implements a dynamic vector of OptInfo values
 */
class OptInfoPtrVector {
public:
  /**
   * \brief This is the default OptInfoPtrVector constructor
   */
  OptInfoPtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the OptInfoPtrVector constructor that creates a copy of an existing OptInfoPtrVector
   * \param initial is the OptInfoPtrVector to copy
   */
  OptInfoPtrVector(const OptInfoPtrVector& initial);
  /**
   * \brief This is the OptInfoPtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~OptInfoPtrVector();
  /**
   * \brief This will add one new entry to the vector
   * \param value is the value that will be entered for the new entry
   */
  void resize(OptInfo* value);
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
  OptInfo*& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  OptInfo* const& operator [] (int pos) const { return v[pos]; };
protected:
  /**
   * \brief This is the vector of OptInfo values
   */
  OptInfo** v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#endif
