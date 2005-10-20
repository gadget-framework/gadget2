#ifndef stockptrvector_h
#define stockptrvector_h

class Stock;

/**
 * \class StockPtrVector
 * \brief This class implements a dynamic vector of Stock values
 */
class StockPtrVector {
public:
  /**
   * \brief This is the default StockPtrVector constructor
   */
  StockPtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the StockPtrVector constructor for a specified size
   * \param sz is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  StockPtrVector(int sz);
  /**
   * \brief This is the StockPtrVector constructor for a specified size with an initial value
   * \param sz is the size of the vector to be created
   * \param initial is the initial value for all the entries of the vector
   */
  StockPtrVector(int sz, Stock* initial);
  /**
   * \brief This is the StockPtrVector constructor that creates a copy of an existing StockPtrVector
   * \param initial is the StockPtrVector to copy
   */
  StockPtrVector(const StockPtrVector& initial);
  /**
   * \brief This is the StockPtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~StockPtrVector();
  /**
   * \brief This will add new entries to the vector
   * \param add is the number of new entries to the vector
   * \param value is the value that will be entered for the new entries
   */
  void resize(int add, Stock* value);
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
   * \brief This will reset the vector
   * \note This will delete every entry from the vector and set the size to zero
   */
  void Reset();
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  Stock*& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  Stock* const& operator [] (int pos) const { return v[pos]; };
  /**
   * \brief This operator will set the vector equal to an existing StockPtrVector
   * \param sv is the StockPtrVector to copy
   */
  StockPtrVector& operator = (const StockPtrVector& sv);
protected:
  /**
   * \brief This is the vector of Stock values
   */
  Stock** v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#endif
