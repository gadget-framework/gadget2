#include "stochasticdata.h"
#include "readfunc.h"
#include "gadget.h"

StochasticData::StochasticData(const char* const filename) {
  NETRUN = 0;
  error = 0;
  readInfo = new InitialInputFile(filename);
  readInfo->readFromFile();
  if (readInfo->repeatedValuesFileFormat() == 1) {
    if (readInfo->readSwitches() == 1)
      readInfo->getSwitchValue(switches);
    readInfo->getVectorValue(values);

  } else
    readInfo->getReadValues(switches, values, lowerbound, upperbound, optimize);

  if (this->SwitchesGiven()) {
    if (switches.Size() != values.Size()) {
      cerr << "Stochastic data received " << switches.Size() << " switches and "
        << values.Size() << " values - but these should be the same\n";
      exit(EXIT_FAILURE);
    }
  }
}

StochasticData::StochasticData(int netrun) {
  NETRUN = netrun;
  error = 0;
  readInfo = NULL;
  if (NETRUN == 1) {
    #ifdef GADGET_NETWORK
      slave = new SlaveCommunication();
      GETDATA = 0;
      dataFromMaster = NULL;
      numberOfParam = 0;
      this->ReadFromNetwork();
    #endif
  }
}

StochasticData::~StochasticData() {
  if (NETRUN == 1) {
    #ifdef GADGET_NETWORK
      if (dataFromMaster != NULL) {
        delete[] dataFromMaster;
        dataFromMaster = NULL;
      }
      if (GETDATA == 1)
        slave->stopNetCommunication();

      delete slave;
    #endif
  }

  if (readInfo != NULL) {
    delete readInfo;
    readInfo = NULL;
  }
}

void StochasticData::ReadDataFromNextLine() {
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
  return !NETRUN;
}

#ifdef GADGET_NETWORK
void StochasticData::ReadFromNetwork() {
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
    GETDATA = slave->receiveFromMaster();
    if (GETDATA == 1) {
      if (slave->receivedString()) {
        for (i = 0; i < numberOfParam; i++) {
          Parameter sw(slave->getString(i));
          switches.resize(1, sw);
        }
      } else
        GETDATA = 0;
    }

    //try to receive lowerbounds from master
    GETDATA = slave->receiveFromMaster();
    if (GETDATA == 1) {
      if (slave->receivedBounds()) {
        slave->getBound(dataFromMaster);
        for (i = 0; i < numberOfParam; i++)
          lowerbound[i] = dataFromMaster[i];
      } else
        GETDATA = 0;
    }

    //try to receive upperbounds from master
    GETDATA = slave->receiveFromMaster();
    if (GETDATA == 1) {
      if (slave->receivedBounds()) {
        slave->getBound(dataFromMaster);
        for (i = 0; i < numberOfParam; i++)
          upperbound[i] = dataFromMaster[i];
      } else
        GETDATA = 0;
    }

    //try to receive vector value from master
    GETDATA = slave->receiveFromMaster();
    if (GETDATA == 1) {
      if (slave->receivedVector()) {
        slave->getVector(dataFromMaster);
        for (i = 0; i < values.Size(); i++)
          values[i] = dataFromMaster[i];
      } else
        GETDATA = 0;
    }

  } else
    GETDATA = 0;

  if (this->SwitchesGiven()) {
    if (switches.Size() != values.Size()) {
      cerr << "Stochastic data received " << switches.Size() << " switches and "
        << values.Size() << " values - but these should be the same\n";
      exit(EXIT_FAILURE);
    }
    check = 0;
    for (i = 0; i < values.Size(); i++) {
      if (lowerbound[i] > upperbound[i]) {
        check++;
        cerr << "Error - for switch " << i << " the lowerbound " << lowerbound[i]
          << " is greater than upperbound " << upperbound[i] << endl;
      }
      if (values[i] > upperbound[i]) {
        check++;
        cerr << "Error - for switch " << i << " the value " << values[i]
          << " is greater than upperbound " << upperbound[i] << endl;
      }
      if (values[i] < lowerbound[i]) {
        check++;
        cerr << "Error - for switch " << i << " the value " << values[i]
          << " is lower than lowerbound " << lowerbound[i] << endl;
      }
    }
    if (check > 0) {
      cerr << "Exiting with " << check << " errors in the stochastic data\n";
      exit(EXIT_FAILURE);
    }
  }
}

void StochasticData::ReadNextLineFromNet() {
  int i;
  GETDATA = slave->receiveFromMaster();
  if (GETDATA == 1) {
    if (slave->receivedVector()) {
      slave->getVector(dataFromMaster);
      for (i = 0; i < values.Size(); i++)
        values[i] = dataFromMaster[i];
    } else
      GETDATA = 0;
  }
}

int StochasticData::getDataFromNet() {
  return GETDATA;
}

void StochasticData::SendDataToMaster(double funcValue) {
  int info = slave->sendToMaster(funcValue);
  if (info < 0) {
    cerr << "Could not send data to master, slave is bailing out\n";
    slave->stopNetCommunication();
    exit(EXIT_FAILURE);
  }
}
#endif
