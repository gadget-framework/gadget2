#ifndef predatorptrvector_h
#define predatorptrvector_h

class Predator;

/**
 * \class PredatorPtrVector
 * \brief This class implements a dynamic vector of Predator values
 */
class PredatorPtrVector {
public:
  /**
   * \brief This is the default PredatorPtrVector constructor
   */
  PredatorPtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the PredatorPtrVector constructor that creates a copy of an existing PredatorPtrVector
   * \param initial is the PredatorPtrVector to copy
   */
  PredatorPtrVector(const PredatorPtrVector& initial);
  /**
   * \brief This is the PredatorPtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~PredatorPtrVector();
  /**
   * \brief This will add one new entry to the vector
   * \param value is the value that will be entered for the new entry
   */
  void resize(Predator* value);
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
  Predator*& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  Predator* const& operator [] (int pos) const { return v[pos]; };
protected:
  /**
   * \brief This is the vector of Predator values
   */
  Predator** v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#endif
