#include "errorhandler.h"
#include "runid.h"
#include "keeper.h"
#include "gadget.h"

extern RunID RUNID;

ErrorHandler::ErrorHandler() {
  files = new StrStack();
  uselog = 0;
}

ErrorHandler::~ErrorHandler() {
  if (uselog) {
    logfile.close();
    logfile.clear();
  }
  files->ClearStack();
  delete files;
}

void ErrorHandler::setLogFile(const char* filename) {
  if (uselog) {
    logfile.close();
    logfile.clear();
  }
  logfile.open(filename, ios::out);
  if (logfile.fail()) {
    cerr << "Error - unable to open log file " << filename << endl;
    exit(EXIT_FAILURE);
  }
  RUNID.print(logfile);
  logfile << "Log file to record Gadget actions that take place during this run"
    << endl << endl;
  logfile.flush();
  uselog = 1;
}

void ErrorHandler::Open(const char* file) {
  if (uselog) {
    logfile << "Opening file " << file << endl;
    logfile.flush();
  }
  files->PutInStack(file);
}

void ErrorHandler::Close() {
  if (uselog) {
    char* strFilename = files->SendTop();
    logfile << "Closing file " << strFilename << endl;
    logfile.flush();
    delete[] strFilename;
  }
  files->OutOfStack();
}

void ErrorHandler::LogMessage(const char* msg) {
  if (uselog) {
    logfile << msg << endl;
    logfile.flush();
  }
}

void ErrorHandler::LogMessage(const char* msg, int number) {
  if (uselog) {
    logfile << msg << sep << number << endl;
    logfile.flush();
  }
}

void ErrorHandler::LogMessage(const char* msg, double number) {
  if (uselog) {
    logfile << msg << sep << number << endl;
    logfile.flush();
  }
}

void ErrorHandler::LogMessage(const char* msg1, const char* msg2) {
  if (uselog) {
    logfile << msg1 << sep << msg2 << endl;
    logfile.flush();
  }
}

void ErrorHandler::LogWarning(const char* msg) {
  if (uselog) {
    logfile << msg << endl;
    logfile.flush();
  }
  cerr << msg << endl;
}

void ErrorHandler::LogWarning(const char* msg, int number) {
  if (uselog) {
    logfile << msg << sep << number << endl;
    logfile.flush();
  }
  cerr << msg << sep << number << endl;
}

void ErrorHandler::LogWarning(const char* msg, double number) {
  if (uselog) {
    logfile << msg << sep << number << endl;
    logfile.flush();
  }
  cerr << msg << sep << number << endl;
}

void ErrorHandler::LogWarning(const char* msg, double num1, double num2) {
  if (uselog) {
    logfile << msg << sep << num1 << sep << num2 << endl;
    logfile.flush();
  }
  cerr << msg << sep << num1 << sep << num2 << endl;
}

void ErrorHandler::LogWarning(const char* msg1, const char* msg2) {
  if (uselog) {
    logfile << msg1 << sep << msg2 << endl;
    logfile.flush();
  }
  cerr << msg1 << sep << msg2 << endl;
}

void ErrorHandler::Message(const char* msg) {
  char* strFilename = files->SendTop();
  if (uselog) {
    logfile << "Error in file " << strFilename << ":\n" << msg << endl;
    logfile.flush();
  }
  cerr << "Error in file " << strFilename << ":\n" << msg << endl;
  delete[] strFilename;
  exit(EXIT_FAILURE);
}

void ErrorHandler::Message(const char* msg1, const char* msg2) {
  char* strFilename = files->SendTop();
  if (uselog) {
    logfile << "Error in file " << strFilename << ":\n" << msg1 << sep << msg2 << endl;
    logfile.flush();
  }
  cerr << "Error in file " << strFilename << ":\n" << msg1 << sep << msg2 << endl;
  delete[] strFilename;
  exit(EXIT_FAILURE);
}

void ErrorHandler::Unexpected(const char* exp, const char* unexp) {
  char* strFilename = files->SendTop();
  if (uselog) {
    logfile << "Error in file " << strFilename << ":\n"
      << "Expected " << exp << " but found instead " << unexp << endl;
    logfile.flush();
  }
  cerr << "Error in file " << strFilename << ":\n"
    << "Expected " << exp << " but found instead " << unexp << endl;
  delete[] strFilename;
  exit(EXIT_FAILURE);
}

void ErrorHandler::Warning(const char* msg) {
  char* strFilename = files->SendTop();
  if (uselog) {
    logfile << "Warning in file " << strFilename << ": " << msg << endl;
    logfile.flush();
  }
  cerr << "Warning in file " << strFilename << ": " << msg << endl;
  delete[] strFilename;
}

void ErrorHandler::Eof() {
  char* strFilename = files->SendTop();
  if (uselog) {
    logfile << "Unexpected end of file " << strFilename << endl;
    logfile.flush();
  }
  cerr << "Unexpected end of file " << strFilename << endl;
  delete[] strFilename;
  exit(EXIT_FAILURE);
}

void ErrorHandler::Failure() {
  char* strFilename = files->SendTop();
  if (uselog) {
    logfile << "Failure in file " << strFilename << endl;
    logfile.flush();
  }
  cerr << "Failure in file " << strFilename << endl;
  delete[] strFilename;
  exit(EXIT_FAILURE);
}

void ErrorHandler::UndefinedArea(int area) {
  char* strFilename = files->SendTop();
  if (uselog) {
    logfile << "Error in file " << strFilename << ": Undefined area " << area << endl;
    logfile.flush();
  }
  cerr << "Error in file " << strFilename << ": Undefined area " << area << endl;
  delete[] strFilename;
  exit(EXIT_FAILURE);
}

void ErrorHandler::checkIfFailure(ios& infile, const char* text) {
  if (uselog)
    logfile << "Checking to see if file " << text << " can be opened ... ";

  if (infile.fail()) {
    char* strFilename = files->SendTop();
    if (uselog) {
      logfile << "Failed" << endl;
      logfile.flush();
    }
    cerr << "Error in file " << strFilename << ":\nUnable to open datafile " << text << endl;
    delete[] strFilename;
    exit(EXIT_FAILURE);
  }

  if (uselog) {
    logfile << "OK" << endl;
    logfile.flush();
  }
  return;
}
