#ifndef formulamatrix_h
#define formulamatrix_h

#include "intvector.h"
#include "formulavector.h"

/**
 * \class Formulamatrix
 * \brief This class implements a dynamic vector of Formulavector values
 */
class Formulamatrix {
public:
  /**
   * \brief This is the default Formulamatrix constructor
   */
  Formulamatrix() { nrow = 0; v = 0; };
  /**
   * \brief This is the Formulamatrix constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of each row to be created (ie. the size of the Formulavector to be created for each row)
   * \note The elements of the vector will all be created, and set to zero
   */
  Formulamatrix(int nrow, int ncol);
  /**
   * \brief This is the Formulamatrix constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of each row to be created (ie. the size of the Formulavector to be created for each row)
   * \param initial this is the initial value for all the entries of the vector
   */
  Formulamatrix(int nrow, int ncol, Formula initial);
  /**
   * \brief This is the Formulamatrix constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of the rows to be created (ie. the size of the Formulavector to be created for each row)
   * \note The elements of the vector will all be created, and set to zero
   */
  Formulamatrix(int nrow, const intvector& ncol);
  /**
   * \brief This is the Formulamatrix destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~Formulamatrix();
  Formulamatrix& operator = (const Formulamatrix& formulaM);
  /**
   * \brief This will return the number of columns in row i of the vector
   * \param i this is the row of the vector to have the number of columns counted
   * \return the number of columns in row i of the vector
   * \note This is the number of entries in the Formulavector that is entry i of the Formulamatrix
   */
  int Ncol(int i = 0) const { return v[i]->Size(); };
  /**
   * \brief This will return the number of rows of the vector
   * \return the number of rows of the vector
   */
  int Nrow() const { return nrow; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  Formulavector& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  const Formulavector& operator [] (int pos) const;
  /**
   * \brief This will add new empty entries to the vector
   * \note The new elements of the vector will be created, and set to zero
   * \param add this is the number of new entries to the vector
   * \param length this is the number of entries to the Formulavector that is created
   */
  void AddRows(int add, int length);
  /**
   * \brief This will add new entries to the vector
   * \param add this is the number of new entries to the vector
   * \param length this is the number of entries to the Formulavector that is created
   * \param formula this is the value that will be entered for the new entries
   */
  void AddRows(int add, int length, Formula formula);
  void Inform(Keeper* keeper);
  friend CommentStream& operator >> (CommentStream& infile, Formulamatrix& Fmatrix);
protected:
  /**
   * \brief This is number of rows of the vector
   */
  int nrow;
  /**
   * \brief This is the vector of Formulavector values
   */
  Formulavector** v;
};

#ifdef GADGET_INLINE
#include "formulamatrix.icc"
#endif

#endif
