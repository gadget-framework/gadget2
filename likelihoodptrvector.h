#ifndef likelihoodptrvector_h
#define likelihoodptrvector_h

class Likelihood;

/**
 * \class LikelihoodPtrVector
 * \brief This class implements a dynamic vector of Likelihood values
 */
class LikelihoodPtrVector {
public:
  /**
   * \brief This is the default LikelihoodPtrVector constructor
   */
  LikelihoodPtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the LikelihoodPtrVector constructor for a specified size
   * \param sz is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  LikelihoodPtrVector(int sz);
  /**
   * \brief This is the LikelihoodPtrVector constructor for a specified size with an initial value
   * \param sz is the size of the vector to be created
   * \param initial is the initial value for all the entries of the vector
   */
  LikelihoodPtrVector(int sz, Likelihood* initial);
  /**
   * \brief This is the LikelihoodPtrVector constructor that create a copy of an existing LikelihoodPtrVector
   * \param initial is the LikelihoodPtrVector to copy
   */
  LikelihoodPtrVector(const LikelihoodPtrVector& initial);
  /**
   * \brief This is the LikelihoodPtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~LikelihoodPtrVector();
  /**
   * \brief This will add new entries to the vector
   * \param add is the number of new entries to the vector
   * \param value is the value that will be entered for the new entries
   */
  void resize(int add, Likelihood* value);
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
  Likelihood*& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  Likelihood* const& operator [] (int pos) const;
protected:
  /**
   * \brief This is the vector of Likelihood values
   */
  Likelihood** v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#ifdef GADGET_INLINE
#include "likelihoodptrvector.icc"
#endif

#endif
