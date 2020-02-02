#ifndef netdata_h
#define netdata_h

/**
 * \class NetDataVariables
 * \brief this class is used to keep a vector of relevant data to be sent/received using PVM communication
 */
class NetDataVariables {
public:
  int tag;
  int x_id;
  /**
   * \brief this is the vector used to store the variables to be sent/received
   */
  double* x;
  /**
   * \brief this is the default NetDataVariables constructor
   * \param numVar is the number of variables to be stored
   */
  NetDataVariables(int numVar);
  /**
   * \brief this is the default NetDataVariables destructor
   */
  ~NetDataVariables();
};

/**
 * \class NetDataResult
 * \brief this class is used to keep relevant data to be sent/received using PVM communication
 */
class NetDataResult {
public:
  int tag;
  int x_id;
  /**
   * \brief this is the variable to be sent/received
   */
  double result;
  /**
   * \brief is the identifier of the process that is sending the data
   */
  int who;
  /**
   * \brief this is the default NetDataResult constructor
   */
  NetDataResult();
  /**
   * \brief this is the default NetDataResult destructor
   */
  ~NetDataResult();
};

#endif
