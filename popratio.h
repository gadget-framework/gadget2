#ifndef popratio_h
#define popratio_h

//Class popratio stores number and ratio.
class popratio {
public:
  double* N;
  double R;
  popratio();
  popratio& operator += (const popratio& a);
  popratio& operator = (const popratio& a);
  void operator -= (double a);
  void operator *= (double a);
};

#endif
