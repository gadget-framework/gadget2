#ifndef slavecommunication_h
#define slavecommunication_h

#include "pvm3.h"
#include "netdata.h"
#include "pvmconstants.h"

#ifdef GADGET_NETWORK
#include "vectorofcharptr.h"
#endif

/* AJ, 07.09.99
 * The class slaveCommunication handles netcommunication for a
 * slave process communicating with a master process using PVM.
 * It provides functions for starting/stopping communication with
 * master and sending/receiving data to/from master. The class can
 * receive data of the type: netDataVariables and vectorofcharptr and
 * send data of the type: netDataResult.
 * To successfully start netcommunication pvmd must be running on
 * host and the process must have been spawned by a master process. */

class slaveCommunication {
private:
  int MAXWAIT;
  pvmconstants* pvmConst;
  int typeReceived;
  //identities for netcommunication
  int parenttid;      //id of my master recognized by pvm.
                      //parenttid = -1 if has not started communications with master.
  int mytid;          //id of myself recognized by pvm.
                      //mytid = -1 if has not enrolled in pvm else >= 0.
  int myId;           //id recognized by master, equals -1 if has not started
                      //initial communication with master,

  int numberOfVar;
  netDataVariables* netDataVar;
  double* netDataDouble;
  struct timeval tmout;
  #ifdef GADGET_NETWORK
    vectorofcharptr netDataStr;
  #endif

public:
  slaveCommunication();
  ~slaveCommunication();
  void printErrorMsg(const char* errorMsg);
  int startNetCommunication();
  void stopNetCommunication();
  int receive();
  int receiveFromMaster();
  int send(netDataResult* sendData);
  int sendToMaster(double res);
  int receivedVector();
  void getVector(double* vec);
  int getReceiveType();
  int receiveString();
  int receiveBound();
  int receivedString();
  int receivedBounds();
  void getBound(double* vec);
  char* getString(int num);
};
#endif
