#include "ecosystem.h"
#include "runid.h"
#include "global.h"
#include <ctime>
#include <string>
#include <iostream>

#ifdef _OPENMP
#include <omp.h>
#endif
#include "runid.h"

#ifdef _OPENMP
extern Ecosystem** EcoSystems;
//extern StochasticData* data;
#endif
extern volatile int interrupted_print;

time_t starttime, stoptime;

void Ecosystem::create_convergence_files(){
	char best[30];
	char time[30];
	char eval[30];
	char conv[30];

	if (runParallel){
        int numThr = omp_get_max_threads ( );
        	sprintf(best, "convergence_bestOMP%d_run%d", numThr,rrid);
        	sprintf(time, "convergence_timeOMP%d_run%d", numThr,rrid);
        	sprintf(eval, "convergence_evalOMP%d_run%d", numThr,rrid);
        	sprintf(conv, "convergence_OMP%d_run%d.m", numThr,rrid);
	}
	else {
        	sprintf(best, "convergence_best_run%d",rrid);
        	sprintf(time, "convergence_time_run%d",rrid);
        	sprintf(eval, "convergence_eval_run%d",rrid);
        	sprintf(conv, "convergence_run%d.m",rrid);
 	}
  convergence_time.open(time);
  convergence_eval.open(eval);
  convergence_best.open(best);

  convergence.open(conv, fstream::app);

  convergence_best <<  "best  = [ ";
  convergence_time <<  "time  = [ ";
  convergence_eval <<  "evals = [ ";

}

void Ecosystem::add_convergence_data( double best, double time, double eval,  const char* msg){

  convergence_best << msg << std::setprecision (15) << best ;
  convergence_time << msg << std::setprecision (15) << time ;
  convergence_eval << msg << std::setprecision (15) << eval ;

}

void Ecosystem::delete_convergence_files(){
        char best[30];
        char time[30];
        char eval[30];
        char conv[30];
	int numThr;

	if (runParallel){
	        numThr = omp_get_max_threads ( );
	        sprintf(best, "convergence_bestOMP%d_run%d", numThr,rrid);
	        sprintf(time, "convergence_timeOMP%d_run%d", numThr,rrid);
	        sprintf(eval, "convergence_evalOMP%d_run%d", numThr,rrid);
	        sprintf(conv, "convergence_OMP%d_run%d.m", numThr,rrid);
	}
	else {
	        sprintf(best, "convergence_best_run%d",rrid);
	        sprintf(time, "convergence_time_run%d",rrid);
	        sprintf(eval, "convergence_eval_run%d",rrid);
	        sprintf(conv, "convergence_run%d.m",rrid);
	}



  convergence_best <<  "];" << endl;
  convergence_time <<  "];" << endl;
  convergence_eval <<  "];" << endl;

  ifstream file1(best) ;
  ifstream file2(time) ;
  ifstream file3(eval) ;

  convergence << file1.rdbuf() << file2.rdbuf() << file3.rdbuf();
  convergence << endl;
  convergence << endl;

  convergence << "figure();" << endl;
  convergence << "stairs(time,best,'-k','LineWidth',2,'Markersize',4); hold on;" << endl;
  convergence << "title('CONVERGENCE CURVE PSO');" << endl;
  convergence << "xlabel('time(s)','FontSize',12);" << endl;
  convergence << "ylabel('f(x)','FontSize',12);" << endl;
  if (runParallel){
        convergence << "legend('PSO using " << numThr << " threads ',16);" << endl;
  } else {
        convergence << "legend('PSO',16);" << endl;
  }
  convergence << endl;
  convergence << endl;
  convergence << "figure();" << endl;
  convergence << "stairs(evals,best,'-k','LineWidth',2,'Markersize',4); hold on;" << endl;
  convergence << "title('CONVERGENCE CURVE PSO');" << endl;
  convergence << "xlabel('time(s)','FontSize',12);" << endl;
  convergence << "ylabel('f(x)','FontSize',12);" << endl;
  if (runParallel){
  	convergence << "legend('PSO using " << numThr << " threads ',16);" << endl;
  } else {
	convergence << "legend('PSO',16);" << endl;
  }
  convergence << endl;
  convergence << endl;  

  convergence_best.close();
  convergence_time.close();
  convergence_eval.close();
  remove(best);
  remove(time);
  remove(eval);
  convergence.close();
}


Ecosystem::Ecosystem(const MainInfo& main) : printinfo(main.getPI()) {

  funceval = 0;
  //interrupted = 0;
  likelihood = 0.0;
  keeper = new Keeper;

  // initialise counter used when printing output files
  printcount = printinfo.getPrintIteration() - 1;

  // read the model specification from the main file
  char* filename = main.getMainGadgetFile();
  ifstream infile;
  infile.open(filename, ios::in);
  CommentStream commin(infile);
  handle.checkIfFailure(infile, filename);
  handle.Open(filename);
  this->readMain(commin, main);
  handle.Close();
  infile.close();
  infile.clear();

  // if this is an optimising run then read the optimisation parameters from file
  if (main.runOptimise()) {
    handle.logMessage(LOGMESSAGE, "");  //write blank line to log file
    if (main.getOptInfoGiven()) {
      filename = main.getOptInfoFile();
      infile.open(filename, ios::in);
      handle.checkIfFailure(infile, filename);
      handle.Open(filename);
      unsigned seed;
      main.getForcePrint();
      this->readOptimisation(commin, main.getSeed());
      handle.Close();
      infile.close();
      infile.clear();
    } else {
      handle.logMessage(LOGINFO, "Warning - no optimisation file specified, using default values");
      optvec.resize(new OptInfoHooke());
    }
  }
#ifdef _OPENMP
  runParallel=main.runParallel();
  this->create_convergence_files();

  if (!omp_in_parallel()){
  	if (main.runOptimise())
	{
    		switch(runParallel){
			case SPECULATIVE: 
				handle.logMessage(LOGINFO, "\nFinished reading model data files, starting to run parallel speculative optimisation with ", omp_get_max_threads(), " threads");
				break;
			case REPRODUCIBLE:
				handle.logMessage(LOGINFO, "\nFinished reading model data files, starting to run parallel reproducible optimisation with ", omp_get_max_threads(), " threads");
				break;
			default:
				handle.logMessage(LOGINFO, "\nFinished reading model data files, starting to serial optimisation with ", omp_get_num_threads() , "threads");
		}
	}
  	else
    		handle.logMessage(LOGINFO, "\nFinished reading model data files, starting to run simulation ", omp_get_num_threads(), " threads");
  	handle.logMessage(LOGMESSAGE, "");  //write blank line to log file
  }
#else
  if (main.runOptimise())
    handle.logMessage(LOGINFO, "\nFinished reading model data files, starting to run optimisation");
  else
    handle.logMessage(LOGINFO, "\nFinished reading model data files, starting to run simulation");
  handle.logMessage(LOGMESSAGE, "");  //write blank line to log file
#endif
}

Ecosystem::~Ecosystem() {
  int i;
  for (i = 0; i < optvec.Size(); i++)
    delete optvec[i];
  for (i = 0; i < printvec.Size(); i++)
    delete printvec[i];
  for (i = 0; i < likevec.Size(); i++)
    delete likevec[i];
  for (i = 0; i < tagvec.Size(); i++)
    delete tagvec[i];
  for (i = 0; i < basevec.Size(); i++)
    delete basevec[i];

  delete Area;
  delete TimeInfo;
  delete keeper;

  delete_convergence_files();

}

void Ecosystem::writeStatus(const char* filename) const {
  ofstream outfile;
  outfile.open(filename, ios::out);
  handle.checkIfFailure(outfile, filename);
  handle.Open(filename);
  RUNID.Print(outfile);
  outfile << "The current simulation time is " << TimeInfo->getYear()
    << ", step " << TimeInfo->getStep() << endl;

  int i;
  for (i = 0; i < basevec.Size(); i++)
    basevec[i]->Print(outfile);
  for (i = 0; i < likevec.Size(); i++)
    likevec[i]->Print(outfile);
  for (i = 0; i < tagvec.Size(); i++)
    tagvec[i]->Print(outfile);

  handle.Close();
  outfile.close();
  outfile.clear();
}

void Ecosystem::Reset() {
  int i;
  TimeInfo->Reset();
  for (i = 0; i < likevec.Size(); i++)
    likevec[i]->Reset(keeper);
  for (i = 0; i < basevec.Size(); i++)
    basevec[i]->Reset(TimeInfo);
}

void Ecosystem::Optimise() {
  int i;
  for (i = 0; i < optvec.Size(); i++) {
	  time(&starttime);
#ifdef _OPENMP
          if (runParallel){
	  	int j, numThr = omp_get_max_threads ( ); //AGT REMOVED
	  	DoubleVector v = this->getValues();
	  	for (j = 0; j < numThr; j++)
		  	EcoSystems[j]->Update(v);
	  	if(runParallel==SPECULATIVE)
		{
    			handle.logMessage(LOGINFO, "\nCalling Speculative Optimization");
         	  	optvec[i]->OptimiseLikelihoodOMP();
		}
	  	else
		{
    			handle.logMessage(LOGINFO, "\nCalling Reproducible parallel Optimization");
			optvec[i]->OptimiseLikelihoodREP();
		}
	  }
	  else
	  {
    			handle.logMessage(LOGINFO, "\nCalling Reproducible serial Optimization");
		 optvec[i]->OptimiseLikelihood();
	}
#else
//#ifdef SPECULATIVE
//	  optvec[i]->OptimiseLikelihoodOMP();
//#else
	  optvec[i]->OptimiseLikelihood();
#endif
	  time(&stoptime);
    this->writeOptValues();
  }
}

double Ecosystem::SimulateAndUpdate(const DoubleVector& x) {
  int i, j;

  if (funceval == 0) {
    // JMB - only need to create these vectors once
    initialval.resize(keeper->numVariables(), 0.0);
    currentval.resize(keeper->numVariables(), 0.0);
    optflag.resize(keeper->numVariables(), 0);
    keeper->getOptFlags(optflag);
    
  }

  j = 0;
  keeper->getCurrentValues(currentval);
  keeper->getInitialValues(initialval);
  for (i = 0; i < currentval.Size(); i++) {
    if (optflag[i]) {
      currentval[i] = x[j] * initialval[i];
      j++;
    }
  }

  keeper->Update(currentval);
  this->Simulate(0);  //dont print whilst optimising

  if (printinfo.getPrint()) {
    printcount++;
    if (printcount == printinfo.getPrintIteration()) {
      keeper->writeValues(likevec, printinfo.getPrecision());
      printcount = 0;
    }
  }

  funceval++;
  return likelihood;
}

void Ecosystem::writeOptValues() {
  int i;
  DoubleVector tmpvec(likevec.Size(), 0.0);
  for (i = 0; i < likevec.Size(); i++)
    tmpvec[i] = likevec[i]->getUnweightedLikelihood();

  int iters = 0;
#ifdef _OPENMP
  if (runParallel){
  	int numThr = omp_get_max_threads ( );
    	for (i = 0; i < numThr; i++)
  	  iters += EcoSystems[i]->getFuncEval();
  }
#endif
  handle.logMessage(LOGINFO, "\nAfter a total of", funceval+iters, "function evaluations the best point found is");
  keeper->writeBestValues();
  handle.logMessage(LOGINFO, "\nThe scores from each likelihood component are");
  handle.logMessage(LOGINFO, tmpvec);
  if (!isZero(keeper->getBestLikelihoodScore())) // no better point has been found
    handle.logMessage(LOGINFO, "\nThe overall likelihood score is", keeper->getBestLikelihoodScore());
  else
    handle.logMessage(LOGINFO, "\nThe overall likelihood score is", this->getLikelihood());

  handle.logMessage(LOGINFO, "\n Runtime for the algorithm was ", difftime(stoptime, starttime)," seconds\n");
}

void Ecosystem::writeInitialInformation(const char* const filename) {
  keeper->openPrintFile(filename);
  keeper->writeInitialInformation(likevec);
}

void Ecosystem::writeValues() {
  keeper->writeValues(likevec, printinfo.getPrecision());
}

void Ecosystem::writeParams(const char* const filename, int prec) const {
  if ((funceval > 0) && (interrupted_print == 0)) {
    //JMB - print the final values to any output files specified
    //in case they have been missed by the -print value
    if (printinfo.getPrint())
      keeper->writeValues(likevec, printinfo.getPrecision());
  }
  keeper->writeParams(optvec, filename, prec, interrupted_print);
}
