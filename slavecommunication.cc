#include "slavecommunication.h"
#include "gadget.h"

SlaveCommunication::SlaveCommunication() {
  #ifdef CONDOR
    MAXWAIT = 100000000;
  #else
    MAXWAIT = 30;
  #endif
  pvmConst = new PVMConstants();
  typeReceived = -1;
  parenttid = -1;
  mytid = -1;
  myID = -1;
  numVar = 0;
  netDataVar = NULL;
  tmout.tv_sec = MAXWAIT;
  tmout.tv_usec = 0;
}

SlaveCommunication::~SlaveCommunication() {
  delete pvmConst;
  if (netDataVar != NULL) {
    delete netDataVar;
    netDataVar = NULL;
  }
}

void SlaveCommunication::printErrorMsg(const char* errorMsg) {
  char* msg;
  msg = new char[strlen(errorMsg) + 1];
  strcpy(msg, errorMsg);
  pvm_perror(msg);
  delete[] msg;
}

int SlaveCommunication::startNetCommunication() {
  int info, bytes, type, source;
  int bufID = 0;

  //enroll in pvm and get identity of process for myself
  mytid = pvm_mytid();
  if (mytid < 0) {
    printErrorMsg("Error in slavecommunication - unable to join PVM");
    return 0;
  }
  parenttid = pvm_parent();
  if (parenttid == PvmNoParent) {
    printErrorMsg("Error in slavecommunication - process has not been spawned");
    return 0;
  }

  //Wait for message from parenttid
  bufID = pvm_trecv(parenttid, -1, &tmout);
  if (bufID < 0) {
    cerr << "Error in slavecommunication - no message from master\n";
    return 0;

  } else if (bufID == 0) {
    cerr << "Error in slavecommunication - no message from master in " << MAXWAIT << " seconds\n";
    return 0;

  } else {
    //there is a waiting message to be received from master in buffer with id = bufID
    info = pvm_bufinfo(bufID, &bytes, &type, &source);
    if (info < 0) {
      printErrorMsg("Error in slavecommunication - invalid buffer");
      return 0;
    }

    if (type == pvmConst->getStopTag()) {
      int stopMessage;
      info = pvm_upkint(&stopMessage, 1, 1);
      if (info < 0) {
        printErrorMsg("Error in slavecommunication - can not receive data");
        return 0;
      }
      typeReceived = pvmConst->getStopTag();
      return 0;

    } else if (type == pvmConst->getStartTag()) {
      info = pvm_upkint(&numVar, 1, 1);
      if (info < 0) {
        printErrorMsg("Error in slavecommunication - can not receive data");
        return 0;
      }
      if (numVar <= 0) {
        cerr << "Error in slavecommunication - number of variables received from master is less than zero\n";
        return 0;
      }

      info = pvm_upkint(&myID, 1, 1);
      if (info < 0) {
        printErrorMsg("Error in slavecommunication - can not receive data");
        return 0;
      }
      if (myID < 0) {
        cerr << "Error in slavecommunication - received invalid id of " << myID << endl;
        return 0;
      }
      netDataVar = new NetDataVariables(numVar);
      typeReceived = pvmConst->getStartTag();
      return numVar;

    } else {
      cerr << "Error in slavecommunication - received unrecognised tag of type " << type << endl;
      return 0;
    }
  }
}

void SlaveCommunication::stopNetCommunication() {
  if (pvm_mytid() >= 0)
    pvm_exit();
}

int SlaveCommunication::sendToMaster(double res) {
  int info;
  assert(netDataVar != NULL);
  if (netDataVar->x_id < 0 || netDataVar->tag < 0) {
    printErrorMsg("Error in slavecommunication - invalid id received\n");
    return 0;
  } else if (myID < 0) {
    printErrorMsg("Error in slavecommunication - invalid id received\n");
    return 0;
  } else {
    NetDataResult* sendData = new NetDataResult;
    sendData->who = myID;
    sendData->result = res;
    sendData->x_id = netDataVar->x_id;
    sendData->tag = netDataVar->tag;
    info = send(sendData);
    delete sendData;

    if (info > 0) {
      netDataVar->tag = -1;
      netDataVar->x_id = -1;
      return 1;
    } else
      return 0;
  }
  return 0;
}

int SlaveCommunication::send(NetDataResult* sendData) {
  int info;
  info = pvm_initsend(pvmConst->getDataEncode());
  if (info < 0) {
    printErrorMsg("Error in slavecommunication - can not send data");
    return 0;
  }
  info =  pvm_pkint(&sendData->tag, 1, 1);
  if (info < 0) {
    printErrorMsg("Error in slavecommunication - can not send data");
    return 0;
  }
  info = pvm_pkdouble(&sendData->result, 1, 1);
  if (info < 0) {
    printErrorMsg("Error in slavecommunication - can not send data");
    return 0;
  }
  info = pvm_pkint(&sendData->who, 1, 1);
  if (info < 0) {
    printErrorMsg("Error in slavecommunication - can not send data");
    return 0;
  }
  info = pvm_pkint(&sendData->x_id, 1, 1);
  if (info < 0) {
    printErrorMsg("Error in slavecommunication - can not send data");
    return 0;
  }
  info = pvm_send(parenttid, pvmConst->getMasterReceiveDataTag());
  if (info < 0) {
    printErrorMsg("Error in slavecommunication - can not send data");
    return 0;
  }
  return 1;
}

int SlaveCommunication::receiveFromMaster() {
  int bufID = 0;
  int info, bytes, source, type;
  typeReceived = -1;

  bufID = pvm_trecv(parenttid, -1, &tmout);
  if (bufID < 0) {
    cerr << "Error in slavecommunication - no message from master\n";
    return 0;

  } else if (bufID == 0) {
    cerr << "Error in slavecommunication - no message from master in " << MAXWAIT << " seconds\n";
    return 0;

  } else {
    //there is a waiting message to be received from master in buffer with id = bufID
    info = pvm_bufinfo(bufID, &bytes, &type, &source);
    if (info < 0) {
      printErrorMsg("Error in slavecommunication - invalid buffer");
      return 0;
    }

    if (type == pvmConst->getStopTag()) {
      //receive information from master to quit
      int stopMessage;
      info = pvm_upkint(&stopMessage, 1, 1);
      if (info < 0) {
        printErrorMsg("Error in slavecommunication - can not receive data");
      }
      // return 0 if should stop or error occured
      typeReceived = pvmConst->getStopTag();
      return 0;

    } else if (type == pvmConst->getMasterSendStringTag()) {
      // There is an incoming message of data type stringDataVariables
      info = receiveString();
      typeReceived = pvmConst->getMasterSendStringTag();
      if (info > 0)
        return 1;
      return 0;

    } else if (type == pvmConst->getMasterSendBoundTag()) {
      // There is an incoming message of data type double for the bounds
      info = receiveBound();
      typeReceived = pvmConst->getMasterSendBoundTag();
      if (info > 0)
        return 1;
      return 0;

    } else if (type == pvmConst->getMasterSendVarTag()) {
      //There is an incoming message of data type NetDataVariables
      info = receive();
      if (info > 0) {
        typeReceived = pvmConst->getMasterSendVarTag();
        return 1;
      } else
        return 0;

    } else {
      cerr << "Error in slavecommunication - received unrecognised tag of type " << type << endl;
      return 0;
    }
  }
}

int SlaveCommunication::receiveBound() {
  int i;
  double* temp = new double[numVar];
  int info = pvm_upkdouble(temp, numVar, 1);
  if (info < 0) {
    printErrorMsg("Error in slavecommunication - receive bound failed");
    delete[] temp;
    return 0;
  }
  netDataDouble.Reset();
  netDataDouble.resize(numVar, 0.0);
  for (i = 0; i < numVar; i++)
    netDataDouble[i] = temp[i];
  delete[] temp;
  return 1;
}

int SlaveCommunication::receive() {
  int i;
  int info = pvm_upkint(&netDataVar->tag, 1, 1);
  if (info < 0) {
    printErrorMsg("Error in slavecommunication - receive data failed");
    return 0;
  }
  info = pvm_upkint(&netDataVar->x_id, 1, 1);
  if (info < 0) {
    printErrorMsg("Error in slavecommunication - receive data failed");
    return 0;
  }
  assert(numVar > 0);
  info = pvm_upkdouble(netDataVar->x, numVar, 1);
  if (info < 0) {
    printErrorMsg("Error in slavecommunication - receive data failed");
    return 0;
  }
  return 1;
}

int SlaveCommunication::receivedVector() {
  if (pvmConst->getMasterSendVarTag() == typeReceived)
    return 1;
  return 0;
}

void SlaveCommunication::getVector(DoubleVector& vec) {
  int i;
  assert(vec.Size() == numVar);
  for (i = 0; i < numVar; i++)
    vec[i] = netDataVar->x[i];
}

int SlaveCommunication::getReceiveType() {
  return typeReceived;
}

int SlaveCommunication::receiveString() {
  int i, info;
  char* tempString = new char[MaxStrLength + 1];
  strncpy(tempString, "", MaxStrLength);
  tempString[MaxStrLength] = '\0';

  for (i = 0; i < numVar; i++) {
    info = pvm_upkstr(tempString);
    if (info < 0) {
      printErrorMsg("Error in slavecommunication - receive string failed");
      delete[] tempString;
      return 0;
    }
    Parameter pm(tempString);
    netDataStr.resize(pm);
  }
  delete[] tempString;
  return 1;
}

int SlaveCommunication::receivedString() {
  if (pvmConst->getMasterSendStringTag() == typeReceived)
    return 1;
  return 0;
}

int SlaveCommunication::receivedBounds() {
  if (pvmConst->getMasterSendBoundTag() == typeReceived)
    return 1;
  return 0;
}

const ParameterVector& SlaveCommunication::getStringVector() {
  return netDataStr;
}

const Parameter& SlaveCommunication::getString(int num) {
  assert(num >= 0);
  assert(netDataStr.Size() == numVar);
  return netDataStr[num];
}

void SlaveCommunication::getBound(DoubleVector& vec) {
  int i;
  assert(vec.Size() == numVar);
  for (i = 0; i < numVar; i++)
    vec[i] = netDataDouble[i];
}
