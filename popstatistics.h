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
  double MeanLength() const { return meanlength; };
  double MeanWeight() const { return meanweight; };
  double TotalNumber() const { return totalnumber; };
  double StdDevOfLength() const { return stddevoflength; };
protected:
  void CalcStatistics(const PopInfoVector& pop,
    const LengthGroupDivision* const lgrpdiv, int calcweight);
  double meanlength;
  double meanweight;
  double totalnumber;
  double stddevoflength;
};

#endif
