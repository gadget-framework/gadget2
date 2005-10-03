#ifndef migrationareaptrvector_h
#define migrationareaptrvector_h

class MigrationArea;

/**
 * \class MigrationAreaPtrVector
 * \brief This class implements a dynamic vector of MigrationArea values
 */
class MigrationAreaPtrVector {
public:
  /**
   * \brief This is the default MigrationAreaPtrVector constructor
   */
  MigrationAreaPtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the MigrationAreaPtrVector constructor for a specified size
   * \param sz is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  MigrationAreaPtrVector(int sz);
  /**
   * \brief This is the MigrationAreaPtrVector constructor for a specified size with an initial value
   * \param sz is the size of the vector to be created
   * \param initial is the initial value for all the entries of the vector
   */
  MigrationAreaPtrVector(int sz, MigrationArea* initial);
  /**
   * \brief This is the MigrationAreaPtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~MigrationAreaPtrVector();
  /**
   * \brief This will add new entries to the vector
   * \param add is the number of new entries to the vector
   * \param value is the value that will be entered for the new entries
   */
  void resize(int add, MigrationArea* value);
  /**
   * \brief This will add new empty entries to the vector
   * \param add is the number of new entries to the vector
   * \note The new elements of the vector will be created, and set to zero
   */
  void resize(int add);
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
  MigrationArea*& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  MigrationArea* const& operator [] (int pos) const { return v[pos]; };
protected:
  /**
   * \brief This is the vector of MigrationArea values
   */
  MigrationArea** v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#endif
