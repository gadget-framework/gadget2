/* PARALLEL ADAPTIVE MULTIRESTART DIFFERENTIAL EVOLUTION 
 * This method of optimization is self-adaptive, that is, it adjusts its configuration
 * parameters throughout the execution time. Therefore, it does not need any input argument,
 * so the user does not spend time customizing the algorithm. In addition, a multi-restart
 * mechanism was built, to avoid stagnation in local optima, and a parallelization was implemented
 * using openMP.
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

void OptInfoDE::OptimiseLikelihood() {
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
    growth_popul=0;

//  INITIALIZE VARIABLES
    iter_without_improv_global_best=0; 
    improv_with_stack_global_best=0;
    consecutive_iters_global_best=0;
    previous_fbest_fitness=DBL_MAX;
    growth_trend=0;
    STATE=0;
    growth_trend_popul=1;
    size= (int) (nvars);
    threshold_acept=0.1;
    nrestarts=0;

// SOLVER
    printf("SOLVER DE\n");
    DoubleMatrix pos(size, nvars, 0.0);  // new population    
    DoubleMatrix vel(size, nvars, 0.0);    // velocity matrix
    DoubleMatrix pos_new(size, nvars, 0.0);  // new population
    DoubleMatrix pos_b(size, nvars, 0.0);  // best position matrix
    DoubleVector fit(size); // particle fitness vector
    DoubleVector fit_b(size); // best fitness vector
    IntMatrix comm(size, size, 0); // communications:who informs who rows : those who inform cols : those who are informed
    int improved; // whether solution->error was improved during the last iteration
    int d, step, index_del;
    int p1,p2,p3,p4;
    double prob;
    double dist;
    double a, b;
    double rho1, rho2; //random numbers (coefficients)
    double w; //current omega
    int steps = 0; //!= iters?
    ostringstream spos, svel;
    int numThr = 1;

    printf("seed %d\n", seed);

    handle.logMessage(LOGINFO, "Starting DE with particles ", size, "\n");

    F=0.8; CR=0.9;
	
    handle.logMessage(LOGINFO, "DE initial inertia ", w, "\n");
    handle.logMessage(LOGINFO, "DE F", F, "\n");
    handle.logMessage(LOGINFO, "DE CR", CR, "\n");
    handle.logMessage(LOGINFO, "DE goal", goal, "\n");

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

    if (best.bestf != best.bestf) { //check for NaN
        handle.logMessage(LOGINFO, "Error starting DE optimisation with f(x) = infinity");
        converge = -1;
        iters = 1;
        return;
    }


    offset = EcoSystem->getFuncEval(); //number of function evaluations done before loop
    iters = 0;

// Initialize the particles
    i = 0;
    handle.logMessage(LOGINFO, "Initialising DE ", size, "particles\n");


// Initialize the fist particle with the best known position
    for (d = 0; d < nvars; d++) {
        // generate two numbers within the specified range and around trialx
        a = x[d];
        // initialize position
        pos[i][d] = a;
        pos_b[i][d] = a;
        b = lowerb[d] + (upperb[d] - lowerb[d]) * (rand_r(&seed) * 1.0 / RAND_MAX);
        vel[i][d] = (a - b) / 2.;
    }
    fit[i] = EcoSystem->SimulateAndUpdate(pos[i]);

    printf("\n\n** INIT BEST FUNCTION %lf  \n\n", fit[i]);

    fit_b[i] = fit[i]; // this is also the personal best
    if (fit[i] < best.bestf) {
        best.bestf = fit[i];
        bestx = pos[i];
        best.index=i;
    }


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
    steps = 0;

    while (1) {
//        handle.logMessage(LOGINFO, "PSO optimisation after", psosteps * size, "\n");
        if (isZero(best.bestf)) {
            iters=0;
            for (d = 0; d < numThr; d++)
                 iters = iters + (EcoSystem->getFuncEval());
            iters= iters - offset;

            handle.logMessage(LOGINFO, "Error in DE optimisation after", iters, "function evaluations, f(x) = 0");
            converge = -1;
            return;
        }
        /* if too many function evaluations occur, terminate the algorithm */
        if (steps > iter) {
            iters=0;
            for (d = 0; d < numThr; d++)
                 iters = iters + (EcoSystem->getFuncEval());
	    iters= iters - offset;

            handle.logMessage(LOGINFO, "\nStopping DE optimisation algorithm\n");
            handle.logMessage(LOGINFO, "The optimisation stopped after", iters, "function evaluations");
            handle.logMessage(LOGINFO, "The optimisation stopped because the maximum number of DE steps was reached");
            handle.logMessage(LOGINFO, "was reached and NOT because an optimum was found for this run");

            score = EcoSystem->SimulateAndUpdate(bestx);
            for (i = 0; i < nvars; i++)
                bestx[i] = bestx[i] * init[i];
            handle.logMessage(LOGINFO, "\nPSO finished with a likelihood score of", score);
            EcoSystem->storeVariables(score, bestx);
            return;
        }
		F = calc_adapt_parameters(steps);
	//}
        // check optimization goal
        if (best.bestf <= goal) {
            handle.logMessage(LOGINFO, "\nStopping DE optimisation algorithm\n");
            handle.logMessage(LOGINFO, "Goal achieved!!! @ step ", steps);
            score = EcoSystem->SimulateAndUpdate(bestx);
            handle.logMessage(LOGINFO, "\nPSO finished with a likelihood score of", score);
            break;
        }


        improved = 0;
	
	// If state var is equal to 3, the algorithm restart its population
        if (STATE==3){
		iter_without_improv_global_best=0;
		improv_with_stack_global_best=0;
		init_reset = 0;
           	if (growth_popul != 0) {
           		printf("growth_popul %d\n",growth_popul);
           		pos.AddRows(growth_popul, nvars, 0.0);
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
//      handle.logMessage(LOGDEBUG, "In parallel",omp_get_thread_num(),"\n");
            ostringstream spos, svel;
            spos << "New position of particle " << i << " [";
            svel << "New velocity of particle " << i << " [";
	    p1= (int) randDE[i][0];
	    p2= (int) randDE[i][1];
	    p3= (int) randDE[i][2];
	    p4= (int) randDE[i][3];

		for (d = 0; d < nvars; d++) {
	                prob = randDE[i][d+4];
			if (prob < CR ) {
                                if (STATE == 2)
                                        pos_new[i][d] = bestx[d]    + F *  ( pos[p1][d] - pos[p2][d] ) + F *  ( pos[p3][d] - pos[p4][d] );
                                if (STATE == 0)
                                        pos_new[i][d] = pos[i][d]   + F *  ( pos[p1][d] - pos[i][d] )  + F *  ( pos[p2][d] - pos[p3][d] );
                                if (STATE == 4)
                                        pos_new[i][d] = pos[p3][d]  + F *  ( bestx[d]   - pos[p3][d] ) + F * ( pos[p1][d] - pos[p2][d] ) ;
                                if ((STATE == 1) || (STATE == 3))
                                        pos_new[i][d] = pos[i][d]   + F *  ( bestx[d]   - pos[i][d] )  + F * ( pos[p1][d] - pos[p2][d] ) ;
				position_within_bounds(pos_new,vel,lowerb,upperb,i,d);
			}
		}
		fit[i] = EcoSystem->SimulateAndUpdate(pos_new[i]);
        }

	
	for (i = 0; i < size; i++) {
              if (fit[i] < fit_b[i]) {
                        fit_b[i] = fit[i];
                        pos[i] = pos_new[i];
              }
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
	steps++;
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
    handle.logMessage(LOGINFO, "Existing DE after ", iters, "function evaluations ...");
    for (d = 0; d < nvars; d++)
        bestx[d] = bestx[d] * init[d];
    EcoSystem->storeVariables(best.bestf, bestx);
    EcoSystem->writeBestValues();
    
}





#ifdef _OPENMP
void OptInfoDE::OptimiseLikelihoodREP() {
    OptimiseLikelihoodOMP();

}

void OptInfoDE::OptimiseLikelihoodOMP() {
    handle.logMessage(LOGINFO, "\nStarting PARALLEL ADAPTIVE MULTIRESTART DE optimisation algorithm\n");
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

    printf("SOLVER DE\n");
    size= (int) (nvars);
    
    DoubleMatrix pos(size, nvars, 0.0);    // position matrix
    DoubleMatrix vel(size, nvars, 0.0);    // velocity matrix
    DoubleMatrix pos_new(size, nvars, 0.0);  // new population
    DoubleMatrix pos_b(size, nvars, 0.0);  // best position matrix
    DoubleVector fit(size); // particle fitness vector
    DoubleVector fit_b(size); // best fitness vector
    IntMatrix comm(size, size, 0); // communications:who informs who rows : those who inform cols : those who are informed
    int improved; // whether solution->error was improved during the last iteration
    int d, step, index_del;
    int p1,p2,p3,p4;
    double prob;
    double dist;
    double a, b;
    int steps = 0; //!= iters?
    ostringstream spos, svel;
    growth_popul=0;
    printf("seed %d\n", seed);
    threshold_acept=0.1;
    nrestarts=0;
//    if (seed == 0) seed = 1234;

    handle.logMessage(LOGINFO, "Starting DE with particles ", size, "\n");

    F=0.8; CR=0.9;
	
    handle.logMessage(LOGINFO, "DE F ", F , "\n");
    handle.logMessage(LOGINFO, "DE CR", CR, "\n");
    handle.logMessage(LOGINFO, "DE goal", goal, "\n");

    EcoSystem->scaleVariables();
    int numThr = omp_get_max_threads ( );
#pragma omp parallel for
    for (i=0;i<numThr;i++)
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
    handle.logMessage(LOGINFO, "Initialising DE ", size, "particles\n");

// Initialize the fist particle with the best known position
    for (d = 0; d < nvars; d++) {
        // generate two numbers within the specified range and around trialx
        a = x[d];
        // initialize position
        pos[i][d] = a;
        pos_b[i][d] = a;
        b = lowerb[d] + (upperb[d] - lowerb[d]) * (rand_r(&seed) * 1.0 / RAND_MAX);
        vel[i][d] = (a - b) / 2.;
    }
    fit[i] = EcoSystems[0]->SimulateAndUpdate(pos[i]);
    
    printf("\n\n** INIT BEST FUNCTION %lf  \n\n", fit[i]);

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
    steps = 0;

    while (1) {
//        handle.logMessage(LOGINFO, "PSO optimisation after", psosteps * size, "\n");
        if (isZero(best.bestf)) {
            iters=0;
            for (d = 0; d < numThr; d++)
                 iters = iters + (EcoSystems[d]->getFuncEval());
            iters= iters - offset;

            handle.logMessage(LOGINFO, "Error in DE optimisation after", iters, "function evaluations, f(x) = 0");
            converge = -1;
            return;
        }
        /* if too many function evaluations occur, terminate the algorithm */
        if (steps > iter) {
            iters=0;
            for (d = 0; d < numThr; d++)
                 iters = iters + (EcoSystems[d]->getFuncEval());
	    iters= iters - offset;

            handle.logMessage(LOGINFO, "\nStopping DE optimisation algorithm\n");
            handle.logMessage(LOGINFO, "The optimisation stopped after", iters, "function evaluations");
            handle.logMessage(LOGINFO, "The optimisation stopped because the maximum number of PSO steps was reached");
            handle.logMessage(LOGINFO, "was reached and NOT because an optimum was found for this run");

            score = EcoSystems[0]->SimulateAndUpdate(bestx);
            for (i = 0; i < nvars; i++)
                bestx[i] = bestx[i] * init[i];
            handle.logMessage(LOGINFO, "\nPSO finished with a likelihood score of", score);
            EcoSystem->storeVariables(score, bestx);
            return;
        }
		F = calc_adapt_parameters(steps);
        // check optimization goal
        if (best.bestf <= goal) {
            handle.logMessage(LOGINFO, "\nStopping PSO optimisation algorithm\n");
            handle.logMessage(LOGINFO, "Goal achieved!!! @ step ", steps);
            score = EcoSystems[0]->SimulateAndUpdate(bestx);
            handle.logMessage(LOGINFO, "\nPSO finished with a likelihood score of", score);
            break;
        }

        improved = 0;
	
	// If state var is equal to 3, the algorithm restart its population
        if (STATE==3){
		iter_without_improv_global_best=0;
		improv_with_stack_global_best=0;
		init_reset = 0;
           	if (growth_popul != 0) {
           		printf("growth_popul %d\n",growth_popul);
           		pos.AddRows(growth_popul, nvars, 0.0);
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
	        printf("SOLVER DE\n");
           	size=size+growth_popul;
           	growth_popul=0;
		STATE=0;
	}
	

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

#pragma omp parallel for default(shared) private(i,d,p1,p2,p3,p4,prob) schedule(dynamic,1)
        for (i = 0; i < size; i++) {
//      handle.logMessage(LOGDEBUG, "In parallel",omp_get_thread_num(),"\n");
            ostringstream spos, svel;
            spos << "New position of particle " << i << " [";
            svel << "New velocity of particle " << i << " [";
	    p1= (int) randDE[i][0];
	    p2= (int) randDE[i][1];
	    p3= (int) randDE[i][2];
	    p4= (int) randDE[i][3];
  	    for (d = 0; d < nvars; d++) {
	                prob = randDE[i][d+4];
			if (prob < CR ) {
				// DE/best/2
				if (STATE == 2)
                                        pos_new[i][d] = bestx[d] + F *  ( pos[p1][d] - pos[p2][d] ) + F *  ( pos[p3][d] - pos[p4][d] );
				// DE/current-to-rand/1
				if (STATE == 0)
                                        pos_new[i][d] = pos[i][d]   + F *  ( pos[p1][d] - pos[i][d] )  + F *  ( pos[p2][d] - pos[p3][d] );
				// DE/rand-to-best/1
                                if (STATE == 4)
                                        pos_new[i][d] = pos[p3][d]  + F *  ( bestx[d] - pos[p3][d] ) + F * ( pos[p1][d] - pos[p2][d] ) ; 
                                // DE/current-to-best/1/bin
				if ((STATE == 1) || (STATE == 3))
                                        pos_new[i][d] = pos[i][d]   + F *  ( bestx[d] - pos[i][d] )  + F * ( pos[p1][d] - pos[p2][d] ) ;

				position_within_bounds(pos_new,vel,lowerb,upperb,i,d);
			}
	    }
	    
	    fit[i] = EcoSystems[omp_get_thread_num()]->SimulateAndUpdate(pos_new[i]);
        }

	
	for (i = 0; i < size; i++) {
              if (fit[i] < fit_b[i]) {
                       fit_b[i] = fit[i];
                       pos[i] = pos_new[i];
              } 
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
	steps++;
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
    handle.logMessage(LOGINFO, "Existing DE after ", iters, "function evaluations ...");
    for (d = 0; d < nvars; d++)
        bestx[d] = bestx[d] * init[d];
    EcoSystem->storeVariables(best.bestf, bestx);
    EcoSystem->writeBestValues();
    
}
#endif

void OptInfoDE::growth_trend_manager() {
    double MAX_CR  = 0.9;
    double MIN_CR  = 0.1;
	
    if (growth_trend==1){
	if ((CR+0.1) <= MAX_CR) {
		CR=CR+0.1;
	} else growth_trend=2;			
    } 
    else if (growth_trend==2){
	if ((CR-0.1) >= MIN_CR) {
                CR=CR-0.1;
	} else growth_trend=1;			
    }
}



double OptInfoDE::F_manager(int step){
    double rand_n;
    double max_w1,min_w1;
    double max_F, min_F;

    if (STATE==0) {
	max_F  = 0.80;
	min_F  = 0.60;
    }
    else if (STATE==4) {
	max_F  = 0.40;
	min_F  = 0.10;		
    }
    else if ((STATE==1)||(STATE==3)) {
	max_F  = 0.80;
	min_F  = 0.10;			
    }
    else if (STATE==2) {
	// BUSQUEDA CON INERCIAS DE VALORES MEDIOS
	max_F  = 0.60;
	min_F  = 0.40;		
    }
    
    // CALC W AND F
    rand_n=(rand_r(&seed) * 1.0 / RAND_MAX);
    F =  ( min_F  )+ rand_n * ( max_F  - min_F  ); 
    return F;
}

/**
 *  * \brief calculate inertia weight based on convergence information
 *   */
double OptInfoDE::calc_adapt_parameters(int step) {
    int MAX_NO_IMP=10;
    int MAX_STUCK =10;
    int MAX_IMP   =2;
    double max_1, max_2, min_1, min_2;
    double rand_n;
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
    
    // F STATE MANAGER
    F = F_manager(step);


    return F;
}


void OptInfoDE::position_within_bounds(DoubleMatrix& pos, DoubleMatrix& vel, DoubleVector& lowerb, DoubleVector& upperb, int i, int d) {
//                if (clamp_pos) {
                    if (pos[i][d] < lowerb[d]) {
                        pos[i][d] = lowerb[d];
                        vel[i][d] = 0.0;
                    }
                    else if (pos[i][d] > upperb[d]) {
                        pos[i][d] = upperb[d];
                        vel[i][d] = 0.0;
                    }
}

