#ifndef errorhandler_h
#define errorhandler_h

#include "strstack.h"
#include "gadget.h"

class ErrorHandler {
public:
  ErrorHandler();
  ~ErrorHandler();
  void Open(const char* file);
  void Close();
  void LogMessage(const char* msg);
  void LogMessage(const char* msg, int number);
  void LogMessage(const char* msg, double number);
  void LogMessage(const char* msg1, const char* msg2);
  void LogWarning(const char* msg);
  void LogWarning(const char* msg, int number);
  void LogWarning(const char* msg, double number);
  void LogWarning(const char* msg, double num1, double num2);
  void LogWarning(const char* msg1, const char* msg2);
  void Message(const char* msg);
  void Message(const char* msg1, const char* msg2);
  void Unexpected(const char* exp, const char* unexp);
  void Eof();
  void UndefinedArea(int area);
  void Failure();
  void Warning(const char* msg);
  void checkIfFailure(ios& infile, const char* text);
  void setLogFile(const char* filename);
  int checkLogFile() { return uselog; };
protected:
  ofstream logfile;
  int uselog;
  StrStack* files;
};

#endif
