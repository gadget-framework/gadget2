#ifndef stochasticdata_h
#define stochasticdata_h

#include "doublevector.h"
#include "intvector.h"
#include "parameter.h"
#include "charptrvector.h"
#include "commentstream.h"
#include "initialinputfile.h"
#include "gadget.h"

#ifdef GADGET_NETWORK
#include "pvm3.h"
#include "slavecommunication.h"
#endif

class StochasticData {
public:
  StochasticData(const char* const filename);
  StochasticData(int netrun);
  virtual ~StochasticData();
  int DataIsLeft();
  int NoVariables() const;
  int Error() const;
  int DataFromFile();
  void ReadDataFromNextLine();
  double Values(int i) const { return(values[i]); };
  double Lower(int i) const { return(lowerbound[i]); };
  double Upper(int i) const { return(upperbound[i]); };
  int Optimize(int i) const {
    if (NETRUN == 1)
      return 0;
    else
      return(optimize[i]);
  };
  Parameter Switches(int i) const { return(switches[i]); };
  int SwitchesGiven() const { return(switches.Size() > 0 ? 1 : 0); };
  int OptGiven() const {
    if (NETRUN == 1)
      return 0;
    else
      return(optimize.Size() > 0 ? 1 : 0);
  };
  #ifdef GADGET_NETWORK
    void ReadFromNetwork();
    void SendDataToMaster(double funcValue);
    void ReadNextLineFromNet();
    int getDataFromNet();
  #endif
protected:
  InitialInputFile* readInfo;
  int error;
  doublevector values;
  doublevector lowerbound;
  doublevector upperbound;
  Parametervector switches;
  intvector optimize;
  int NETRUN;
  #ifdef GADGET_NETWORK
    slaveCommunication* slave;   //Takes care of netcommunication, sending/receiving data
    int numberOfParam;           //number of parameters in vector from master
    double* dataFromMaster;      //dataFromMaster[0..numberOfParam-1] is received from master
    int GETDATA;
  #endif
};

#endif
