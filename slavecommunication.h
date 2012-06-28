#ifndef slavecommunication_h
#define slavecommunication_h

#include "mpi.h"
#include "netdata.h"
#include "pvmconstants.h"
#include "doublevector.h"
#include "parametervector.h"

// AJ, 07.09.99
/**
 * \class SlaveCommunication 
 * \brief The class SlaveCommunication handles netcommunication for a slave process communicating with a master process using PVM. It provides functions for starting/stopping communication with master and sending/receiving data to/from master. The class can receive data of the type: NetDataVariables and CharPtrVector and send data of the type: NetDataResult. To successfully start netcommunication pvmd must be running on host and the process must have been spawned by a master process. 
 */

class SlaveCommunication {
private:
  int MAXWAIT;
  PVMConstants* pvmConst;
  int typeReceived;
  //identities for netcommunication
  /**
   * \brief id of my master recognized by pvm.parenttid = -1 if has not started communications with master.
   */
  int parenttid;    
  /**
   * \brief id of myself recognized by pvm.mytid = -1 if has not enrolled in pvm else >= 0.
   */
  int mytid;          
  /**
   *\brief id recognized by master, equals -1 if has not started initial communication with master,
   */
  int myID;           

  int numVar;
  NetDataVariables* netDataVar;
  DoubleVector netDataDouble;
  struct timeval tmout;
  ParameterVector netDataStr;

public:
  SlaveCommunication();
  ~SlaveCommunication();
  void printErrorMsg(const char* errorMsg);
  int startNetCommunication();
  void stopNetCommunication();
  int receive();
  int receiveFromMaster();
  int send(NetDataResult* sendData);
  int sendToMaster(double res);
  int receivedVector();
  //void getVector(DoubleVector& vec);
  void getVector(double* vec);
  int getReceiveType();
  int receiveString();
  int receiveBound();
  int receivedString();
  int receivedBounds();
  //void getBound(DoubleVector& vec);
  void getBound(double* vec);
  const Parameter& getString(int num);
  const ParameterVector& getStringVector();
};
#endif
