#ifndef printerptrvector_h
#define printerptrvector_h

class Printer;

/**
 * \class PrinterPtrVector
 * \brief This class implements a dynamic vector of Printer values
 */
class PrinterPtrVector {
public:
  /**
   * \brief This is the default PrinterPtrVector constructor
   */
  PrinterPtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the PrinterPtrVector constructor that creates a copy of an existing PrinterPtrVector
   * \param initial is the PrinterPtrVector to copy
   */
  PrinterPtrVector(const PrinterPtrVector& initial);
  /**
   * \brief This is the PrinterPtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~PrinterPtrVector();
  /**
   * \brief This will add one new entry to the vector
   * \param value is the value that will be entered for the new entry
   */
  void resize(Printer* value);
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
  Printer*& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  Printer* const& operator [] (int pos) const { return v[pos]; };
protected:
  /**
   * \brief This is the vector of Printer values
   */
  Printer** v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#endif
