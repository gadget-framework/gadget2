#ifndef charptrvector_h
#define charptrvector_h

/**
 * \class CharPtrVector
 * \brief This class implements a dynamic vector of char values
 */
class CharPtrVector {
public:
  /**
   * \brief This is the default CharPtrVector constructor
   */
  CharPtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the CharPtrVector constructor that creates a copy of an existing CharPtrVector
   * \param initial is the CharPtrVector to copy
   */
  CharPtrVector(const CharPtrVector& initial);
  /**
   * \brief This is the CharPtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~CharPtrVector();
  /**
   * \brief This will add new blank (ie. NULL) entries to the vector
   * \param addsize is the number of blank entries that will be entered
   */
  void resizeBlank(int addsize);
  /**
   * \brief This will add one new entry to the vector
   * \param value is the value that will be entered for the new entry
   */
  void resize(char* value);
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
  char*& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  char* const& operator [] (int pos) const { return v[pos]; };
  /**
   * \brief This will reset the vector
   * \note This will delete every entry from the vector and set the size to zero
   */
  void Reset();
  /**
   * \brief This operator will set the vector equal to an existing CharPtrVector
   * \param cv is the CharPtrVector to copy
   */
  CharPtrVector& operator = (const CharPtrVector& cv);
protected:
  /**
   * \brief This is the vector of char values
   */
  char** v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#endif
