/* PARALLEL ADAPTIVE MULTIRESTART PARTICLE SWARM OPTIMIZATION
 * This method of optimization is self-adaptive, that is, it adjusts its configuration
 * parameters throughout the execution time. Therefore, it does not need any input argument, 
 * so the user does not spend time customizing the algorithm. In addition, a multi-restart
 * mechanism was built, to avoid stagnation in local optima, and a parallelization was implemented
 *  using openMP.
 *
 * calling sequence:                                               
 *  int hooke(nvars, startpt, endpt, rho, epsilon, itermax)        
*/


#include "gadget.h"    //All the required standard header files are in here
#include "optinfo.h"
#include "mathfunc.h"
#include "doublevector.h"
#include "intvector.h"
#include "errorhandler.h"
#include "ecosystem.h"
#include "global.h"
#include <float.h>
#include "runid.h"

#ifdef _OPENMP
#include "omp.h"
#endif

extern Ecosystem* EcoSystem;
#ifdef _OPENMP
extern Ecosystem** EcoSystems;
#endif

void OptInfoPso::OptimiseLikelihood() {
    handle.logMessage(LOGINFO, "\nStarting ADAPTIVE SEQUENTIAL PSO optimisation algorithm\n");
    double tmp;
    int ii, i, j, offset, rchange, rcheck, rnumber, init_reset;
    nvars = EcoSystem->numOptVariables();
    DoubleVector x(nvars);
    DoubleVector bestx(nvars);
    DoubleVector scalex(nvars);
    DoubleVector lowerb(nvars);
    DoubleVector upperb(nvars);
    DoubleVector init(nvars);
    IntVector param(nvars, 0);
    IntVector lbound(nvars, 0);
    IntVector rbounds(nvars, 0);
    IntVector trapped(nvars, 0);
    struct Best { double bestf; int index;};
    struct Best best;
    double timestop;
// SOLVER
    printf("SOLVER PSO\n");
    size= (int) (nvars);
    DoubleMatrix pos(size, nvars, 0.0);    // position matrix
    DoubleMatrix vel(size, nvars, 0.0);    // velocity matrix
    DoubleMatrix pos_new(size, nvars, 0.0);  // new population
    DoubleMatrix pos_b(size, nvars, 0.0);  // best position matrix
    DoubleVector fit(size); // particle fitness vector
    DoubleVector fit_b(size); // best fitness vector
    DoubleMatrix gbest(size, nvars, 0.0); // what is the best informed position for each particle
    IntMatrix comm(size, size, 0); // communications:who informs who rows : those who inform cols : those who are informed
    int improved; // whether solution->error was improved during the last iteration
    int d, step, index_del;
    int p1,p2,p3,p4;
    double prob;
    double dist;
    double a, b;
    double rho1, rho2; //random numbers (coefficients)
    double w; //current omega
    Inform_fun inform_fun;
    Calc_inertia_fun calc_inertia_fun; // inertia weight update function
    int psosteps = 0; //!= iters?
    ostringstream spos, svel;
    int numThr = 1;


    growth_popul=0;
    printf("seed %d\n", seed);
    threshold_acept=0.1;
    nrestarts=0;
    iter_without_improv_global_best=0;
    improv_with_stack_global_best=0;
    consecutive_iters_global_best=0;
    previous_fbest_fitness=DBL_MAX;
    growth_trend=0;
    STATE=0;
    growth_trend_popul=1;

// SELECT APPROPRIATE NHOOD UPDATE FUNCTION
    inform_fun = &OptInfoPso::inform_global;

// SELECT APPROPRIATE INERTIA WEIGHT UPDATE FUNCTION
    w = PSO_INERTIA;
    c1=2.5; c2=0.5; 

// SELECT APPROPRIATE INERTIA WEIGHT UPDATE FUNCTION
    calc_inertia_fun = &OptInfoPso::calc_inertia_adapt_dyn;
    handle.logMessage(LOGINFO, "Starting PSO with ", size, " particles\n");
    handle.logMessage(LOGINFO, "PSO goal = ", goal, "\n");

    EcoSystem->scaleVariables();
    EcoSystem->getOptScaledValues(x);
    EcoSystem->getOptLowerBounds(lowerb);
    EcoSystem->getOptUpperBounds(upperb);
    EcoSystem->getOptInitialValues(init);

    for (i = 0; i < nvars; ++i) {
        bestx[i] = x[i];
        param[i] = i;
    }
    
    for (i = 0; i < nvars; i++) {
            scalex[i] = x[i];
            lowerb[i] = lowerb[i] / init[i];
            upperb[i] = upperb[i] / init[i];
            if (lowerb[i] > upperb[i]) {
                tmp = lowerb[i];
                lowerb[i] = upperb[i];
                upperb[i] = tmp;
            }
     }
    best.bestf = EcoSystem->SimulateAndUpdate(x);
    best.index = 0;
    offset = EcoSystem->getFuncEval(); //number of function evaluations done before loop
    iters = 0;

// Initialize the particles
    i = 0;

// SWARM INITIALIZATION
//
// Initialize the fist particle with the best known position
    for (d = 0; d < nvars; d++) {
        // generate two numbers within the specified range and around trialx
	a = x[d];
        // initialize position
        pos[i][d] = a;
        pos_b[i][d] = a;
        gbest[i][d] = a;
        b = lowerb[d] + (upperb[d] - lowerb[d]) * (rand_r(&seed) * 1.0 / RAND_MAX);
        vel[i][d] = (a - b) / 2.;
	
    }
    fit[i] = EcoSystem->SimulateAndUpdate(pos[i]);
    fit_b[i] =  fit[i];
    fit_b[i] = fit[i]; // this is also the personal best
    if (fit[i] < best.bestf) {
        best.bestf = fit[i];
        bestx = pos[i];
        best.index=i;
    }
    // REPO
    double ***rand;
    rand = (double ***) calloc(size,sizeof(double **));
    for (i = 0; i < size; i++) {
	rand[i] = (double **) calloc(nvars,sizeof(double *)); 
        for (d = 0; d < nvars; d++) rand[i][d] = (double *) calloc(2,sizeof(double));
    }
	
    double **randDE;
    randDE = (double **) calloc(size,sizeof(double *));
    for (i=0; i< size; i++)
	randDE[i] = (double *) calloc(nvars+4,sizeof(double));

    for (i = 1; i < size; i++) {
	for (d = 0; d < nvars; d++) {
		rand[i][d][0] = rand_r(&seed);
                rand[i][d][1] = rand_r(&seed);
	}
    }

    // REPO

    for (i = 1; i < size; i++) {
        ostringstream spos, svel;
        spos << "Initial position of particle " << i << " [";
        svel << "Initial velocity of particle " << i << " [";
        // for each dimension
        for (d = 0; d < nvars; d++) {
            // generate two numbers within the specified range and around trialx
            a = lowerb[d] + (upperb[d] - lowerb[d]) * (rand[i][d][0] * 1.0 / RAND_MAX);
            b = lowerb[d] + (upperb[d] - lowerb[d]) * (rand[i][d][1] * 1.0 / RAND_MAX);

            // initialize position
            pos[i][d] = a;
            // best position is trialx
            pos_b[i][d] = a;
            gbest[i][d] = a;
            // initialize velocity
            vel[i][d] = (a - b) / 2.;
            spos << pos[i][d] << " ";
            svel << vel[i][d] << " ";
        }
        fit[i] = EcoSystem->SimulateAndUpdate(pos[i]);
     }

     for (i = 1; i < size; i++) {
        fit_b[i] = fit[i]; // this is also the personal best
        if (fit[i] < best.bestf) {
            best.index=i;
            best.bestf = fit[i];
            bestx = pos[i];
        }
        spos << " ]";
        svel << " ]";
        handle.logMessage(LOGDEBUG, spos.str().c_str());
        handle.logMessage(LOGDEBUG, svel.str().c_str());
    }


// RUN ALGORITHM
    psosteps = 0;
    while (1) {
        if (isZero(best.bestf)) {
            iters=0;
            for (d = 0; d < numThr; d++)
                 iters = iters + (EcoSystem->getFuncEval());
            iters= iters - offset;

            handle.logMessage(LOGINFO, "Error in PSO optimisation after", iters, "function evaluations, f(x) = 0");
            converge = -1;
            return;
        }
        /* if too many function evaluations occur, terminate the algorithm */
        if (psosteps > psoiter) {
            iters=0;
            for (d = 0; d < numThr; d++)
                 iters = iters + (EcoSystem->getFuncEval());
	    iters= iters - offset;

            handle.logMessage(LOGINFO, "\nStopping PSO optimisation algorithm\n");
            handle.logMessage(LOGINFO, "The optimisation stopped after", iters, "function evaluations");
            handle.logMessage(LOGINFO, "The optimisation stopped because the maximum number of PSO steps was reached");
            handle.logMessage(LOGINFO, "was reached and NOT because an optimum was found for this run");

            score = EcoSystem->SimulateAndUpdate(bestx);
            for (i = 0; i < nvars; i++)
                bestx[i] = bestx[i] * init[i];
            handle.logMessage(LOGINFO, "\nPSO finished with a likelihood score of", score);
            EcoSystem->storeVariables(score, bestx);
            return;
        }
	w=(this->*calc_inertia_fun)(psosteps);
        if (best.bestf <= goal) {
            handle.logMessage(LOGINFO, "\nStopping PSO optimisation algorithm\n");
            handle.logMessage(LOGINFO, "Goal achieved!!! @ step ", psosteps);
            score = EcoSystem->SimulateAndUpdate(bestx);
            handle.logMessage(LOGINFO, "\nPSO finished with a likelihood score of", score);
            break;
        }

        // update pos_nb matrix (find best of neighborhood for all particles)
	inform_global(comm, gbest, pos_b, fit_b, bestx, improved);
        // the value of improved was just used; reset it
        improved = 0;
	
	// If state var is equal to 3, the algorithm restart its population
        if (STATE==3){
		iter_without_improv_global_best=0;
		improv_with_stack_global_best=0;
		init_reset = 0;
           	if (growth_popul != 0) {
           		pos.AddRows(growth_popul, nvars, 0.0);
           		gbest.AddRows(growth_popul, nvars, 0.0);
			pos_new.AddRows(growth_popul, nvars, 0.0);
           		pos_b.AddRows(growth_popul, nvars, 0.0);
           		vel.AddRows(growth_popul, nvars, 0.0);
           		fit.resize(growth_popul, 0.0);
           		fit_b.resize(growth_popul, 0.0);
			if (growth_popul < 0 ) {
           			for (i = size; i < size+growth_popul; i++) {
           			    for (d = 0; d < nvars; d++) cfree(rand[i][d]);
               			    cfree(rand[i]);
				    cfree(randDE[i]);
           			}
           			rand   = (double ***) realloc(rand,(size+growth_popul)*sizeof(double));
				randDE = (double **)  realloc(randDE, (size+growth_popul)*sizeof(double));
			} else {
	                	rand = (double ***) realloc(rand,(size+growth_popul)*sizeof(double));
                                randDE = (double **)  realloc(randDE, (size+growth_popul)*sizeof(double));
	                	for (i = size; i < size+growth_popul; i++) {
	                	      rand[i]   = (double **) calloc(nvars,sizeof(double *));
				      randDE[i] = (double *)  calloc(nvars+4 ,sizeof(double));
	                	      for (d = 0; d < nvars; d++) rand[i][d] = (double *) calloc(2,sizeof(double));
	                	}
			}
		
		}
			
		if (nrestarts == 1 ) {
			init_reset=0;
		} 
		else if (nrestarts == 0 ) {
			init_reset=size;
		}
			
           	for (i = init_reset; i < (size+growth_popul); i++) {
                	for (d = 0; d < nvars; d++) {
                       		rand[i][d][0] = (rand_r(&seed) * 1.0) / RAND_MAX;
                       		rand[i][d][1] = (rand_r(&seed) * 1.0) / RAND_MAX;
                	}
           	}


           	for (i=init_reset;i<(size+growth_popul);i++){
	             for (d = 0; d < nvars; d++) {
                        	a = lowerb[d] + (upperb[d] - lowerb[d]) * rand[i][d][0];
                        	b = lowerb[d] + (upperb[d] - lowerb[d]) * rand[i][d][1];
	                	pos[i][d]   = a ;
	                	pos_b[i][d] = a ;
	                	vel[i][d]   = (a - b) / 2.;
	             }
                     fit[i]   = EcoSystem->SimulateAndUpdate(pos[i]);
                     fit_b[i] = fit[i];
	   	    }	
           	size=size+growth_popul;
           	growth_popul=0;
		STATE=0;
	}
	// RANDOM NUMBER FOR THE CASE OF PSO
        for (i = 0; i < size; i++) {
		p1 = (int) (((rand_r(&seed) * 1.0) / RAND_MAX)*size);
		do  
			p2 = (int) (((rand_r(&seed) * 1.0) / RAND_MAX)*size);
		while (p2 == p1);
                do 
                        p3 = (int) (((rand_r(&seed) * 1.0) / RAND_MAX)*size);
                while ((p3 == p1) || (p3 == p2));
                do 
                        p4 = (int) (((rand_r(&seed) * 1.0) / RAND_MAX)*size);
                while ((p4 == p3) || (p4 == p2) || (p4 == p1));
		randDE[i][0]= (double) p1;
                randDE[i][1]= (double) p2;
                randDE[i][2]= (double) p3;
                randDE[i][3]= (double) p4;
		for (d = 0; d < nvars; d++) {
			rand[i][d][0] = (rand_r(&seed) * 1.0) / RAND_MAX;
			rand[i][d][1] = (rand_r(&seed) * 1.0) / RAND_MAX;
	                randDE[i][d+4]= (rand_r(&seed) * 1.0) / RAND_MAX;
		}		
        }

        for (i = 0; i < size; i++) {
            ostringstream spos, svel;
            spos << "New position of particle " << i << " [";
            svel << "New velocity of particle " << i << " [";
		// CASE 1: PSO ALGORITHM. for each dimension 
		for (d = 0; d < nvars; d++) {
       		       // calculate stochastic coefficients
			rho1 = c1 * rand[i][d][0] ;
			rho2 = c2 * rand[i][d][1] ;
                	// update velocity
			vel[i][d] = w * vel[i][d] + rho1 * (pos_b[i][d] - pos[i][d]) + rho2 * (gbest[i][d] - pos[i][d]);
			pos[i][d] =  pos[i][d] + vel[i][d];
                	// update position
			spos << pos[i][d] << " ";
			svel << vel[i][d] << " ";
			position_within_bounds(pos,vel,lowerb,upperb,i,d);
		}
		fit[i] = EcoSystem->SimulateAndUpdate(pos[i]);
		if (fit[i] < fit_b[i]) {
			fit_b[i] = fit[i];
			pos_b[i] = pos[i];
		}					
        }
	
	for (i = 0; i < size; i++) {
              // update gbest??
              handle.logMessage(LOGDEBUG, "Particle ", i);
              handle.logMessage(LOGDEBUG, "The likelihood score is", best.bestf, "at the point");
              handle.logMessage(LOGDEBUG, "Its likelihood score is", fit[i], "at the point");
              if (fit[i] < best.bestf) {
                improved = 1;
                // update best fitness
                best.index=i;
                best.bestf = fit[i];
                for (d = 0; d < nvars; d++)
                    bestx[d] = pos[i][d] * init[d];
                EcoSystem->storeVariables(best.bestf, bestx);
                bestx = pos[i];
              } 
        }
 
	if (improved == 1) {
		iter_without_improv_global_best=0;
		if (((( previous_fbest_fitness - best.bestf ) / previous_fbest_fitness)*100 <= threshold_acept ) &&
			(previous_fbest_fitness != DBL_MAX)) {
			improv_with_stack_global_best++;
			consecutive_iters_global_best=0;
		} else { 
			consecutive_iters_global_best++; 
			improv_with_stack_global_best=0;
		}
                iters=0;
                for (d = 0; d < numThr; d++){
                    iters = iters + (EcoSystem->getFuncEval());
		}
                iters = iters - offset;
                handle.logMessage(LOGINFO, "\nNew optimum found after", iters, "function evaluations");
                handle.logMessage(LOGINFO, "The likelihood score is", best.bestf, "at the point");
                EcoSystem->writeBestValues();
		previous_fbest_fitness=best.bestf;
	} else {
                iter_without_improv_global_best++;
		consecutive_iters_global_best=0;
	}
	psosteps++;
    } // while (newf < bestf)
    
    for (i = 0; i < size; i++) {
		for ( d = 0; d < nvars; d ++ ) {
			cfree(rand[i][d]);
		}
		cfree(rand[i]);
		cfree(randDE[i]);
    }
    cfree(rand);
    cfree(randDE);
	
    iters=0;
    for (d = 0; d < numThr; d++) 
    	iters = iters + (EcoSystem->getFuncEval());
    
    iters = iters - offset;
    timestop = RUNID.returnTime();
    handle.logMessage(LOGINFO, "Existing PSO after ", iters, "function evaluations ...");
    for (d = 0; d < nvars; d++)
        bestx[d] = bestx[d] * init[d];
    EcoSystem->storeVariables(best.bestf, bestx);
    EcoSystem->writeBestValues();
    
}





#ifdef _OPENMP
void OptInfoPso::OptimiseLikelihoodREP() {
    OptimiseLikelihoodOMP();

}

void OptInfoPso::OptimiseLikelihoodOMP() {
    handle.logMessage(LOGINFO, "\nStarting ADAPTIVE PARALLEL PSO optimisation algorithm\n");
    double tmp;
    int ii, i, j, offset, rchange, rcheck, rnumber, init_reset;
    nvars = EcoSystem->numOptVariables();
    DoubleVector x(nvars);
    DoubleVector bestx(nvars);
    DoubleVector scalex(nvars);
    DoubleVector lowerb(nvars);
    DoubleVector upperb(nvars);
    DoubleVector init(nvars);
    IntVector param(nvars, 0);
    IntVector lbound(nvars, 0);
    IntVector rbounds(nvars, 0);
    IntVector trapped(nvars, 0);
    struct Best { double bestf; int index;};
    struct Best best;
    double timestop;
    iter_without_improv_global_best=0; 
    improv_with_stack_global_best=0;
    consecutive_iters_global_best=0;
    previous_fbest_fitness=DBL_MAX;
    growth_trend=0;
    STATE=0;
    growth_trend_popul=1;

// SOLVER
    size= (int) (nvars); 
    DoubleMatrix pos(size, nvars, 0.0);    // position matrix
    DoubleMatrix vel(size, nvars, 0.0);    // velocity matrix
    DoubleMatrix pos_new(size, nvars, 0.0);  // new population
    DoubleMatrix pos_b(size, nvars, 0.0);  // best position matrix
    DoubleVector fit(size); // particle fitness vector
    DoubleVector fit_b(size); // best fitness vector
    DoubleMatrix gbest(size, nvars, 0.0); // what is the best informed position for each particle
    IntMatrix comm(size, size, 0); // communications:who informs who rows : those who inform cols : those who are informed
    int improved; // whether solution->error was improved during the last iteration
    int d, step, index_del;
    int p1,p2,p3,p4;
    double prob;
    double dist;
    double a, b;
    double rho1, rho2; //random numbers (coefficients)
    double w; //current omega
    Inform_fun inform_fun;
    Calc_inertia_fun calc_inertia_fun; // inertia weight update function
    int psosteps = 0; //!= iters?
    ostringstream spos, svel;
    growth_popul=0;
    printf("seed %d\n", seed);
    threshold_acept=0.1;
    nrestarts=0;

// SELECT APPROPRIATE NHOOD UPDATE FUNCTION
    inform_fun = &OptInfoPso::inform_global;

// SELECT APPROPRIATE INERTIA WEIGHT UPDATE FUNCTION
    w = PSO_INERTIA;
    c1=2.5; c2=0.5;
    calc_inertia_fun = &OptInfoPso::calc_inertia_adapt_dyn;
    handle.logMessage(LOGINFO, "Starting PSO with ", size, " particles\n");
    handle.logMessage(LOGINFO, "PSO goal = ", goal, "\n");

  int numThr = omp_get_max_threads ( );
                EcoSystem->scaleVariables();
                for(i = 0; i < numThr; i++) // scale the variables for the ecosystem of every thread
                        EcoSystems[i]->scaleVariables();
  EcoSystem->getOptScaledValues(x);
  EcoSystem->getOptLowerBounds(lowerb);
  EcoSystem->getOptUpperBounds(upperb);
  EcoSystem->getOptInitialValues(init);

    for (i = 0; i < nvars; ++i) {
        bestx[i] = x[i];
        param[i] = i;
      }
    
    for (i = 0; i < nvars; i++) {
            scalex[i] = x[i];
            lowerb[i] = lowerb[i] / init[i];
            upperb[i] = upperb[i] / init[i];
            if (lowerb[i] > upperb[i]) {
                tmp = lowerb[i];
                lowerb[i] = upperb[i];
                upperb[i] = tmp;
            }
     }

    best.bestf = EcoSystems[0]->SimulateAndUpdate(x);
    best.index = 0;
    if (best.bestf != best.bestf) { //check for NaN
        handle.logMessage(LOGINFO, "Error starting PSO optimisation with f(x) = infinity");
        converge = -1;
        iters = 1;
        return;
    }


    offset = EcoSystems[0]->getFuncEval(); //number of function evaluations done before loop
    iters = 0;

// Initialize the particles
    i = 0;

// SWARM INITIALIZATION
//
// Initialize the fist particle with the best known position
    for (d = 0; d < nvars; d++) {
        // generate two numbers within the specified range and around trialx
        a = x[d];
        // initialize position
        pos[i][d] = a;
        pos_b[i][d] = a;
        gbest[i][d] = a;
        b = lowerb[d] + (upperb[d] - lowerb[d]) * (rand_r(&seed) * 1.0 / RAND_MAX);
        vel[i][d] = (a - b) / 2.;
    }
    fit[i] = EcoSystems[0]->SimulateAndUpdate(pos[i]);


    fit_b[i] = fit[i]; // this is also the personal best
    if (fit[i] < best.bestf) {
        best.bestf = fit[i];
        bestx = pos[i];
        best.index=i;
    }


    // REPO
    double ***rand;
    rand = (double ***) calloc(size,sizeof(double **));
    for (i = 0; i < size; i++) {
	rand[i] = (double **) calloc(nvars,sizeof(double *)); 
        for (d = 0; d < nvars; d++) rand[i][d] = (double *) calloc(2,sizeof(double));
    }
	
    double **randDE;
    randDE = (double **) calloc(size,sizeof(double *));
    for (i=0; i< size; i++)
	randDE[i] = (double *) calloc(nvars+4,sizeof(double));

    for (i = 1; i < size; i++) {
	for (d = 0; d < nvars; d++) {
		rand[i][d][0] = rand_r(&seed);
                rand[i][d][1] = rand_r(&seed);
	}
    }

    // REPO

#pragma omp parallel for default(shared) private(i,d,a,b) schedule(dynamic,1)
    for (i = 1; i < size; i++) {
        ostringstream spos, svel;
        spos << "Initial position of particle " << i << " [";
        svel << "Initial velocity of particle " << i << " [";
        // for each dimension
        for (d = 0; d < nvars; d++) {
            // generate two numbers within the specified range and around trialx
            a = lowerb[d] + (upperb[d] - lowerb[d]) * (rand[i][d][0] * 1.0 / RAND_MAX);
            b = lowerb[d] + (upperb[d] - lowerb[d]) * (rand[i][d][1] * 1.0 / RAND_MAX);

            // initialize position
            pos[i][d] = a;
            // best position is trialx
            pos_b[i][d] = a;
            gbest[i][d] = a;
            // initialize velocity
            vel[i][d] = (a - b) / 2.;
            spos << pos[i][d] << " ";
            svel << vel[i][d] << " ";
        }
        fit[i] = EcoSystems[omp_get_thread_num ()]->SimulateAndUpdate(pos[i]);
     }

     for (i = 1; i < size; i++) {
        fit_b[i] = fit[i]; // this is also the personal best
        if (fit[i] < best.bestf) {
            best.index=i;
            best.bestf = fit[i];
            bestx = pos[i];
        }
        spos << " ]";
        svel << " ]";
        handle.logMessage(LOGDEBUG, spos.str().c_str());
        handle.logMessage(LOGDEBUG, svel.str().c_str());
    }


// RUN ALGORITHM
    psosteps = 0;

    while (1) {
        if (isZero(best.bestf)) {
            iters=0;
            for (d = 0; d < numThr; d++)
                 iters = iters + (EcoSystems[d]->getFuncEval());
            iters= iters - offset;

            handle.logMessage(LOGINFO, "Error in PSO optimisation after", iters, "function evaluations, f(x) = 0");
            converge = -1;
            return;
        }
        /* if too many function evaluations occur, terminate the algorithm */
        if (psosteps > psoiter) {
            iters=0;
            for (d = 0; d < numThr; d++)
                 iters = iters + (EcoSystems[d]->getFuncEval());
	    iters= iters - offset;

            handle.logMessage(LOGINFO, "\nStopping PSO optimisation algorithm\n");
            handle.logMessage(LOGINFO, "The optimisation stopped after", iters, "function evaluations");
            handle.logMessage(LOGINFO, "The optimisation stopped because the maximum number of PSO steps was reached");
            handle.logMessage(LOGINFO, "was reached and NOT because an optimum was found for this run");

            score = EcoSystems[0]->SimulateAndUpdate(bestx);
            score = EcoSystem->SimulateAndUpdate(bestx);
            for (i = 0; i < nvars; i++)
                bestx[i] = bestx[i] * init[i];
            handle.logMessage(LOGINFO, "\nPSO finished with a likelihood score of", score);
            EcoSystem->storeVariables(score, bestx);
            return;
        }
        // update inertia weight
	w=(this->*calc_inertia_fun)(psosteps);
        // check optimization goal
        if (best.bestf <= goal) {
            handle.logMessage(LOGINFO, "\nStopping PSO optimisation algorithm\n");
            handle.logMessage(LOGINFO, "Goal achieved!!! @ step ", psosteps);
            score = EcoSystems[0]->SimulateAndUpdate(bestx);
            score = EcoSystem->SimulateAndUpdate(bestx);
            handle.logMessage(LOGINFO, "\nPSO finished with a likelihood score of", score);
            break;
        }

        // update pos_nb matrix (find best of neighborhood for all particles)
	inform_global(comm, gbest, pos_b, fit_b, bestx, improved);
        // the value of improved was just used; reset it
        improved = 0;
	
	// If state var is equal to 3, the algorithm restart its population
        if (STATE==3){
		iter_without_improv_global_best=0;
		improv_with_stack_global_best=0;
		init_reset = 0;
           	if (growth_popul != 0) {
           		pos.AddRows(growth_popul, nvars, 0.0);
           		gbest.AddRows(growth_popul, nvars, 0.0);
			pos_new.AddRows(growth_popul, nvars, 0.0);
           		pos_b.AddRows(growth_popul, nvars, 0.0);
           		vel.AddRows(growth_popul, nvars, 0.0);
           		fit.resize(growth_popul, 0.0);
           		fit_b.resize(growth_popul, 0.0);
			if (growth_popul < 0 ) {
           			for (i = size; i < size+growth_popul; i++) {
           			    for (d = 0; d < nvars; d++) cfree(rand[i][d]);
               			    cfree(rand[i]);
				    cfree(randDE[i]);
           			}
           			rand   = (double ***) realloc(rand,(size+growth_popul)*sizeof(double));
				randDE = (double **)  realloc(randDE, (size+growth_popul)*sizeof(double));
			} else {
	                	rand = (double ***) realloc(rand,(size+growth_popul)*sizeof(double));
                                randDE = (double **)  realloc(randDE, (size+growth_popul)*sizeof(double));
	                	for (i = size; i < size+growth_popul; i++) {
	                	      rand[i]   = (double **) calloc(nvars,sizeof(double *));
				      randDE[i] = (double *)  calloc(nvars+4 ,sizeof(double));
	                	      for (d = 0; d < nvars; d++) rand[i][d] = (double *) calloc(2,sizeof(double));
	                	}
			}
		
		}
			
		if (nrestarts == 1 ) {
			init_reset=0;
		} 
		else if (nrestarts == 0 ) {
			init_reset=size;
		}
	
           	for (i = init_reset; i < (size+growth_popul); i++) {
                	for (d = 0; d < nvars; d++) {
                       		rand[i][d][0] = (rand_r(&seed) * 1.0) / RAND_MAX;
                       		rand[i][d][1] = (rand_r(&seed) * 1.0) / RAND_MAX;
                	}
           	}

#pragma omp parallel for default(shared) private(i,d,a,b) schedule(dynamic,1)	   
           	for (i=init_reset;i<(size+growth_popul);i++){
	             for (d = 0; d < nvars; d++) {
                        	a = lowerb[d] + (upperb[d] - lowerb[d]) * rand[i][d][0];
                        	b = lowerb[d] + (upperb[d] - lowerb[d]) * rand[i][d][1];
	                	pos[i][d]   = a ;
	                	pos_b[i][d] = a ;
	                	vel[i][d]   = (a - b) / 2.;
	             }
                     fit[i]   = EcoSystems[omp_get_thread_num()]->SimulateAndUpdate(pos[i]);
                     fit_b[i] = fit[i];
	   	}
           	size=size+growth_popul;
           	growth_popul=0;
		STATE=0;
	}
	

	// RANDOM NUMBER FOR THE CASE OF PSO
        for (i = 0; i < size; i++) {
		p1 = (int) (((rand_r(&seed) * 1.0) / RAND_MAX)*size);
		do  
			p2 = (int) (((rand_r(&seed) * 1.0) / RAND_MAX)*size);
		while (p2 == p1);
                do 
                        p3 = (int) (((rand_r(&seed) * 1.0) / RAND_MAX)*size);
                while ((p3 == p1) || (p3 == p2));
                do 
                        p4 = (int) (((rand_r(&seed) * 1.0) / RAND_MAX)*size);
                while ((p4 == p3) || (p4 == p2) || (p4 == p1));
		randDE[i][0]= (double) p1;
                randDE[i][1]= (double) p2;
                randDE[i][2]= (double) p3;
                randDE[i][3]= (double) p4;
		for (d = 0; d < nvars; d++) {
			rand[i][d][0] = (rand_r(&seed) * 1.0) / RAND_MAX;
			rand[i][d][1] = (rand_r(&seed) * 1.0) / RAND_MAX;
	                randDE[i][d+4]= (rand_r(&seed) * 1.0) / RAND_MAX;
		}		
        }

#pragma omp parallel for default(shared) private(i,d,rho1,rho2,p1,p2,p3,p4,prob) schedule(dynamic,1)
        for (i = 0; i < size; i++) {
//      handle.logMessage(LOGDEBUG, "In parallel",omp_get_thread_num(),"\n");
            ostringstream spos, svel;
            spos << "New position of particle " << i << " [";
            svel << "New velocity of particle " << i << " [";
		// CASE 1: PSO ALGORITHM. for each dimension 
		for (d = 0; d < nvars; d++) {
       		       // calculate stochastic coefficients
			rho1 = c1 * rand[i][d][0] ;
			rho2 = c2 * rand[i][d][1] ;
                	// update velocity
			vel[i][d] = w * vel[i][d] + rho1 * (pos_b[i][d] - pos[i][d]) + rho2 * (gbest[i][d] - pos[i][d]);
			pos[i][d] = pos[i][d] + vel[i][d];
                	// update position
			spos << pos[i][d] << " ";
			svel << vel[i][d] << " ";
			position_within_bounds(pos,vel,lowerb,upperb,i,d);
		}
		fit[i] = EcoSystems[omp_get_thread_num()]->SimulateAndUpdate(pos[i]);
		if (fit[i] < fit_b[i]) {
			fit_b[i] = fit[i];
			pos_b[i] = pos[i];
		}					
        }

	
	for (i = 0; i < size; i++) {
              // update gbest??
              handle.logMessage(LOGDEBUG, "Particle ", i);
              handle.logMessage(LOGDEBUG, "The likelihood score is", best.bestf, "at the point");
              handle.logMessage(LOGDEBUG, "Its likelihood score is", fit[i], "at the point");
              if (fit[i] < best.bestf) {
                improved = 1;
                // update best fitness
                best.index=i;
                best.bestf = fit[i];
                for (d = 0; d < nvars; d++)
                    bestx[d] = pos[i][d] * init[d];
                EcoSystem->storeVariables(best.bestf, bestx);
                bestx = pos[i];
              } 
        }
 
	if (improved == 1) {
		iter_without_improv_global_best=0;
		if (((( previous_fbest_fitness - best.bestf ) / previous_fbest_fitness)*100 <= threshold_acept ) &&
			(previous_fbest_fitness != DBL_MAX)) {
			improv_with_stack_global_best++;
			consecutive_iters_global_best=0;
		} else { 
			consecutive_iters_global_best++; 
			improv_with_stack_global_best=0;
		}
                iters=0;
                for (d = 0; d < numThr; d++){
                    iters = iters + (EcoSystems[d]->getFuncEval());
		}
                iters = iters - offset;
                handle.logMessage(LOGINFO, "\nNew optimum found after", iters, "function evaluations");
                handle.logMessage(LOGINFO, "The likelihood score is", best.bestf, "at the point");
                EcoSystem->writeBestValues();
		previous_fbest_fitness=best.bestf;
	} else {
                iter_without_improv_global_best++;
		consecutive_iters_global_best=0;
	}
	psosteps++;
    } // while (newf < bestf)
    
    for (i = 0; i < size; i++) {
		for ( d = 0; d < nvars; d ++ ) {
			cfree(rand[i][d]);
		}
		cfree(rand[i]);
		cfree(randDE[i]);
    }
    cfree(rand);
    cfree(randDE);
	
    iters=0;
    for (d = 0; d < numThr; d++) 
    	iters = iters + (EcoSystems[d]->getFuncEval());
    
    iters = iters - offset;
    timestop = RUNID.returnTime();
    handle.logMessage(LOGINFO, "Existing PSO after ", iters, "function evaluations ...");
    for (d = 0; d < nvars; d++)
        bestx[d] = bestx[d] * init[d];
    EcoSystem->storeVariables(best.bestf, bestx);
    EcoSystem->writeBestValues();
        
}
#endif

void OptInfoPso::growth_trend_manager() {
    double MAX_C   = 2.5;
    double MIN_LOW = 0.1;
    double MAX_CR  = 0.9;
    double MIN_CR  = 0.1;
	
    if (growth_trend==1){
		if ((c2+0.1) <= MAX_C) {
		     c2=c2+0.1;
		     c1=c1-0.1;
		} else growth_trend=2;
    } 
    else if (growth_trend==2){
		if ((c2-0.1) >= MIN_LOW) {
	             c2=c2-0.1;
	             c1=c1+0.1;
		} else growth_trend=1;
    }
}



double OptInfoPso::w_manager(int step){
    double rand_n, W;
    double max_w1,min_w1;
    double max_F, min_F;

    if (STATE==0) {
        max_w1 = 0.80;
        min_w1 = 0.60;
	max_F  = 0.80;
	min_F  = 0.60;
    }
    else if (STATE==4) {
        max_w1 = 0.40;
        min_w1 = 0.10;
	max_F  = 0.40;
	min_F  = 0.10;		
    }
    else if ((STATE==1)||(STATE==3)) {
        max_w1 = 0.80;
        min_w1 = 0.10;
	max_F  = 0.80;
	min_F  = 0.10;			
    }
    else if (STATE==2) {
	// BUSQUEDA CON INERCIAS DE VALORES MEDIOS
        max_w1 = 0.60;
        min_w1 = 0.40;
	max_F  = 0.60;
	min_F  = 0.40;		
    }
    
    // CALC W AND F
    rand_n=(rand_r(&seed) * 1.0 / RAND_MAX);
    W =  ( min_w1 )+ rand_n * ( max_w1 - min_w1 ); 
    return W;
}

/**
 *  * \brief calculate inertia weight based on convergence information
 *   */
double OptInfoPso::calc_inertia_adapt_dyn(int step) {
    int MAX_NO_IMP=10;
    int MAX_STUCK =10;
    int MAX_IMP   =2;
    double max_1, max_2, min_1, min_2;
    double W,rand_n;
    int UNIT=(int)(nvars);
    int MAX_UNIT=nvars*5;
    int MIN_UNIT=(int)(nvars);

    if ((iter_without_improv_global_best>MAX_NO_IMP*2)||(improv_with_stack_global_best>MAX_STUCK*2)){
		if (STATE==1) {
			nrestarts=0;
		}
		else if (STATE==4) {
			nrestarts=1;
		}
	
                if (growth_trend_popul==1) {
                        if ((size+UNIT)<=MAX_UNIT) {
                                        growth_popul=growth_popul+UNIT;
                        }
                        else    growth_trend_popul=0;
                }
                if (growth_trend_popul==0) {
                        if ((size-UNIT)>=MIN_UNIT) {
                                        growth_popul=growth_popul-UNIT;
                        }
                        else    growth_trend_popul=1;
                }

	        STATE=3; // RESTART
		
    }
    else if (( iter_without_improv_global_best % MAX_NO_IMP == (MAX_NO_IMP-1)) ) {
        if (STATE!=1) {
           if (growth_trend_popul == 0) growth_trend_popul=1;
        }
	STATE=1;
    }
    else if (( improv_with_stack_global_best % MAX_STUCK    == (MAX_STUCK -1)) ) {  
        if (STATE!=4) {
           if (growth_trend_popul == 1) growth_trend_popul=0;
        }
        STATE=4;
    }
    else if ( consecutive_iters_global_best >= MAX_IMP ) {
        if (STATE!=4) {
           if (growth_trend_popul == 1) growth_trend_popul=0;
        }
        STATE=2;
    }
    else if (((  iter_without_improv_global_best < MAX_NO_IMP  ) &&
	      (  improv_with_stack_global_best   < MAX_STUCK   ) &&
	      (  consecutive_iters_global_best   < MAX_IMP     ) &&
	      (  STATE != 0 ))){
	STATE=0;
    }
    // CALC C1 AND C2
    if ((STATE == 2) ||  (STATE == 0) ) {
	growth_trend=0;
    } else if ((STATE == 1) || (STATE == 3) || (STATE == 4)) {
        rand_n=(rand_r(&seed) * 1.0 / RAND_MAX);
        if (growth_trend==0) 
		growth_trend = ceil(rand_n * 2 );
        growth_trend_manager();
    }
    
    // W STATE MANAGER
    W = w_manager(step);

    return W;
}


/**
 *  * \brief POSITION WITHIN BOUND. Check if the new value is within the range
 *   */
void OptInfoPso::position_within_bounds(DoubleMatrix& pos, DoubleMatrix& vel, DoubleVector& lowerb, DoubleVector& upperb, int i, int d) {
                    if (pos[i][d] < lowerb[d]) {
                        pos[i][d] = lowerb[d];
                        vel[i][d] = 0.0;
                    }
                    else if (pos[i][d] > upperb[d]) {
                        pos[i][d] = upperb[d];
                        vel[i][d] = 0.0;
                    }
}

/**
 * \brief NEIGHBORHOOD (COMM) MATRIX STRATEGIES. global neighborhood
 */
void OptInfoPso::inform_global(IntMatrix& comm, DoubleMatrix& pos_nb, DoubleMatrix& pos_b, DoubleVector& fit_b, DoubleVector& gbest, int improved) { 
    int i;
// all particles have the same attractor (gbest)
// copy the contents of gbest to pos_nb
    for (i = 0; i < size; i++)
        pos_nb[i] = gbest;
}
