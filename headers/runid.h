#ifndef runid_h
#define runid_h

#include "gadget.h"

/**
 * \class RunID
 * \brief This is the class used to store information about the machine used for, and the time of, the model run
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
  ~RunID();
  /**
   * \brief This will print the model run information for the current model
   * \param o is the ofstream that the model run information will be written to
   */
  void Print(ostream& o);
  /**
   * \brief This will print information about the run time for the current model
   * \param o is the ofstream that the model run information will be written to
   */
  void printTime(ostream& o);
protected:
  /**
   * \brief This is the name of the host machine that is running the current model
   */
  char* hostname;
  /**
   * \brief This is the time stamp used to denote the time that the current model run started
   */
  char* timestring;
  /**
   * \brief This is the host machine that is running the current model
   */
#ifdef NOT_WINDOWS
  struct utsname host;
#endif
  /**
   * \brief This is the time that the current model started the simulation
   */
  time_t runtime;
};

#endif
