#ifndef popratiovector_h
#define popratiovector_h

#include "conversion.h"
#include "popratio.h"

/**
 * \class PopRatioVector
 * \brief This class implements a dynamic vector of PopRatio values
 */
class PopRatioVector {
public:
  /**
   * \brief This is the default PopRatioVector constructor
   */
  PopRatioVector() { size = 0; v = 0;};
  /**
   * \brief This is the PopRatioVector constructor for a specified size
   * \param sz is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  PopRatioVector(int sz);
  /**
   * \brief This is the PopRatioVector constructor for a specified size with an initial value
   * \param sz is the size of the vector to be created
   * \param initial is the initial value for all the entries of the vector
   */
  PopRatioVector(int sz, PopRatio initial);
  /**
   * \brief This is the PopRatioVector constructor that create a copy of an existing PopRatioVector
   * \param initial is the PopRatioVector to copy
   */
  PopRatioVector(const PopRatioVector& initial);
  /**
   * \brief This is the PopRatioVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~PopRatioVector();
  /**
   * \brief This will add new entries to the vector
   * \param add is the number of new entries to the vector
   * \param value is the value that will be entered for the new entries
   */
  void resize(int add, PopRatio value);
  void resize(int add, double* num, double rat);
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
  PopRatio& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const PopRatio& operator [] (int pos) const;
  void Sum(const PopRatioVector* const Number, const ConversionIndex& CI);
protected:
  /**
   * \brief This is the vector of PopRatio values
   */
  PopRatio* v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#ifdef GADGET_INLINE
#include "popratiovector.icc"
#endif

#endif
