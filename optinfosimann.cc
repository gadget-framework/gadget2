#include "optinfo.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

OptInfoSimann::OptInfoSimann()
  : OptSearch(), rt(0.85), simanneps(1e-4), ns(5), nt(2), t(100.0),
    cs(2.0), vminit(1.0), simanniter(2000), uratio(0.7), lratio(0.3), check(4) {
  handle.logMessage(LOGMESSAGE, "\nInitialising Simulated Annealing optimisation algorithm");
}

void OptInfoSimann::read(CommentStream& infile, char* text) {
  handle.logMessage(LOGMESSAGE, "Reading Simulated Annealing optimisation parameters");

  while (!infile.eof() && strcasecmp(text, "seed") && strcasecmp(text, "[hooke]") && strcasecmp(text, "[bfgs]")) {
    infile >> ws;
    if (strcasecmp(text, "simanniter") == 0) {
      infile >> simanniter;

    } else if (strcasecmp(text, "t") == 0) {
      infile >> t;

    } else if (strcasecmp(text, "rt") == 0) {
      infile >> rt;

    } else if (strcasecmp(text, "simanneps") == 0) {
      infile >> simanneps;

    } else if (strcasecmp(text, "nt") == 0) {
      infile >> nt;

    } else if (strcasecmp(text, "ns") == 0) {
      infile >> ns;

    } else if (strcasecmp(text, "vm") == 0) {
      infile >> vminit;

    } else if (strcasecmp(text, "cstep") == 0) {
      infile >> cs;

    } else if (strcasecmp(text, "check") == 0) {
      infile >> check;

    } else if (strcasecmp(text, "uratio") == 0) {
      infile >> uratio;

    } else if (strcasecmp(text, "lratio") == 0) {
      infile >> lratio;

    } else if (strcasecmp(text, "[simann]") == 0) {
      handle.logMessage(LOGINFO, "Warning in optinfofile - repeated definition of Simulated Annealing parameters");

    } else {
      handle.logMessage(LOGINFO, "Warning in optinfofile - unrecognised option", text);
      infile >> text;  //read and ignore the next entry
    }
    infile >> text;
  }

  //check the values specified in the optinfo file ...
  if ((uratio < 0.5) || (uratio > 1.0)) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of uratio outside bounds", uratio);
    uratio = 0.7;
  }
  if ((lratio < rathersmall) || (lratio > 0.5)) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of lratio outside bounds", lratio);
    lratio = 0.3;
  }
  if ((rt < rathersmall) || (rt > 1.0)) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of rt outside bounds", rt);
    rt = 0.85;
  }
  if (t < rathersmall) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of t outside bounds", t);
    t = 100.0;
  }
  if (nt < 1) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of nt outside bounds", nt);
    nt = 2;
  }
  if (ns < 1) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of ns outside bounds", ns);
    ns = 5;
  }
  if (vminit < rathersmall) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of vm outside bounds", vminit);
    vminit = 1.0;
  }
  if (cs < rathersmall) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of cstep outside bounds", cs);
    cs = 2.0;
  }
  if (simanneps < rathersmall) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of simanneps outside bounds", simanneps);
    simanneps = 1e-4;
  }
}
