#ifndef popstatistics_h
#define popstatistics_h

#include "lengthgroup.h"
#include "popinfovector.h"
#include "popinfoindexvector.h"
#include "agebandmatrixptrvector.h"

/**
 * \class PopStatistics
 * \brief This is the class used to calculate some standard statistics about a given population
 */
class PopStatistics {
public:
  /**
   * \brief This is the default PopStatistics constructor
   */
  PopStatistics() { meanlength = 0.0; meanweight = 0.0; totalnumber = 0.0; sdevlength = 0.0; };
  /**
   * \brief This is the default PopStatistics destructor
   */
  ~PopStatistics() {};
  /**
   * \brief This function will calculate the standard statistics for a given population
   * \param pop is the PopInfoIndexVector that specifies the population
   * \param lgrpdiv is the LengthGroupDivision of the population
   * \param calcweight is the flag to denote whether the mean weight should be calculated or not (default value 1)
   */
  void calcStatistics(const PopInfoIndexVector& pop, const LengthGroupDivision* const lgrpdiv, int calcweight = 1);
/**
   * \brief This function will calculate the standard statistics for a given population
   * \param agelenum is the  the AgeBandMatrix that specifies the catch
   * \param lengr is the Length group vector for which mean weight should be calculated
   */
  void calcStatistics(const AgeBandMatrix& agelenum, int lengr);
  /**
   * \brief This will return the mean length of the population
   * \return meanlength
   */
  double meanLength() const { return meanlength; };
  /**
   * \brief This will return the mean weight of the population
   * \return meanweight
   */
  double meanWeight() const { return meanweight; };
  /**
   * \brief This will return the total number in the population
   * \return totalnumber
   */
  double totalNumber() const { return totalnumber; };
  /**
   * \brief This will return the standard deviation of the length of the population
   * \return sdevlength
   */
  double sdevLength() const { return sdevlength; };
private:
  /**
   * \brief This is the mean length of the stock in the population
   */
  double meanlength;
  /**
   * \brief This is the mean weight of the stock in the population
   */
  double meanweight;
  /**
   * \brief This is the total number of stock in the population
   */
  double totalnumber;
  /**
   * \brief This is the standard deviation of the length of the stock in the population
   */
  double sdevlength;
};

#endif
