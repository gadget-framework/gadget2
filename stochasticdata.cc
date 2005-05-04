#include "stochasticdata.h"
#include "readfunc.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

StochasticData::StochasticData(const char* const filename) {
  netrun = 0;
  readInfo = new InitialInputFile(filename);
  readInfo->readFromFile();
  if (readInfo->repeatedValuesFileFormat() == 1) {
    if (readInfo->readSwitches() == 1)
      readInfo->getSwitchValue(switches);
    readInfo->getVectorValue(values);

  } else
    readInfo->getValues(switches, values, lowerbound, upperbound, optimise);

  if (this->SwitchesGiven())
    if (switches.Size() != values.Size())
      handle.logMessage(LOGFAIL, "Error in stochasticdata - failed to read values");

}

StochasticData::StochasticData() {
  netrun = 1;
  readInfo = NULL;
  if (netrun == 1) {
    #ifdef GADGET_NETWORK
      slave = new SlaveCommunication();
      getdata = 0;
      dataFromMaster = NULL;
      numParam = 0;
      this->readFromNetwork();
    #endif
  }
}

StochasticData::~StochasticData() {
  if (netrun == 1) {
    #ifdef GADGET_NETWORK
      if (dataFromMaster != NULL) {
        delete[] dataFromMaster;
        dataFromMaster = NULL;
      }
      if (getdata == 1)
        slave->stopNetCommunication();

      delete slave;
    #endif
  } else
    delete readInfo;
}

void StochasticData::readDataFromNextLine() {
  if (netrun == 1)
    return;
  readInfo->readVectorFromLine();
  values.Reset();
  readInfo->getVectorValue(values);
}

int StochasticData::numVariables() const {
  return values.Size();
}

int StochasticData::DataIsLeft() {
  return !(readInfo->reachedEndOfFile());
}

int StochasticData::DataFromFile() {
  return !netrun;
}

int StochasticData::Optimise(int i) const {
  if (netrun == 1)
    return 0;
  return optimise[i];
}

int StochasticData::OptGiven() const {
  if (netrun == 1)
    return 0;
  return (optimise.Size() > 0 ? 1 : 0);
}

int StochasticData::SwitchesGiven() const {
  return (switches.Size() > 0 ? 1 : 0);
}

#ifdef GADGET_NETWORK
void StochasticData::readFromNetwork() {
  int i, check;

  //Receive first data from master
  numParam = slave->startNetCommunication();
  if (numParam > 0) {
    //successfully started netcommunication
    dataFromMaster = new double[numParam];
    if (values.Size() == 0) {
      values.resize(numParam);
      lowerbound.resize(numParam);
      upperbound.resize(numParam);
    }

    //try to receive switches from master
    getdata = slave->receiveFromMaster();
    if (getdata == 1) {
      if (slave->receivedString()) {
        for (i = 0; i < numParam; i++) {
          Parameter sw(slave->getString(i));
          switches.resize(1, sw);
        }
      } else
        getdata = 0;
    }

    //try to receive lowerbounds from master
    getdata = slave->receiveFromMaster();
    if (getdata == 1) {
      if (slave->receivedBounds()) {
        slave->getBound(dataFromMaster);
        for (i = 0; i < numParam; i++)
          lowerbound[i] = dataFromMaster[i];
      } else
        getdata = 0;
    }

    //try to receive upperbounds from master
    getdata = slave->receiveFromMaster();
    if (getdata == 1) {
      if (slave->receivedBounds()) {
        slave->getBound(dataFromMaster);
        for (i = 0; i < numParam; i++)
          upperbound[i] = dataFromMaster[i];
      } else
        getdata = 0;
    }

    //try to receive vector value from master
    getdata = slave->receiveFromMaster();
    if (getdata == 1) {
      if (slave->receivedVector()) {
        slave->getVector(dataFromMaster);
        for (i = 0; i < values.Size(); i++)
          values[i] = dataFromMaster[i];
      } else
        getdata = 0;
    }

  } else
    getdata = 0;

  if (this->SwitchesGiven()) {
    if (switches.Size() != values.Size())
      handle.logMessage(LOGFAIL, "Error in stochasticdata - failed to read values");

    check = 0;
    for (i = 0; i < values.Size(); i++) {
      if (lowerbound[i] > upperbound[i]) {
        check++;
        handle.logMessage(LOGWARN, "Error in stochasticdata - parameter has upper bound", upperbound[i]);
        handle.logMessage(LOGWARN, "which is lower than the corresponding lower bound", lowerbound[i]);
      }
      if (values[i] > upperbound[i]) {
        check++;
        handle.logMessage(LOGWARN, "Error in stochasticdata - parameter has initial value", values[i]);
        handle.logMessage(LOGWARN, "which is higher than the corresponding upper bound", upperbound[i]);
      }
      if (values[i] < lowerbound[i]) {
        check++;
        handle.logMessage(LOGWARN, "Error in stochasticdata - parameter has initial value", values[i]);
        handle.logMessage(LOGWARN, "which is lower than the corresponding lower bound", lowerbound[i]);
      }
    }
    if (check > 0)
      handle.logMessage(LOGFAIL, "Error in stochasticdata - failed to read parameters and bounds correctly");

  }
}

void StochasticData::readNextLineFromNet() {
  int i;
  getdata = slave->receiveFromMaster();
  if (getdata == 1) {
    if (slave->receivedVector()) {
      slave->getVector(dataFromMaster);
      for (i = 0; i < values.Size(); i++)
        values[i] = dataFromMaster[i];
    } else
      getdata = 0;
  }
}

void StochasticData::sendDataToMaster(double funcValue) {
  int info = slave->sendToMaster(funcValue);
  if (info < 0) {
    slave->stopNetCommunication();
    handle.logMessage(LOGFAIL, "Error in stochasticdata - failed to send data to PVM master");
  }
}
#endif
