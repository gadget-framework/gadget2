#ifndef predpreystdprinter_h
#define predpreystdprinter_h

#include "printer.h"
#include "commentstream.h"

class PredStdPrinter;
class TimeClass;
class AreaClass;
class PopPredator;
class Prey;
class PredStdInfoByLength;
class PredStdInfo;
class AbstrPredStdInfo;

class PredPreyStdPrinter : public Printer {
public:
  PredPreyStdPrinter(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo);
  virtual ~PredPreyStdPrinter();
  virtual void SetStocksAndPredAndPrey(const StockPtrVector& stockvec,
    const PopPredatorPtrVector& predvec, const PreyPtrVector& preyvec);
protected:
  //AJ 07.06.00 Changing name of function
  virtual void SetPopPredAndPrey(const PopPredator* predator,
    const Prey* prey, int IsStockPredator, int IsStockPrey) = 0;
  char* predname;
  char* preyname;
  ofstream outfile;
  IntVector areas;
  IntVector outerareas;
};

class PredPreyStdLengthPrinter : public PredPreyStdPrinter {
public:
  PredPreyStdLengthPrinter(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo);
  ~PredPreyStdLengthPrinter();
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  virtual void SetPopPredAndPrey(const PopPredator* predator, const Prey* prey,
    int IsStockPredator, int IsStockPrey);
private:
  PredStdInfoByLength* predinfo;
  const PopPredator* predator;
  const Prey* prey;
};

class PredPreyStdAgePrinter : public PredPreyStdPrinter {
public:
  PredPreyStdAgePrinter(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo);
  ~PredPreyStdAgePrinter();
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  virtual void SetPopPredAndPrey(const PopPredator* predator, const Prey* prey,
    int IsStockPredator, int IsStockPrey);
private:
  AbstrPredStdInfo* predinfo;
  const PopPredator* predator;
  const Prey* prey;
};

#endif
