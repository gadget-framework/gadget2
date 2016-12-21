#ifndef popratiovector_h
#define popratiovector_h

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
   * \brief This is the PopRatioVector constructor for a specified size with an initial value
   * \param sz is the size of the vector to be created
   * \param initial is the initial value for all the entries of the vector
   */
  PopRatioVector(int sz, PopRatio initial);
  /**
   * \brief This is the PopRatioVector constructor that creates a copy of an existing PopRatioVector
   * \param initial is the PopRatioVector to copy
   */
  PopRatioVector(const PopRatioVector& initial);
  /**
   * \brief This is the PopRatioVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~PopRatioVector();
  /**
   * \brief This will add one new entry to the vector
   * \param value is the value that will be entered for the new entry
   */
  void resize(PopRatio value);
  /**
   * \brief This will add one new entry to the vector
   * \param num is the value of N for the new PopRatio entry
   * \param rat is the value of R for the new PopRatio entry
   */
  void resize(double* num, double rat);
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
  PopRatio& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const PopRatio& operator [] (int pos) const { return v[pos]; };
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

#endif
