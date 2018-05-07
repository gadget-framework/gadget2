#ifndef SEQ_OPTIMIZE_TEMPLATE_H
#define SEQ_OPTIMIZE_TEMPLATE_H

#include <iostream>
#include <cstdlib>
#include <sys/time.h>

#ifdef _OPENMP
#include <omp.h>
#endif


class Siman {
	int _nvars;
	unsigned _seed;
	unsigned _seedM;
	unsigned _seedP;
	int _l;
	int _nt;
	int _NT;
	int _ns;
	int _NS;
	IntVector _param;
	DoubleVector* _x;
	DoubleVector* _lowerb;
	DoubleVector* _upperb;
	DoubleVector _vm;
	double _t;
	double _rt;
	IntVector _nacp;
	int _nacc;         //The number of accepted function evaluations
	int _nrej;         //The number of rejected function evaluations
	int _naccmet;    //The number of metropolis accepted function evaluations
	double _nsdiv;
	int _tempcheck;
	double _simanneps;
	DoubleVector _fstar;
	double _lratio;
	double _uratio;
	double _cs;
	DoubleVector* _bestx;
	int _scale;
	int* _converge; //NN
	double* _score; //NN

public:

	//Required Default constructor : Params()
	Siman(){
		_nvars = 0;
		_seed = 1234;
		_seedM = 1234;
		_seedP = 1234;
		_l = 0;
		_nt = 0;
		_NT = 0;
		_ns = 0;
		_NS = 0;
		_param = IntVector();
		_x = NULL;
		_lowerb = NULL;
		_upperb = NULL;
		_vm = DoubleVector();
		_t = 0.;
		_rt = 0.;
		_nacp = IntVector();
		_nacc = 0;         //The number of accepted function evaluations
		_nrej = 0;         //The number of rejected function evaluations
		_naccmet = 0;    //The number of metropolis accepted function evaluations
		_nsdiv = 0;
		_tempcheck = 0;
		_simanneps = 0;
		_fstar = DoubleVector();
		_lratio = 0.;
		_uratio = 0.;
		_cs = 0.;
		_bestx = NULL;
		_scale = 0;
		_converge =NULL;
		_score = 0;
	}

	Siman(unsigned &seed, unsigned &seedM, unsigned &seedP, int &nvars, int &nt, int &ns, IntVector param, DoubleVector* x,
			DoubleVector* lowerb, DoubleVector* upperb, DoubleVector vm, double t, double &rt,
			double nsdiv, int &tempcheck, double &simanneps, DoubleVector fstar,
			double &lratio, double &uratio, double &cs, DoubleVector* bestx, int &scale, int* converge,
			double* score ):
			_seed(seed), _seedM(seedM), _seedP(seedP), _l(0), _nvars(nvars), _nt(nt), _NT(0), _ns(ns), _NS(0), _t(t),
			_rt(rt), _nsdiv(nsdiv), _nacc(1), _tempcheck(tempcheck), _simanneps(simanneps), _lratio(lratio),
			_uratio(uratio), _cs(cs), _scale(scale) {
		_param = IntVector(param);
		_x = x;
		_lowerb = lowerb;
		_upperb = upperb;
		_vm = DoubleVector(vm);
		_nacp = IntVector(_nvars, 0);
		_nrej = 0;
		_naccmet = 0;
		randomizeParams();
		_fstar = DoubleVector(fstar);
		_bestx = bestx;
		_converge = converge;
		_score = score;
	}

	//Required Copy constructor    : Params(const Params& other)
	Siman(const Siman& other): _nvars(other._nvars), _seed(other._seed), _seedM(other._seedM), _seedP(other._seedP),
			_l(other._l), _nt(other._nt), _NT(other._NT), _ns(other._ns), _NS(other._NS), _param(IntVector(other._param)),
			_x(other._x), _lowerb(other._lowerb),_upperb(other._upperb), _vm(DoubleVector(other._vm)), _t(other._t),
			_rt(other._rt), _nacp(IntVector(other._nacp)), _nacc(other._nacc), _nrej(other._nrej),
			_naccmet(other._naccmet), _nsdiv(other._nsdiv), _tempcheck(other._tempcheck), _simanneps(other._simanneps),
			_fstar(DoubleVector(other._fstar)), _lratio(other._lratio), _uratio(other._uratio), _cs(other._cs),
			_bestx(other._bestx), _scale(other._scale), _converge(other._converge), _score(other._score){}

	//Required operator=           : Params& operator= (const Params& other)
	Siman& operator= (const Siman& other){
		_nvars = other._nvars;
		_seed = other._seed;
		_seedM = other._seedM;
		_seedP = other._seedP;
		_l = other._l;
		_nt = other._nt;
		_NT = other._NT;
		_ns = other._ns;
		_NS = other._NS;
		_param = IntVector(other._param);
		_x = other._x;
		_lowerb = other._lowerb;
		_upperb = other._upperb;
		_vm = DoubleVector(other._vm);
		_t = other._t;
		_rt = other._rt;
		_nacp = IntVector(other._nacp);
		_nacc = other._nacc;         //The number of accepted function evaluations
		_nrej = other._nrej;         //The number of rejected function evaluations
		_naccmet = other._naccmet;    //The number of metropolis accepted function evaluations
		_nsdiv = other._nsdiv;
		_tempcheck = other._tempcheck;
		_simanneps = other._simanneps;
		_fstar = DoubleVector(other._fstar);
		_lratio = other._lratio;
		_uratio = other._uratio;
		_cs = other._cs;
		_bestx = other._bestx;
		_scale = other._scale;
		_converge =other._converge;
		_score = other._score;

		return *this;
	}
	  //Destructor              : ~Params()
	~Siman(){

	}

	unsigned* getSeed(){return &_seed;}

	unsigned* getSeedM(){return &_seedM;}

	unsigned getSeedM_(){return _seedM;}

	void setSeedM(unsigned val){_seedM = val;}

	int getNvars(){return _nvars;}

	int getL(){return _l;}

	void incrementL() { _l = (_l +1) % _nvars;}

	IntVector& getParam(){return _param;}

	DoubleVector& getX(){return *_x;}

	void setX(DoubleVector* point) {_x=point;}

	DoubleVector& getLowerb(){return *_lowerb;}

	DoubleVector& getUpperb(){return *_upperb;}

	DoubleVector& getVm(){return _vm;}
	void setVm(DoubleVector vm){_vm = vm;}

	DoubleVector& getFstar(){return _fstar;}

	double getT(){return _t;}

	void setT(double val){_t = val;}

	void incrementNacc() {_nacc++;}

	void incrementNrej(){_nrej++;}

	void incrementNaccmet(){_naccmet++;}

	void incrementNacp(int i){_nacp[i]++;}

	void randomizeParams(){
		int rnumber, rcheck, i, rchange = 0;
		while (rchange < _nvars) {
			rnumber = rand_r(&_seedP) % _nvars;
			rcheck = 1;
			for (i = 0; i < rchange; i++)
			  if ((_param)[i] == rnumber)
				rcheck = 0;
			if (rcheck) {
			  (_param)[rchange] = rnumber;
			  rchange++;
			}
		}
	}

	int getNs() const {return _ns;}

	int getNS() const {return _NS;}

	void setNS(int ns = 0) {_NS = ns;}

	void incrementNS() {_NS++;}

	int getNt() const {return _nt;}

	int getNT() const {return _NT;}

	void setNT(int nt) {_NT = nt;}

	void incrementNT() {_NT++;};

	double getSimanneps() const {return _simanneps;}

	int getTempcheck() const {return _tempcheck;}

	const int getNacp(int i) const {return _nacp[i];}

	void setNacp(int i, int val) {_nacp[i] = val;}

	double getCs() const {return _cs;}

	double getLratio() const {return _lratio;}

	double getUratio() const {return _uratio;}

	void setConverge(int val) {(*_converge) = val;}

	DoubleVector* getBestx() {return _bestx;}

	int getScale() const {return _scale;}

	double getRt() const {return _rt;}

	double getNsdiv() const {return _nsdiv;}

	int* getConverge() const {return _converge;}

	int getNacc() const {return _nacc;}

	int getNaccmet() const {return _naccmet;}

	int getNrej() const {return _nrej;}

	void setNrej(int nrej) {_nrej = nrej;};

	double* getScore() const {return _score;}
};

/* IMPROVEMENTS:

 - OpenMP parallel implementation added
 - Can be compiled with OpenMP using macro _OPENMP
 - Memory for items is dynamically reused in the parallel searches
 - Items whose evaluation has not begun when a new optimum is found are not evaluated (only possible in TBB search)
 */

/// Performs a reproducible optimization process, sequentially in seq_opt or in parallel in paral_opt
template<class SEED_T, class PARAMS_T, class CONTROL_T, double Evaluator(
		const PARAMS_T&), void BuildNewParams(SEED_T&, PARAMS_T&)>
class ReproducibleSearch {

	const SEED_T initialSeed_;
	const PARAMS_T initialParams_;
	const int maxIters_;

	SEED_T seed_;
	PARAMS_T p_;
	int iters_;
	double optimumValue_;
	double funcval_;
	double searchTime_;
	/// Stores an individual item to evaluate and the data to restore the search state
	/// if it is chosen as new optimum item
	struct ParalEvalItem_t {
		int order_;
		PARAMS_T p_;
		SEED_T seed_; ///< seed AFTER generation of p_
		double value_;

		ParalEvalItem_t(int order, const PARAMS_T& p, const SEED_T& seed,
				double value = 0.) :
				order_(order), p_(p), seed_(seed), value_(value) {
		}

		ParalEvalItem_t() :
				value_(0.) {
		}

		void evaluateItem() {
			value_ = Evaluator(p_);
		}

		void print() const {
			std::cout << order_ << ' ' << p_ << " v= " << value_ << std::endl;
		}

	};

	/// Internal storage for search items with a padding field to avoid false sharing
	struct ParalEvalItemStorage_t {
		ParalEvalItem_t pei_;
		char padding_[128];
	};

public:

	ReproducibleSearch(const SEED_T& seed, const PARAMS_T& params, int max_iters = 200) :
			initialSeed_(seed), initialParams_(params), maxIters_(max_iters) {}

	// Set the problem in its initial state
	void reset() {
		seed_ = initialSeed_;
		p_ = initialParams_;
		std::cout << ": " << p_ << " v= " << Evaluator(p_) << std::endl;
	}

//########################################################


	void seq_opt(double funcval) {
		struct timeval t0, t1, t;
		reset();
		double timestop;
		CONTROL_T control_object;
		gettimeofday(&t0, NULL);

		seed_.setX(&p_);

		funcval_ = funcval;
		optimumValue_ = funcval_;
		bool quit = false;

		for (iters_ = 1; iters_ <= maxIters_; ++iters_) {
			PARAMS_T p = p_;

			BuildNewParams(seed_, p);

			const double prev_val = funcval_;
			const double val = Evaluator(p);

			if (control_object.mustAccept(prev_val, val, seed_, iters_)) {
				int i;
				for (i = 0; i < seed_.getNvars(); i++){
					p_[i] = p[i];
				}
				funcval_ = val;
			} else if ((*seed_.getConverge()) == -1) return;

			control_object.optimum(val, optimumValue_, iters_, p_, initialParams_, seed_);

                	gettimeofday(&t1, NULL);
        	        timersub(&t1, &t0, &t);
	                searchTime_ = t.tv_sec + t.tv_usec / 1000000.0;

                        if ((quit=control_object.mustTerminate(optimumValue_, funcval_, seed_, iters_))) {
                                break;
                        }


		}

		control_object.printResult(quit, seed_, iters_);
		gettimeofday(&t1, NULL);
		timersub(&t1, &t0, &t);
		searchTime_ = t.tv_sec + t.tv_usec / 1000000.0;
	}

	void paral_opt_omp(double funcval, int nthreads, int paral_tokens = -1) {
		struct timeval t0, t1, t;

		reset();

		CONTROL_T control_object;

		gettimeofday(&t0, NULL);

#if defined(_OPENMP)
		omp_set_num_threads(nthreads);
#endif

		if (paral_tokens < 0) {
			paral_tokens = nthreads;
		}

		seed_.setX(&p_);
		funcval_ = funcval;
		optimumValue_ = funcval_;
		bool quit = false;

		ParalEvalItemStorage_t * const storage =
				new ParalEvalItemStorage_t[paral_tokens]; //Guarda los datos de los diferentes threads

		int old_paral_tokens = paral_tokens;
		for (iters_ = 1; iters_ <= maxIters_;) {

			if ((seed_.getL() + paral_tokens -1) >= seed_.getNvars())
				paral_tokens = seed_.getNvars() - seed_.getL();

			if ((iters_ + paral_tokens - 1) > maxIters_) {
				paral_tokens = maxIters_ - (iters_ - 1);
			}

			for (int i = 0; i < paral_tokens; i++) {
				storage[i].pei_.p_ = p_;
				BuildNewParams(seed_, storage[i].pei_.p_);
				storage[i].pei_.seed_ = seed_;
				seed_.incrementL();
			}
			const double prev_val = funcval_;

#pragma omp parallel for
			for (int i = 0; i < paral_tokens; i++) {
				storage[i].pei_.evaluateItem();
			}

			int j, i=0; //Notice that we need it after the loop
			if (control_object.mustAccept(prev_val, storage[i].pei_.value_, storage[i].pei_.seed_, iters_+i)) {
				for (j = 0; j < seed_.getNvars(); j++)
					p_[j] = storage[i].pei_.p_[j];
				funcval_ = storage[i].pei_.value_;
			} else{
				if ((*storage[i].pei_.seed_.getConverge()) == -1) return;

				for (i = 1; i < paral_tokens; i++) {
					storage[i].pei_.seed_.setSeedM(storage[i-1].pei_.seed_.getSeedM_());
					if (control_object.mustAccept(prev_val, storage[i].pei_.value_, storage[i].pei_.seed_, iters_+i)) {
						for (j = 0; j < seed_.getNvars(); j++)
							p_[j] = storage[i].pei_.p_[j];
						funcval_ = storage[i].pei_.value_;
						break;
					} else if ((*storage[i].pei_.seed_.getConverge()) == -1) return;
				}
			}
			i = (i == paral_tokens) ? (i - 1) : i;

			seed_ = storage[i].pei_.seed_;
			seed_.setNrej(seed_.getNrej()+i);
			control_object.optimum(storage[i].pei_.value_, optimumValue_, iters_,
					storage[i].pei_.p_, initialParams_, seed_);

			iters_ += i+1;

			if ((quit=control_object.mustTerminate(optimumValue_, funcval_, seed_, iters_))) {
				break;
			}
			paral_tokens = old_paral_tokens;
		}
		control_object.printResult(quit, seed_ ,iters_);

		gettimeofday(&t1, NULL);
		timersub(&t1, &t0, &t);
		searchTime_ = t.tv_sec + t.tv_usec / 1000000.0;

		delete[] storage;
	}

	/// Only used by sequential search when verbose
	void print(int iter, double val) const {
		ParalEvalItem_t v(iter, p_, seed_, val);
		v.print();
	}

	const PARAMS_T& getResult() const {
		return p_;
	}

	double getOptimumValue() const {
		return funcval_;
	}

	double getSearchTime() const {
		return searchTime_;
	}

	int iterations() const {
		return iters_;
	}

};

#endif
