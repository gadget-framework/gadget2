#ifndef preystdinfobylength_h
#define preystdinfobylength_h

#include "abstrpreystdinfobylength.h"

class PreyStdInfoByLength;

class PreyStdInfoByLength : public AbstrPreyStdInfoByLength {
public:
  PreyStdInfoByLength(const Prey* prey, const intvector& areas);
  virtual ~PreyStdInfoByLength();
  virtual void Sum(const TimeClass* const TimeInfo, int area);
private:
  const Prey* prey;
};

#endif
