#ifndef stochasticdata_h
#define stochasticdata_h

#include "doublevector.h"
#include "intvector.h"
#include "parameter.h"
#include "parametervector.h"
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
  /**
   * \brief This is the default StochasticData constructor
   */
  StochasticData();
  /**
   * \brief This is the StochasticData constructor specifying a file to read the data from
   * \param filename is the name of the file to read the data from
   */
  StochasticData(const char* const filename);
  /**
   * \brief This is the default StochasticData destructor
   */
  virtual ~StochasticData();
  int DataIsLeft();
  int numVariables() const;
  int DataFromFile();
  void readDataFromNextLine();
  int SwitchesGiven() const;
  int OptGiven() const;
  double Values(int i) const { return values[i]; };
  double Lower(int i) const { return lowerbound[i]; };
  double Upper(int i) const { return upperbound[i]; };
  int Optimize(int i) const;
  Parameter Switches(int i) const { return switches[i]; };
#ifdef GADGET_NETWORK
  void readFromNetwork();
  void sendDataToMaster(double funcValue);
  void readNextLineFromNet();
  int getDataFromNet() { return getdata; };
#endif
protected:
  /**
   * \brief This is the InitialInputFile used to read from file the initial values and bounds of the parameters for the current model
   */
  InitialInputFile* readInfo;
  /**
   * \brief This is the ParameterVector used to store the parameters read from file
   */
  ParameterVector switches;
  /**
   * \brief This is the DoubleVector used to store the values of the parameters read from file
   */
  DoubleVector values;
  /**
   * \brief This is the DoubleVector used to store the lower bounds of the parameters read from file
   */
  DoubleVector lowerbound;
  /**
   * \brief This is the DoubleVector used to store the upper bounds of the parameters read from file
   */
  DoubleVector upperbound;
  /**
   * \brief This is the IntVector used to store the flag to denote whether to optimise the parameters read from file
   */
  IntVector optimize;
  /**
   * \brief This is the flag to denote whether gadget is running in network mode or not
   */
  int netrun;
#ifdef GADGET_NETWORK
  /**
   * \brief This is the SlaveCommunication that handles the network communication to send and received data
   */
  SlaveCommunication* slave;
  /**
   * \brief This is the number of parameters received from the network communication
   */
  int numParam;
  /**
   * \brief This is the vector of parameters received from the network communication
   */
  double* dataFromMaster;
  /**
   * \brief This is the flag to denote whether network communication has been succesful or not
   */
  int getdata;
#endif
};

#endif
