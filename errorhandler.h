#ifndef errorhandler_h
#define errorhandler_h

#include "strstack.h"
#include "gadget.h"

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
   * \brief This function will log a message to a log file if one exists
   * \param msg is the message to be logged
   */
  void logMessage(const char* msg);
  /**
   * \brief This function will log a message and a number to a log file if one exists
   * \param msg is the message to be logged
   * \param number is the number to be logged
   */
  void logMessage(const char* msg, int number);
  /**
   * \brief This function will log a message and a number to a log file if one exists
   * \param msg is the message to be logged
   * \param number is the number to be logged
   */
  void logMessage(const char* msg, double number);
  /**
   * \brief This function will log two messages to a log file if one exists
   * \param msg1 is the first message to be logged
   * \param msg2 is the second message to be logged
   */
  void logMessage(const char* msg1, const char* msg2);
  /**
   * \brief This function will log a message to std::cout and a log file if one exists
   * \param msg is the message to be logged
   */
  void logInformation(const char* msg);
  /**
   * \brief This function will log a message and a number to std::cout and a log file if one exists
   * \param msg is the message to be logged
   * \param number is the number to be logged
   */
  void logInformation(const char* msg, int number);
  /**
   * \brief This function will log a message and a number to std::cout and a log file if one exists
   * \param msg is the message to be logged
   * \param number is the number to be logged
   */
  void logInformation(const char* msg, double number);
  /**
   * \brief This function will log two messages to std::cout and a log file if one exists
   * \param msg1 is the first message to be logged
   * \param msg2 is the second message to be logged
   */
  void logInformation(const char* msg1, const char* msg2);
  /**
   * \brief This function will log a warning message to std::cerr and a log file if one exists
   * \param msg is the message to be logged
   */
  void logWarning(const char* msg);
  /**
   * \brief This function will log a warning message and a number to std::cerr and a log file if one exists
   * \param msg is the message to be logged
   * \param number is the number to be logged
   */
  void logWarning(const char* msg, int number);
  /**
   * \brief This function will log a warning message and a number to std::cerr and a log file if one exists
   * \param msg is the message to be logged
   * \param number is the number to be logged
   */
  void logWarning(const char* msg, double number);
  /**
   * \brief This function will log two warning messages to std::cerr and a log file if one exists
   * \param msg1 is the first message to be logged
   * \param msg2 is the second message to be logged
   */
  void logWarning(const char* msg1, const char* msg2);
  /**
   * \brief This function will log an error message to std::cerr and a log file if one exists, and then exit Gadget with exit(EXIT_FAILURE)
   * \param msg is the message to be logged
   */
  void logFailure(const char* msg);
  /**
   * \brief This function will log an error message and a number to std::cerr and a log file if one exists, and then exit Gadget with exit(EXIT_FAILURE)
   * \param msg is the message to be logged
   * \param number is the number to be logged
   */
  void logFailure(const char* msg, int number);
  /**
   * \brief This function will log an error message and a number to std::cerr and a log file if one exists, and then exit Gadget with exit(EXIT_FAILURE)
   * \param msg is the message to be logged
   * \param number is the number to be logged
   */
  void logFailure(const char* msg, double number);
  /**
   * \brief This function will log two errors message to std::cerr and a log file if one exists, and then exit Gadget with exit(EXIT_FAILURE)
   * \param msg1 is the first message to be logged
   * \param msg2 is the second message to be logged
   */
  void logFailure(const char* msg1, const char* msg2);
  /**
   * \brief This function will log an "error in file" message to std::cerr and a log file if one exists, and then exit Gadget with exit(EXIT_FAILURE)
   * \param msg is the message to be logged
   */
  void Message(const char* msg);
  /**
   * \brief This function will log two "error in file" messages to std::cerr and a log file if one exists, and then exit Gadget with exit(EXIT_FAILURE)
   * \param msg1 is the first message to be logged
   * \param msg2 is the second message to be logged
   */
  void Message(const char* msg1, const char* msg2);
  /**
   * \brief This function will log an "unexpected text" error message to std::cerr and a log file if one exists, and then exit Gadget with exit(EXIT_FAILURE)
   * \param exp is the text that was expected
   * \param unexp is the text that was found
   */
  void Unexpected(const char* exp, const char* unexp);
  /**
   * \brief This function will log an "end of file" error message to std::cerr and a log file if one exists, and then exit Gadget with exit(EXIT_FAILURE)
   */
  void Eof();
  /**
   * \brief This function will log an "undefined area" message to std::cerr and a log file if one exists, and then exit Gadget with exit(EXIT_FAILURE)
   * \param area is the undefined area
   */
  void UndefinedArea(int area);
  /**
   * \brief This function will log a "file failed" error message to std::cerr and a log file if one exists, and then exit Gadget with exit(EXIT_FAILURE)
   */
  void Failure();
  /**
   * \brief This function will log a "file warning" message to std::cerr and a log file if one exists
   * \param msg is the message to be logged
   */
  void Warning(const char* msg);
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
   * \brief This function will log information about the simulation finish to std::cout and a log file if one exists
   * \param printtime is a flag to denote whether to print the runtime or not
   */
  void logFinish(int printtime);
  /**
   * \brief This function will set a flag to denote whether gadget is running in network mode for paramin or not
   * \param runnetwork is a flag to denote whether gadget is running in network mode for paramin or not
   * \note if gadget is running in network mode for paramin, most of the messages that are written to std::cerr or std::cout will be supressed to avoid having multiple copies of the messages.  Gadget will only display messages that cause a crash - most of these will be caused by incorrect initial input files.
   */
  void setNetwork(int runnetwork);
protected:
  /**
   * \brief This ofstream is the file that all the logging information will get sent to
   */
  ofstream logfile;
  /**
   * \brief This is the flag used to denote whether a logfile exists or not
   */
  int uselog;
  /**
   * \brief This is the flag used to denote whether gadget is running in network mode for paramin or not
   */
  int network;
  /**
   * \brief This is the StrStack of the names of files that are currently open to read from
   */
  StrStack* files;
};

#endif
