#ifndef poppredatorptrvector_h
#define poppredatorptrvector_h

class PopPredator;

/**
 * \class PopPredatorPtrVector
 * \brief This class implements a dynamic vector of PopPredator values
 */
class PopPredatorPtrVector {
public:
  /**
   * \brief This is the default PopPredatorPtrVector constructor
   */
  PopPredatorPtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the PopPredatorPtrVector constructor for a specified size
   * \param sz this is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  PopPredatorPtrVector(int sz);
  /**
   * \brief This is the PopPredatorPtrVector constructor for a specified size with an initial value
   * \param sz this is the size of the vector to be created
   * \param initial this is the initial value for all the entries of the vector
   */
  PopPredatorPtrVector(int sz, PopPredator* initial);
  /**
   * \brief This is the PopPredatorPtrVector constructor that create a copy of an existing PopPredatorPtrVector
   * \param initial this is the PopPredatorPtrVector to copy
   */
  PopPredatorPtrVector(const PopPredatorPtrVector& initial);
  /**
   * \brief This is the PopPredatorPtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~PopPredatorPtrVector();
  /**
   * \brief This will add new entries to the vector
   * \param add this is the number of new entries to the vector
   * \param value this is the value that will be entered for the new entries
   */
  void resize(int add, PopPredator* value);
  /**
   * \brief This will add new empty entries to the vector
   * \param add this is the number of new entries to the vector
   * \note The new elements of the vector will be created, and set to zero
   */
  void resize(int add);
  /**
   * \brief This will delete an entry from the vector
   * \param pos this is the element of the vector to be deleted
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
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  PopPredator*& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  PopPredator* const& operator [] (int pos) const;
protected:
  /**
   * \brief This is the vector of PopPredator values
   */
  PopPredator** v;
  /**
   * \brief This is size of the vector
   */
  int size;
};

#ifdef GADGET_INLINE
#include "poppredatorptrvector.icc"
#endif

#endif
