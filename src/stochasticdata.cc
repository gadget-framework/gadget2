#include "stochasticdata.h"
#include "readfunc.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"



StochasticData::StochasticData(const char* const filename) {
  netrun = 0;
  readInfo = new InitialInputFile(filename);
  readInfo->readFromFile();
  if (readInfo->isRepeatedValues()) {
    if (readInfo->numSwitches() > 0)
      readInfo->getSwitches(switches);
    readInfo->getValues(values);

  } else
    readInfo->getVectors(switches, values, lowerbound, upperbound, optimise);

  if ((switches.Size() > 0) && (switches.Size() != values.Size()))
    handle.logMessage(LOGFAIL, "Error in stochasticdata - failed to read values");
}

StochasticData::StochasticData(const char* const filename, int p) {
  //netrun = 1;
  netrun = p;
  //readInfo = NULL;
  // Prufa Hér
  readInfo = new InitialInputFile(filename);
  readInfo->readFromFile();
  if (readInfo->isRepeatedValues()) {
    if (readInfo->numSwitches() > 0)
      readInfo->getSwitches(switches);
    readInfo->getValues(values);

  } else
    readInfo->getVectors(switches, values, lowerbound, upperbound, optimise);

  if ((switches.Size() > 0) && (switches.Size() != values.Size()))
    handle.logMessage(LOGFAIL, "Error in stochasticdata - failed to read values");
  // Klárast hér.
#ifdef GADGET_NETWORK
  slave = new SlaveCommunication();
  getdata = 0;
  dataFromMaster = NULL;
  this->readFromNetwork();
#endif
}


StochasticData::StochasticData() {
  netrun = 1;
  readInfo = NULL;
#ifdef GADGET_NETWORK
  slave = new SlaveCommunication();
  getdata = 0;
  dataFromMaster = NULL;
  this->readFromNetwork();
#endif
}

StochasticData::~StochasticData() {
  if (netrun) {
#ifdef GADGET_NETWORK
    if (dataFromMaster != NULL)
      delete[] dataFromMaster;
	// Breytti hér
	//-----------------------------------------------------------------------------------------
    if (!getdata)
      slave->stopNetCommunication();
    delete slave;
#endif
  } else
    delete readInfo;
}

void StochasticData::readNextLine() {
  if ((netrun) || (!readInfo->isRepeatedValues()))
    return;
  readInfo->readNextLine();
  values.Reset();
  readInfo->getValues(values);
}

int StochasticData::getOptFlag(int i) const {
  if (netrun)
    return 0;
  return optimise[i];
}

int StochasticData::isOptGiven() const {
  if (netrun)
    return 0;
  return (optimise.Size() > 0);
}

#ifdef GADGET_NETWORK
void StochasticData::readFromNetwork() {
  int i, numParam;

  //Receive first data from master
  numParam = slave->startNetCommunication();
  if (numParam > 0) {
    //successfully started netcommunication
    dataFromMaster = new double[numParam];
    if (values.Size() == 0) {
      values.resize(numParam, 0.0);
      lowerbound.resize(numParam, 0.0);
      upperbound.resize(numParam, 0.0);
    }
    //try to receive switches from master
    getdata = slave->receiveFromMaster();
    if (getdata) {
      if (slave->receivedString()) {
        for (i = 0; i < numParam; i++) {
          Parameter sw(slave->getString(i));
          switches.resize(sw);
        }
      } else
        getdata = 0;
    }
    //try to receive lowerbounds from master
    getdata = slave->receiveFromMaster();
    if (getdata) {
      if (slave->receivedBounds()) {
        slave->getBound(dataFromMaster);
        for (i = 0; i < numParam; i++)
          lowerbound[i] = dataFromMaster[i];
      } else
        getdata = 0;
    }
    //try to receive upperbounds from master
    getdata = slave->receiveFromMaster();
    if (getdata) {
      if (slave->receivedBounds()) {
        slave->getBound(dataFromMaster);
        for (i = 0; i < numParam; i++)
          upperbound[i] = dataFromMaster[i];
      } else
        getdata = 0;
    }
    //try to receive vector value from master
    getdata = slave->receiveFromMaster();
    if (getdata) {
      if (slave->receivedVector()) {
        slave->getVector(dataFromMaster);
        for (i = 0; i < values.Size(); i++)
          values[i] = dataFromMaster[i];
      } else
        getdata = 0;
    }
  } else
    getdata = 0;

  if (getdata) {
    if (switches.Size() != values.Size())
      handle.logMessage(LOGFAIL, "Error in stochasticdata - failed to read values");

    for (i = 0; i < values.Size(); i++) {
      if ((lowerbound[i] > values[i]) || (upperbound[i] < values[i]))
        handle.logMessage(LOGFAIL, "Error in stochasticdata - initial value outside bounds for parameter", switches[i].getName());
      if (upperbound[i] < lowerbound[i])
        handle.logMessage(LOGFAIL, "Error in stochasticdata - upper bound lower than lower bound for parameter", switches[i].getName());
      if ((lowerbound[i] < 0.0) && (upperbound[i] > 0.0))
        handle.logMessage(LOGWARN, "Warning in stochasticdata - bounds span zero for parameter", switches[i].getName());
    }
  }
}

void StochasticData::readNextLineFromNetwork() {
  int i;
  getdata = slave->receiveFromMaster();
  if (getdata) {
    if (slave->receivedVector()) {
      slave->getVector(dataFromMaster);
      for (i = 0; i < values.Size(); i++)
        values[i] = dataFromMaster[i];
    } else
      getdata = 0;
  }
}

void StochasticData::sendDataToNetwork(double score) {
  int info = slave->sendToMaster(score);
  if (info < 0) {
    slave->stopNetCommunication();
    handle.logMessage(LOGFAIL, "Error in stochasticdata - failed to send data to PVM master");
  }
}
#endif
