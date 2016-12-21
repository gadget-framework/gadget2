#ifndef errorhandler_h
#define errorhandler_h

#include "strstack.h"
#include "doublevector.h"
#include "gadget.h"

enum LogLevel { LOGNONE = 0, LOGFAIL, LOGINFO, LOGWARN, LOGDEBUG, LOGMESSAGE, LOGDETAIL };

/**
 * \class ErrorHandler
 * \brief This is the class used to handle errors in the model, by displaying error messages to the user and logging information to a log file
 */
class ErrorHandler {
public:
  /**
   * \brief This is the ErrorHandler constructor
   */
  ErrorHandler();
  /**
   * \brief This is the default ErrorHandler destructor
   */
  ~ErrorHandler();
  /**
   * \brief This function will open a file for reading data from, and store the name of the file
   * \param filename is the name of the file
   */
  void Open(const char* filename);
  /**
   * \brief This function will close the last file opened for reading data from
   */
  void Close();
  /**
   * \brief This function will check to see if an iostream can be opened, and exit Gadget with exit(EXIT_FAILURE) if this check fails
   * \param infile is the iostream to be checked
   * \param text is the name of the iostream to be checked
   */
  void checkIfFailure(ios& infile, const char* text);
  /**
   * \brief This function will open the file that the logging information will be written to
   * \param filename is the name of the file
   */
  void setLogFile(const char* filename);
  /**
   * \brief This function will return the flag used to determine whether ta logfile exists
   * \return uselog
   */
  int checkLogFile() { return uselog; };
  /**
   * \brief This function will log information about the finish of the current model run to std::cout and a log file if one exists
   */
  void logFinish();
  /**
   * \brief This function will log a warning message
   * \param mlevel is the logging level of the message to be logged
   * \param msg is the message to be logged
   */
  void logMessage(LogLevel mlevel, const char* msg);
  /**
   * \brief This function will log 2 warning messages
   * \param mlevel is the logging level of the message to be logged
   * \param msg1 is the first message to be logged
   * \param msg2 is the second message to be logged
   */
  void logMessage(LogLevel mlevel, const char* msg1, const char* msg2);
  /**
   * \brief This function will log a warning message and a number
   * \param mlevel is the logging level of the message to be logged
   * \param msg is the message to be logged
   * \param number is the number to be logged
   */
  void logMessage(LogLevel mlevel, const char* msg, int number);
  /**
   * \brief This function will log a warning message and a number
   * \param mlevel is the logging level of the message to be logged
   * \param msg is the message to be logged
   * \param number is the number to be logged
   */
  void logMessage(LogLevel mlevel, const char* msg, double number);
  /**
   * \brief This function will log a warning message, a number and a second message
   * \param mlevel is the logging level of the message to be logged
   * \param msg1 is the first message to be logged
   * \param number is the number to be logged
   * \param msg2 is the second message to be logged
   */
  void logMessage(LogLevel mlevel, const char* msg1, int number, const char* msg2);
  /**
   * \brief This function will log a warning message, a number and a second message
   * \param mlevel is the logging level of the message to be logged
   * \param msg1 is the first message to be logged
   * \param number is the number to be logged
   * \param msg2 is the second message to be logged
   */
  void logMessage(LogLevel mlevel, const char* msg1, double number, const char* msg2);
  /**
   * \brief This function will log a vector of values
   * \param mlevel is the logging level of the values to be logged
   * \param vec is the DoubleVector of values to be logged
   */
  void logMessage(LogLevel mlevel, DoubleVector vec);
  /**
   * \brief This function will log a warning message about a NaN found in the model
   * \param mlevel is the logging level of the message to be logged
   * \param msg is the message to be logged
   */
  void logMessageNaN(LogLevel mlevel, const char* msg);
  /**
   * \brief This function will log a warning message generated when reading information from file
   * \param mlevel is the logging level of the message to be logged
   * \param msg is the message to be logged
   */
  void logFileMessage(LogLevel mlevel, const char* msg);
  /**
   * \brief This function will log a warning message and a number, generated when reading information from file
   * \param mlevel is the logging level of the message to be logged
   * \param msg is the message to be logged
   * \param number is the number to be logged
   */
  void logFileMessage(LogLevel mlevel, const char* msg, int number);
  /**
   * \brief This function will log a warning message and a number, generated when reading information from file
   * \param mlevel is the logging level of the message to be logged
   * \param msg is the message to be logged
   * \param number is the number to be logged
   */
  void logFileMessage(LogLevel mlevel, const char* msg, double number);
  /**
   * \brief This function will log 2 warning messages generated when reading information from file
   * \param mlevel is the logging level of the message to be logged
   * \param msg1 is the first message to be logged
   * \param msg2 is the second message to be logged
   */
  void logFileMessage(LogLevel mlevel, const char* msg1, const char* msg2);
  /**
   * \brief This function will log an EOF warning message generated when reading information from file
   * \param mlevel is the logging level of the message to be logged
   */
  void logFileEOFMessage(LogLevel mlevel);
  /**
   * \brief This function will log an 'unexpected' warning message generated when reading information from file
   * \param mlevel is the logging level of the message to be logged
   * \param msg1 is the first (expected) message to be logged
   * \param msg2 is the second (unexpected) message to be logged
   */
  void logFileUnexpected(LogLevel mlevel, const char* msg1, const char* msg2);
  /**
   * \brief This function will set the optimise flag for the current model run
   * \param opt is the optimise flag to be set
   */
  void setRunOptimise(int opt) { runopt = opt; };
  /**
   * \brief This function will return the optimise flag for the current model run
   * \return runopt
   */
  int getRunOptimise() const { return runopt; };
  /**
   * \brief This function will set the level of logging information used for the current model run
   * \param level is the logging level to be set
   */
  void setLogLevel(int level);
  /**
   * \brief This function will return the level of logging information used for the current model run
   * \return loglevel
   */
  LogLevel getLogLevel() const { return loglevel; };
  /**
   * \brief This function will return the flag denoting whether a NaN error has been rasied or not
   * \return nanflag
   */
  int getNaNFlag() const { return nanflag; };
  /**
   * \brief This function will return the flag denoting whether a NaN error has been rasied or not
   */
  void setNaNFlag(int flag) { nanflag = flag; };
protected:
  /**
   * \brief This ofstream is the file that all the logging information will get sent to
   */
  ofstream logfile;
  /**
   * \brief This is the StrStack of the names of files that are currently open to read from
   */
  StrStack* files;
private:
  /**
   * \brief This is the flag used to denote whether a NaN error has been raised or nont
   */
  int nanflag;
  /**
   * \brief This is the flag used to denote whether the current run will optimise the model or not
   */
  int runopt;
  /**
   * \brief This is the flag used to denote whether a logfile exists or not
   */
  int uselog;
  /**
   * \brief This is the number of warning messages that gadget has displayed
   */
  int numwarn;
  /**
   * \brief This denotes what level of logging information is used for the current model run
   */
  LogLevel loglevel;
};

#endif
