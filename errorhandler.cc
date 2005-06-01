#include "errorhandler.h"
#include "runid.h"
#include "gadget.h"

extern RunID RUNID;

ErrorHandler::ErrorHandler() {
  files = new StrStack();
  uselog = 0;
  numwarn = 0;
  loglevel = LOGFAIL;
}

ErrorHandler::~ErrorHandler() {
  if (uselog) {
    logfile.close();
    logfile.clear();
  }
  files->clearStack();
  delete files;
}

void ErrorHandler::setLogLevel(int level) {
  switch (level) {
    case 0:
      //no messages displayed at all - only used for paramin runs
      loglevel = LOGNONE;
      break;
    case 1:
      //only get the failure messages written to std::cerr
      //gadget will exit with exit(EXIT_FAILURE) once it receives a message here
      loglevel = LOGFAIL;
      break;
    case 2:
      //also get the information messages written to std::cout
      loglevel = LOGINFO;
      break;
    case 3:
      //also get the warning messages written to std::cerr
      loglevel = LOGWARN;
      break;
    case 4:
      //also get more information messages to be written to the logfile (if it exists)
      loglevel = LOGMESSAGE;
      break;
    case 5:
      //also get more detailed messages to be written to the logfile (if it exists)
      loglevel = LOGDETAIL;
      break;
    case 6:
      //also get debug messages to be written to the logfile (if it exists)
      loglevel = LOGDEBUG;
      break;
    default:
      cerr << "Error in errorhandler - invalid log level " << level << endl;
      break;
  }
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
  RUNID.Print(logfile);
  logfile << "Log file to record Gadget actions that take place during this run"
    << endl << endl;
  logfile.flush();
  uselog = 1;
}

void ErrorHandler::Open(const char* filename) {
  if (uselog) {
    logfile << "Opening file " << filename << endl;
    logfile.flush();
  }
  files->storeString(filename);
}

void ErrorHandler::Close() {
  if (uselog) {
    char* strFilename = files->sendTop();
    logfile << "Closing file " << strFilename << endl;
    logfile.flush();
    delete[] strFilename;
  }
  files->clearString();
}

void ErrorHandler::logMessage(LogLevel mlevel, const char* msg) {
  if (mlevel > loglevel)
    return;

  switch (mlevel) {
    case LOGNONE:
      break;
    case LOGFAIL:
      if (uselog) {
        logfile << msg << endl;
        logfile.flush();
      }
      cerr << msg << endl;
      exit(EXIT_FAILURE);
      break;
    case LOGINFO:
      if (uselog) {
        logfile << msg << endl;
        logfile.flush();
      }
      cout << msg << endl;
      break;
    case LOGWARN:
      numwarn++;
      if (uselog) {
        logfile << msg << endl;
        logfile.flush();
      }
      cerr << msg << endl;
      break;
    case LOGMESSAGE:
    case LOGDETAIL:
    case LOGDEBUG:
      if (uselog) {
        logfile << msg << endl;
        logfile.flush();
      }
      break;
    default:
      cerr << "Error in errorhandler - invalid log level " << mlevel << endl;
      break;
  }
}

void ErrorHandler::logMessage(LogLevel mlevel, const char* msg1, const char* msg2) {
  if (mlevel > loglevel)
    return;

  switch (mlevel) {
    case LOGNONE:
      break;
    case LOGFAIL:
      if (uselog) {
        logfile << msg1 << sep << msg2 << endl;
        logfile.flush();
      }
      cerr << msg1 << sep << msg2 << endl;
      exit(EXIT_FAILURE);
      break;
    case LOGINFO:
      if (uselog) {
        logfile << msg1 << sep << msg2 << endl;
        logfile.flush();
      }
      cout << msg1 << sep << msg2 << endl;
      break;
    case LOGWARN:
      numwarn++;
      if (uselog) {
        logfile << msg1 << sep << msg2 << endl;
        logfile.flush();
      }
      cerr << msg1 << sep << msg2 << endl;
      break;
    case LOGMESSAGE:
    case LOGDETAIL:
    case LOGDEBUG:
      if (uselog) {
        logfile << msg1 << sep << msg2 << endl;
        logfile.flush();
      }
      break;
    default:
      cerr << "Error in errorhandler - invalid log level " << mlevel << endl;
      break;
  }
}

void ErrorHandler::logMessage(LogLevel mlevel, const char* msg, int number) {
  if (mlevel > loglevel)
    return;

  switch (mlevel) {
    case LOGNONE:
      break;
    case LOGFAIL:
      if (uselog) {
        logfile << msg << sep << number << endl;
        logfile.flush();
      }
      cerr << msg << sep << number << endl;
      exit(EXIT_FAILURE);
      break;
    case LOGINFO:
      if (uselog) {
        logfile << msg << sep << number << endl;
        logfile.flush();
      }
      cout << msg << sep << number << endl;
      break;
    case LOGWARN:
      numwarn++;
      if (uselog) {
        logfile << msg << sep << number << endl;
        logfile.flush();
      }
      cerr << msg << sep << number << endl;
      break;
    case LOGMESSAGE:
    case LOGDETAIL:
    case LOGDEBUG:
      if (uselog) {
        logfile << msg << sep << number << endl;
        logfile.flush();
      }
      break;
    default:
      cerr << "Error in errorhandler - invalid log level " << mlevel << endl;
      break;
  }
}

void ErrorHandler::logMessage(LogLevel mlevel, const char* msg, double number) {
  if (mlevel > loglevel)
    return;

  switch (mlevel) {
    case LOGNONE:
      break;
    case LOGFAIL:
      if (uselog) {
        logfile << msg << sep << number << endl;
        logfile.flush();
      }
      cerr << msg << sep << number << endl;
      exit(EXIT_FAILURE);
      break;
    case LOGINFO:
      if (uselog) {
        logfile << msg << sep << number << endl;
        logfile.flush();
      }
      cout << msg << sep << number << endl;
      break;
    case LOGWARN:
      numwarn++;
      if (uselog) {
        logfile << msg << sep << number << endl;
        logfile.flush();
      }
      cerr << msg << sep << number << endl;
      break;
    case LOGMESSAGE:
    case LOGDETAIL:
    case LOGDEBUG:
      if (uselog) {
        logfile << msg << sep << number << endl;
        logfile.flush();
      }
      break;
    default:
      cerr << "Error in errorhandler - invalid log level " << mlevel << endl;
      break;
  }
}

void ErrorHandler::logMessage(LogLevel mlevel, const char* msg1, int number, const char* msg2) {
  if (mlevel > loglevel)
    return;

  switch (mlevel) {
    case LOGNONE:
      break;
    case LOGFAIL:
      if (uselog) {
        logfile << msg1 << sep << number << sep << msg2 << endl;
        logfile.flush();
      }
      cerr << msg1 << sep << number << sep << msg2 << endl;
      exit(EXIT_FAILURE);
      break;
    case LOGINFO:
      if (uselog) {
        logfile << msg1 << sep << number << sep << msg2 << endl;
        logfile.flush();
      }
      cout << msg1 << sep << number << sep << msg2 << endl;
      break;
    case LOGWARN:
      numwarn++;
      if (uselog) {
        logfile << msg1 << sep << number << sep << msg2 << endl;
        logfile.flush();
      }
      cerr << msg1 << sep << number << sep << msg2 << endl;
      break;
    case LOGMESSAGE:
    case LOGDETAIL:
    case LOGDEBUG:
      if (uselog) {
        logfile << msg1 << sep << number << sep << msg2 << endl;
        logfile.flush();
      }
      break;
    default:
      cerr << "Error in errorhandler - invalid log level " << mlevel << endl;
      break;
  }
}

void ErrorHandler::logMessage(LogLevel mlevel, const char* msg1, double number, const char* msg2) {
  if (mlevel > loglevel)
    return;

  switch (mlevel) {
    case LOGNONE:
      break;
    case LOGFAIL:
      if (uselog) {
        logfile << msg1 << sep << number << sep << msg2 << endl;
        logfile.flush();
      }
      cerr << msg1 << sep << number << sep << msg2 << endl;
      exit(EXIT_FAILURE);
      break;
    case LOGINFO:
      if (uselog) {
        logfile << msg1 << sep << number << sep << msg2 << endl;
        logfile.flush();
      }
      cout << msg1 << sep << number << sep << msg2 << endl;
      break;
    case LOGWARN:
      numwarn++;
      if (uselog) {
        logfile << msg1 << sep << number << sep << msg2 << endl;
        logfile.flush();
      }
      cerr << msg1 << sep << number << sep << msg2 << endl;
      break;
    case LOGMESSAGE:
    case LOGDETAIL:
    case LOGDEBUG:
      if (uselog) {
        logfile << msg1 << sep << number << sep << msg2 << endl;
        logfile.flush();
      }
      break;
    default:
      cerr << "Error in errorhandler - invalid log level " << mlevel << endl;
      break;
  }
}

void ErrorHandler::logFileMessage(LogLevel mlevel, const char* msg) {
  if (mlevel > loglevel)
    return;

  char* strFilename = files->sendTop();
  switch (mlevel) {
    case LOGNONE:
    case LOGINFO:
      break;
    case LOGFAIL:
      if (uselog) {
        logfile << "Error in file " << strFilename << ":\n" << msg << endl;
        logfile.flush();
      }
      cerr << "Error in file " << strFilename << ":\n" << msg << endl;
      delete[] strFilename;
      exit(EXIT_FAILURE);
      break;
    case LOGWARN:
      numwarn++;
      if (uselog) {
        logfile << "Warning in file " << strFilename << ":\n" << msg << endl;
        logfile.flush();
      }
      cerr << "Warning in file " << strFilename << ":\n" << msg << endl;
      break;
    case LOGMESSAGE:
    case LOGDETAIL:
    case LOGDEBUG:
      if (uselog) {
        logfile << "Message in file " << strFilename << ":\n" << msg << endl;
        logfile.flush();
      }
      break;
    default:
      cerr << "Error in errorhandler - invalid log level " << mlevel << endl;
      break;
  }
  delete[] strFilename;
}

void ErrorHandler::logFileMessage(LogLevel mlevel, const char* msg1, const char* msg2) {
  if (mlevel > loglevel)
    return;

  char* strFilename = files->sendTop();
  switch (mlevel) {
    case LOGNONE:
    case LOGINFO:
      break;
    case LOGFAIL:
      if (uselog) {
        logfile << "Error in file " << strFilename << ":\n" << msg1 << sep << msg2 << endl;
        logfile.flush();
      }
      cerr << "Error in file " << strFilename << ":\n" << msg1 << sep << msg2 << endl;
      delete[] strFilename;
      exit(EXIT_FAILURE);
      break;
    case LOGWARN:
      numwarn++;
      if (uselog) {
        logfile << "Warning in file " << strFilename << ":\n" << msg1 << sep << msg2 << endl;
        logfile.flush();
      }
      cerr << "Warning in file " << strFilename << ":\n" << msg1 << sep << msg2 << endl;
      break;
    case LOGMESSAGE:
    case LOGDETAIL:
    case LOGDEBUG:
      if (uselog) {
        logfile << "Message in file " << strFilename << ":\n" << msg1 << sep << msg2 << endl;
        logfile.flush();
      }
      break;
    default:
      cerr << "Error in errorhandler - invalid log level " << mlevel << endl;
      break;
  }
  delete[] strFilename;
}

void ErrorHandler::logFileEOFMessage(LogLevel mlevel) {
  if (mlevel > loglevel)
    return;

  char* strFilename = files->sendTop();
  switch (mlevel) {
    case LOGNONE:
    case LOGINFO:
      break;
    case LOGFAIL:
      if (uselog) {
        logfile << "Unexpected end of file " << strFilename << endl;
        logfile.flush();
      }
      cerr << "Unexpected end of file " << strFilename << endl;
      delete[] strFilename;
      exit(EXIT_FAILURE);
      break;
    case LOGWARN:
      numwarn++;
      if (uselog) {
        logfile << "Unexpected end of file " << strFilename << endl;
        logfile.flush();
      }
      cerr << "Unexpected end of file " << strFilename << endl;
      break;
    case LOGMESSAGE:
    case LOGDETAIL:
    case LOGDEBUG:
      if (uselog) {
        logfile << "Unexpected end of file " << strFilename << endl;
        logfile.flush();
      }
      break;
    default:
      cerr << "Error in errorhandler - invalid log level " << mlevel << endl;
      break;
  }
  delete[] strFilename;
}

void ErrorHandler::logFileUnexpected(LogLevel mlevel, const char* msg1, const char* msg2) {
  if (mlevel > loglevel)
    return;

  char* strFilename = files->sendTop();
  switch (mlevel) {
    case LOGNONE:
    case LOGINFO:
      break;
    case LOGFAIL:
      if (uselog) {
        logfile << "Error in file " << strFilename << ":\n"
          << "Expected " << msg1 << " but found instead " << msg2 << endl;
        logfile.flush();
      }
      cerr << "Error in file " << strFilename << ":\n"
        << "Expected " << msg1 << " but found instead " << msg2 << endl;
      delete[] strFilename;
      exit(EXIT_FAILURE);
      break;
    case LOGWARN:
      numwarn++;
      if (uselog) {
        logfile << "Warning in file " << strFilename << ":\n"
          << "Expected " << msg1 << " but found instead " << msg2 << endl;
        logfile.flush();
      }
      cerr << "Warning in file " << strFilename << ":\n"
        << "Expected " << msg1 << " but found instead " << msg2 << endl;
      break;
    case LOGMESSAGE:
    case LOGDETAIL:
    case LOGDEBUG:
      if (uselog) {
        logfile << "Message in file " << strFilename << ":\n"
          << "Expected " << msg1 << " but found instead " << msg2 << endl;
        logfile.flush();
      }
      break;
    default:
      cerr << "Error in errorhandler - invalid log level " << mlevel << endl;
      break;
  }
  delete[] strFilename;
}

void ErrorHandler::checkIfFailure(ios& infile, const char* text) {
  if (uselog)
    logfile << "Checking to see if file " << text << " can be opened ... ";

  if (infile.fail()) {
    if (uselog) {
      logfile << "Failed" << endl;
      logfile.flush();
    }
    char* strFilename = files->sendTop();
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

void ErrorHandler::logFinish(int opt) {
  if (uselog) {
    if (numwarn > 0)
      logfile << "\nTotal number of warnings was " << numwarn << endl;

    if (opt)
      logfile << "\nGadget optimisation finished OK - runtime was ";
    else
      logfile << "\nGadget simulation finished OK - runtime was ";
    RUNID.printTime(logfile);
    logfile << " seconds\n";
    logfile.flush();
  }

  if (loglevel != LOGNONE) {
    if (numwarn > 0)
      cout << "\nTotal number of warnings was " << numwarn << endl;

    if (opt) {
      cout << "\nGadget optimisation finished OK - runtime was ";
      RUNID.printTime(cout);
      cout << " seconds\n";
    } else
      cout << "\nGadget simulation finished OK\n";
    cout << endl;
  }
}
