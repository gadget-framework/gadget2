#include "optinfo.h"
#include "ecosystem.h"
#include "gadget.h"

extern Ecosystem* EcoSystem;
extern ErrorHandler handle;

extern int bfgs(int maxiter, double epsilon, double beta, double sigma, double step,
  double gradacc, double gradstep, double errortol, int diffgrad, int scale);

OptInfoBFGS::OptInfoBFGS()
  : OptSearch(), bfgsiter(10000), bfgseps(0.01), beta(0.3), sigma(0.01), step(1.0),
    gradacc(0.01), gradstep(0.1), errortol(1e-6), diffgrad(-1), scale(0) {
  handle.logMessage(LOGMESSAGE, "\nInitialising BFGS optimisation algorithm");
}

void OptInfoBFGS::read(CommentStream& infile, char* text) {
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

    } else if (strcasecmp(text, "scale") == 0) {
      infile >> scale;

    } else if (strcasecmp(text, "errortol") == 0) {
      infile >> errortol;

    } else if (strcasecmp(text, "difficultgrad") == 0) {
      infile >> diffgrad;

    } else if (strcasecmp(text, "printing") == 0) {
      handle.logMessage(LOGINFO, "Warning in optinfofile - bfgsprinting is not implemented in gadget");
      infile >> text;  //read and ignore the next entry

    } else if (strcasecmp(text, "shannonscaling") == 0) {
      handle.logMessage(LOGINFO, "Warning in optinfofile - shannonscaling is not implemented in gadget");
      infile >> text;  //read and ignore the next entry

    } else if (strcasecmp(text, "bfgspar") == 0) {
      handle.logMessage(LOGINFO, "Warning in optinfofile - bfgspar is not implemented gadget");
      infile >> text;  //read and ignore the next entry

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
    gradacc = 0.0001;
  }
  if ((gradstep < rathersmall) || (gradstep > 1)) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of gradstep outside bounds", gradstep);
    gradstep = 0.1;
  }
  if (errortol < rathersmall) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of errortol outside bounds", errortol);
    errortol = 1e-6;
  }
  if (scale != 0 && scale != 1) {
    handle.logMessage(LOGINFO, "Warning in optinfofile - value of scale outside bounds", scale);
    scale = 0;
  }
}

void OptInfoBFGS::OptimiseLikelihood() {
  handle.logMessage(LOGINFO, "\nStarting BFGS optimisation algorithm\n");
  int opt = bfgs(bfgsiter, bfgseps, beta, sigma, step, gradacc, gradstep, errortol, diffgrad, scale);
  handle.logMessage(LOGINFO, "\nBFGS finished with a final likelihood score of", EcoSystem->getLikelihood());
  handle.logMessage(LOGINFO, "after a total of", EcoSystem->getFuncEval(), "function evaluations at the point");
  EcoSystem->writeOptValues();
}
