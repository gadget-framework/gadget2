#include "optinfo.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"
#include <cfloat>

OptInfoPso::OptInfoPso()
  : OptInfo(),  goal(1e-5), psoiter(100000), c1(1.496), c2(1.496), scale(0)
     {
  type = OPTPSO;
  handle.logMessage(LOGMESSAGE, "Initialising PSO optimisation algorithm");
}

void OptInfoPso::read(CommentStream& infile, char* text) {
  handle.logMessage(LOGMESSAGE, "Reading PSO optimisation parameters");

//  unsigned s1 = 0;
//  unsigned s2 = 0;
//  unsigned s3 = 0;

  int count = 0;
  while (!infile.eof() && strcasecmp(text, "[PSO]") && strcasecmp(text, "[simann]") && strcasecmp(text, "[hooke]") && strcasecmp(text, "[bfgs]")) {
    infile >> ws;
    if (strcasecmp(text, "seed") == 0) {
      int s = 0;
      infile >> s >> ws;
//      s1 = s;
      handle.logMessage(LOGMESSAGE, "Initialising random number generator with", s);
      srand(s);
	} 
    else if (strcasecmp(text, "psoiter") == 0) {
      infile >> psoiter;
      count++;

    } else if (strcasecmp(text, "goal") == 0) {
      infile >> goal;
      count++;

    } else if (strcasecmp(text, "c1") == 0) {
      infile >> c1;
      count++;

    } else if (strcasecmp(text, "c2") == 0) {
      infile >> c2;
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

  if (scale != 0 && scale != 1) {
      handle.logMessage(LOGINFO, "Warning in optinfofile - value of scale outside bounds", scale);
      scale = 0;
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
