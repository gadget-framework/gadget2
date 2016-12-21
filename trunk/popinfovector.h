#ifndef popinfovector_h
#define popinfovector_h

#include "conversionindex.h"
#include "popinfo.h"

/**
 * \class PopInfoVector
 * \brief This class implements a dynamic vector of PopInfo values
 */
class PopInfoVector {
public:
  /**
   * \brief This is the default PopInfoVector constructor
   */
  PopInfoVector() { size = 0; v = 0; };
  /**
   * \brief This is the PopInfoVector constructor for a specified size
   * \param sz is the size of the vector to be created
   * \param initial is the initial value for all the entries of the vector
   */
  PopInfoVector(int sz, PopInfo initial);
  /**
   * \brief This is the PopInfoVector constructor that creates a copy of an existing PopInfoVector
   * \param initial is the PopInfoVector to copy
   */
  PopInfoVector(const PopInfoVector& initial);
  /**
   * \brief This is the PopInfoVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~PopInfoVector();
  /**
   * \brief This will add new blank (ie. NULL) entries to the vector
   * \param addsize is the number of blank entries that will be entered
   */
  void resizeBlank(int addsize);
  /**
   * \brief This will add one new entry to the vector
   * \param value is the value that will be entered for the new entry
   */
  void resize(PopInfo value);
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
  PopInfo& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const PopInfo& operator [] (int pos) const { return v[pos]; };
  /**
   * \brief This function will sum the elements of an existing PopInfoVector, possibly allowing for the length grouping to be changed using a ConversionIndex
   * \param Number is the PopInfoVector containing the elements to be summed
   * \param CI is the ConversionIndex to be used to convert from one length grouping to another
   */
  void Sum(const PopInfoVector* const Number, const ConversionIndex& CI);
protected:
  /**
   * \brief This is the vector of PopInfo values
   */
  PopInfo* v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#endif
