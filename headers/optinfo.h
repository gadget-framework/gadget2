#ifndef optinfo_h
#define optinfo_h

#include <vector>
#include "maininfo.h"
#include "doublematrix.h"
#include "doublevector.h"
#include "intvector.h"
#include "intmatrix.h"
#include "seq_optimize_template.h"

enum OptType { OPTHOOKE = 1, OPTSIMANN, OPTBFGS, OPTPSO, OPTDE };

/**
 * \class OptInfo
 * \brief This is the base class used to perform the optimisation calculation for the model
 *
 * \note This will always be overridden by the derived classes that actually perform the optimisation calculation
*/
class OptInfo {
public:
  /**
   * \brief This is the default OptInfo constructor
   */
  OptInfo() { converge = 0; iters = 0; score = 0.0; };
  /**
   * \brief This is the default OptInfo destructor
   */
  ~OptInfo() {};
  /**
   * \brief This is the function used to read in the optimisation parameters
   * \param infile is the CommentStream to read the optimisation parameters from
   * \param text is a text string used to compare parameter names
   */
  virtual void read(CommentStream& infile, char* text) {};
  /**
   * \brief This function will print information from the optimisation algorithm
   * \param outfile is the ofstream that the optimisation information gets sent to
   * \param prec is the precision to use in the output file
   */
  virtual void Print(ofstream& outfile, int prec) {};
  /**
   * \brief This is the function used to call the optimisation algorithms
   */
  virtual void OptimiseLikelihood() {};
#ifdef _OPENMP
  /**
   * \brief This is the function used to call the optimisation algorithms parallelized with OpenMP of the reproducible version
   */
  virtual void OptimiseLikelihoodOMP() {};
  virtual void OptimiseLikelihoodREP() {};
#endif
  /**
   * \brief This function set the seeds used in SA
   * \param val array of unsigned int with the seeds
   */
  void setSeed(unsigned* val) {seed = val[0]; seedM =  val[1]; seedP = val[2];};
  /**
   * \brief This will return the type of optimisation class
   * \return type
   */
  OptType getType() const { return type; };
protected:
  double TIME;
  double VTR;
  /**
   * \brief This is the flag used to denote whether the optimisation converged or not
   */
  int converge;
  /**
   * \brief This is the number of iterations that took place during the optimisation
   */
  int iters;
  /**
   * \brief This is the value of the best likelihood score from the optimisation
   */
  double score;
  /**
   * \brief This denotes what type of optimisation class has been created
   */
  OptType type;
  /**
   * \brief This is the seed used for the calculation of the new value of the parameters
   */
  unsigned seed;
  /**
   * \brief This is the seed used for the acceptance of the metropolis
   */
  unsigned seedM;
  /**
   * \brief This is the seed used to change the order of the parameters
   */
  unsigned seedP;
};

/**
 * \class OptInfoHooke
 * \brief This is the class used for the Hooke & Jeeves optimisation
 *
 * The Hooke & Jeeves optimisation is the default optimisation, and is a simple and fast optimising method, but somewhat unreliable, which is often described as a "hill climbing" technique.  From the initial starting point the algorithm takes a step in various directions, and conducts a new model run.  If the new likelihood score is better than the old one then the algorithm uses the new point as it's best guess.  If it is worse then the algorithm retains the old point. The search proceeds in series of these steps, each step slightly smaller than the previous one.  When the algorithm finds a point which it cannot improve on with a small step in any direction then it accepts this point as being the "solution", and exits.  It is recommended that you re-run the optimisation, using the final point of one run as the start of the next.
 *
 * The Hooke & Jeeves algorithm used in Gadget is derived from that originally presented by R. Hooke and T. A. Jeeves, ''Direct Search Solution of Numerical and Statistical Problems'' in the April 1961 (Vol. 8, pp. 212-229) issue of the Journal of the ACM, with improvements presented by Arthur F Kaupe Jr., ''Algorithm 178: Direct Search'' in the June 1963 (Vol 6, pp.313-314) issue of the Communications of the ACM.

 */
class OptInfoHooke : public OptInfo {
public:
  /**
   * \brief This is the default OptInfoHooke constructor
   */
  OptInfoHooke();
  /**
   * \brief This is the default OptInfoHooke destructor
   */
  virtual ~OptInfoHooke() {};
  /**
   * \brief This is the function used to read in the Hooke & Jeeves parameters
   * \param infile is the CommentStream to read the optimisation parameters from
   * \param text is a text string used to compare parameter names
   */
  virtual void read(CommentStream& infile, char* text);
  /**
   * \brief This function will print information from the optimisation algorithm
   * \param outfile is the ofstream that the optimisation information gets sent to
   * \param prec is the precision to use in the output file
   */
  virtual void Print(ofstream& outfile, int prec);
  /**
   * \brief This is the function that will calculate the likelihood score using the Hooke & Jeeves optimiser
   */
  virtual void OptimiseLikelihood();
#ifdef _OPENMP
  /**
   * \brief This is the function that will calculate the likelihood score using the Hooke & Jeeves optimiser parallelized with the reproducible version implemented OpenMP
   */

  virtual void OptimiseLikelihoodOMP();
  virtual void OptimiseLikelihoodREP();
#endif
private:
  /**
   * \brief This function will calculate the best point that can be found close to the current point
   * \param delta is the DoubleVector of the steps to take when looking for the best point
   * \param point is the DoubleVector that will contain the parameters corresponding to the best function value found from the search
   * \param prevbest is the current best point value
   * \param param is the IntVector containing the order that the parameters should be searched in
   * \return the best function value found from the search
   */
  double bestNearby(DoubleVector& delta, DoubleVector& point, double prevbest, IntVector& param);
  /**
   * \brief This function implemented the reproducible version with OpenMP will calculate the best point that can be found close to the current point
   * \param delta is the DoubleVector of the steps to take when looking for the best point
   * \param point is the DoubleVector that will contain the parameters corresponding to the best function value found from the search
   * \param prevbest is the current best point value
   * \param param is the IntVector containing the order that the parameters should be searched in
   * \return the best function value found from the search
   */
  double bestNearbyRepro(DoubleVector& delta, DoubleVector& point, double prevbest, IntVector& param);
  /**
     * \brief This function implemented the speculative version with OpenMP will calculate the best point that can be found close to the current point
     * \param delta is the DoubleVector of the steps to take when looking for the best point
     * \param point is the DoubleVector that will contain the parameters corresponding to the best function value found from the search
     * \param prevbest is the current best point value
     * \param param is the IntVector containing the order that the parameters should be searched in
     * \return the best function value found from the search
     */
  double bestNearbySpec(DoubleVector& delta, DoubleVector& point, double prevbest, IntVector& param);
  /**
   * \brief This is the maximum number of iterations for the Hooke & Jeeves optimisation
   */
  int hookeiter;
  /**
   * \brief This is the reduction factor for the step length
   */
  double rho;
  /**
   * \brief This is the initial step length
   */
  double lambda;
  /**
   * \brief This is the minimum step length, use as the halt criteria for the optimisation process
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
 * The Simulated Annealing algorithm used in Gadget is derived from that presented by Corana et al, ''Minimising Multimodal Functions of Continuous Variables with the 'Simulated Annealing' Algorithm'' in the September 1987 (Vol. 13, pp. 262-280) issue of the ACM Transactions on Mathematical Software and Goffe et al, ''Global Optimisation of Statistical Functions with Simulated Annealing'' in the January/February 1994 (Vol. 60, pp. 65-100) issue of the Journal of Econometrics.
 */
class OptInfoSimann : public OptInfo {
public:
  /**
   * \brief This is the default OptInfoSimann constructor
   */
  OptInfoSimann();
  /**
   * \brief This is the default OptInfoSimann destructor
   */
  virtual ~OptInfoSimann() {};
  /**
   * \brief This is the function used to read in the Simulated Annealing parameters
   * \param infile is the CommentStream to read the optimisation parameters from
   * \param text is a text string used to compare parameter names
   */
  virtual void read(CommentStream& infile, char* text);
  /**
   * \brief This function will print information from the optimisation algorithm
   * \param outfile is the ofstream that the optimisation information gets sent to
   * \param prec is the precision to use in the output file
   */
  virtual void Print(ofstream& outfile, int prec);
  /**
   * \brief This is the function that will calculate the likelihood score using the Simulated Annealing optimiser
   */
  virtual void OptimiseLikelihood();
#ifdef _OPENMP
//#ifdef SPECULATIVE
  /**
   * \brief This is the function that will calculate the likelihood score using the Simulated Annealing optimiser parallelized with the reproducible version implemented OpenMP
   */
  virtual void OptimiseLikelihoodOMP();
  virtual void OptimiseLikelihoodREP();
//#endif
#endif
  /**
   * \brief This function calculate a new valor for the parameter l
   * \param nvars the number of variables to be optimised
   * \param l the parameter to change
   * \param param IntVector with the order of the parameters
   * \param trialx DoubleVector that storage the values of the parameters to evaluate during this iteration
   * \param x DoubleVector that storage the best values of the parameters
   * \param lowerb DoubleVector with the lower bounds of the variables to be optimised
   * \param upperb DoubleVector with the upper bounds of the variables to be optimised
   * \param vm DoubleVector with the value for the maximum step length for each parameter
   */
  virtual void newValue(int nvars, int l, IntVector& param, DoubleVector& trialx,
  		DoubleVector& x, DoubleVector& lowerb, DoubleVector& upperb, DoubleVector& vm);
private:
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
  double t;
  /**
   * \brief This is the factor used to adjust the step length
   */
  double cs;
  /**
   * \brief This is the initial value for the maximum step length
   */
  double vminit;
  /**
   * \brief This is the maximum number of function evaluations for the Simulated Annealing optimiation
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
   * \brief This is the number of temperature loops to check when testing for convergence
   */
  int tempcheck;
  /**
   * \brief This is the flag to denote whether the parameters should be scaled or not (default 0, not scale)
   */
  int scale;
};

/**
 * \class OptInfoBFGS
 * \brief This is the class used for the BFGS optimisation
 *
 * BFGS is a quasi-Newton global optimisation method that uses information about the gradient of the function at the current point to calculate the best direction to look in to find a better point.  Using this information, the BFGS algorithm can iteratively calculate a better approximation to the inverse Hessian matrix, which will lead to a better approximation of the minimum value.  From an initial starting point, the gradient of the function is calculated and then the algorithm uses this information to calculate the best direction to perform a linesearch for a point that is ''sufficiently better''.  The linesearch that is used in Gadget to look for a better point in this direction is the ''Armijo'' linesearch.  The algorithm will then adjust the current estimate of the inverse Hessian matrix, and restart from this new point.  If a better point cannot be found, then the inverse Hessian matrix is reset and the algorithm restarts from the last accepted point.
 *
 * The BFGS algorithm used in Gadget is derived from that presented by Dimitri P Bertsekas, ''Nonlinear Programming'' (2nd edition, pp22-61) published by Athena Scientific.
 */
class OptInfoBFGS : public OptInfo  {
public:
  /**
   * \brief This is the default OptInfoBFGS constructor
   */
  OptInfoBFGS();
  /**
   * \brief This is the default OptInfoBFGS destructor
   */
  ~OptInfoBFGS() {};
  /**
   * \brief This is the function used to read in the BFGS parameters
   * \param infile is the CommentStream to read the optimisation parameters from
   * \param text is a text string used to compare parameter names
   */
  virtual void read(CommentStream& infile, char* text);
  /**
   * \brief This function will print information from the optimisation algorithm
   * \param outfile is the ofstream that the optimisation information gets sent to
   * \param prec is the precision to use in the output file
   */
  virtual void Print(ofstream& outfile, int prec);
  /**
   * \brief This is the function that will calculate the likelihood score using the BFGS optimiser
   */
  virtual void OptimiseLikelihood();
#ifdef _OPENMP
//#ifdef SPECULATIVE
  /**
  * \brief This function call the sequential function. BFGS isn't implemented with OpenMP
  */
  virtual void OptimiseLikelihoodOMP();
  virtual void OptimiseLikelihoodREP();
//#endif
#endif
private:
  /**
   * \brief This function will numerically calculate the gradient of the function at the current point
   * \param point is the DoubleVector that contains the parameters corresponding to the current function value
   * \param pointvalue is the current function value
   * \param newgrad is the DoubleVector that will contain the gradient vector for the current point
   */
  void gradient(DoubleVector& point, double pointvalue, DoubleVector& newgrad);
  /**
   * \brief This function will calculate the smallest eigenvalue of the inverse Hessian matrix
   * \param M is the DoubleMatrix containing the inverse Hessian matrix
   * \return the smallest eigen value of the matrix
   */
  double getSmallestEigenValue(DoubleMatrix M);
  /**
   * \brief This is the maximum number of function evaluations for the BFGS optimiation
   */
  int bfgsiter;
  /**
   * \brief This is the halt criteria for the BFGS algorithm
   */
  double bfgseps;
  /**
   * \brief This is the adjustment factor in the Armijo linesearch
   */
  double beta;
  /**
   * \brief This is the halt criteria for the Armijo linesearch
   */
  double sigma;
  /**
   * \brief This is the initial step size for the Armijo linesearch
   */
  double step;
  /**
   * \brief This is the accuracy term used when calculating the gradient
   */
  double gradacc;
  /**
   * \brief This is the factor used to adjust the gradient accuracy term
   */
  double gradstep;
  /**
   * \brief This is the halt criteria for the gradient accuracy term
   */
  double gradeps;
};


class OptInfoDE : public OptInfo  {
public:
  OptInfoDE();
  ~OptInfoDE() {};
  virtual void read(CommentStream& infile, char* text);
  virtual void Print(ofstream& outfile, int prec);
  virtual void OptimiseLikelihood();
#ifdef _OPENMP
  virtual void OptimiseLikelihoodOMP();
  virtual void OptimiseLikelihoodREP();
#endif
private:
 double F;
 double CR;
 double goal;
 int iter;
 int scale;
 double previous_fbest_fitness;
 int iter_without_improv_global_best; // Iteration without improvements of global best solution
 int improv_with_stack_global_best;   // Consecutive improvements of global best solution, but with insignificant value
 int consecutive_iters_global_best;   // Consecutive improvements of global best solution, with a good percentage of improvement
 int STATE; // state of the search
 int nrestarts;
 int growth_trend; // trend of decrease or increase of the configuration parameters C1 and C2
 int growth_popul; // peding growth of the population
 int nvars;
 double threshold_acept;
 int growth_trend_popul;
 int num_restart;
 int size;

 void   growth_trend_manager();
 double F_manager(int step);
 double calc_adapt_parameters(int step);
 void   position_within_bounds(DoubleMatrix& pos, DoubleMatrix& vel, DoubleVector& lowerb, DoubleVector& upperb, int i, int d);
};
 
/**
 * \class OptInfoPso
 * \brief This is the class used for the PSO optimisation
 *
 * PSO or Particle Swarm Optimization 
 *
 * The PSO algorithm used in Gadget is derived from that presented by Kyriakos Kentzoglanakis.
 */
class OptInfoPso : public OptInfo  {
public:
  /**
   * \brief This is the default OptInfoBFGS constructor
   */
  OptInfoPso();
  /**
   * \brief This is the default OptInfoBFGS destructor
   */
  ~OptInfoPso() {};
  /**
   * \brief This is the function used to read in the BFGS parameters
   * \param infile is the CommentStream to read the optimisation parameters from
   * \param text is a text string used to compare parameter names
   */
  virtual void read(CommentStream& infile, char* text);
  /**
   * \brief This function will print information from the optimisation algorithm
   * \param outfile is the ofstream that the optimisation information gets sent to
   * \param prec is the precision to use in the output file
   */
  virtual void Print(ofstream& outfile, int prec);
  /**
   * \brief This is the function that will calculate the likelihood score using the PSO optimiser
   */
  virtual void OptimiseLikelihood();
#ifdef _OPENMP
//#ifdef SPECULATIVE
  /**
  * \brief This function call the sequential function. PSO isn't implemented with OpenMP
  */
  virtual void OptimiseLikelihoodOMP();
  virtual void OptimiseLikelihoodREP();
//#endif
#endif
private:

 /**
  * \brief CONSTANTS: max swarm size
  */
 #define PSO_MAX_SIZE 100
 
 /**
  * \brief CONSTANTS: default value of w (see clerc02) 
  */
 #define PSO_INERTIA 0.7298 
 
 
 
 /**
  * \brief NEIGHBORHOOD SCHEMES: global best topology 
  */
#define PSO_NHOOD_GLOBAL 0
 
 /**
  * \brief NEIGHBORHOOD SCHEMES: ring topology 
  */
#define PSO_NHOOD_RING 1
 
 /**
  * \brief NEIGHBORHOOD SCHEMES: Random neighborhood topology. see http://clerc.maurice.free.fr/pso/random_topology.pdf 
  */
#define PSO_NHOOD_RANDOM 2
 
 
 
 /**
  * \brief INERTIA WEIGHT UPDATE FUNCTIONS 
  */
#define PSO_W_CONST 0
#define PSO_W_LIN_DEC 1
#define PSO_W_ADAPT 2


 /**
  * \brief PSO SOLUTION -- Initialized by the user
  */
 typedef struct {
 
     double error;
     double *gbest; // should contain DIM elements!!
 
 } pso_result_t;
 
 /**
  * \brief optimization goal (error threshold) 
  */
 double goal; 
 
 /**
  * \brief swarm size (number of particles)
  */
 int size; 
 
 /**
  * \brief maximum number of iterations
  */
 int psoiter; 
 
 /**
  * \brief cognitive coefficient
  */
 double c1; 
 
 /**
  * \brief social coefficient
  */
 double c2; 

#ifdef OPENMP
#pragma omp threadprivate(c1,c2)  
#endif
 /**
  * \brief max inertia weight value
  */
 double w_max;
 
 /**
  * \brief min inertia weight value
  */
 double w_min; 

 // ultimo fitness
 double previous_fbest_fitness;

 int iter_without_improv_global_best; // Iteration without improvements of global best solution
 int improv_with_stack_global_best;   // Consecutive improvements of global best solution, but with insignificant value
 int consecutive_iters_global_best;   // Consecutive improvements of global best solution, with a good percentage of improvement
 int STATE; // state of the search
 int nrestarts; 
 int growth_trend; // trend of decrease or increase of the configuration parameters C1 and C2
 int growth_popul; // peding growth of the population
 int nvars;
// double k_vmax; 
 double threshold_acept;
 int growth_trend_popul;
 int num_restart;
 /**
  * \brief whether to keep particle position within defined bounds (TRUE) or apply periodic boundary conditions (FALSE)
  */
 int clamp_pos; 
 
 /**
  * \brief neighborhood strategy (see PSO_NHOOD_*)
  */
 int nhood_strategy;
 
 /**
  * \brief neighborhood size
  */
 int nhood_size;  
 
 /**
  * \brief inertia weight strategy (see PSO_W_*)
  */
 int w_strategy;

 /**
    * \brief This is the flag to denote whether the parameters should be scaled or not (default 0, not scale)
    */
 int scale;
  
// /**
//  * \brief seed for the generator
//  */
// long seed;
 
 
 

 /**
  * \brief return the swarm size based on dimensionality
  */
 void growth_trend_manager();
 double w_manager(int step);

 int pso_calc_swarm_size(int dim);
 double calc_inertia_const(int step);
 double calc_inertia_lin_dec(int step);
 double calc_inertia_adapt_dyn(int step);
 
 void position_within_bounds(DoubleMatrix&, DoubleMatrix&, DoubleVector&, DoubleVector&, int , int );

 void inform_global(IntMatrix& comm, DoubleMatrix& pos_nb,
                   DoubleMatrix& pos_b, DoubleVector& fit_b,
                   DoubleVector& gbest, int improved);
 void inform_ring(IntMatrix& comm, DoubleMatrix& pos_nb,
                 DoubleMatrix& pos_b, DoubleVector& fit_b,
                 DoubleVector& gbest, int improved);
 void inform_random(IntMatrix& comm, DoubleMatrix& pos_nb,
                   DoubleMatrix& pos_b, DoubleVector& fit_b,
                   DoubleVector& gbest,  int improved);
 void inform(IntMatrix& comm, DoubleMatrix& pos_nb, DoubleMatrix& pos_b, DoubleVector& fit_b, int improved);
 void init_comm_ring(IntMatrix& comm);
 void init_comm_random(IntMatrix& comm) ;

 double coord_transformation(double *, double *, int , double *, double *);
 double calc_euclidean_distance(double *, double *, int , double *, double *);


 typedef void (OptInfoPso::*Inform_fun)(IntMatrix&, DoubleMatrix&, DoubleMatrix&, DoubleVector&, DoubleVector&,  int); // neighborhood update function
 typedef double (OptInfoPso::*Calc_inertia_fun)(int); // inertia weight update function

};
#endif
