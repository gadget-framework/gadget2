#ifndef errorhandler_h
#define errorhandler_h

#include "strstack.h"
#include "gadget.h"

class ErrorHandler {
public:
  ErrorHandler() {};
  ~ErrorHandler() {};
  static void Open(const char* file) { files.PutInStack(file); };
  static void Close() { files.OutOfStack(); };
  void Message(const char*) const;
  void Unexpected(const char*, const char*) const;
  void Eof() const;
  void Eof(const char*) const;
  void UndefinedArea(int) const;
  void Failure(const char*) const;
  void Failure() const;
  void Warning(const char*) const;
  void Warning() const;
private:
  static StrStack files;
};

void CheckIfFailure(ios& infile, const char* text);

#endif
