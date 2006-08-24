#ifndef lengthgroup_h
#define lengthgroup_h

#include "doublevector.h"

/**
 * \class LengthGroupDivision
 * \brief This is the class used to store information about the length groups of the modelled stock population
 */
class LengthGroupDivision {
public:
  /**
   * \brief This is the LengthGroupDivision constructor with evenly spaced length groups
   * \param minlength is the minimum length of the smallest length group
   * \param maxlength is the maximum length of the biggest length group
   * \param dl is the step length for each length group
   */
  LengthGroupDivision(double minlength, double maxlength, double dl);
  /**
   * \brief This is the LengthGroupDivision constructor with length groups that are possibly not evenly spaced
   * \param vec is the DoubleVector containing the end point of each length group
   */
  LengthGroupDivision(const DoubleVector& vec);
  /**
   * \brief This is the LengthGroupDivision constructor that creates a copy of an existing LengthGroupDivision
   * \param lgrpdiv is the LengthGroupDivision to copy
   */
  LengthGroupDivision(const LengthGroupDivision& lgrpdiv);
  /**
   * \brief This is the default LengthGroupDivision destructor
   */
  ~LengthGroupDivision() {};
  /**
   * \brief This function will return the mean length of a specified length group
   * \param i is the identifier of the length group
   * \return the mean length of the length group
   */
  double meanLength(int i) const;
  /**
   * \brief This function will return the minimum length of a specified length group
   * \param i is the identifier of the length group
   * \return the minimum length of the length group
   */
  double minLength(int i) const;
  /**
   * \brief This function will return the maximum length of a specified length group
   * \param i is the identifier of the length group
   * \return the maximum length of the length group
   */
  double maxLength(int i) const;
  /**
   * \brief This function will return the minimum length of the smallest length group
   * \return the minimum length
   */
  double minLength() const { return minlen; };
  /**
   * \brief This function will return the maximum length of the biggest length group
   * \return the maximum length
   */
  double maxLength() const { return maxlen; };
  /**
   * \brief This function will return the step length of the length groups
   * \return the step length
   */
  double dl() const { return Dl; };
  /**
   * \brief This function will return the total number of the length groups
   * \return the number of length groups
   */
  int numLengthGroups() const { return size; };
  /**
   * \brief This function will return the identifier of a length group containing a specified length
   * \param len is the length to find in the length group
   * \return the identifier of the length group
   */
  int numLengthGroup(double len) const;
  /**
   * \brief This function will combine a second LengthGroupDivision with the current LengthGroupDivision
   * \return 1 if the LengthGroupDivision has been combined sucessfully, 0 otherwise
   */
  int Combine(const LengthGroupDivision* const addition);
  /**
   * \brief This function will return the flag denoting whether an error has occured or not
   * \return error
   */
  int Error() const { return error; }
  /**
   * \brief This function will print an error message about the LengthGroupDivision
   */
  void printError() const;
  /**
   * \brief This function will print the LengthGroupDivision information
   * \param outfile is the ofstream that the information gets sent to
   */
  void Print(ofstream& outfile) const;
protected:
  /**
   * \brief This is the flag to denote whether an error has occured or not
   */
  int error;
  /**
   * \brief This is the size of the length group
   */
  int size;
  /**
   * \brief This is the step length for each length group
   */
  double Dl;
  /**
   * \brief This is the minimum length of the smallest length group
   */
  double minlen;
  /**
   * \brief This is the maximum length of the biggest length group
   */
  double maxlen;
  /**
   * \brief This is the DoubleVector of the mean lengths for each length group
   */
  DoubleVector meanlength;
  /**
   * \brief This is the DoubleVector of the minimum lengths for each length group
   */
  DoubleVector minlength;
};

/**
 * \brief This is the function that will check whether one LengthGroupDivision has length groups that are finer than a second LengthGroupDivision, and print an error if this is not the case
 * \param finer is the LengthGroupDivision that should be on a finer scale
 * \param coarser is the LengthGroupDivision that should be on a coarser scale
 * \return 1 if the structure of the LengthGroupDivision objects are compatible, 0 otherwise
 */
extern int checkLengthGroupStructure(const LengthGroupDivision* finer,
  const LengthGroupDivision* coarser);

#endif
