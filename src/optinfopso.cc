#include "optinfo.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"
#include <cfloat>

OptInfoPso::OptInfoPso()
  : OptInfo(),  goal(1e-5), psoiter(100000) {
  type = OPTPSO;
  handle.logMessage(LOGMESSAGE, "Initialising PSO optimisation algorithm");
}

void OptInfoPso::read(CommentStream& infile, char* text) {
  handle.logMessage(LOGMESSAGE, "Reading PSO optimisation parameters");

  int count = 0;
  while (!infile.eof() && strcasecmp(text, "[PSO]") && strcasecmp(text, "[simann]") && strcasecmp(text, "[hooke]") && strcasecmp(text, "[bfgs]") && strcasecmp(text, "[DE]")) {
    infile >> ws;
    if (strcasecmp(text, "seed") == 0) {
      int s = 0;
      infile >> s >> ws;
      handle.logMessage(LOGMESSAGE, "Initialising random number generator with", s);
      srand(s);
    } 
    else if (strcasecmp(text, "psoiter") == 0) {
      infile >> psoiter;
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

void OptInfoPso::Print(ofstream& outfile, int prec) {
  outfile << "; PSO algorithm ran for " << iters
    << " function evaluations\n; and stopped when the likelihood value was "
    << setprecision(prec) << score;
  if (converge == -1)
    outfile << "\n; because an error occured during the optimisation\n";
  else if (converge == 1)
    outfile << "\n; because the convergence criteria were met\n";
  else
    outfile << "\n; because the maximum number of function evaluations was reached\n";
}

/**
 * \brief calculate constant  inertia weight equal to w_max
 */
double OptInfoPso::calc_inertia_const(int step) {
    return w_max;
}



/**
 * \brief calculate linearly decreasing inertia weight
 */
double OptInfoPso::calc_inertia_lin_dec(int step) {
    int dec_stage = 3 * psoiter / 4;
    double W;
    if (step <= dec_stage)
        W= w_min + (w_max - w_min) * (dec_stage - step) / dec_stage;
    else
        W= w_min;
    return W;
}
