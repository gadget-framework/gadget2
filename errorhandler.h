#ifndef errorhandler_h
#define errorhandler_h

#include "strstack.h"
#include "gadget.h"

class ErrorHandler {
public:
  ErrorHandler() {};
  ~ErrorHandler() {};
  void Open(const char* file) { 
    files.PutInStack(file); 
    //    WriteOpenToLogFile(*file);
    logfile.open(".gadget.log",ios::app);
    logfile << "Opening" << TAB << file << endl;
    logfile.close();
  };
  void Close() { 
    files.OutOfStack();
    //    WriteCloseToLogfile();
    logfile.open(".gadget.log",ios::app);
    logfile << "closing" << endl;
    logfile.close();
  };
  void Message(const char*) const;
  void Message(const char*, const char*) const;
  void Unexpected(const char*, const char*) const;
  void Eof() const;
  void Eof(const char*) const;
  void UndefinedArea(int) const;
  void Failure(const char*) const;
  void Failure() const;
  void Warning(const char*) const;
  void Warning() const;
private:
  ofstream logfile;
  static StrStack files;
};

void checkIfFailure(ios& infile, const char* text);

#endif
