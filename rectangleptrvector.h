#ifndef rectangleptrvector_h
#define rectangleptrvector_h

class Rectangle;

/**
 * \class RectanglePtrVector
 * \brief This class implements a dynamic vector of Rectangle values
 */
class RectanglePtrVector {
public:
  /**
   * \brief This is the default RectanglePtrVector constructor
   */
  RectanglePtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the RectanglePtrVector constructor that creates a copy of an existing RectanglePtrVector
   * \param initial is the RectanglePtrVector to copy
   */
  RectanglePtrVector(const RectanglePtrVector& initial);
  /**
   * \brief This is the RectanglePtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~RectanglePtrVector();
  /**
   * \brief This will add one new entry to the vector
   * \param value is the value that will be entered for the new entry
   */
  void resize(Rectangle* value);
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
  Rectangle*& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  Rectangle* const& operator [] (int pos) const { return v[pos]; };
protected:
  /**
   * \brief This is the vector of Rectangle values
   */
  Rectangle** v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#endif
