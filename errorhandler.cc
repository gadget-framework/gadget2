#include "errorhandler.h"
#include "keeper.h"
#include "gadget.h"

StrStack ErrorHandler::files;

void ErrorHandler::Message(const char* msg) const {
  cerr << "Error in file " << files.SendTop() << ":\n" << msg << endl;
  exit(EXIT_FAILURE);
}

void ErrorHandler::Message(const char* msg1, const char* msg2) const {
  cerr << "Error in file " << files.SendTop() << ":\n" << msg1 << sep << msg2 << endl;
  exit(EXIT_FAILURE);
}

void ErrorHandler::Unexpected(const char* exp, const char* unexp) const {
  cerr << "Error in file " << files.SendTop() << ":\n"
    << "Expected " << exp << " but found instead " << unexp << endl;
  exit(EXIT_FAILURE);
}

void ErrorHandler::Warning() const {
  cerr << "Warning in file " << files.SendTop() << endl;
}

void ErrorHandler::Warning(const char* msg) const {
  cerr << "Warning in file " << files.SendTop() << ": " << msg << endl;
}

void ErrorHandler::Eof() const {
  cerr << "Unexpected end of file " << files.SendTop() << endl;
  exit(EXIT_FAILURE);
}

void ErrorHandler::Eof(const char* exp) const {
  cerr << "Unexpected end of file " << files.SendTop()
    << " -- expected " << exp << " etc\n";
  exit(EXIT_FAILURE);
}

void ErrorHandler::Failure() const {
  cerr << "Failure in file " << files.SendTop() << endl;
  exit(EXIT_FAILURE);
}

void ErrorHandler::Failure(const char* variable) const {
  cerr << "Failure in file " << files.SendTop()
    << " when reading information on " << variable << endl;
  exit(EXIT_FAILURE);
}

void ErrorHandler::UndefinedArea(int area) const {
  cerr << "Error in file " << files.SendTop() << ": Undefined area " << area << endl;
  exit(EXIT_FAILURE);
}

void checkIfFailure(ios& infile, const char* text) {

  if (infile.fail()) {
    ErrorHandler handle;
    char errormsg[MaxStrLength];
    strncpy(errormsg, "", MaxStrLength);
    strcpy(errormsg, "Unable to open datafile ");
    strcat(errormsg, text);
    handle.Message(errormsg);
  }
  return;
}
