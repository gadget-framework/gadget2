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
   * \brief This is the LikelihoodPtrVector constructor that creates a copy of an existing LikelihoodPtrVector
   * \param initial is the LikelihoodPtrVector to copy
   */
  LikelihoodPtrVector(const LikelihoodPtrVector& initial);
  /**
   * \brief This is the LikelihoodPtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~LikelihoodPtrVector();
  /**
   * \brief This will add one new entry to the vector
   * \param value is the value that will be entered for the new entry
   */
  void resize(Likelihood* value);
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
  Likelihood*& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  Likelihood* const& operator [] (int pos) const { return v[pos]; };
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

#endif
