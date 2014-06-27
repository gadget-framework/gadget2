#ifndef preyptrvector_h
#define preyptrvector_h

class Prey;

/**
 * \class PreyPtrVector
 * \brief This class implements a dynamic vector of Prey values
 */
class PreyPtrVector {
public:
  /**
   * \brief This is the default PreyPtrVector constructor
   */
  PreyPtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the PreyPtrVector constructor that creates a copy of an existing PreyPtrVector
   * \param initial is the PreyPtrVector to copy
   */
  PreyPtrVector(const PreyPtrVector& initial);
  /**
   * \brief This is the PreyPtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~PreyPtrVector();
  /**
   * \brief This will add new blank (ie. NULL) entries to the vector
   * \param addsize is the number of blank entries that will be entered
   */
  void resizeBlank(int addsize);
  /**
   * \brief This will add one new entry to the vector
   * \param value is the value that will be entered for the new entry
   */
  void resize(Prey* value);
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
  Prey*& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  Prey* const& operator [] (int pos) const { return v[pos]; };
protected:
  /**
   * \brief This is the vector of Prey values
   */
  Prey** v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#endif
