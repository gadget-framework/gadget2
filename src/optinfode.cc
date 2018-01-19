#include "optinfo.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"
#include <cfloat>

OptInfoDE::OptInfoDE()
  : OptInfo(),  goal(1e-5), iter(100000), CR(0.9), F(0.8), scale(0)
{
  type = OPTDE;
  handle.logMessage(LOGMESSAGE, "Initialising PSO optimisation algorithm");
}

void OptInfoDE::read(CommentStream& infile, char* text) {
  handle.logMessage(LOGMESSAGE, "Reading PSO optimisation parameters");

  TIME = DBL_MAX;
  VTR  = -DBL_MAX;

  int count = 0;
  while (!infile.eof() && strcasecmp(text, "[PSO]") && strcasecmp(text, "[simann]") && strcasecmp(text, "[hooke]") && strcasecmp(text, "[bfgs]") && strcasecmp(text, "[DE]")) {
    infile >> ws;
    if (strcasecmp(text, "seed") == 0) {
      int s = 0;
      infile >> s >> ws;
      handle.logMessage(LOGMESSAGE, "Initialising random number generator with", s);
      srand(s);
    } else if   (strcasecmp(text, "iter") == 0) {
      infile >> iter;
      count++;
    } else if (strcasecmp(text, "scale") == 0) {
        infile >> scale;
        count++;
    } else if (strcasecmp(text, "time") == 0) {
      infile >> TIME;
      count++;
    } else if (strcasecmp(text, "vtr") == 0) {
      infile >> VTR;
      count++;
    } else {
      handle.logMessage(LOGINFO, "Warning in optinfofile - unrecognised option", text);
      infile >> text;  //read and ignore the next entry
    }
    infile >> text;
  }

  if (count == 0)
    handle.logMessage(LOGINFO, "Warning - no parameters specified for Simulated Annealing optimisation algorithm");

  if (scale != 0 && scale != 1) {
      handle.logMessage(LOGINFO, "Warning in optinfofile - value of scale outside bounds", scale);
      scale = 0;
    }
}

void OptInfoDE::Print(ofstream& outfile, int prec) {
  outfile << "; DE algorithm ran for " << iters
    << " function evaluations\n; and stopped when the likelihood value was "
    << setprecision(prec) << score;
  if (converge == -1)
    outfile << "\n; because an error occured during the optimisation\n";
  else if (converge == 1)
    outfile << "\n; because the convergence criteria were met\n";
  else
    outfile << "\n; because the maximum number of function evaluations was reached\n";
}
