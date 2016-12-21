#ifndef conversionindexptrvector_h
#define conversionindexptrvector_h

class ConversionIndex;

/**
 * \class ConversionIndexPtrVector
 * \brief This class implements a dynamic vector of ConversionIndex values
 */
class ConversionIndexPtrVector {
public:
  /**
   * \brief This is the default ConversionIndexPtrVector constructor
   */
  ConversionIndexPtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the ConversionIndexPtrVector constructor that creates a copy of an existing ConversionIndexPtrVector
   * \param initial is the ConversionIndexPtrVector to copy
   */
  ConversionIndexPtrVector(const ConversionIndexPtrVector& initial);
  /**
   * \brief This is the ConversionIndexPtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~ConversionIndexPtrVector();
  /**
   * \brief This will add one new entry to the vector
   * \param value is the value that will be entered for the new entry
   */
  void resize(ConversionIndex* value);
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
  ConversionIndex*& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  ConversionIndex* const& operator [] (int pos) const { return v[pos]; };
protected:
  /**
   * \brief This is the vector of ConversionIndex values
   */
  ConversionIndex** v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#endif
