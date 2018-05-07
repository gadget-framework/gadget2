#include "gadget.h"    //All the required standard header files are in here
#include "optinfo.h"
#include "mathfunc.h"
#include "doublevector.h"
#include "intvector.h"
#include "errorhandler.h"
#include "ecosystem.h"
#include "global.h"
#include "pso.h"
#include <float.h>
#include "runid.h"

#ifdef _OPENMP
#include "omp.h"
#endif

extern Ecosystem* EcoSystem;
#ifdef _OPENMP
extern Ecosystem** EcoSystems;
#endif

//==============================================================
//// calulate swarm size based on dimensionality
int OptInfoPso::pso_calc_swarm_size(int dim) {
    int size = 10. + 2. * sqrt(dim);
    return (size > PSO_MAX_SIZE ? PSO_MAX_SIZE : size);
}

void OptInfoPso::OptimiseLikelihood() {

    handle.logMessage(LOGINFO, "\nStarting PSO optimisation algorithm\n");
    double bestf, tmp, MAXTIME=1000;
    int ii, i, offset, rchange, rcheck, rnumber;
    int nvars = EcoSystem->numOptVariables();
    int logspace=1;
    int eps=10e-16;
    double timestop;
    double t1, t2;


    DoubleVector x(nvars);
//    DoubleVector trialx(nvars);
    DoubleVector bestx(nvars);
    DoubleVector lowerb(nvars);
    DoubleVector upperb(nvars);
    DoubleVector init(nvars);
//DoubleVector initialstep(nvars, rho);
    IntVector param(nvars, 0);
    IntVector lbound(nvars, 0);
    IntVector rbounds(nvars, 0);
    IntVector trapped(nvars, 0);

    /**
     * Internal variables for PSO
     */
//    printf("nvars:%d\n",nvars);
    
    if (size == 0) size = pso_calc_swarm_size(nvars);
    DoubleMatrix pos(size, nvars, 0.0); // position matrix
    DoubleMatrix vel(size, nvars, 0.0); // velocity matrix
    DoubleMatrix pos_b(size, nvars, 0.0); // best position matrix
    DoubleVector fit(size); // particle fitness vector
    DoubleVector fit_b(size); // best fitness vector
    DoubleMatrix pos_nb(size, nvars, 0.0); // what is the best informed position for each particle
    IntMatrix comm(size, size, 0); // communications:who informs who rows : those who inform cols : those who are informed
    int improved; // whether solution->error was improved during the last iteration
    int d, step;
    double a, b;
    double rho1, rho2; //random numbers (coefficients)
    double w; //current omega
    Inform_fun inform_fun;
    Calc_inertia_fun calc_inertia_fun; // inertia weight update function
    int psosteps = 0;
    time_t stoptime;

    if (seed == 0) seed = 1234;
    iter_without_improv=0;
    consecutive_improv_iters=0;

    handle.logMessage(LOGINFO, "Starting PSO with particles ", size, "\n");

// SELECT APPROPRIATE NHOOD UPDATE FUNCTION
    switch (nhood_strategy) {
        case PSO_NHOOD_GLOBAL:
            // comm matrix not used
            handle.logMessage(LOGINFO, "PSO will use global communication\n");
            inform_fun = &OptInfoPso::inform_global;
            break;
        case PSO_NHOOD_RING:
            handle.logMessage(LOGINFO, "PSO will use ring communication\n");
            init_comm_ring(comm);
            inform_fun = &OptInfoPso::inform_ring;
            break;
        case PSO_NHOOD_RANDOM:
            handle.logMessage(LOGINFO, "PSO will use random communication\n");
            init_comm_random(comm);
            inform_fun = &OptInfoPso::inform_random;
            break;
    }
// SELECT APPROPRIATE INERTIA WEIGHT UPDATE FUNCTION
    switch (w_strategy) {
        case PSO_W_CONST:
            handle.logMessage(LOGINFO, "PSO will use constant inertia\n");
            calc_inertia_fun = &OptInfoPso::calc_inertia_const;
            break;
        case PSO_W_LIN_DEC:
            handle.logMessage(LOGINFO, "PSO will use linear decreasing inertia\n");
            calc_inertia_fun = &OptInfoPso::calc_inertia_lin_dec;
            break;
	case PSO_W_ADAPT:
	    handle.logMessage(LOGINFO, "PSO will use adative dynamic inertia\n");
            calc_inertia_fun = &OptInfoPso::calc_inertia_adapt_dyn;
            break;	   
    }
//
//initialize omega using standard value
//
    w = PSO_INERTIA;
    handle.logMessage(LOGINFO, "PSO initial inertia ", w, "\n");
    handle.logMessage(LOGINFO, "PSO c1", c1, "\n");
    handle.logMessage(LOGINFO, "PSO c2", c2, "\n");
    handle.logMessage(LOGINFO, "PSO goal", goal, "\n");

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
            lowerb[i] = lowerb[i] / init[i];
            upperb[i] = upperb[i] / init[i];
            if (lowerb[i] > upperb[i]) {
                tmp = lowerb[i];
                lowerb[i] = upperb[i];
                upperb[i] = tmp;
            }
    }

    bestf = EcoSystem->SimulateAndUpdate(x);

    if (bestf != bestf) { //check for NaN
        handle.logMessage(LOGINFO, "Error starting PSO optimisation with f(x) = infinity");
        converge = -1;
        iters = 1;
        return;
    }

    offset = EcoSystem->getFuncEval(); //number of function evaluations done before loop
    iters = 0;
    i = 0;
    handle.logMessage(LOGINFO, "Initialising PSO ", size, "particles\n");
    for (d = 0; d < nvars; d++) {
        a = x[d];
        pos[i][d] = a;
        pos_b[i][d] = a;
        pos_nb[i][d] = a;
        b = lowerb[d] + (upperb[d] - lowerb[d]) * (rand_r(&seed) * 1.0 / RAND_MAX);
        vel[i][d] = (a - b) / 2.;
    }
    fit[i] = EcoSystem->SimulateAndUpdate(pos[i]);
    fit_b[i] = fit[i]; // this is also the personal best
    if (fit[i] < bestf) {
        bestf = fit[i];
        bestx = pos[i];
    }

// Now other particles with a random position in the solution space
//
//#pragma omp parallel for default(shared) private(i,d,a,b)
    for (i = 1; i < size; i++) {
        ostringstream spos, svel;
        spos << "Initial position of particle " << i << " [";
        svel << "Initial velocity of particle " << i << " [";
        // for each dimension
        for (d = 0; d < nvars; d++) {
            // generate two numbers within the specified range and around trialx
            a = lowerb[d] + (upperb[d] - lowerb[d]) * (rand_r(&seed) * 1.0 / RAND_MAX);
            b = lowerb[d] + (upperb[d] - lowerb[d]) * (rand_r(&seed) * 1.0 / RAND_MAX);
            // initialize position
            pos[i][d] = a;
            // best position is trialx
            pos_b[i][d] = a;
            pos_nb[i][d] = a;
            // initialize velocity
            vel[i][d] = (a - b) / 2.;
            spos << pos[i][d] << " ";
            svel << vel[i][d] << " ";
        }
        fit[i] = EcoSystem->SimulateAndUpdate(pos[i]);
        fit_b[i] = fit[i]; // this is also the personal best
        if (fit[i] < bestf) {
            bestf = fit[i];
            bestx = pos[i];
        }
        spos << " ]";
        svel << " ]";
        handle.logMessage(LOGDEBUG, spos.str().c_str());
        handle.logMessage(LOGDEBUG, svel.str().c_str());
    }

    EcoSystem->add_convergence_data( bestf, 0e0, 0e0,  " ");
//
// RUN ALGORITHM
//
    psosteps = 0;
    while (1) {
        handle.logMessage(LOGINFO, "PSO optimisation after", psosteps * size, "\n");
        if (isZero(bestf)) {
            iters = EcoSystem->getFuncEval() - offset;
            handle.logMessage(LOGINFO, "Error in PSO optimisation after", iters, "function evaluations, f(x) = 0");
            converge = -1;
            return;
        }
        /* if too many function evaluations occur, terminate the algorithm */
        if (psosteps > psoiter) {
            iters = EcoSystem->getFuncEval() - offset;
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
        // update inertia weight
        if (w_strategy) w = (this->*calc_inertia_fun)(psosteps);
        // check optimization goal
        if (bestf <= goal) {
            handle.logMessage(LOGINFO, "\nStopping PSO optimisation algorithm\n");
            handle.logMessage(LOGINFO, "Goal achieved!!! @ step ", psosteps);
            score = EcoSystem->SimulateAndUpdate(bestx);
            handle.logMessage(LOGINFO, "\nPSO finished with a likelihood score of", score);
            break;
        }

        // update pos_nb matrix (find best of neighborhood for all particles)
        (this->*inform_fun)(comm, pos_nb, pos_b, fit_b, bestx, improved);

        // the value of improved was just used; reset it
        improved = 0;

        // update all particles
        for (i = 0; i < size; i++) {
            ostringstream spos, svel;
            spos << "New position of particle " << i << " [";
            svel << "New velocity of particle " << i << " [";
            // for each dimension
            for (d = 0; d < nvars; d++) {
                // calculate stochastic coefficients
                rho1 = c1 * (rand_r(&seed) * 1.0) / RAND_MAX;
                rho2 = c2 * (rand_r(&seed) * 1.0) / RAND_MAX;

                // update velocity
                vel[i][d] = w * vel[i][d] + rho1 * (pos_b[i][d] - pos[i][d]) + rho2 * (pos_nb[i][d] - pos[i][d]);

                // update position
                pos[i][d] += vel[i][d];
                spos << pos[i][d] << " ";
                svel << vel[i][d] << " ";
                // clamp position within bounds?
                if (clamp_pos) {
                    if (pos[i][d] < lowerb[d]) {
                        pos[i][d] = lowerb[d];
                        vel[i][d] = 0;
                    }
                    else if (pos[i][d] > upperb[d]) {
                        pos[i][d] = upperb[d];
                        vel[i][d] = 0;
                    }
                }
                else {
                    // enforce periodic boundary conditions
                    if (pos[i][d] < lowerb[d]) {
                        pos[i][d] = upperb[d] - fmod(lowerb[d] - pos[i][d], upperb[d] - lowerb[d]);
                        vel[i][d] = 0;
                    }
                    else if (pos[i][d] > upperb[d]) {
                        pos[i][d] = lowerb[d] + fmod(pos[i][d] - upperb[d], upperb[d] - lowerb[d]);
                        vel[i][d] = 0;

                    }
                }
            } //end for nvars
            spos << "]" << endl;
            svel << "]" << endl;
            handle.logMessage(LOGDEBUG, spos.str().c_str());
            handle.logMessage(LOGDEBUG, svel.str().c_str());

            // update particle fitness
	    // fit[i] = EcoSystems[omp_get_thread_num()]->SimulateAndUpdate(pos[i]);
            fit[i] = EcoSystem->SimulateAndUpdate(pos[i]);
            // update personal best position?
            if (fit[i] < fit_b[i]) {
                fit_b[i] = fit[i];
                pos_b[i] = pos[i];
            }            
	}

        for (i = 0; i < size; i++) {
            // update gbest??
            handle.logMessage(LOGDEBUG, "Particle ", i);
            handle.logMessage(LOGDEBUG, "The likelihood score is", bestf, "at the point");
            handle.logMessage(LOGDEBUG, "Its likelihood score is", fit[i], "at the point");
            if (fit[i] < bestf) {
                improved = 1;
                // update best fitness
                bestf = fit[i];
                for (d = 0; d < nvars; d++)
                    bestx[d] = pos[i][d] * init[d];
                EcoSystem->storeVariables(bestf, bestx);
                bestx = pos[i];

            }
        } // end i


        timestop = RUNID.returnTime();
        if (timestop > MAXTIME) {
	        handle.logMessage(LOGINFO, "\nPSO finished. Maximum time achieved");
		break;
	}

        if (improved == 1) {
	        iter_without_improv=0;
		consecutive_improv_iters++;
                iters = EcoSystem->getFuncEval() - offset;
		EcoSystem->add_convergence_data( bestf, timestop , iters,  ", ");
                handle.logMessage(LOGINFO, "\nNew optimum found after", iters, "function evaluations");
                handle.logMessage(LOGINFO, "The likelihood score is", bestf, "at the point");
                EcoSystem->writeBestValues();
	} else  {
		iter_without_improv++;
		consecutive_improv_iters=0;
	}
        
        psosteps++;

    } // while (newf < bestf)

    iters = EcoSystem->getFuncEval() - offset;
    handle.logMessage(LOGINFO, "Existing PSO after ", iters, "function evaluations ...");
    for (d = 0; d < nvars; d++)
        bestx[d] = bestx[d] * init[d];
    EcoSystem->storeVariables(bestf, bestx);
    EcoSystem->writeBestValues();

}

#ifdef _OPENMP
void OptInfoPso::OptimiseLikelihoodREP() {
    OptimiseLikelihood();

}

void OptInfoPso::OptimiseLikelihoodOMP() {

    handle.logMessage(LOGINFO, "\nStarting PSO optimisation algorithm\n");
    double tmp, MAXTIME=1000;
    int ii, i, j, offset, rchange, rcheck, rnumber;
    int nvars = EcoSystem->numOptVariables();
    DoubleVector x(nvars);
//    DoubleVector trialx(nvars);
    DoubleVector bestx(nvars);
    DoubleVector scalex(nvars);
    DoubleVector lowerb(nvars);
    DoubleVector upperb(nvars);
    DoubleVector init(nvars);
//DoubleVector initialstep(nvars, rho);
    IntVector param(nvars, 0);
    IntVector lbound(nvars, 0);
    IntVector rbounds(nvars, 0);
    IntVector trapped(nvars, 0);
    struct Best { double bestf; int index;};
    struct Best best;
    double timestop;
    iter_without_improv=0; 
    consecutive_improv_iters=0;
    /**
     * Internal variables for PSO
     */
//    printf("nvars:%d\n",nvars);
    if (size == 0) size = pso_calc_swarm_size(nvars);
    DoubleMatrix pos(size, nvars, 0.0); // position matrix
    DoubleMatrix vel(size, nvars, 0.0); // velocity matrix
    DoubleMatrix pos_b(size, nvars, 0.0); // best position matrix
    DoubleVector fit(size); // particle fitness vector
    DoubleVector fit_b(size); // best fitness vector
    DoubleMatrix pos_nb(size, nvars, 0.0); // what is the best informed position for each particle
    IntMatrix comm(size, size, 0); // communications:who informs who rows : those who inform cols : those who are informed
    int improved; // whether solution->error was improved during the last iteration
    int d, step, index_del;
    double dist;
    double threshold_ED=0.000001;
    double a, b;
    double rho1, rho2; //random numbers (coefficients)
    double w; //current omega
    Inform_fun inform_fun;
    Calc_inertia_fun calc_inertia_fun; // inertia weight update function
    int psosteps = 0; //!= iters?
    ostringstream spos, svel;
    int *duplir;
    int *counter_restart;

    printf("seed %d", seed);
    if (seed == 0) seed = 1234;

    handle.logMessage(LOGINFO, "Starting PSO with particles ", size, "\n");
    duplir         = (int *) malloc(size*sizeof(int));
    counter_restart= (int *) malloc(size*sizeof(int));
    for (i=0;i<size;i++) counter_restart[i]=0;

// SELECT APPROPRIATE NHOOD UPDATE FUNCTION
    switch (nhood_strategy) {
        case PSO_NHOOD_GLOBAL:
            // comm matrix not used
            handle.logMessage(LOGINFO, "PSO will use global communication\n");
            inform_fun = &OptInfoPso::inform_global;
            break;
        case PSO_NHOOD_RING:
            handle.logMessage(LOGINFO, "PSO will use ring communication\n");
            init_comm_ring(comm);
            inform_fun = &OptInfoPso::inform_ring;
            break;
        case PSO_NHOOD_RANDOM:
            handle.logMessage(LOGINFO, "PSO will use random communication\n");
            init_comm_random(comm);
            inform_fun = &OptInfoPso::inform_random;
            break;
    }
// SELECT APPROPRIATE INERTIA WEIGHT UPDATE FUNCTION
    switch (w_strategy) {
        case PSO_W_CONST:
            handle.logMessage(LOGINFO, "PSO will use constant inertia\n");
            calc_inertia_fun = &OptInfoPso::calc_inertia_const;
            break;
        case PSO_W_LIN_DEC:
            handle.logMessage(LOGINFO, "PSO will use linear decreasing inertia\n");
            calc_inertia_fun = &OptInfoPso::calc_inertia_lin_dec;
            break;
        case PSO_W_ADAPT:
            handle.logMessage(LOGINFO, "PSO will use adative dynamic inertia\n");
            calc_inertia_fun = &OptInfoPso::calc_inertia_adapt_dyn;
            break;
    }
//
//initialize omega using standard value
//
    w = PSO_INERTIA;
    handle.logMessage(LOGINFO, "PSO initial inertia ", w, "\n");
    handle.logMessage(LOGINFO, "PSO c1", c1, "\n");
    handle.logMessage(LOGINFO, "PSO c2", c2, "\n");
    handle.logMessage(LOGINFO, "PSO goal", goal, "\n");

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
    handle.logMessage(LOGINFO, "Initialising PSO ", size, "particles\n");

// SWARM INITIALIZATION
//
// Initialize the fist particle with the best known position
    for (d = 0; d < nvars; d++) {
        // generate two numbers within the specified range and around trialx
        a = x[d];
        // initialize position
        pos[i][d] = a;
        pos_b[i][d] = a;
        pos_nb[i][d] = a;
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


    // REPRO
    double ***rand;
    rand = (double ***) calloc(size,sizeof(double **));
    for (i = 0; i < size; i++) {
	rand[i] = (double **) calloc(nvars,sizeof(double *)); 
        for (d = 0; d < nvars; d++) rand[i][d] = (double *) calloc(2,sizeof(double));
    }

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
            a = lowerb[d] + (upperb[d] - lowerb[d]) * (rand_r(&seed) * 1.0 / RAND_MAX);
            b = lowerb[d] + (upperb[d] - lowerb[d]) * (rand[i][d][1] * 1.0 / RAND_MAX);

            // initialize position
            pos[i][d] = a;
            // best position is trialx
            pos_b[i][d] = a;
            pos_nb[i][d] = a;
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

//
// Compare the reduction with the best value
//

//      if (best.bestf < bestf){
//            bestx=pos[best.index];
//            bestf=best.bestf;
//      }

// RUN ALGORITHM
    EcoSystem->add_convergence_data( best.bestf, 0e0, 0e0,  " ");
    psosteps = 0;
    while (1) {
        handle.logMessage(LOGINFO, "PSO optimisation after", psosteps * size, "\n");
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
            for (i = 0; i < nvars; i++)
                bestx[i] = bestx[i] * init[i];
            handle.logMessage(LOGINFO, "\nPSO finished with a likelihood score of", score);
            EcoSystem->storeVariables(score, bestx);
            return;
        }
        // update inertia weight
        if (w_strategy) {
		w = (this->*calc_inertia_fun)(psosteps);
	}
        // check optimization goal
        if (best.bestf <= goal) {
            handle.logMessage(LOGINFO, "\nStopping PSO optimisation algorithm\n");
            handle.logMessage(LOGINFO, "Goal achieved!!! @ step ", psosteps);
            score = EcoSystems[0]->SimulateAndUpdate(bestx);
            handle.logMessage(LOGINFO, "\nPSO finished with a likelihood score of", score);
            break;
        }

        // update pos_nb matrix (find best of neighborhood for all particles)
        (this->*inform_fun)(comm, pos_nb, pos_b, fit_b, bestx, improved);

        // the value of improved was just used; reset it
        improved = 0;
/*
if (psosteps%10 == 0) {
	// delete duplicates
	for (i=0;i<size;i++) duplir[i]=0;

#pragma omp parallel for default(shared) private(i,j,dist) schedule(dynamic,1)
        for (i=0;i<size;i++) {
	   if (duplir[i]==0) {
	     for (j = 0; j < size; j++) {
		if (i!=j){
		dist = calc_euclidean_distance(pos[i].getv(), pos[j].getv(), nvars, upperb.getv(), lowerb.getv());
		  if (dist <= threshold_ED){
		    if (fit[i] >= fit[j]) {
                        duplir[i]=1;			
			break;
		    }
		    else {
			duplir[j]=1;
		    }
		  }
		}
	     }
	   }	
        }
#pragma omp parallel for default(shared) private(i,d,a,b) schedule(dynamic,1)
	for (i=0;i<size;i++){
	 if ((duplir[i]==1) ) {
           for (d = 0; d < nvars; d++) {
                a = lowerb[d] + (upperb[d] - lowerb[d]) * (rand_r(&seed) * 1.0 / RAND_MAX);
                b = lowerb[d] + (upperb[d] - lowerb[d]) * (rand_r(&seed) * 1.0 / RAND_MAX);
                pos[i][d] = a;
                pos_b[i][d] = a;
                pos_nb[i][d] = a;
                vel[i][d] = (a - b) / 2.;
           }
           fit[i]   = EcoSystems[omp_get_thread_num()]->SimulateAndUpdate(pos[i]);
	   fit_b[i] = fit[i];
           printf("check duplicate %lf\n", fit[i]);	   	   
	 }
	}
}*/


        for (i = 0; i < size; i++) {
   	    for (d = 0; d < nvars; d++) {
        	rand[i][d][0] = rand_r(&seed);
                rand[i][d][1] = rand_r(&seed);
	    }
        }



//printf("POP: ");
//for (i = 0; i < size; i++) {
//	printf("%lf  ", fit[i]);
//}
//printf("\n");

#pragma omp parallel for default(shared) private(i,d,rho1,rho2,w) schedule(dynamic,1)
        for (i = 0; i < size; i++) {
//      handle.logMessage(LOGDEBUG, "In parallel",omp_get_thread_num(),"\n");
            ostringstream spos, svel;
            spos << "New position of particle " << i << " [";
            svel << "New velocity of particle " << i << " [";

            // for each dimension
            for (d = 0; d < nvars; d++) {
                // calculate stochastic coefficients
                rho1 = c1 * (rand[i][d][0] * 1.0) / RAND_MAX;
                rho2 = c2 * (rand[i][d][1] * 1.0) / RAND_MAX;
                
                // update velocity
                vel[i][d] = w * vel[i][d] + rho1 * (pos_b[i][d] - pos[i][d]) + rho2 * (pos_nb[i][d] - pos[i][d]);
                // update position
                pos[i][d] += vel[i][d];
                spos << pos[i][d] << " ";
                svel << vel[i][d] << " ";
                // clamp position within bounds?
                if (clamp_pos) {
                    if (pos[i][d] < lowerb[d]) {
                        pos[i][d] = lowerb[d];
                        vel[i][d] = 0;
                    }
                    else if (pos[i][d] > upperb[d]) {
                        pos[i][d] = upperb[d];
                        vel[i][d] = 0;
                    }
                }
                else {
                    // enforce periodic boundary conditions
                    if (pos[i][d] < lowerb[d]) {
                        pos[i][d] = upperb[d] - fmod(lowerb[d] - pos[i][d], upperb[d] - lowerb[d]);
                        vel[i][d] = 0;
                    }
                    else if (pos[i][d] > upperb[d]) {
                        pos[i][d] = lowerb[d] + fmod(pos[i][d] - upperb[d], upperb[d] - lowerb[d]);
                        vel[i][d] = 0;

                    }
                }
            } //end for nvars

            spos << "]" << endl;
            svel << "]" << endl;
            handle.logMessage(LOGDEBUG, spos.str().c_str());
            handle.logMessage(LOGDEBUG, svel.str().c_str());

            // update particle fitness
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
		counter_restart[i]=0;
              } 
            } // end i

            timestop = RUNID.returnTime();
            if (timestop > MAXTIME) {
                handle.logMessage(LOGINFO, "\nPSO finished. Maximum time achieved");
                break;
            }

	    if (improved == 1) {
		iter_without_improv=0;
		consecutive_improv_iters++;

                iters=0;
                for (d = 0; d < numThr; d++){
                    iters = iters + (EcoSystems[d]->getFuncEval());
		}
                iters = iters - offset;
                handle.logMessage(LOGINFO, "\nNew optimum found after", iters, "function evaluations");
                handle.logMessage(LOGINFO, "The likelihood score is", best.bestf, "at the point");
		EcoSystem->add_convergence_data( best.bestf, timestop , iters,  ", ");
                EcoSystem->writeBestValues();
	    } else {
                iter_without_improv++;
                consecutive_improv_iters=0;
	    }
	    psosteps++;
    } // while (newf < bestf)
  
    
    for (i = 0; i < size; i++) {
	for ( d = 0; d < nvars; d ++ ) {
 		cfree(rand[i][d]);
	}
	cfree(rand[i]);
    }
    cfree(rand);

    iters=0;
    for (d = 0; d < numThr; d++) 
    	iters = iters + (EcoSystems[d]->getFuncEval());
    
    iters = iters - offset;

    handle.logMessage(LOGINFO, "Existing PSO after ", iters, "function evaluations ...");
    for (d = 0; d < nvars; d++)
        bestx[d] = bestx[d] * init[d];
    EcoSystem->storeVariables(best.bestf, bestx);
    EcoSystem->writeBestValues();

    free(duplir);
    free(counter_restart);
}
#endif

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

/**
 *  * \brief calculate inertia weight based on convergence information
 *   */
double OptInfoPso::calc_inertia_adapt_dyn(int step) {
    int MAX_NO_IMP=5;
    int MAX_IMP=5;
    double max_1, max_2, min_1, min_2;
    double W,rand_n,rand_c1,rand_c2; 
    double max_c1, min_c1, max_c2, min_c2, max_c1_c2;   

    if ( iter_without_improv  >= MAX_NO_IMP ) {
	max_1 =1.5;
	min_1 =0.5;
        max_c1=3.75;
        min_c1=1.75;
	max_c1_c2=4.1;
	rand_c1=(rand_r(&seed) * 1.0 / RAND_MAX);
        c1=(min_c1)+rand_c1*(max_c1-min_c1);
        c2=max_c1_c2-c1;        
	rand_n=(rand_r(&seed) * 1.0 / RAND_MAX);
	W =  (min_1)+rand_n*(max_1 - min_1);
    }
    else if ( consecutive_improv_iters >= MAX_IMP ) {
	max_2=1.0;
	min_2=0.5;
        max_c2=3.75;
        min_c2=1.75;
        max_c1_c2=4.1;
        rand_c2=(rand_r(&seed) * 1.0 / RAND_MAX);
        c2=(min_c2)+rand_c2*(max_c2-min_c2);
	c1=max_c1_c2-c2;
        rand_n=(rand_r(&seed) * 1.0 / RAND_MAX);
	W =  (min_2)+rand_n*(max_2 - min_2);
    } else {
        max_1 =0.8;
        min_1 =0.6;
        max_c2=3.1;
        min_c2=1.1;
        max_c1_c2=4.1;
        rand_c2=(rand_r(&seed) * 1.0 / RAND_MAX);
        c2=(min_c2)+rand_c2*(max_c2-min_c2);
        c1=max_c1_c2-c2;
        rand_n=(rand_r(&seed) * 1.0 / RAND_MAX);
        W =  (min_1)+rand_n*(max_1 - min_1);
    }
    printf("W ---> %lf  iter_without_improv %d  consecutive_improv_iters %d c1 %lf c2 %lf\n", W,iter_without_improv,consecutive_improv_iters,c1,c2);
    return W;
}


/**
 * \brief NEIGHBORHOOD (COMM) MATRIX STRATEGIES. global neighborhood
 */
void OptInfoPso::inform_global(IntMatrix& comm, DoubleMatrix& pos_nb, DoubleMatrix& pos_b, DoubleVector& fit_b, DoubleVector& gbest, int improved) { 
    int i;
// all particles have the same attractor (gbest)
// copy the contents of gbest to pos_nb
    for (i = 0; i < size; i++)
        //memmove((void *)&pos_nb[i][0], &gbest[0],
//		sizeof(double) * gbest.Size());
        pos_nb[i] = gbest;
}

/**
 * \brief  general inform function :: according to the connectivity matrix COMM, it copies the best position (from pos_b) of the informers of each particle to the pos_nb matrix
 */
void OptInfoPso::inform(IntMatrix& comm, DoubleMatrix& pos_nb, DoubleMatrix& pos_b, DoubleVector& fit_b, int improved) {
    int i, j;
    int b_n; // best neighbor in terms of fitness

// for each particle
    for (j = 0; j < size; j++) {
        b_n = j; // self is best
// who is the best informer??
        for (i = 0; i < size; i++)
            // the i^th particle informs the j^th particle
            if (comm[i][j] && fit_b[i] < fit_b[b_n])
            // found a better informer for j^th particle
                b_n = i;
// copy pos_b of b_n^th particle to pos_nb[j]
        pos_nb[j] = pos_b[b_n];
    }
}

/**
 * \brief topology initialization :: this is a static (i.e. fixed) topology
 */
void OptInfoPso::init_comm_ring(IntMatrix& comm) {
    int i;
// reset array
    comm.setToZero();

// choose informers
    for (i = 0; i < size; i++) {
// set diagonal to 1
        comm[i][i] = 1;
        if (i == 0) {
// look right
            comm[i][1] = 1;
// look left
            comm[i][size - 1] = 1;
        }
        else if (i == size - 1) {
// look right
            comm[i][0] = 1;
// look left
            comm[i][i - 1] = 1;
        }
        else {
// look right
            comm[i][i + 1] = 1;
// look left
            comm[i][i - 1] = 1;
        }

    }
// Print Matrix
    handle.logMessage(LOGDEBUG, "\nPSO ring communication\n");
    for (i = 0; i < size; i++) {
        string message = "[";
        for (int j = 0; j < size; j++) {
            std::ostringstream ss;
            ss << comm[i][j];
            message = message + " " + ss.str();
        }
        message = message + "]\n";
        handle.logMessage(LOGDEBUG, message.c_str());
    }

}

void OptInfoPso::inform_ring(IntMatrix& comm, DoubleMatrix& pos_nb, DoubleMatrix& pos_b, DoubleVector& fit_b, DoubleVector& gbest, int improved) {

// update pos_nb matrix
    inform(comm, pos_nb, pos_b, fit_b, improved);
    
}

/**
 * \brief random neighborhood topology
 */
void OptInfoPso::init_comm_random(IntMatrix& comm) {

    int i, j, k;
// reset array
    comm.setToZero();

// choose informers
    for (i = 0; i < size; i++) {
// each particle informs itself
        comm[i][i] = 1;
// choose kappa (on average) informers for each particle
        for (k = 0; k < nhood_size; k++) {
// generate a random index
//j = gsl_rng_uniform_int(settings->rng, settings->size);
            j = rand_r(&this->seed) * size / RAND_MAX;
// particle i informs particle j
            comm[i][j] = 1;
        }
    }
}

void OptInfoPso::inform_random(IntMatrix& comm, DoubleMatrix& pos_nb, DoubleMatrix& pos_b, DoubleVector& fit_b, DoubleVector& gbest, int improved) {
// regenerate connectivity??
    if (!improved) init_comm_random(comm);
    inform(comm, pos_nb, pos_b, fit_b, improved);

}

double OptInfoPso::coord_transformation(double *OLD, double *New, int D, double *UB, double *LB) {
    int i;

    for (i = 0; i < D; i++) {
        if (OLD[i] == LB[i]) New[i] = 0.0;
        else if ((fabs(LB[i]) + fabs(UB[i])) == 0) New[i] = 0.0;
        else if (LB[i] == UB[i]) New[i] = 0.0;
        else New[i] = (OLD[i] - LB[i]) / (fabs(LB[i]) + fabs(UB[i]));
    }


}

double OptInfoPso::calc_euclidean_distance(double *U1, double *U2, int D, double *UB, double *LB) {
    int i;
    double sum;
    double *U1_NEW, *U2_NEW;
    double *tempU1, *tempU2;

    tempU1 = (double *) malloc(D* sizeof (double));
    tempU2 = (double *) malloc(D* sizeof (double));
    memmove(tempU1,U1,D*sizeof(double));
    memmove(tempU2,U2,D*sizeof(double));
    U1_NEW = (double *) malloc(D* sizeof (double));
    U2_NEW = (double *) malloc(D* sizeof (double));
    coord_transformation(tempU1, U1_NEW, D, UB, LB);
    coord_transformation(tempU2, U2_NEW, D, UB, LB);
    sum = 0.0;
    
    for (i = 0; i < D; i++) {
        sum = sum + pow(U1_NEW[i] - U2_NEW[i], 2.0);      
    }
    if (tempU1!=NULL) {
        free(tempU1);
        tempU1 = NULL;
    }
    if (tempU2!=NULL) {
        free(tempU2);
        tempU2 = NULL;
    }
    if (U1_NEW != NULL) {
        free(U1_NEW);
        U1_NEW = NULL;
    }
    
    if (U2_NEW != NULL) {
        free(U2_NEW);
        U2_NEW = NULL; 
    }
    
    return sqrt(fabs(sum));
}

