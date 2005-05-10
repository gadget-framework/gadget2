#include "optinfo.h"
#include "ecosystem.h"
#include "gadget.h"

extern ErrorHandler handle;
extern Ecosystem* EcoSystem;

extern int hooke(double rho, double lambda, double epsilon, int itermax, double bndcheck);

OptInfoHooke::OptInfoHooke()
  : OptSearch(), hookeiter(1000), rho(0.5), lambda(0.0), hookeeps(1e-4), bndcheck(0.9999) {
  handle.logMessage(LOGMESSAGE, "\nInitialising Hooke & Jeeves optimisation algorithm");
}

void OptInfoHooke::read(CommentStream& infile, char* text) {
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

void OptInfoHooke::OptimiseLikelihood() {
  handle.logMessage(LOGINFO, "\nStarting Hooke & Jeeves optimisation algorithm\n");
  int opt = hooke(rho, lambda, hookeeps, hookeiter, bndcheck);
  handle.logMessage(LOGINFO, "\nHooke & Jeeves finished with a final likelihood score of", EcoSystem->getLikelihood());
  handle.logMessage(LOGINFO, "after a total of", EcoSystem->getFuncEval(), "function evaluations at the point");
  EcoSystem->writeOptValues();
}
