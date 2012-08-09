#include "optinfo.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

OptInfoHooke::OptInfoHooke()
  : OptInfo(), hookeiter(1000), rho(0.5), lambda(0.0), hookeeps(1e-4), bndcheck(0.9999) {
  type = OPTHOOKE;
  handle.logMessage(LOGMESSAGE, "Initialising Hooke & Jeeves optimisation algorithm");
}

void OptInfoHooke::read(CommentStream& infile, char* text) {
  handle.logMessage(LOGMESSAGE, "Reading Hooke & Jeeves optimisation parameters");

  int count = 0;
  while (!infile.eof() && strcasecmp(text, "[simann]") && strcasecmp(text, "[hooke]") && strcasecmp(text, "[bfgs]")) {
    infile >> ws;
    if (strcasecmp(text, "seed") == 0) {
      int seed = 0;
      infile >> seed >> ws;
      handle.logMessage(LOGMESSAGE, "Initialising random number generator with", seed);
      srand(seed);

    } else if (strcasecmp(text, "rho") == 0) {
      infile >> rho;
      count++;

    } else if (strcasecmp(text, "lambda") == 0) {
      infile >> lambda;
      count++;

    } else if (strcasecmp(text, "hookeeps") == 0) {
      infile >> hookeeps;
      count++;

    } else if (strcasecmp(text, "hookeiter") == 0) {
      infile >> hookeiter;
      count++;

    } else if (strcasecmp(text, "bndcheck") == 0) {
      infile >> bndcheck;
      count++;

    } else {
      handle.logMessage(LOGINFO, "Warning in optinfofile - unrecognised option", text);
      infile >> text;  //read and ignore the next entry
    }
    infile >> text;
  }

  if (count == 0)
    handle.logMessage(LOGINFO, "Warning - no parameters specified for Hooke & Jeeves optimisation algorithm");

  //check the values specified in the optinfo file ...
  if ((rho < rathersmall) || (rho > 1.0)) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of rho outside bounds", rho);
    rho = 0.5;
  }
  if ((lambda < 0.0) || (lambda > 1.0)) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of lambda outside bounds", lambda);
    lambda = rho;
  }
  if ((bndcheck < 0.5) || (bndcheck > 1.0)) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of bndcheck outside bounds", bndcheck);
    bndcheck = 0.9999;
  }
  if (hookeeps < rathersmall) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of hookeeps outside bounds", hookeeps);
    hookeeps = 1e-4;
  }
}

void OptInfoHooke::Print(ofstream& outfile, int prec) {
  outfile << "; Hooke & Jeeves algorithm ran for " << iters
    << " function evaluations\n; and stopped when the likelihood value was "
    << setprecision(prec) << score;
  if (converge == -1)
    outfile << "\n; because an error occured during the optimisation\n";
  else if (converge == 1)
    outfile << "\n; because the convergence criteria were met\n";
  else
    outfile << "\n; because the maximum number of function evaluations was reached\n";
}
