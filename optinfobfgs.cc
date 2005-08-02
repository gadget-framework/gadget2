#include "optinfo.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

OptInfoBFGS::OptInfoBFGS()
  : OptSearch(), bfgsiter(10000), bfgseps(0.01), beta(0.3), sigma(0.01),
    step(1.0), gradacc(1e-6), gradstep(0.5) {
  handle.logMessage(LOGMESSAGE, "\nInitialising BFGS optimisation algorithm");
}

void OptInfoBFGS::read(CommentStream& infile, char* text) {
  handle.logMessage(LOGMESSAGE, "Reading BFGS optimisation parameters");
  while (!infile.eof() && strcasecmp(text, "seed") && strcasecmp(text, "[simann]") && strcasecmp(text, "[hooke]")) {
    infile >> ws;
    if (strcasecmp(text,"beta") == 0) {
      infile >> beta;

    } else if (strcasecmp(text, "sigma") == 0) {
      infile >> sigma;

    } else if (strcasecmp(text, "step") == 0) {
      infile >> step;

    } else if (strcasecmp(text, "gradacc") == 0) {
      infile >> gradacc;

    } else if (strcasecmp(text, "gradstep") == 0) {
      infile >> gradstep;

    } else if ((strcasecmp(text, "bfgsiter") == 0) || (strcasecmp(text, "maxiter") == 0)) {
      infile >> bfgsiter;

    } else if (strcasecmp(text, "bfgseps") == 0) {
      infile >> bfgseps;

    } else {
      handle.logMessage(LOGINFO, "Warning in optinfofile - unrecognised option", text);
      infile >> text;  //read and ignore the next entry
    }
    infile >> text;
  }

  //check the values specified in the optinfo file ...
  if ((beta < rathersmall) || (beta > 1)) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of beta outside bounds", beta);
    beta = 0.3;
  }
  if ((sigma < rathersmall)|| (sigma > 1)) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of sigma outside bounds", sigma);
    sigma = 0.01;
  }
  if (step < 1) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of step outside bounds", step);
    step = 1.0;
  }
  if (bfgseps < rathersmall) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of bfgseps outside bounds", bfgseps);
    bfgseps = 0.01;
  }
  if ((gradacc < rathersmall) || (gradacc > 1)) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of gradacc outside bounds", gradacc);
    gradacc = 1e-6;
  }
  if ((gradstep < rathersmall) || (gradstep > 1)) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of gradstep outside bounds", gradstep);
    gradstep = 0.5;
  }
}
