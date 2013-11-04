#ifndef modelvariablevector_h
#define modelvariablevector_h

#include "modelvariable.h"

/**
 * \class ModelVariableVector
 * \brief This class implements a dynamic vector of ModelVariable values
 */
class ModelVariableVector {
public:
  /**
   * \brief This is the default ModelVariableVector constructor
   */
  ModelVariableVector() { size = 0; v = 0; };
  /**
   * \brief This is the ModelVariableVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~ModelVariableVector();
  /**
   * \brief This will add new empty entries to the vector
   * \param addsize is the number of new entries to the vector
   * \note All elements of the vector will be created, and set to zero
   */
  void setsize(int addsize);
  /**
   * \brief This will add new empty entries to the vector and inform Keeper of the change
   * \param addsize is the number of new entries to the vector
   * \param keeper is the Keeper for the current model
   * \note The new elements of the vector will be created, and set to zero
   */
  void resize(int addsize, Keeper* const keeper);
  /**
   * \brief This will add one new entry to the vector and inform Keeper of the change
   * \param mvar is the new ModelVariable entry to the vector
   * \param keeper is the Keeper for the current model
   */
  void resize(const ModelVariable& mvar, Keeper* const keeper);
  /**
   * \brief This will return the size of the vector
   * \return the size of the vector
   */
  int Size() const { return size; };
  /**
   * \brief This function will read the value of the ModelVariable entries from file
   * \param infile is the CommentStream to read the parameters from
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  void read(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  ModelVariable& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const ModelVariable& operator [] (int pos) const { return v[pos]; };
  /**
   * \brief This function will check to see if the ModelVariable values have changed
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the values have changed, 0 otherwise
   */
  int didChange(const TimeClass* const TimeInfo) const;
  /**
   * \brief This function will update the ModelVariable values
   * \param TimeInfo is the TimeClass for the current model
   */
  void Update(const TimeClass* const TimeInfo);
protected:
  /**
   * \brief This is the size of the vector
   */
  int size;
  /**
   * \brief This is the vector of ModelVariable values
   */
  ModelVariable* v;
};

#endif
