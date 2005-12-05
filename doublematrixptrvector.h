#ifndef doublematrixptrvector_h
#define doublematrixptrvector_h

class DoubleMatrix;

/**
 * \class DoubleMatrixPtrVector
 * \brief This class implements a dynamic vector of DoubleMatrix values
 */
class DoubleMatrixPtrVector {
public:
  /**
   * \brief This is the default DoubleMatrixPtrVector constructor
   */
  DoubleMatrixPtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the DoubleMatrixPtrVector constructor for a specified size
   * \param sz is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  DoubleMatrixPtrVector(int sz);
  /**
   * \brief This is the DoubleMatrixPtrVector constructor that creates a copy of an existing DoubleMatrixPtrVector
   * \param initial is the DoubleMatrixPtrVector to copy
   */
  DoubleMatrixPtrVector(const DoubleMatrixPtrVector& initial);
  /**
   * \brief This is the DoubleMatrixPtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~DoubleMatrixPtrVector();
  /**
   * \brief This will add new blank (ie. NULL) entries to the vector
   * \param addsize is the number of blank entries that will be entered
   */
  void resizeBlank(int addsize);
  /**
   * \brief This will add one new entry to the vector
   * \param value is the value that will be entered for the new entry
   */
  void resize(DoubleMatrix* value);
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
  DoubleMatrix*& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  DoubleMatrix* const& operator [] (int pos) const { return v[pos]; };
protected:
  /**
   * \brief This is the vector of DoubleMatrix values
   */
  DoubleMatrix** v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#endif
