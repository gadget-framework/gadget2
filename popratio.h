#ifndef popratio_h
#define popratio_h

//Class PopRatio stores number and ratio.
class PopRatio {
public:
  double* N;
  double R;
  PopRatio();
  PopRatio& operator += (const PopRatio& a);
  PopRatio& operator = (const PopRatio& a);
  void operator -= (double a);
  void operator *= (double a);
};

#endif
