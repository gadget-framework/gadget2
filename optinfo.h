#ifndef optinfo_h
#define optinfo_h

#include "ecosystem.h"
#include "parameter.h"
#include "maininfo.h"
#include "errorhandler.h"

/**
 * \class OptSearch
 * \brief This is the base class for the optimization, methods declared here are implemented in the derived classes
*/
class OptSearch {
 public:
  /**
   * \brief This is the default constructor
   */
  OptSearch() {};
  /**
   * \brief This is the default destructor
   */
  ~OptSearch(){};
  /**
   * \brief This is the file reader that reads parameters for the optimising
   * \param infile is the CommentStream to read the optimisation parameters from
   * \param text is the latest entry from infile
   */
  virtual void Read(CommentStream& infile, char* text){};
  /**
   * \brief This method calls the optimisation function
   * \param infile is the CommentStream to read the optimisation parameters from
   */
  virtual void MaximizeLikelihood(){};
};

/**
 * \class OptInfo
 * \brief This is the master class that syncronizes gadget's optimization
 */
class OptInfo {
 public:
  /**
   * \brief This is the default constructor
   */
  OptInfo(MainInfo* MainInfo);
  /**
   * \brief This is the default destructor
   */
  ~OptInfo();
  /**
   * \brief This is the function that will optimize the likelihood score
   */
  void Optimize();
  /**
   * \brief This is the file reader that determines what optimization methods are to used during optimization.
   * \param infile is the CommentStream to read the optimisation parameters from
   */
  void ReadOptInfo(CommentStream& infile);
 private:
  /**
   * \brief This switch determines whether or Simulated Annealing is use in the optimisation
   */
  int useSimann;
  /**
   * \brief This switch determines whether or Hooke & Jeeves is use in the optimisation
   */
  int useHJ;
  /**
   * \brief This switch determines whether or BFGS is use in the optimisation
   */
  int useBFGS;
  /**
   * \brief This is the seed for the random number generator
   */
  int seed;
  /**
   * \brief This is a pointer to the class that handles the Simulated Annealing optimisation
   */
  OptSearch* optSimann;
  /**
   * \brief This is a pointer to the class that handles the Hooke & Jeeves optimisation
   */
  OptSearch* optHJ;
  /**
   * \brief This is a pointer to the class that handles the BFGS optimisation
   */
  OptSearch* optBFGS;
};

/**
 * \class OptInfoHooke
 * \brief This is the class used for the Hooke & Jeeves optimisation
 *
 * The Hooke & Jeeves optimisation is the default optimisation, and is a simple and fast optimising method, but somewhat unreliable, which is often described as a "hill climbing" technique.  From the initial starting point the algorithm takes a step in various directions, and conducts a new model run.  If the new likelihood score is better than the old one then the algorithm uses the new point as it's best guess.  If it is worse then the algorithm retains the old point. The search proceeds in series of these steps, each step slightly smaller than the previous one.  When the algorithm finds a point which it cannot improve on with a small step in any direction then it accepts this point as being the "solution", and exits.  It is recommended that you re-run the optimisation, using the final point of one run as the start of the next.
 *
 * The Hooke & Jeeves algorithm used in Gadget is derived from that presented by R. Hooke and T. A. Jeeves, "Direct Search Solution of Numerical and Statistical Problems", Journal of the ACM, Vol. 8, April 1961, pp. 212-229.
 */
class OptInfoHooke : public OptSearch {
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
  virtual void Read(CommentStream& infile, char* text);
  /**
   * \brief This is the function used to read in the Hooke & Jeeves parameters
   * \param infile is the CommentStream to read the optimisation parameters from
   * \param text is a text string used to compare parameter names
   * \return 1 for success, 0 for failure
   */
  //  int read(CommentStream& infile, char* text);
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
class OptInfoSimann : public OptSearch {
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
  virtual void Read(CommentStream& infile, char* text);
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
 * \class OptInfoBfgs
 * \brief This is the class used for the BFGS optimisation
 *
 * (To be continued)
 */
class OptInfoBfgs : public OptSearch  {
 public:
  /**
   * \brief This is the BFGS constructor
   */
  OptInfoBfgs();
  /**
   * \brief This is the BFGS destructor
   */
  ~OptInfoBfgs();
    /**
   * \brief This is the BFGS file reader
   * \param infile is the CommentStream to read the optimisation parameters from
   */
  virtual void Read(CommentStream& infile, char* text);
  /**
   * \brief This is the function that will calculate the likelihood score using the BFGS optimiser
   */
  virtual void MaximizeLikelihood();
 private:
  /**
   * \brief The actual BFGS iteration
   * \param x0 is the starting point of the optimisation
   */
  int iteration(double* x0);
  /**
   * \brief This method calculates the gradient
   * \param p is the point where the gradinent should be calculated
   * \param fp is the function value at p
   */ 
  void gradient(double* p, double fp);
  /**
   * \brief This method solves the equation Bk*s=-grad(f)
   */
  int gaussian(double mult);  
  /**
   * \brief This method performes a linesearch a long the search direction s
   */
  double linesearch();
  /**
   * \brief This is the current gradient
   */
  double* gk;        
  /**
   * \brief This is the previous gradient
   */
  double* g0;        //old gradient direction
  /**
   * \brief This is the search direction (for linesearch)
   */
  double* s;         
  /**
   * \brief This is the BFGS updated hessian approximation
   */
  double** Bk;       
  /**
   * \brief This is the current approximation
   */ 
  double* x;         
  /**
   * \brief This is the current function value
   */ 
  double fk;    
  /**
   * \brief This is the function to optimise
   */
  double (*f)(double*, int);
  /**
   * \brief This is the number of varibles
   */
  int numvar;

  /**
   * \brief This is the maximum number of BFGS iterations
   */
  int maxiter;
  /**
   * \brief This is the convergence parameter (BFGS halts when norm(gk) < eps)
   */
  double eps;
  /**
   * \brief  linesearch convergence parameters
   */
  double rho;        //linesearch convergence parameters
  double tau;        //---------------------------------

};
#endif
