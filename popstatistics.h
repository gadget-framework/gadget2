#ifndef popstatistics_h
#define popstatistics_h

#include "lengthgroup.h"
#include "popinfovector.h"
#include "popinfoindexvector.h"

class PopStatistics;

class PopStatistics {
public:
  PopStatistics(const PopInfoIndexVector& pop,
    const LengthGroupDivision* const lgrpdiv, int calcweight = 0);
  PopStatistics(const PopInfoVector& pop,
    const LengthGroupDivision* const lgrpdiv, int calcweight = 0);
  ~PopStatistics() {};
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
   * \return stddevoflength
   */
  double sdevLength() const { return sdevlength; };
protected:
  void calcStatistics(const PopInfoVector& pop,
    const LengthGroupDivision* const lgrpdiv, int calcweight);
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
