#ifndef netdata_h
#define netdata_h

/* classes netDataVariables and netDataResult keep together
 * relevant data to be sent/received in netcommunication */

class netDataVariables {
public:
  int tag;
  int x_id;
  double* x;                     //x[0..n-1], where 0 <= n < numVar
  netDataVariables(int numVar);  //numVar is the number of variables (>0)
  ~netDataVariables();
};

class netDataResult {
public:
  int tag;
  int x_id;
  double result;                 //result of f(x) where x is identified by tag, x_id.
  int who;                       //identifies which process is sending result
  netDataResult();
  ~netDataResult();
};

#endif
