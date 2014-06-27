#include "optinfo.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

OptInfoSimann::OptInfoSimann()
  : OptInfo(), rt(0.85), simanneps(1e-4), ns(5), nt(2), t(100.0), cs(2.0),
    vminit(1.0), simanniter(2000), uratio(0.7), lratio(0.3), tempcheck(4), scale(0) {
  type = OPTSIMANN;
  handle.logMessage(LOGMESSAGE, "Initialising Simulated Annealing optimisation algorithm");
}

void OptInfoSimann::read(CommentStream& infile, char* text) {
  handle.logMessage(LOGMESSAGE, "Reading Simulated Annealing optimisation parameters");

  int count = 0;
  while (!infile.eof() && strcasecmp(text, "[simann]") && strcasecmp(text, "[hooke]") && strcasecmp(text, "[bfgs]")) {
    infile >> ws;
    if (strcasecmp(text, "seed") == 0) {
      int seed = 0;
      infile >> seed >> ws;
      handle.logMessage(LOGMESSAGE, "Initialising random number generator with", seed);
      srand(seed);

    } else if (strcasecmp(text, "simanniter") == 0) {
      infile >> simanniter;
      count++;

    } else if (strcasecmp(text, "t") == 0) {
      infile >> t;
      count++;

    } else if (strcasecmp(text, "rt") == 0) {
      infile >> rt;
      count++;

    } else if (strcasecmp(text, "simanneps") == 0) {
      infile >> simanneps;
      count++;

    } else if (strcasecmp(text, "nt") == 0) {
      infile >> nt;
      count++;

    } else if (strcasecmp(text, "ns") == 0) {
      infile >> ns;
      count++;

    } else if (strcasecmp(text, "vm") == 0) {
      infile >> vminit;
      count++;

    } else if (strcasecmp(text, "cstep") == 0) {
      infile >> cs;
      count++;

    } else if (strcasecmp(text, "check") == 0) {
      infile >> tempcheck;
      count++;

    } else if (strcasecmp(text, "uratio") == 0) {
      infile >> uratio;
      count++;

    } else if (strcasecmp(text, "lratio") == 0) {
      infile >> lratio;
      count++;

    } else if (strcasecmp(text, "scale") == 0) {
      infile >> scale;
      count++;

    } else {
      handle.logMessage(LOGINFO, "Warning in optinfofile - unrecognised option", text);
      infile >> text;  //read and ignore the next entry
    }
    infile >> text;
  }

  if (count == 0)
    handle.logMessage(LOGINFO, "Warning - no parameters specified for Simulated Annealing optimisation algorithm");

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
  if (tempcheck < 1) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of check outside bounds", tempcheck);
    tempcheck = 4;
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
  if (scale != 0 && scale != 1) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of scale outside bounds", scale);
    scale = 0;
  }
}

void OptInfoSimann::Print(ofstream& outfile, int prec) {
  outfile << "; Simulated Annealing algorithm ran for " << iters
    << " function evaluations\n; and stopped when the likelihood value was "
    << setprecision(prec) << score;
  if (converge == -1)
    outfile << "\n; because an error occured during the optimisation\n";
  else if (converge == 1)
    outfile << "\n; because the convergence criteria were met\n";
  else
    outfile << "\n; because the maximum number of function evaluations was reached\n";
}
