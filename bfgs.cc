#include "optinfo.h"
#include "errorhandler.h"
#include "mathfunc.h"
#include "ecosystem.h"
#include "gadget.h"

/* global ecosystem used to store whether the model converged */
extern Ecosystem* EcoSystem;

/* global variable, defined and initialised in gadget.cc and not modified here */
extern int FuncEval;

int OptInfoBfgs::iteration(double* x0, double* init) {
  double h[NUMVARS];
  double y[NUMVARS];
  double By[NUMVARS];
  double g0[NUMVARS];  //old gradient
  double tmp[NUMVARS];
  double hy, yBy, alpha, normgrad, normgrad0=0.0, normdeltax;
  double temphy, tempyby, eigen;
  int i, j, k, check, offset;

  fk = (*f)(x0, numvar);
  offset = FuncEval;
  gradient(x0, fk);
  for (i = 0; i < numvar; i++) {
    g0[i] = gk[i];
    normgrad0 += gk[i]*gk[i];
    x[i] = x0[i];
  }

  check = 0;
  k = 0;
  alpha = 1.0;

  if (fk != fk) { //check for NaN
    cout << "\nError starting BFGS optimisation with"
      << " f(x) = infinity\nReturning to calling routine ...\n";
    return 0;
  }

  while (1) {
    if (isZero(fk)) {
      cout << "\nError in BFGS optimisation after " << (FuncEval - offset)
       << " function evaluations f(x) = 0\nReturning to calling routine ...\n";
      return 0;
    }

    //If too many function evaluations occur, terminate the algorithm
    if ((FuncEval - offset) > bfgsiter) {
      cout << "\nStopping BFGS optimisation algorithm\n\n"
	   << "The optimisation stopped after " << (FuncEval - offset)
	   << " function evaluations (max " << bfgsiter 
	   << ")\nThe optimisation stopped because the "
	   << "maximum number of function evaluations" << endl 
	   << "was reached and NOT because an "
	   << "optimum was found for this run\n";
      
      fk = (*f)(x, numvar);
      EcoSystem->setFuncEvalBFGS(FuncEval - offset);
      EcoSystem->setLikelihoodBFGS(fk);
      for(i = 0; i < numvar; i++)
	x[i] *= init[i];
      EcoSystem->StoreVariables(fk, x);
      return 0;
    }

    if (check == 0 || alpha < 0.0) {
      if (k != 0)
        cerr << "\nWarning in BFGS search - resetting algorithm after "
	     << (FuncEval - offset) << " function evaluations" << endl;
      for (i = 0; i < numvar; i++) {
        for (j = 0; j < numvar; j++)
          Bk[i][j] = 0.0;
        Bk[i][i] = diaghess[i];
      }
      check = 1;
      k++;
      //JMB - make the step size when calculating the gradient smaller
      gradacc *= gradstep;
      if (gradacc < rathersmall)
        gradacc = rathersmall;
    }

    for (i = 0; i < numvar; i++) {
      s[i] = 0.0;
      for (j = 0; j < numvar; j++)
        s[i] -= Bk[i][j] * gk[j];
    }

    alpha = Armijo();
    if (isZero(alpha)) {
      difficultgrad = 1;
      gradient(x,fk);
      continue;
    }
    if (alpha < 0.0)
      continue;
  
    normgrad = 0.0;
    normdeltax = 0.0;
    hy = 0.0;
    yBy = 0.0;

    for (i = 0; i < numvar; i++) {
      h[i] = alpha * s[i];
      x[i] += h[i];
      y[i] = gk[i] - g0[i];
      g0[i] = gk[i];
      hy += h[i] * y[i];
      normgrad += gk[i] * gk[i];
      normdeltax += h[i] * h[i];
    }
    normgrad = sqrt(normgrad);
    normdeltax = sqrt(normdeltax);
    if (normdeltax < xtol) {
      cerr << "BFGS - failed because normdeltax < xtol" << endl;
      check = 1;
      continue;
    }

    for (i = 0; i < numvar; i++) {
      By[i] = 0.0;
      for (j = 0; j < numvar; j++)
        By[i] += Bk[i][j] * y[j];
      yBy += y[i] * By[i];
    }

    if ((isZero(hy)) || (isZero(yBy))) {
      check = 0;
    } else {
      temphy = 1.0 / hy;
      tempyby = 1.0 / yBy;
      for (i = 0; i < numvar; i++)
        for (j = 0; j < numvar; j++)
          Bk[i][j] += (h[i] * h[j] * temphy) - (By[i] * By[j] * tempyby)
            + yBy * (h[i] * temphy - By[i] * tempyby) * 
	    (h[j] * temphy - By[j] * tempyby);
    }

    cout << "\nNew optimum after " << (FuncEval - offset)
	 << " function evaluations, f(x) = " << fk 
	 << "\nwith normed gradient value " << normgrad <<  " at\n";
    for (i = 0; i < numvar; i++) {
      tmp[i] = x[i]*init[i];
      cout << tmp[i] << sep;
    }
    cout << endl;

    //store this point in case the algorithm is interrupted
    EcoSystem->StoreVariables(fk, tmp);  

    // If the algorithm has met the convergence criteria, 
    //            terminate the algorithm
    if (normgrad/(1+fk) < bfgseps) {
    cout << "\nStopping BFGS optimisation algorithm\n\n"
	 << "The optimisation stopped after " << (FuncEval - offset)
	 << " function evaluations (max " << bfgsiter 
	 << ")\nThe optimisation stopped because "
	 << "an optimum was found for this run\n";

      fk = (*f)(x, numvar);
      EcoSystem->setConvergeBFGS(1);
      EcoSystem->setFuncEvalBFGS(FuncEval - offset);
      EcoSystem->setLikelihoodBFGS(fk);
      for(i = 0; i < numvar; i++)
	x[i] *= init[i];
      EcoSystem->StoreVariables(fk, x);
      if (bfgsDebug  == 1) {
	eigen = SmallestEigenValue();
	EcoSystem->setEigenBFGS(eigen);
	printInverseHessian();
      }
      return 1;
    }    
  }
}

void OptInfoBfgs::gradient(double* p, double fp) {
  int i, j;
  double tmpacc;
  double tmp[NUMVARS];
  double mtmp[NUMVARS];
  double tmp1[NUMVARS];
  double mtmp1[NUMVARS];
  double f1 = 0, f2 = 0, mf1 = 0, mf2 = 0; 
  
  if (difficultgrad == 0) {
    tmpacc = 1.0 / (2.0 * gradacc);
    for (i = 0; i < numvar; i++) {
      for (j = 0; j < numvar; j++) {
	tmp[j] = p[j];
	mtmp[j] = p[j];
      }
      tmp[i] += gradacc;
      mtmp[i] -= gradacc;
      f1 = (*f)(tmp, numvar);
      mf1 = (*f)(mtmp, numvar);
      gk[i] = (f1 - mf1) * tmpacc;
      if ((mf1 - f1)/fp < rathersmall) {
      	gradacc = min(0.001,10*gradacc);
      	cerr << "Warning in BFGS - possible roundoff errors in gradient\n";
      }
      diaghess[i] = 1.0;//4.0/((f1 - 2.0*fk + mf1)*tmpacc*tmpacc);
    }
  } else if (difficultgrad > 0) {
    tmpacc = 1.0 / (12.0 * gradacc);
    for (i = 0; i < numvar; i++) {
      for (j = 0; j < numvar; j++) {
	tmp[j] = p[j];
	mtmp[j] = p[j];
	tmp1[j] = p[j];
	mtmp1[j] = p[j];
      }
      tmp[i] += gradacc;
      tmp1[i] += 2.0*gradacc;
      mtmp[i] -= gradacc;
      mtmp1[i] -= 2.0*gradacc;
      f1 = (*f)(tmp, numvar);
      f2 = (*f)(tmp1, numvar);
      mf1 = (*f)(mtmp, numvar);
      mf2 = (*f)(mtmp1, numvar);
      gk[i] = (8.0*f1 - f2 - 8.0*mf1 + mf2) * tmpacc;
      if ((mf2 - f2)/fp < rathersmall) {
      	gradacc = min(0.001,10*gradacc);
      	cerr << "Warning in BFGS - possible roundoff errors in gradient\n";
      }
      diaghess[i] = (-f2 + 16.0*f1 - 30.0 * fp + 16.0*mf1 - mf2 )*12.0*tmpacc*tmpacc; 
    }
    }
}

double OptInfoBfgs::Armijo() {
  int i, cond;
  double bn, fn, sg;
  double tmp[NUMVARS];

  cond = 0;
  bn = step;
  fn = fk;
  sg = 0.0;
  for (i = 0; i < numvar; i++)
    sg += gk[i] * s[i];
  if ( sg >= 0.0) {
    return -1.0;
  }
  sg *= sigma;

  while (!cond && (bn > verysmall)) {
    for (i = 0; i < numvar; i++)
      tmp[i] = x[i] + bn * s[i];

    fn = (*f)(tmp, numvar);
    if (((fk - fn) >= (-bn * sg)) && (fk > fn))
      cond = 1;
    else
      bn *= beta;
  }

  if ((cond) && (fn == fn)) {  //check for NaN
    fk = fn;
    gradient(tmp, fk);
    return bn;
  } else
    return 0.0;
}

double OptInfoBfgs::SmallestEigenValue() {
  double eigen = 0, L[NUMVARS][NUMVARS], xo[NUMVARS],temp = 0.0, phi = 0.0, norm = 0.0;
  int i,k,s,j;
  // cholesky factor of Bk
  for (i = 0; i < numvar; i++)
    xo[i] = 1;
  for (k = 0; k < numvar; k++) {
    L[k][k] = Bk[k][k];
    for (s = 0; s < k - 1; s++)
      L[k][k] -= L[k][s]*L[k][s];
    for (i = k + 1; i < numvar; i++) {
      L[i][k] = Bk[i][k];
      for (s = 0; s < k - 1; s++)
	L[i][k] -= L[i][s]*L[k][s];
      L[i][k] /= L[k][k];
    }
  }
  temp = numvar;
  for(k = 0; k < numvar; k++){
    for (i = 0; i < numvar; i++) {
      for (j = 0; j < i - 1; j++)
	xo[i] -= L[i][j]*xo[j];
      xo[i] /= L[i][i];
    }
    
    for (i = numvar - 1; i >= 0; i--) {
      for (j = numvar - 1; j > i + 1; j--)
	xo[i] -= L[j][i]*xo[j];
      xo[i] /= L[i][i];
    }
    phi = 0.0;
    norm = 0.0;
    for(i = 0; i < numvar; i++) {
      phi += xo[i];
      norm += xo[i]*xo[i];
    }
    //    for(i = 0; i < numvar; i++)
    //      xo[i] /= norm;
    eigen = phi/temp;
    temp = phi;
  }
  return 1.0/eigen;
}

void OptInfoBfgs::printInverseHessian() {
  int i, j;
  ofstream outputfile;
  outputfile.open("hessian");
  if (!outputfile) {
    cerr << "Error in BFGS - could not print hessian\n";
    exit(EXIT_FAILURE);
  }
  for (i = 0; i < numvar; i++) {
    for (j = 0; j < numvar; j++) 
      outputfile << Bk[i][j] << " ";
    outputfile << endl;
  }
  outputfile.close();
}
