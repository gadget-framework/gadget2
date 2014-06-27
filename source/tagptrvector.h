#ifndef tagptrvector_h
#define tagptrvector_h

#include "tags.h"

class Tags;

/**
 * \class TagPtrVector
 * \brief This class implements a dynamic vector of Tags values
 */
class TagPtrVector {
public:
  /**
   * \brief This is the default TagPtrVector constructor
   */
  TagPtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the TagPtrVector constructor that creates a copy of an existing TagPtrVector
   * \param initial is the TagPtrVector to copy
   */
  TagPtrVector(const TagPtrVector& initial);
  /**
   * \brief This is the TagPtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~TagPtrVector();
  /**
   * \brief This will add one new entry to the vector
   * \param value is the value that will be entered for the new entry
   */
  void resize(Tags* value);
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
  Tags*& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  Tags* const& operator [] (int pos) const { return v[pos]; };
  /**
   * \brief This function will update all the elements of the vector for the current timestep
   * \param TimeInfo is the TimeClass for the current model
   */
  void updateTags(const TimeClass* const TimeInfo);
  /**
   * \brief This function will delete all the tagging experiments from the vector that have expired for the current timestep
   * \param TimeInfo is the TimeClass for the current model
   */
  void deleteTags(const TimeClass* const TimeInfo);
  /**
   * \brief This function will delete all the tagging experiments from the vector
   */
  void deleteAllTags();
  /**
   * \brief This function will delete all the elements of the vector
   */
  void deleteAll();
protected:
  /**
   * \brief This is the vector of Tags values
   */
  Tags** v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#endif
