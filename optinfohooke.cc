#include "optinfo.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

OptInfoHooke::OptInfoHooke()
  : OptSearch(), hookeiter(1000), rho(0.5), lambda(0.0), hookeeps(1e-4), bndcheck(0.9999) {
  handle.logMessage(LOGMESSAGE, "\nInitialising Hooke & Jeeves optimisation algorithm");
}

void OptInfoHooke::read(CommentStream& infile, char* text) {
  handle.logMessage(LOGMESSAGE, "Reading Hooke & Jeeves optimisation parameters");
  while (!infile.eof() && strcasecmp(text, "seed") && strcasecmp(text, "[simann]") && strcasecmp(text, "[bfgs]")) {
    infile >> ws;
    if (strcasecmp(text, "rho") == 0) {
      infile >> rho;

    } else if (strcasecmp(text, "lambda") == 0) {
      infile >> lambda;

    } else if (strcasecmp(text, "hookeeps") == 0) {
      infile >> hookeeps;

    } else if (strcasecmp(text, "hookeiter") == 0) {
      infile >> hookeiter;

    } else if (strcasecmp(text, "bndcheck") == 0) {
      infile >> bndcheck;

    } else {
      handle.logMessage(LOGINFO, "Warning in optinfofile - unrecognised option", text);
      infile >> text;  //read and ignore the next entry
    }
    infile >> text;
  }

  //check the values specified in the optinfo file ...
  if ((rho < rathersmall) || (rho > 1)) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of rho outside bounds", rho);
    rho = 0.5;
  }
  if ((lambda < 0) || (lambda > 1)) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of lambda outside bounds", lambda);
    lambda = rho;
  }
  if ((bndcheck < 0.5) || (bndcheck > 1)) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of bndcheck outside bounds", bndcheck);
    bndcheck = 0.9999;
  }
  if (hookeeps < rathersmall) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of hookeeps outside bounds", hookeeps);
    hookeeps = 1e-4;
  }
}
