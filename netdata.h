#ifndef netdata_h
#define netdata_h

/* classes NetDataVariables and NetDataResult keep together
 * relevant data to be sent/received in netcommunication */

class NetDataVariables {
public:
  int tag;
  int x_id;
  double* x;                     //x[0..n-1], where 0 <= n < numVar
  NetDataVariables(int numVar);  //numVar is the number of variables (>0)
  ~NetDataVariables();
};

class NetDataResult {
public:
  int tag;
  int x_id;
  double result;                 //result of f(x) where x is identified by tag, x_id.
  int who;                       //identifies which process is sending result
  NetDataResult();
  ~NetDataResult();
};

#endif
