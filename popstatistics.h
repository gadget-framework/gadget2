#ifndef popstatistics_h
#define popstatistics_h

#include "popinfovector.h"
#include "popinfoindexvector.h"

class PopStatistics;
class LengthGroupDivision;

class PopStatistics {
public:
  PopStatistics(const popinfoindexvector& pop, const LengthGroupDivision* const lgrpdiv);
  PopStatistics(const popinfovector& pop, const LengthGroupDivision* const lgrpdiv);
  ~PopStatistics();
  double MeanLength() const { return meanlength; };
  double MeanWeight() const { return meanweight; };
  double TotalNumber() const { return totalnumber; };
  double StdDevOfLength() const { return stddevOflength; };
protected:
  void CalcStatistics(const popinfovector& pop, const LengthGroupDivision* const lgrpdiv);
  double meanlength;
  double meanweight;
  double totalnumber;
  double stddevOflength;
};

#endif
