#ifndef runid_h
#define runid_h

#include "gadget.h"

/**
 * \class RunID
 * \brief This is the class used store information about the machione used, and the time of, the model run
 */
class RunID {
public:
  /**
   * \brief This is the default RunID constructor
   */
  RunID();
  /**
   * \brief This is the default RunID destructor
   */
  ~RunID() {};
  /**
   * \brief This will print the model run information for the current model
   * \param o is the ofstream that the model run information will be writtews to
   */
  void print(ostream& o);
protected:
  /**
   * \brief This is the name of the host machine that is running the current model
   */
  const char* hostname;
  /**
   * \brief This is the time stamp used to denote the time that the current model run started
   */
  const char* timestring;
  /**
   * \brief This is the host machine that is running the current model
   */
  struct utsname host;
};

#endif
