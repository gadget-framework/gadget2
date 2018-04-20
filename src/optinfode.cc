#include "optinfo.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"
#include <cfloat>

OptInfoDE::OptInfoDE()
  : OptInfo(),  goal(1e-5), iter(100000) {
  type = OPTDE;
  handle.logMessage(LOGMESSAGE, "Initialising DE optimisation algorithm");
}

void OptInfoDE::read(CommentStream& infile, char* text) {
  handle.logMessage(LOGMESSAGE, "Reading PSO optimisation parameters");

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

    } else if (strcasecmp(text, "goal") == 0) {
      infile >> goal;
      count++;

    } else {
      handle.logMessage(LOGINFO, "Warning in optinfofile - unrecognised option", text);
      infile >> text;  //read and ignore the next entry
    }
    infile >> text;
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
