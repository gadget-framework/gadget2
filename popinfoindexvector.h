#ifndef popinfoindexvector_h
#define popinfoindexvector_h

#include "popinfo.h"
#include "conversionindex.h"

/**
 * \class PopInfoIndexVector
 * \brief This class implements a dynamic vector of PopInfo values, indexed from minpos not 0
 */
class PopInfoIndexVector {
public:
  /**
   * \brief This is the default PopInfoIndexVector constructor
   */
  PopInfoIndexVector() { minpos = 0; size = 0; v = 0; };
  /**
   * \brief This is the PopInfoIndexVector constructor for a specified size
   * \param sz is the size of the vector to be created
   * \param minpos is the index for the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  PopInfoIndexVector(int sz, int minpos);
  /**
   * \brief This is the PopInfoIndexVector constructor for a specified size with an initial value
   * \param sz is the size of the vector to be created
   * \param minpos is the index for the vector to be created
   * \param initial is the initial value for all the entries of the vector
   */
  PopInfoIndexVector(int sz, int minpos, PopInfo initial);
  /**
   * \brief This is the PopInfoIndexVector constructor that creates a copy of an existing PopInfoIndexVector
   * \param initial is the PopInfoIndexVector to copy
   */
  PopInfoIndexVector(const PopInfoIndexVector& initial);
  /**
   * \brief This is the PopInfoIndexVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~PopInfoIndexVector();
  /**
   * \brief This will add new entries to the vector
   * \param addsize is the number of new entries to the vector
   * \param newminpos is the index for the vector to be created
   * \param value is the value that will be entered for the new entries
   */
  void resize(int addsize, int newminpos, PopInfo value);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  PopInfo& operator [] (int pos) { return v[pos - minpos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const PopInfo& operator [] (int pos) const { return v[pos - minpos]; };
  /**
   * \brief This will return the index of the vector
   * \return the index of the vector
   */
  int minCol() const { return minpos; };
  /**
   * \brief This will return the length of the vector
   * \return the length of the vector
   */
  int maxCol() const { return minpos + size; };
  /**
   * \brief This will return the size of the vector
   * \return the size of the vector
   */
  int Size() const { return size; };
  /**
   * \brief This function will add a PopInfoIndexVector to the current vector
   * \param Addition is the PopInfoIndexVector that will be added to the current vector
   * \param CI is the ConversionIndex that will convert between the length groups of the 2 vectors
   * \param ratio is a multiplicative constant applied to each entry (default value 1.0)
   */
  void Add(const PopInfoIndexVector& Addition, const ConversionIndex& CI, double ratio = 1.0);
  /**
   * \brief This function will add a PopInfoIndexVector to the current vector
   * \param Addition is the PopInfoIndexVector that will be added to the current vector
   * \param CI is the ConversionIndex that will convert between the length groups of the 2 vectors
   * \param Ratio is a DoubleVector of multiplicative constants to be applied to entries individually
   * \param ratio is a multiplicative constant applied to each entry (default value 1.0)
   */
  void Add(const PopInfoIndexVector& Addition, const ConversionIndex& CI, const DoubleVector& Ratio, double ratio = 1.0);
protected:
  /**
   * \brief This is the index for the vector
   */
  int minpos;
  /**
   * \brief This is the size of the vector
   */
  int size;
  /**
   * \brief This is the indexed vector of PopInfo values
   */
  PopInfo* v;
};

#endif
