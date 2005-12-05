#ifndef fleetptrvector_h
#define fleetptrvector_h

class Fleet;

/**
 * \class FleetPtrVector
 * \brief This class implements a dynamic vector of Fleet values
 */
class FleetPtrVector {
public:
  /**
   * \brief This is the default FleetPtrVector constructor
   */
  FleetPtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the FleetPtrVector constructor that creates a copy of an existing FleetPtrVector
   * \param initial is the FleetPtrVector to copy
   */
  FleetPtrVector(const FleetPtrVector& initial);
  /**
   * \brief This is the FleetPtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~FleetPtrVector();
  /**
   * \brief This will add one new entry to the vector
   * \param value is the value that will be entered for the new entry
   */
  void resize(Fleet* value);
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
  Fleet*& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  Fleet* const& operator [] (int pos) const { return v[pos]; };
protected:
  /**
   * \brief This is the vector of Fleet values
   */
  Fleet** v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#endif
