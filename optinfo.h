#ifndef optinfo_h
#define optinfo_h

#include "ecosystem.h"
#include "parameter.h"

/**
 * \class OptInfo
 * \brief This is the base class used for the optimisation parameters
 */
class OptInfo {
public:
  /**
   * \brief This is the default constructor
   */
  OptInfo();
  /**
   * \brief This is the default destructor
   */
  virtual ~OptInfo();
  /**
   * \brief This is the function that will calculate the likelihood score
   */
  virtual void MaximizeLikelihood() = 0;
  /**
   * \brief This is the file reader common to all optimisation types
   * \param infile is the CommentStream to read the optimisation parameters from
   */
  virtual void Read(CommentStream& infile) = 0;
  /**
   * \brief This is the function used to read in the specific parameters
   * \param infile is the CommentStream to read the optimisation parameters from
   * \param text is a text string used to compare parameter names
   * \return 1 for success, 0 for failure
   */
  int Read(CommentStream& infile, char* text);
protected:
  /**
   * \brief This is the number of iterations done
   */
  int nopt;
};

/**
 * \class OptInfoHooke
 * \brief This is the class used for the Hooke & Jeeves optimisation
 *
 * The Hooke & Jeeves optimisation is the default optimisation, and is a simple and fast optimising method, but somewhat unreliable, which is often described as a "hill climbing" technique.  From the initial starting point the algorithm takes a step in various directions, and conducts a new model run.  If the new likelihood score is better than the old one then the algorithm uses the new point as it's best guess.  If it is worse then the algorithm retains the old point. The search proceeds in series of these steps, each step slightly smaller than the previous one.  When the algorithm finds a point which it cannot improve on with a small step in any direction then it accepts this point as being the "solution", and exits.  It is recommended that you re-run the optimisation, using the final point of one run as the start of the next.
 *
 * The Hooke & Jeeves algorithm used in Gadget is derived from that presented by R. Hooke and T. A. Jeeves, "Direct Search Solution of Numerical and Statistical Problems", Journal of the ACM, Vol. 8, April 1961, pp. 212-229.
 */
class OptInfoHooke : public OptInfo {
public:
  /**
   * \brief This is the Hooke & Jeeves constructor
   */
  OptInfoHooke();
  /**
   * \brief This is the Hooke & Jeeves destructor
   */
  virtual ~OptInfoHooke();
  /**
   * \brief This is the Hooke & Jeeves file reader
   * \param infile is the CommentStream to read the optimisation parameters from
   */
  virtual void Read(CommentStream& infile);
  /**
   * \brief This is the function used to read in the Hooke & Jeeves parameters
   * \param infile is the CommentStream to read the optimisation parameters from
   * \param text is a text string used to compare parameter names
   * \return 1 for success, 0 for failure
   */
  int Read(CommentStream& infile, char* text);
  /**
   * \brief This is the function that will calculate the likelihood score using the Hooke & Jeeves optimiser
   */
  virtual void MaximizeLikelihood();
protected:
  /**
   * \brief This is the maximum number of iterations
   */
  int hookeiter;
  /**
   * \brief This is the reduction factor for each step length
   */
  double rho;
  /**
   * \brief This is the initial step length
   */
  double lambda;
  /**
   * \brief This is the minimum step length
   */
  double hookeeps;
  /**
   * \brief This is the limit when checking if a parameter is stuck on the bound
   */
  double bndcheck;
};

/**
 * \class OptInfoSimann
 * \brief This is the class used for the Simualted Annealing optimisation
 *
 * Simulated Annealing is a global optimisation method that distinguishes different local optima.  Starting from an initial point, the algorithm takes a step and the function is evaluated.  When minimizing a function, any downhill step is accepted and the process repeats from this new point.  An uphill step may be accepted (thus, it can escape from local optima).  This uphill decision is made by the Metropolis criteria.  It uses a parameter known as "temperature" and the size of the uphill step in a probabilistic manner, and varying the temperature will affect the number of the uphill moves that are accepted.  As the optimisation process proceeds, the length of the steps decline and the algorithm closes in on the global optimum.
 *
 * The Simulated Annealing algorithm used in Gadget is derived from that presented by Corana et al, "Minimizing Multimodal Functions of Continuous Variables with the 'Simulated Annealing' Algorithm" in the September 1987 (Vol. 13, no. 3, pp. 262-280) issue of the ACM Transactions on Mathematical Software.
 */
class OptInfoSimann : public OptInfo {
public:
  /**
   * \brief This is the Simulated Annealing constructor
   */
  OptInfoSimann();
  /**
   * \brief This is the Simulated Annealing destructor
   */
  virtual ~OptInfoSimann();
  /**
   * \brief This is the Simulated Annealing file reader
   * \param infile is the CommentStream to read the optimisation parameters from
   */
  virtual void Read(CommentStream& infile);
  /**
   * \brief This is the function used to read in the Simulated Annealing parameters
   * \param infile is the CommentStream to read the optimisation parameters from
   * \param text is a text string used to compare parameter names
   * \return 1 for success, 0 for failure
   */
  int Read(CommentStream& infile, char* text);
  /**
   * \brief This is the function that will calculate the likelihood score using the Simulated Annealing optimiser
   */
  virtual void MaximizeLikelihood();
protected:
  /**
   * \brief This is the temperature reduction factor
   */
  double rt;
  /**
   * \brief This is the halt criteria for the Simulated Annealing algorithm
   */
  double simanneps;
  /**
   * \brief This is the number of loops before the step length is adjusted
   */
  int ns;
  /**
   * \brief This is the number of loops before the temperature is adjusted
   */
  int nt;
  /**
   * \brief This is the "temperature" used for the Simulated Annealing algorithm
   */
  double T;
  /**
   * \brief This is the factor used for to adjust the step length
   */
  double cs;
  /**
   * \brief This is the step length
   */
  double vm;
  /**
   * \brief This is the maximum number of function evaluations
   */
  int simanniter;
  /**
   * \brief This is the upper bound when adjusting the step length
   */
  double uratio;
  /**
   * \brief This is the lower bound when adjusting the step length
   */
  double lratio;
  /**
   * \brief This is the number of temperature loops to check
   */
  int check;
};

/**
 * \class OptInfoHookeAndSimann
 * \brief This is the class used for both the Hooke & Jeeves and Simualted Annealing optimisations
 *
 * This method attempts to combine the wide search of Simulated Annealing and the rapid convergence of Hooke & Jeeves.  It relies on the observation that the likelihood function for many Gadget models consists of a large "valley" in which the best solution lies, surrounded by much more "rugged" terrain.  A small amount of Simulated Annealing at the start of the run serves to move the search into this valley, at which point Hooke & Jeeves takes over an homes in on a solution within that valley.  Hopefully the Simulated Annealing will move to the correct side of any "hills" and avoid Hooke & Jeeves becoming trapped into unrealistic local optima.
 *
 * \see OptInfoHooke
 * \see OptInfoSimann
 */
class OptInfoHookeAndSimann : public OptInfo {
public:
  /**
   * \brief This is the Hooke & Jeeves and Simulated Annealing constructor
   */
  OptInfoHookeAndSimann();
  /**
   * \brief This is the Hooke & Jeeves and Simulated Annealing destructor
   */
  virtual ~OptInfoHookeAndSimann();
  /**
   * \brief This is the Hooke & Jeeves and Simulated Annealing file reader
   * \param infile is the CommentStream to read the optimisation parameters from
   */
  virtual void Read(CommentStream& infile);
  /**
   * \brief This is the function used to read in the Hooke & Jeeves and Simulated Annealing parameters
   * \param infile is the CommentStream to read the optimisation parameters from
   * \param text is a text string used to compare parameter names
   * \return 1 for success, 0 for failure
   */
  int Read(CommentStream& infile, char* text);
  /**
   * \brief This is the function that will calculate the likelihood score using the Hooke & Jeeves and Simulated Annealing optimisers
   */
  virtual void MaximizeLikelihood();
protected:
  /**
   * \brief This is the maximum number of Hooke & Jeeves iterations
   */
  int hookeiter;
  /**
   * \brief This is the reduction factor for each step length for the Hooke & Jeeves algorithm
   */
  double rho;
  /**
   * \brief This is the initial step length for Hooke & Jeeves
   */
  double lambda;
  /**
   * \brief This is the minimum step length for Hooke & Jeeves
   */
  double hookeeps;
  /**
   * \brief This is the limit when checking if a parameter is stuck on the bound
   */
  double bndcheck;
  /**
   * \brief This is the maximum number of function evaluations for the Simulated Annealing algorithm
   */
  int simanniter;
  /**
   * \brief This is the temperature reduction factor
   */
  double rt;
  /**
   * \brief This is the halt criteria for the Simulated Annealing algorithm
   */
  double simanneps;
  /**
   * \brief This is the number of loops before the step length is adjusted
   */
  int ns;
  /**
   * \brief This is the number of loops before the temperature is adjusted
   */
  int nt;
  /**
   * \brief This is the "temperature" used for the Simulated Annealing algorithm
   */
  double T;
  /**
   * \brief This is the factor used for to adjust the step length
   */
  double cs;
  /**
   * \brief This is the step length
   */
  double vm;
  /**
   * \brief This is the upper bound when adjusting the step length
   */
  double uratio;
  /**
   * \brief This is the lower bound when adjusting the step length
   */
  double lratio;
  /**
   * \brief This is the number of temperature loops to check
   */
  int check;
};

#endif
