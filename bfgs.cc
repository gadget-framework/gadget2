#include "optinfo.h"
#include "errorhandler.h"
#include "mathfunc.h"
#include "ecosystem.h"
#include "gadget.h"

/* global ecosystem used to store whether the model converged */
extern Ecosystem* EcoSystem;

/* global variable, defined and initialised in gadget.cc and not modified here */
extern int FuncEval;

int OptInfoBfgs::iteration(double* x0) {
  double h[NUMVARS];
  double y[NUMVARS];
  double By[NUMVARS];
  double hy = 0.0, yBy = 0.0, alpha = 1, normgrad, temp, normdeltax;
  int i, j, k = 0, check = 0, offset = FuncEval;

  fk = (*f)(x0, numvar);
  gradient(x0, fk);
  for (i = 0; i < numvar; i++) {
    g0[i] = gk[i];
    x[i] = x0[i];
  }

  while (k < bfgsiter) {
    k++;

    if (check == 0 || isZero(alpha)) {
      check = 1;
      for (i = 0; i < numvar; i++) { // bfgs algoritm is restarted
	for (j = 0; j < numvar; j++)
	  Bk[i][j] = 0.0;
	Bk[i][i] = 1.0;
      }
    }

    for (i = 0; i < numvar; i++) {
      s[i] = 0.0;
      for (j = 0; j < numvar; j++)
	s[i] -= Bk[i][j]*gk[j];
    }

    alpha = Armijo();

    if (isZero(alpha))
      continue;

    normgrad = 0.0;
    normdeltax = 0.0;
    hy = 0.0;

    for (i = 0; i < numvar; i++) {
      h[i] = alpha * s[i];
      x[i] += h[i];
      y[i] = gk[i] - g0[i];
      g0[i] = gk[i];
      hy += h[i] * y[i];
      normgrad += gk[i] * gk[i];
      normdeltax += h[i]*h[i];
    }

    for (i = 0; i < numvar; i++) {
      By[i] = 0.0;
      for (j = 0; j < numvar; j++)
	By[i] += Bk[i][j] * y[j];
      yBy += y[i] * By[i];
    }

    if (hy != 0 && yBy != 0)
      for (i = 0; i < numvar; i++)
	for (j = 0; j < numvar; j++)
	  Bk[i][j] += h[i]*h[j]/hy - By[i]*By[j]/yBy + yBy*(h[i]/hy - By[i]/yBy)*(h[j]/hy - By[j]/yBy);

    else
      check = 0;


    if ((normgrad/(1.0 + abs(fk)) < bfgseps) && (normdeltax < bfgseps)) {
      EcoSystem->setConvergeBFGS(1);
      break;
    }
    cout << "\nNew optimum after " << FuncEval << " function evaluations, f(x) = " << fk << " at\n";
    for (i = 0; i < numvar; i++)
      cout << x[i] << sep;
    cout << endl;
  }

  EcoSystem->setFuncEvalBFGS(FuncEval - offset);
  EcoSystem->setLikelihoodBFGS(fk);
  return k;
}

void OptInfoBfgs::gradient(double* p, double fp) {
  int i, j;
  double h = 0.00001, tmp[NUMVARS], tmp2[NUMVARS], temp;
  for (i = 0; i < numvar; i++) {
    for (j = 0; j < numvar; j++) {
      tmp[j] = p[j];
      tmp2[j] = p[j];
    }
    tmp[i] += h;
    tmp2[i] -=h;
    gk[i] = ((*f)(tmp,numvar) - fp) / (2 * h);
  }
}

double OptInfoBfgs::Armijo() {
  double bn, fn = fk, sg = 0.0, df, tmp[NUMVARS];
  int cond_satisfied = 0, i;
  bn = st;
  for (i = 0; i < numvar; i++)
    sg += gk[i]*s[i];
  //sg *= sigma;
  
  while(!cond_satisfied && bn > verysmall) {
    for (i = 0; i < numvar; i++)
      tmp[i] = x[i] + bn * s[i];
    
    fn = (*f)(tmp,numvar);

    
    if((( fk - fn ) >= ( - bn * sigma * sg )) && ( fk > fn ))
      cond_satisfied = 1;
    else
      bn *= beta;
  }
  
  fk = fn;
  gradient(tmp, fk);
  if (cond_satisfied)
    return bn;
  else {
    cerr << "alpha == 0\n";
    return 0;
  }
}
