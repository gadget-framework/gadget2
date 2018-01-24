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
#include "mpi.h"
#include "slavecommunication.h"
#endif

/**
 * \class StochasticData
 * \brief This is the class used to get values for the variables used in the model simulation.  These values are either read from an InitialInputFile, or received from the network if the simulation is part of a parallel optimisation, using paramin
 */
class StochasticData {
public:
  /**
   * \brief This is the default StochasticData constructor
   */
  StochasticData();
  /**
   * \brief This is the StochasticData constructor specifying a file to read the data from
   * \param filename is the name of the file to read the data from
   * \param p is the number of the node
   */
  StochasticData(const char* const filename, int p);
  /**
   * \brief This is the StochasticData constructor specifying a file to read the data from
   * \param filename is the name of the file to read the data from
   */
  StochasticData(const char* const filename);
  /**
   * \brief This is the default StochasticData destructor
   */
  virtual ~StochasticData();
  /**
   * \brief This function will read the next line of data from the input file
   * \note This function is only called if the input file has data using the 'repeated values' format
   */
  void readNextLine();
  /**
   * \brief This function will check to see if there is more data to read in the input file
   * \return 1 if there is more data to read from the input file, 0 otherwise
   */
  int isDataLeft() { return readInfo->isDataLeft(); };
  /**
   * \brief This function will return the number of variables read from the input file
   * \return number of variables
   */
  int numVariables() const { return values.Size(); };
  /**
   * \brief This function will return the number of switches read from the input file
   * \return number of switches
   */
  int numSwitches() const { return switches.Size(); };
  /**
   * \brief This function will check to see if optimising flags have been specified in the input file
   * \return 1 if the optimising flags have been specified, 0 otherwise
   */
  int isOptGiven() const;
  /**
   * \brief This function will return the optimising flag for a switch in the input file
   * \param i is the index for the switch
   * \return optimise flag for the switch
   */
  int getOptFlag(int i) const;
  /**
   * \brief This function will return the value of a switch in the input file
   * \param i is the index for the switch
   * \return value of the switch
   */
  double getValue(int i) const { return values[i]; };
  /**
   * \brief This function will return the lower bound for a switch in the input file
   * \param i is the index for the switch
   * \return lower bound for the switch
   */
  double getLowerBound(int i) const { return lowerbound[i]; };
  /**
   * \brief This function will return the upper bound for a switch in the input file
   * \param i is the index for the switch
   * \return upper bound for the switch
   */
  double getUpperBound(int i) const { return upperbound[i]; };
  /**
   * \brief This function will return the paramter name for a switch in the input file
   * \param i is the index for the switch
   * \return parameter name for the switch
   */
  Parameter getSwitch(int i) const { return switches[i]; };
#ifdef GADGET_NETWORK
  /**
   * \brief This function will read all the initial data that is sent to the model from the PVM network
   */
  void readFromNetwork();
  /**
   * \brief This function will send data to the PVM network from the model
   * \param score is the likelihood score obtained from the current run, to be sent to the PVM master
   */
  void sendDataToNetwork(double score);
  /**
   * \brief This function will read updated data that is sent to the model from the PVM network
   */
  void readNextLineFromNetwork();
  /**
   * \brief This function will check to ensure that network communication has been successful
   * \return getdata, a flag to denote whether network communication has been succesful or not
   */
  int getDataFromNetwork() { return getdata; };
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
  IntVector optimise;
  /**
   * \brief This is the flag to denote whether gadget is running in network mode or not
   */
  int netrun;
#ifdef GADGET_NETWORK
  /**
   * \brief This is the SlaveCommunication that handles the network communication to send and receive data
   */
  SlaveCommunication* slave;
  /**
   * \brief This is the vector of parameters received from the network communication
   */
  double* dataFromMaster;
  /**
   * \brief This is the flag used to denote whether network communication has been succesful or not
   */
  int getdata;
#endif
};

#endif
