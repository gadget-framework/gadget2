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
  myId = -1;
  numberOfVar = 0;
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
  int OK = 1;
  int bufId = 0;

  //enroll in pvm and get identity of process for myself
  mytid = pvm_mytid();
  if (mytid < 0) {
    printErrorMsg("Error in slavecommunication - unable to join PVM");
    return !OK;
  }
  parenttid = pvm_parent();
  if (parenttid == PvmNoParent) {
    printErrorMsg("Error in slavecommunication - process has not been spawned");
    return !OK;
  }

  //Wait for message from parenttid
  bufId = pvm_trecv(parenttid, -1, &tmout);
  if (bufId < 0) {
    cerr << "Error in slavecommunication - no message from master\n";
    return !OK;
  } else if (bufId == 0) {
    cerr << "Error in slavecommunication - no message from master in " << MAXWAIT << " seconds\n";
    return !OK;

  } else {
    //there is a waiting message to be received from master in buffer with id = bufId
    info = pvm_bufinfo(bufId, &bytes, &type, &source);
    if (info < 0) {
      printErrorMsg("Error in slavecommunication - invalid buffer");
      return !OK;
    }

    if (type == pvmConst->getStopTag()) {
      int stopMessage;
      info = pvm_upkint(&stopMessage, 1, 1);
      if (info < 0) {
        printErrorMsg("Error in slavecommunication - can not receive data");
        return !OK;
      }
      typeReceived = pvmConst->getStopTag();
      return !OK;

    } else if (type == pvmConst->getStartTag()) {
      info = pvm_upkint(&numberOfVar, 1, 1);
      if (info < 0) {
        printErrorMsg("Error in slavecommunication - can not receive data");
        return !OK;
      }
      if (numberOfVar <= 0) {
        cerr << "Error in slavecommunication - number of variables received from master is less than zero\n";
        return !OK;
      }

      info = pvm_upkint(&myId, 1, 1);
      if (info < 0) {
        printErrorMsg("Error in slavecommunication - can not receive data");
        return !OK;
      }
      if (myId < 0) {
        cerr << "Error in slavecommunication - received invalid id of " << myId << endl;
        return !OK;
      }
      netDataVar = new NetDataVariables(numberOfVar);
      typeReceived = pvmConst->getStartTag();
      return numberOfVar;

    } else {
      cerr << "Error in slavecommunication - received unrecognised tag of type " << type << endl;
      return !OK;
    }
  }
}

void SlaveCommunication::stopNetCommunication() {
  int tid;
  tid = pvm_mytid();
  if (tid >= 0)
    pvm_exit();
}

int SlaveCommunication::sendToMaster(double res) {
  int OK, info;
  assert(netDataVar != NULL);
  if (netDataVar->x_id < 0 || netDataVar->tag < 0) {
    cerr << "Error in slavecommunication - invalid id received\n";
    OK = 0;
  } else if (myId < 0) {
    cerr << "Error in slavecommunication - invalid id received\n";
    OK = 0;
  } else {
    NetDataResult* sendData = new NetDataResult;
    sendData->who = myId;
    sendData->result = res;
    sendData->x_id = netDataVar->x_id;
    sendData->tag = netDataVar->tag;
    info = send(sendData);
    if (info > 0) {
      netDataVar->tag = -1;
      netDataVar->x_id = -1;
      OK = 1;
    } else
      OK = 0;
    delete sendData;
  }
  return OK;
}

int SlaveCommunication::send(NetDataResult* sendData) {
  int info;
  int OK = 1;
  info = pvm_initsend(pvmConst->getDataEncode());
  if (info < 0) {
    printErrorMsg("Error in slavecommunication - can not send data");
    cerr << "Error in slavecommunication - cannot initiate buffer\n";
    return !OK;
  }
  info =  pvm_pkint(&sendData->tag, 1, 1);
  if (info < 0) {
    printErrorMsg("Error in slavecommunication - can not send data");
    cerr << "Error in slavecommunication - cannot initiate buffer\n";
    return !OK;
  }
  info = pvm_pkdouble(&sendData->result, 1, 1);
  if (info < 0) {
    printErrorMsg("Error in slavecommunication - can not send data");
    cerr << "Error in slavecommunication - cannot initiate buffer\n";
    return !OK;
  }
  info = pvm_pkint(&sendData->who, 1, 1);
  if (info < 0) {
    printErrorMsg("Error in slavecommunication - can not send data");
    cerr << "Error in slavecommunication - cannot initiate buffer\n";
    return !OK;
  }
  info = pvm_pkint(&sendData->x_id, 1, 1);
  if (info < 0) {
    printErrorMsg("Error in slavecommunication - can not send data");
    cerr << "Error in slavecommunication - cannot initiate buffer\n";
    return !OK;
  }
  info = pvm_send(parenttid, pvmConst->getMasterReceiveDataTag());
  if (info < 0) {
    printErrorMsg("Error in slavecommunication - can not send data");
    cerr << "Error in slavecommunication - cannot send data to master\n";
    return !OK;
  }
  return OK;
}

int SlaveCommunication::receiveFromMaster() {
  int bufId = 0;
  int OK = 1;
  int info, bytes, source, type;
  typeReceived = -1;

  bufId = pvm_trecv(parenttid, -1, &tmout);
  if (bufId < 0) {
    cerr << "Error in slavecommunication - no message from master\n";
    return !OK;
  } else if (bufId == 0) {
    cerr << "Error in slavecommunication - no message from master in " << MAXWAIT << " seconds\n";
    return !OK;

  } else {
    //there is a waiting message to be received from master in buffer with id = bufId
    info = pvm_bufinfo(bufId, &bytes, &type, &source);
    if (info < 0) {
      printErrorMsg("Error in slavecommunication - invalid buffer");
      return !OK;
    }

    if (type == pvmConst->getStopTag()) {
      //receive information from master to quit
      int stopMessage;
      info = pvm_upkint(&stopMessage, 1, 1);
      if (info < 0) {
        printErrorMsg("Error in slavecommunication - can not receive data");
        cerr << "Error in slavecommunication - can not unpack data\n";
      }
      // return 0 if should stop or error occured
      typeReceived = pvmConst->getStopTag();
      return !OK;

    } else if (type == pvmConst->getMasterSendStringTag()) {
      // There is an incoming message of data type stringDataVariables
      info = receiveString();
      typeReceived = pvmConst->getMasterSendStringTag();
      if (info > 0)
        return OK;
      else
        return !OK;

    } else if (type == pvmConst->getMasterSendBoundTag()) {
      // There is an incoming message of data type double for the bounds
      info = receiveBound();
      typeReceived = pvmConst->getMasterSendBoundTag();
      if (info > 0)
        return OK;
      else
        return !OK;

    } else if (type == pvmConst->getMasterSendVarTag()) {
      //There is an incoming message of data type NetDataVariables
      info = receive();
      if (info > 0) {
        typeReceived = pvmConst->getMasterSendVarTag();
        return OK;
      } else
        return !OK;

    } else {
      cerr << "Error in slavecommunication - received unrecognised tag of type " << type << endl;
      return !OK;
    }
  }
}

int SlaveCommunication::receiveBound() {
  int info;
  int OK = 1;

  netDataDouble = new double[numberOfVar];
  info = pvm_upkdouble(netDataDouble, numberOfVar, 1);
  if (info < 0) {
    printErrorMsg("Error in slavecommunication - receive bound failed");
    cerr << "Error in slavecommunication - receive bound failed\n";
    return !OK;
  }
  return OK;
}

int SlaveCommunication::receive() {
  int OK = 1;
  int info;

  info = pvm_upkint(&netDataVar->tag, 1, 1);
  if (info < 0) {
    printErrorMsg("Error in slavecommunication - receive data failed");
    cerr << "Error in slavecommunication - receive data failed\n";
    return !OK;
  }
  info = pvm_upkint(&netDataVar->x_id, 1, 1);
  if (info < 0) {
    printErrorMsg("Error in slavecommunication - receive data failed");
    cerr << "Error in slavecommunication - receive data failed\n";
    return !OK;
  }
  assert(numberOfVar > 0);
  info = pvm_upkdouble(netDataVar->x, numberOfVar, 1);
  if (info < 0) {
    printErrorMsg("Error in slavecommunication - receive data failed");
    cerr << "Error in slavecommunication - receive data failed\n";
    return !OK;
  }
  return OK;
}

int SlaveCommunication::receivedVector() {
  if (pvmConst->getMasterSendVarTag() == typeReceived)
    return 1;
  else
    return 0;
}

void SlaveCommunication::getVector(double* vec) {
  int i;
  for (i = 0; i < numberOfVar; i++)
    vec[i] = netDataVar->x[i];
}

int SlaveCommunication::getReceiveType() {
  return typeReceived;
}

int SlaveCommunication::receiveString() {
  int OK = 1;
  int i, info;
  char* tempString = new char[MaxStrLength + 1];
  strncpy(tempString, "", MaxStrLength);
  tempString[MaxStrLength] = '\0';

  if (netDataStr.Size() == 0) {
    CharPtrVector tempvec;
    netDataStr = tempvec;
  }

  for (i = 0; i < numberOfVar; i++) {
    info = pvm_upkstr(tempString);
    if (info < 0) {
      printErrorMsg("Error in slavecommunication - receive string failed");
      cerr << "Error in slavecommunication - receive string failed\n";
      delete tempString;
      return !OK;
    }
    netDataStr.resize(tempString);
  }
  delete tempString;
  return OK;
}

int SlaveCommunication::receivedString() {
  if (pvmConst->getMasterSendStringTag() == typeReceived)
    return 1;
  else
    return 0;
}

int SlaveCommunication::receivedBounds() {
  if (pvmConst->getMasterSendBoundTag() == typeReceived)
    return 1;
  else
    return 0;
}

char* SlaveCommunication::getString(int num) {
  assert(num >= 0);
  assert(netDataStr.Size() == numberOfVar);
  return netDataStr[num];
}

void SlaveCommunication::getBound(double* vec) {
  int i;
  for (i = 0; i < numberOfVar; i++)
    vec[i] = netDataDouble[i];
}
