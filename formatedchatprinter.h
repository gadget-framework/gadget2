#ifndef formatedchatprinter_h
#define formatedchatprinter_h

#include "commentstream.h"
#include "printer.h"
#include "mortpredlength.h"
#include "gadget.h"

class FormatedCHatPrinter;
class TimeClass;
class LengthGroupDivision;
class StockAggregator;
class AreaClass;

class FormatedCHatPrinter : public Printer {
public:
  FormatedCHatPrinter(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual ~FormatedCHatPrinter();
  void SetFleet(FleetPtrVector& fleetvec);
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  IntVector areas;
  CharPtrVector areaindex;
  CharPtrVector fleetnames;
  ofstream outfile;
  StockPtrVector stocks;
  const AreaClass* Area;
  FleetPtrVector fleets;
};

#endif
