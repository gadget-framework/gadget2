#include "stochasticdata.h"
#include "readfunc.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

StochasticData::StochasticData(const char* const filename) {
  netrun = 0;
  error = 0;
  readInfo = new InitialInputFile(filename);
  readInfo->readFromFile();
  if (readInfo->repeatedValuesFileFormat() == 1) {
    if (readInfo->readSwitches() == 1)
      readInfo->getSwitchValue(switches);
    readInfo->getVectorValue(values);

  } else
    readInfo->getValues(switches, values, lowerbound, upperbound, optimize);

  if (this->SwitchesGiven())
    if (switches.Size() != values.Size())
      handle.logFailure("Error in stochasticdata - failed to read values");

}

StochasticData::StochasticData(int net) {
  netrun = net;
  error = 0;
  readInfo = NULL;
  if (netrun == 1) {
    #ifdef GADGET_NETWORK
      slave = new SlaveCommunication();
      getdata = 0;
      dataFromMaster = NULL;
      numberOfParam = 0;
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
  readInfo->readVectorFromLine();
  values.Reset();
  readInfo->getVectorValue(values);
}

int StochasticData::NoVariables() const {
  return values.Size();
}

int StochasticData::DataIsLeft() {
  return !(readInfo->reachedEndOfFile());
}

int StochasticData::Error() const {
  return error;
}

int StochasticData::DataFromFile() {
  return !netrun;
}

int StochasticData::Optimize(int i) const {
  if (netrun == 1)
    return 0;
  return optimize[i];
}

int StochasticData::OptGiven() const {
  if (netrun == 1)
    return 0;
  return (optimize.Size() > 0 ? 1 : 0);
}

int StochasticData::SwitchesGiven() const {
  return (switches.Size() > 0 ? 1 : 0);
}

#ifdef GADGET_NETWORK
void StochasticData::readFromNetwork() {
  int i, check;

  //Receive first data from master
  numberOfParam = slave->startNetCommunication();
  if (numberOfParam > 0) {
    //successfully started netcommunication
    dataFromMaster = new double[numberOfParam];
    if (values.Size() == 0) {
      values.resize(numberOfParam);
      lowerbound.resize(numberOfParam);
      upperbound.resize(numberOfParam);
    }

    //try to receive switches from master
    getdata = slave->receiveFromMaster();
    if (getdata == 1) {
      if (slave->receivedString()) {
        for (i = 0; i < numberOfParam; i++) {
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
        for (i = 0; i < numberOfParam; i++)
          lowerbound[i] = dataFromMaster[i];
      } else
        getdata = 0;
    }

    //try to receive upperbounds from master
    getdata = slave->receiveFromMaster();
    if (getdata == 1) {
      if (slave->receivedBounds()) {
        slave->getBound(dataFromMaster);
        for (i = 0; i < numberOfParam; i++)
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
      handle.logFailure("Error in stochasticdata - failed to read values");

    check = 0;
    for (i = 0; i < values.Size(); i++) {
      if (lowerbound[i] > upperbound[i]) {
        check++;
        handle.logWarning("Error in stochasticdata - failed to correctly read bounds", i);
      }
      if (values[i] > upperbound[i]) {
        check++;
        handle.logWarning("Error in stochasticdata - failed to correctly read upperbound", i);
      }
      if (values[i] < lowerbound[i]) {
        check++;
        handle.logWarning("Error in stochasticdata - failed to correctly read lowerbound", i);
      }
    }
    if (check > 0)
      handle.logFailure("Error in stochasticdata - failed to read bounds");

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

int StochasticData::getDataFromNet() {
  return getdata;
}

void StochasticData::SendDataToMaster(double funcValue) {
  int info = slave->sendToMaster(funcValue);
  if (info < 0) {
    slave->stopNetCommunication();
    handle.logFailure("Error in stochasticdata - failed to send data to PVM master");
  }
}
#endif
