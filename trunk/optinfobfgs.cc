#include "optinfo.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

OptInfoBFGS::OptInfoBFGS()
  : OptInfo(), bfgsiter(10000), bfgseps(0.01), beta(0.3), sigma(0.01),
    step(1.0), gradacc(1e-6), gradstep(0.5), gradeps(1e-10) {
  type = OPTBFGS;
  handle.logMessage(LOGMESSAGE, "Initialising BFGS optimisation algorithm");
}

void OptInfoBFGS::read(CommentStream& infile, char* text) {
  handle.logMessage(LOGMESSAGE, "Reading BFGS optimisation parameters");

  int count = 0;
  while (!infile.eof() && strcasecmp(text, "[simann]") && strcasecmp(text, "[hooke]") && strcasecmp(text, "[bfgs]")) {
    infile >> ws;
    if (strcasecmp(text, "seed") == 0) {
      int seed = 0;
      infile >> seed >> ws;
      handle.logMessage(LOGMESSAGE, "Initialising random number generator with", seed);
      srand(seed);

    } else if (strcasecmp(text,"beta") == 0) {
      infile >> beta;
      count++;

    } else if (strcasecmp(text, "sigma") == 0) {
      infile >> sigma;
      count++;

    } else if (strcasecmp(text, "step") == 0) {
      infile >> step;
      count++;

    } else if (strcasecmp(text, "gradacc") == 0) {
      infile >> gradacc;
      count++;

    } else if (strcasecmp(text, "gradstep") == 0) {
      infile >> gradstep;
      count++;

    } else if (strcasecmp(text, "gradeps") == 0) {
      infile >> gradeps;
      count++;

    } else if ((strcasecmp(text, "bfgsiter") == 0) || (strcasecmp(text, "maxiter") == 0)) {
      infile >> bfgsiter;
      count++;

    } else if (strcasecmp(text, "bfgseps") == 0) {
      infile >> bfgseps;
      count++;

    } else {
      handle.logMessage(LOGINFO, "Warning in optinfofile - unrecognised option", text);
      infile >> text;  //read and ignore the next entry
    }
    infile >> text;
  }

  if (count == 0)
    handle.logMessage(LOGINFO, "Warning - no parameters specified for BFGS optimisation algorithm");

  //check the values specified in the optinfo file ...
  if ((beta < rathersmall) || (beta > 1.0)) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of beta outside bounds", beta);
    beta = 0.3;
  }
  if ((sigma < rathersmall)|| (sigma > 1.0)) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of sigma outside bounds", sigma);
    sigma = 0.01;
  }
  if (step < 1.0) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of step outside bounds", step);
    step = 1.0;
  }
  if (bfgseps < rathersmall) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of bfgseps outside bounds", bfgseps);
    bfgseps = 0.01;
  }
  if ((gradacc < rathersmall) || (gradacc > 1.0)) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of gradacc outside bounds", gradacc);
    gradacc = 1e-6;
  }
  if ((gradstep < rathersmall) || (gradstep > 1.0)) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of gradstep outside bounds", gradstep);
    gradstep = 0.5;
  }
  if ((gradeps < verysmall) || (gradeps > gradacc)) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of gradeps outside bounds", gradeps);
    gradeps = 1e-10;
  }
}

void OptInfoBFGS::Print(ofstream& outfile, int prec) {
  outfile << "; BFGS algorithm ran for " << iters
    << " function evaluations\n; and stopped when the likelihood value was "
    << setprecision(prec) << score;
  if (converge == -1)
    outfile << "\n; because an error occured during the optimisation\n";
  else if (converge == 1)
    outfile << "\n; because the convergence criteria were met\n";
  else if (converge == 2)
    outfile << "\n; because the accuracy limit for the gradient calculation was reached\n";
  else
    outfile << "\n; because the maximum number of function evaluations was reached\n";
}
