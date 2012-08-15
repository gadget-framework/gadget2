#include "slavecommunication.h"
#include "gadget.h"

SlaveCommunication::SlaveCommunication() 
{
	MAXWAIT = 30;
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

SlaveCommunication::~SlaveCommunication() 
{
  	delete pvmConst;
  	if (netDataVar != NULL) 
	{
    	delete netDataVar;
    	netDataVar = NULL;
  	}
}

void SlaveCommunication::printErrorMsg(const char* errorMsg) 
{
	/*
		Laga þetta þegar ég fer að pæla í error handler...
	*/
  	char* msg;
  	msg = new char[strlen(errorMsg) + 1];
  	strcpy(msg, errorMsg);
	//Prófum þetta hér, var notað pvm_perror sem skrifar villuna sem síðasta pvm kallið olli.
	std::cout << msg << "\n";
  	cerr << msg;
  	delete[] msg;
}

int SlaveCommunication::startNetCommunication() 
{
	/*
		Þetta fall er komið í bili!
	*/
  	int info, bytes, type, source;
  	int OK = 1;
  	int bufID = 0;

  	//enroll in pvm and get identity of process for myself
	MPI_Init(NULL,NULL);
	MPI_Comm parentcomm;
	MPI_Status status;
	
	MPI_Comm_get_parent(&parentcomm);
  	if (parentcomm == MPI_COMM_NULL) 
	{
    	printErrorMsg("Error in slavecommunication - process has not been spawned");
    	return 0;
  	}
	int flag;
	// Þetta þarf að vera gert með timeout, prófum venjulegt blocking probe:
	//MPI_Iprobe(0, MPI_ANY_TAG, parentcomm, &flag,&status);
	MPI_Probe(0, MPI_ANY_TAG, parentcomm,&status);
	//if(flag == true)
	//{
		if (status.MPI_TAG == pvmConst->getStopTag()) 
		{
	    	int stopMessage;
			MPI_Recv(&stopMessage, 1, MPI_INT, 0, MPI_ANY_TAG, parentcomm, &status);
	    	typeReceived = pvmConst->getStopTag();
	    	return !OK;
	    } 
		else if (status.MPI_TAG == pvmConst->getStartTag()) 
		{
			MPI_Recv(&numVar, 1, MPI_INT, 0, MPI_ANY_TAG, parentcomm, &status);
	    	if (numVar <= 0) 
			{
	    		cerr << "Error in slavecommunication - number of variables received from master is less than zero\n";
	    		return !OK;
	    	}
			MPI_Recv(&myID, 1, MPI_INT, 0, MPI_ANY_TAG, parentcomm, &status);
	    	if (myID < 0) 
			{
	    		cerr << "Error in slavecommunication - received invalid id of " << myID << endl;
	    		return !OK;
	    	}
	    	netDataVar = new NetDataVariables(numVar);
	    	typeReceived = pvmConst->getStartTag();
	    	return numVar;
	    }
		else
		{
			cerr << "Error in slavecommunication - received unrecognised tag of type " << status.MPI_TAG << endl;
	    	return !OK;
		}
	//}
	//else
	//{
	//	cerr << "Error in slavecommunication - non-blocking-probe fail" << endl;
    //	return !OK;
	//}
}

void SlaveCommunication::stopNetCommunication() 
{
	MPI_Finalize();
}

int SlaveCommunication::sendToMaster(double res) 
{
  	int info;
  	assert(netDataVar != NULL);
  	if (netDataVar->x_id < 0 || netDataVar->tag < 0) 
	{
    	printErrorMsg("Error in slavecommunication - invalid id received\n");
    	return 0;
  	} 
	else if (myID < 0) 
	{
    	printErrorMsg("Error in slavecommunication - invalid id received\n");
    	return 0;
  	} 
	else 
	{
		//cout << "Slave að skrifa result: " << res << "\n";
    	NetDataResult* sendData = new NetDataResult;
    	sendData->who = myID;
    	sendData->result = res;
    	sendData->x_id = netDataVar->x_id;
    	sendData->tag = netDataVar->tag;
    	info = send(sendData);
    	delete sendData;
    	if (info > 0) 
		{
      		netDataVar->tag = -1;
      		netDataVar->x_id = -1;
      		return 1;
    	} 
		else
      		return 0;
  	}
  	return 0;
}

int SlaveCommunication::send(NetDataResult* sendData) 
{
  	int info;
	MPI_Comm parentcomm;
	//cout << "Slave að skrifa result: " << sendData->result << "\n";
	MPI_Comm_get_parent(&parentcomm);
	MPI_Send(&sendData->tag,1,MPI_INT, 0, pvmConst->getMasterReceiveDataTag(),parentcomm);
	MPI_Send(&sendData->result,1,MPI_DOUBLE, 0, pvmConst->getMasterReceiveDataTag(),parentcomm);
	MPI_Send(&sendData->who,1,MPI_INT, 0, pvmConst->getMasterReceiveDataTag(),parentcomm);
	MPI_Send(&sendData->x_id,1,MPI_INT, 0, pvmConst->getMasterReceiveDataTag(),parentcomm);
  	return 1;
}

int SlaveCommunication::receiveFromMaster() 
{
	/*
		Þetta fall er komið í bili, þarf samt að skoða með þetta TIMEOUT fall...
	*/
  	int bufID = 0;
  	int OK = 1;
  	int info, bytes, source, type;
  	typeReceived = -1;
	MPI_Status status;
	MPI_Comm parentcomm;
	MPI_Comm_get_parent(&parentcomm);
    
	// Hér er hægt að nota non-blocking probe til að komast eitthvað til móts við þetta timeout, hægt
	// að láta það bíða í einhvern tíma og probe-a aftur... Hérna gæti verið góð pæling að útfæra bara
	// sjálfur Timout fall sem testar á fullu í Maxwait tíma og skilar ef flag er true... Ætla að  breyta
	// þessu í bili, en ræð við Bjarka upp á framhaldið...
	// Update: Þetta virkar svona, svo það er e.t.v. ekki nein ástæða til að breyta þessu.
	
    MPI_Probe(0, MPI_ANY_TAG, parentcomm, &status);
  	//bufID = pvm_trecv(parenttid, -1, &tmout);
    if (status.MPI_TAG == pvmConst->getStopTag()) 
	{
    	//receive information from master to quit
		int stopMessage;
		MPI_Recv(&stopMessage, 1, MPI_INT, 0, MPI_ANY_TAG, parentcomm, &status);
    	typeReceived = pvmConst->getStopTag();
    	return !OK;
    } 
	else if (status.MPI_TAG == pvmConst->getMasterSendStringTag()) 
	{
    	// There is an incoming message of data type stringDataVariables
    	info = receiveString();
    	typeReceived = pvmConst->getMasterSendStringTag();
    	if (info > 0)
    		return 1;
    	return 0;
    } 
	else if (status.MPI_TAG == pvmConst->getMasterSendBoundTag()) 
	{
    	// There is an incoming message of data type double for the bounds
    	info = receiveBound();
    	typeReceived = pvmConst->getMasterSendBoundTag();
    	if (info > 0)
    		return 1;
    	return 0;
    } 
	else if (status.MPI_TAG == pvmConst->getMasterSendVarTag()) 
	{
    	//There is an incoming message of data type NetDataVariables
    	info = receive();
    	if (info > 0) 
		{
    		typeReceived = pvmConst->getMasterSendVarTag();
    		return 1;
    	}
    	return 0;
    } 
	else 
	{
    	cerr << "Error in slavecommunication - received unrecognised tag of type " << type << endl;
    	return !OK;
    }
  	
}

int SlaveCommunication::receiveBound() 
{
 	int i;
    double* temp = new double[numVar];
	MPI_Status status;
	MPI_Comm parentcomm;
	MPI_Comm_get_parent(&parentcomm);
	MPI_Recv(temp,numVar,MPI_DOUBLE,0,MPI_ANY_TAG,parentcomm,&status);
  	netDataDouble.Reset();
  	netDataDouble.resize(numVar, 0.0);
  	for (i = 0; i < numVar; i++) 
	{
      	netDataDouble[i] = temp[i];
  	}
  	delete [] temp;
  	return 1;
}

int SlaveCommunication::receive() 
{
  	int info, i;
	MPI_Status status;
	MPI_Comm parentcomm;
	MPI_Comm_get_parent(&parentcomm);
	MPI_Recv(&netDataVar->tag,1,MPI_INT, 0,MPI_ANY_TAG, parentcomm,&status);
	MPI_Recv(&netDataVar->x_id, 1, MPI_INT, 0,MPI_ANY_TAG, parentcomm, &status);
	MPI_Recv(netDataVar->x, numVar,MPI_DOUBLE,0,MPI_ANY_TAG, parentcomm, &status);
  	return 1;
}

int SlaveCommunication::receivedVector() 
{
  	if (pvmConst->getMasterSendVarTag() == typeReceived)
    	return 1;
  	return 0;
}

// Þetta var ekki að virka rétt þar sem kallað var á þetta í Gadget.
//void SlaveCommunication::getVector(DoubleVector& vec) 
//{
//	/*
//		Senda út villu ef þetta if condition er ekki uppfyllt... G.E.
//	*/
//  	int i;
//  	if (vec.Size() != numVar) 
//	{
//      // error....
//  	}
// 
//  	for (i = 0; i < numVar; i++) 
//	  	vec[i] = netDataVar->x[i];
//}

void SlaveCommunication::getVector(double* vec) {
  int i;
  for (i = 0; i < numVar; i++)
    vec[i] = netDataVar->x[i];
}

int SlaveCommunication::getReceiveType() 
{
  	return typeReceived;
}

int SlaveCommunication::receiveString() 
{
  	int OK = 1;
  	int i, info;
  	char* tempString = new char[MaxStrLength + 1];
  	strncpy(tempString, "", MaxStrLength);
  	tempString[MaxStrLength] = '\0';
	MPI_Status status;
	MPI_Comm parentcomm;
	MPI_Comm_get_parent(&parentcomm);
	
  	for (i = 0; i < numVar; i++) 
	{
		MPI_Recv(tempString, MaxStrLength, MPI_BYTE, 0,MPI_ANY_TAG, parentcomm, &status);
    	Parameter pm(tempString);
    	netDataStr.resize(pm);
  	}
  	delete [] tempString;
  	return OK;
}

int SlaveCommunication::receivedString() 
{
  	if (pvmConst->getMasterSendStringTag() == typeReceived)
    	return 1;
  	return 0;
}

int SlaveCommunication::receivedBounds() 
{
  	if (pvmConst->getMasterSendBoundTag() == typeReceived)
    	return 1;
  	return 0;
}

const ParameterVector& SlaveCommunication::getStringVector() 
{
  	return netDataStr;
}

const Parameter& SlaveCommunication::getString(int num) 
{
  	assert(num >= 0);
  	assert(netDataStr.Size() == numVar);
  	return netDataStr[num];
}

// Þetta var ekki að virka rétt þar sem kallað var á þetta í Gadget.
//void SlaveCommunication::getBound(DoubleVector& vec) 
//{
//  	int i;
//  	if (vec.Size() != numVar)
//      // errror...
//  /*
//  for (i = 0; i < numVar; i++)
//    vec[i] = netDataDouble[i];
//  */
//  vec = netDataDouble;
//}

void SlaveCommunication::getBound(double* vec) {
  int i;
  for (i = 0; i < numVar; i++)
    vec[i] = netDataDouble[i];
}