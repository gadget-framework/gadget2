#include "optinfo.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"
#include <cfloat>

OptInfoPso::OptInfoPso()
  : OptInfo(),  goal(1e-5), psoiter(100000), c1(1.496), c2(1.496),
        w_max(PSO_INERTIA), w_min(0.3), clamp_pos(1), nhood_strategy(PSO_NHOOD_RING),
        nhood_size(5), w_strategy(PSO_W_LIN_DEC), scale(0)
     {
  type = OPTPSO;
  handle.logMessage(LOGMESSAGE, "Initialising PSO optimisation algorithm");
}

void OptInfoPso::read(CommentStream& infile, char* text) {
  handle.logMessage(LOGMESSAGE, "Reading PSO optimisation parameters");

//  unsigned s1 = 0;
//  unsigned s2 = 0;
//  unsigned s3 = 0;
  TIME = DBL_MAX;
  VTR  = -DBL_MAX;

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

    } else if (strcasecmp(text, "w_max") == 0) {
      infile >> w_max;
      count++;

    } else if (strcasecmp(text, "w_min") == 0) {
      infile >> w_min; 
      count++;

    } else if (strcasecmp(text, "clamp_pos") == 0) {
      infile >> clamp_pos;
      count++;
    } else if (strcasecmp(text, "nhood_strategy") == 0) {
      infile >> nhood_strategy;
      count++;

    } else if (strcasecmp(text, "nhood_size") == 0) {
      infile >> nhood_size;
      count++;

    } else if (strcasecmp(text, "w_strategy") == 0) {
      infile >> w_strategy;
      count++;
    } else if (strcasecmp(text, "size") == 0) {
      infile >> size;
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

  //check the values specified in the optinfo file ...
  if (nhood_strategy!=PSO_NHOOD_GLOBAL &&
      nhood_strategy!=PSO_NHOOD_RING &&
      nhood_strategy!=PSO_NHOOD_RANDOM) {
 	handle.logMessage(LOGINFO, "Warning in optinfofile - Invalid nhood_strategy. Setting to default PSO_NHOOD_RANDOM");
	nhood_strategy=PSO_NHOOD_RANDOM;
		
  } 
  if (w_strategy!=PSO_W_LIN_DEC &&
      w_strategy!=PSO_W_CONST &&
      w_strategy!=PSO_W_ADAPT &&
      w_strategy!=3 ) {
  	handle.logMessage(LOGINFO, "Warning in optinfofile - Invalid w_strategy. Setting to default PSO_W_LIN_DEC");
	w_strategy=PSO_W_LIN_DEC;
  }

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
