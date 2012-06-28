#ifndef pvmconstants_h
#define pvmconstants_h

/**
 * \class PVMConstants
 * \brief This is the class used to store information about the different tags and data encoding that can be used during the PVM network communication
 */
class PVMConstants {
public:
  /**
   * \brief This is the default PVMConstants constructor
   */
  PVMConstants();
  /**
   * \brief This is the default PVMConstants destructor
   */
  ~PVMConstants() {};
  /**
   * \brief This function will return the flag used to indicate that PVM has been stopped
   * \return stopPVM
   */
  const int getStopTag() const { return stopPVM; };
  /**
   * \brief This function will return the flag used to indicate that PVM has been started
   * \return startPVM
   */
  const int getStartTag() const { return startPVM; };
  /**
   * \brief This function will return the flag used to indicate that the PVM master is sending variables data
   * \return masterSendVar
   */
  const int getMasterSendVarTag() const { return masterSendVar; };
  /**
   * \brief This function will return the flag used to indicate that the PVM master is sending string data
   * \return masterSendString
   */
  const int getMasterSendStringTag() const { return masterSendString; };
  /**
   * \brief This function will return the flag used to indicate that the PVM master is sending bounds data
   * \return masterSendBound
   */
  const int getMasterSendBoundTag() const { return masterSendBound; };
  /**
   * \brief This function will return the flag used to indicate that the PVM master is receiving data
   * \return masterReceiveData
   */
  const int getMasterReceiveDataTag() const { return masterReceiveData; };
  /**
   * \brief This function will return the flag used to indicate that a task has died
   * \return taskDied
   */
  const int getTaskDiedTag() const { return taskDied; };
  /**
   * \brief This function will return the flag used to indicate that a task has exited
   * \return taskExit
   */
  const int getDataEncode() const { return dataEncode; };
  /**
   * \brief This function will set the flag used to indicate the data encoding that will take place during the PVM network communication
   * \param dc the new value for dataEncode
   */
  void setDataEncode(const int dc);
private:
  /**
   * \brief This is the flag used to indicate that PVM has been stopped
   */
  int stopPVM;
  /**
   * \brief This is the flag used to indicate that PVM has been started
   */
  int startPVM;
  /**
   * \brief This is the flag used to indicate that a task has died
   */
  int taskDied;
  /**
   * \brief This is the flag used to indicate that the PVM master is sending variables data
   */
  int masterSendVar;
  /**
   * \brief This is the flag used to indicate that the PVM master is sending strings data
   */
  int masterSendString;
  /**
   * \brief This is the flag used to indicate that the PVM master is sending bounds data
   */
  int masterSendBound;
  /**
   * \brief This is the flag used to indicate that the PVM master is receiving data
   */
  int masterReceiveData;
  /**
   * \brief This is the flag used to indicate the data encoding that will take place during the PVM network communication
   * \note Possible values for dataEncode are 0 (for XDR encoding), 1 (for no encoding, the default) and 2 (to leave the data in place during the network communication)
   */
  int dataEncode;
};

#endif
