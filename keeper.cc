#include "keeper.h"
#include "errorhandler.h"
#include "runid.h"
#include "ecosystem.h"
#include "gadget.h"

extern RunID RUNID;
extern Ecosystem* EcoSystem;
extern ErrorHandler handle;

Keeper::Keeper() {
  stack = new StrStack();
  boundsgiven = 0;
  bestlikelihood = 0.0;
}

void Keeper::KeepVariable(double& value, const Parameter& attr) {

  //Try to find the value attr in the vector switches.
  int i, index = -1;
  for (i = 0; i < switches.Size(); i++)
    if (switches[i] == attr)
      index = i;

  if (index == -1) {
    //attr was not found -- add it to switches and values.
    index = switches.Size();
    switches.resize(1);
    values.resize(1);
    bestvalues.resize(1);
    lowerbds.resize(1, -9999);  // default lower bound
    upperbds.resize(1, 9999);   // default upper bound
    opt.resize(1, 1);
    scaledvalues.resize(1, 1.0);
    initialvalues.resize(1, 1.0);
    address.AddRows(1, 1);
    switches[index] = attr;
    values[index] = value;
    bestvalues[index] = value;
    address[index][0] = &value;
    address[index][0] = stack->sendAll();

  } else {
    if (value != values[index]) {
      handle.Message("Error in keeper - received a switch with a repeated name but different value");

    } else {
      //Everything is ok, just add the address &value to address.
      i = address[index].Size();
      address[index].resize(1);
      address[index][i] = &value;
      address[index][i] = stack->sendAll();
    }
  }
}

Keeper::~Keeper() {
  stack->clearStack();
  delete stack;
}

void Keeper::DeleteParam(const double& var) {
  int found = 0;
  int i, j;
  for (i = 0; i < address.Nrow() && !found; i++) {
    for (j = 0; j < address[i].Size() && !found; j++) {
      if (address[i][j] == &var) {
        address[i].Delete(j);
        found = 1;
        if (address[i].Size() == 0) {
          //The variable we deleted was the only one with this switch.
          address.DeleteRow(i);
          switches.Delete(i);
          values.Delete(i);
          bestvalues.Delete(i);
          opt.Delete(i);
          lowerbds.Delete(i);
          upperbds.Delete(i);
          i--;
        }
      }
    }
  }
}

void Keeper::ChangeVariable(const double& pre, double& post) {
  int found = 0;
  int i, j = 0;
  for (i = 0; i < address.Nrow() && !found; i++)
    for (j = 0; j < address[i].Size() && !found; j++)
      if (address[i][j] == &pre) {
        address[i][j] = &post;
        found = 1;
      }

  if (found == 1 && pre != post)
    handle.logFailure("Error in keeper - failed to change variables");
}

void Keeper::clearLast() {
  stack->OutOfStack();
}

void Keeper::clearAll() {
  stack->clearStack();
}

void Keeper::setString(const char* str) {
  stack->clearStack();
  stack->PutInStack(str);
}

void Keeper::addString(const char* str) {
  stack->PutInStack(str);
}

int Keeper::numVariables() const {
  return switches.Size();
}

int Keeper::numOptVariables() const {
  int i, nr;
  nr = 0;
  if (opt.Size() == 0)
    return switches.Size();
  else
    for (i = 0; i < opt.Size(); i++)
      if (opt[i] == 1)
        nr++;

  return nr;
}

void Keeper::CurrentValues(DoubleVector& val) const {
  int i;
  for (i = 0; i < values.Size(); i++)
    val[i] = values[i];
}

void Keeper::InitialValues(DoubleVector& val) const {
  int i;
  for (i = 0; i < values.Size(); i++)
    val[i] = initialvalues[i];
}

void Keeper::ScaledValues(DoubleVector& val) const {
  int i;
  for (i = 0; i < values.Size(); i++)
    val[i] = scaledvalues[i];
}

void Keeper::ScaledOptValues(DoubleVector& val) const {
  int i, k;
  if (val.Size() != this->numOptVariables())
    handle.logFailure("Error in keeper - received invalid number of optimising variables");

  if (opt.Size() == 0)
    this->ScaledValues(val);
  else {
    k = 0;
    for (i = 0; i < scaledvalues.Size(); i++)
      if (opt[i] == 1) {
        val[k] = scaledvalues[i];
        k++;
      }
  }
}

void Keeper::InitialOptValues(DoubleVector& val) const {
  int i, k;
  if (val.Size() != this->numOptVariables())
    handle.logFailure("Error in keeper - received invalid number of optimising variables");

  if (opt.Size() == 0)
    this->InitialValues(val);
  else {
    k = 0;
    for (i = 0; i < initialvalues.Size(); i++)
      if (opt[i] == 1) {
        val[k] = initialvalues[i];
        k++;
      }
  }
}

void Keeper::OptSwitches(ParameterVector& sw) const {
  int i, k;
  if (sw.Size() != this->numOptVariables())
    handle.logFailure("Error in keeper - received invalid number of optimising variables");

  if (opt.Size() == 0)
    this->Switches(sw);
  else {
    k = 0;
    for (i = 0; i < switches.Size(); i++)
      if (opt[i] == 1) {
        sw[k] = switches[i];
        k++;
      }
  }
}

void Keeper::ScaleVariables() {
  int i;
  for (i = 0; i < values.Size(); i++) {
    if (isZero(values[i])) {
      if ((opt.Size() == 0) || (opt[i] == 1)) {
        //JMB - only print a warning if we are optimising this variable ...
        handle.logWarning("Warning in keeper - cannot scale switch with initial value zero", switches[i].getValue());
      }
      initialvalues[i] = 1.0;
      scaledvalues[i] = values[i];
    } else {
      initialvalues[i] = values[i];
      scaledvalues[i] = 1.0;
    }
  }
}

void Keeper::Update(const DoubleVector& val) {
  int i, j;
  if (val.Size() != values.Size())
    handle.logFailure("Error in keeper - received wrong number of variables to update");

  for (i = 0; i < address.Nrow(); i++) {
    for (j = 0; j < address.Ncol(i); j++)
      *address[i][j].addr = val[i];

    values[i] = val[i];
    if (isZero(initialvalues[i])) {
      if ((opt.Size() == 0) || (opt[i] == 1)) {
        //JMB - only print a warning if we are optimising this variable ...
        handle.logWarning("Warning in keeper - cannot scale switch with initial value zero", switches[i].getValue());
      }
      scaledvalues[i] = val[i];
    } else
      scaledvalues[i] = val[i] / initialvalues[i];
  }
}

//Memberfunction added so boundlikelihood could change variables
void Keeper::Update(int pos, double& value) {
  int i;
  if (pos <= 0 && pos >= address.Nrow())
    handle.logFailure("Error in keeper - received invalid variable to update");

  for (i = 0; i < address.Ncol(pos); i++)
    *address[pos][i].addr = value;

  values[pos] = value;
  if (isZero(initialvalues[pos])) {
    if ((opt.Size() == 0) || (opt[pos] == 1)) {
      //JMB - only print a warning if we are optimising this variable ...
      handle.logWarning("Warning in keeper - cannot scale switch with initial value zero", switches[pos].getValue());
    }
    scaledvalues[pos] = value;
  } else
    scaledvalues[pos] = value / initialvalues[pos];
}

void Keeper::writeOptValues(const LikelihoodPtrVector& likevec) const {
  int i;
  for (i = 0; i < values.Size(); i++)
    if (opt[i] == 1 || opt.Size() == 0)
      cout << values[i] << sep;

  cout << "\n\nThe scores from each likelihood component are\n";
  for (i = 0; i < likevec.Size(); i++)
    cout << likevec[i]->returnUnweightedLikelihood() << sep;

  cout << "\n\nThe overall likelihood score is " << EcoSystem->getLikelihood() << endl;
}

void Keeper::writeInitialInformation(const char* const filename, const LikelihoodPtrVector& likevec) {
  ofstream outfile;
  outfile.open(filename, ios::out);
  handle.checkIfFailure(outfile, filename);
  handle.Open(filename);
  int i, j;

  outfile << "; ";
  RUNID.print(outfile);
  outfile << "; Listing of the switches used in the current Gadget run\n";
  for (i = 0; i < address.Nrow(); i++) {
    outfile << switches[i] << TAB;
    for (j = 0; j < address.Ncol(i); j++)
      outfile << address[i][j].name << TAB;
    outfile << endl;
  }

  outfile << ";\n; Listing of the likelihood components used in the current Gadget run\n;\n";
  outfile << "; Component\tType\tWeight\n";
  for (i = 0; i < likevec.Size(); i++)
    outfile << likevec[i]->getName() << TAB << likevec[i]->Type() << TAB << likevec[i]->returnWeight() << endl;
  outfile << ";\n; Listing of the output from the likelihood components for the current Gadget run\n;\n";
  handle.Close();
  outfile.close();
  outfile.clear();
}

void Keeper::writeValues(const char* const filename,
  const LikelihoodPtrVector& likevec, int prec) const {

  int i, p, w;
  ofstream outfile;
  outfile.open(filename, ios::app);
  handle.checkIfFailure(outfile, filename);
  handle.Open(filename);

  //JMB - print the number of function evaluations at the start of the line
  outfile << EcoSystem->getFuncEval() << TAB;

  p = prec;
  w = p + 4;
  if (prec == 0) {
    p = printprecision;
    w = printwidth;
  }
  for (i = 0; i < values.Size(); i++)
    outfile << setw(w) << setprecision(p) << values[i] << sep;

  if (prec == 0) {
    p = smallprecision;
    w = smallwidth;
  }
  outfile << TAB << TAB;
  for (i = 0; i < likevec.Size(); i++)
    outfile << setw(w) << setprecision(p) << likevec[i]->returnUnweightedLikelihood() << sep;

  if (prec == 0) {
    p = fullprecision;
    w = fullwidth;
  }
  outfile << TAB << TAB << setw(w) << setprecision(p) << EcoSystem->getLikelihood() << endl;
  handle.Close();
  outfile.close();
  outfile.clear();
}

void Keeper::writeInitialInformationInColumns(const char* const filename) const {
  ofstream outfile;
  outfile.open(filename, ios::out);
  handle.checkIfFailure(outfile, filename);
  handle.Open(filename);

  outfile << "; ";
  RUNID.print(outfile);
  handle.Close();
  outfile.close();
  outfile.clear();
}

void Keeper::writeValuesInColumns(const char* const filename, int prec) const {

  int i, p, w;
  ofstream outfile;
  outfile.open(filename, ios::app);
  handle.checkIfFailure(outfile, filename);
  handle.Open(filename);

  p = prec;
  if (prec == 0)
    p = largeprecision;
  w = p + 4;

  outfile << ";\n; the optimisation has run for " << EcoSystem->getFuncEval()
    << " function evaluations\n; the current likelihood value is "
    << setprecision(p) << EcoSystem->getLikelihood()
    << "\nswitch\tvalue\t\tlower\tupper\toptimise\n";

  if (opt.Size() == 0) {
    for (i = 0; i < values.Size(); i++) {
      outfile << switches[i] << TAB << setw(w) << setprecision(p) << values[i];
      outfile << setw(smallwidth) << setprecision(smallprecision) << lowerbds[i] << TAB
        << setw(smallwidth) << setprecision(smallprecision) << upperbds[i] << "\t1\n";
   }

  } else {
    for (i = 0; i < values.Size(); i++) {
      outfile << switches[i] << TAB << setw(w) << setprecision(p) << values[i];
      outfile << setw(smallwidth) << setprecision(smallprecision) << lowerbds[i] << TAB
        << setw(smallwidth) << setprecision(smallprecision) << upperbds[i] << TAB << opt[i] << endl;
    }
  }
  handle.Close();
  outfile.close();
  outfile.clear();
}

void Keeper::Update(const StochasticData* const Stoch) {

  int i, j;
  if (Stoch->SwitchesGiven()) {
    boundsgiven = 1;
    IntVector match(Stoch->numVariables(), 0);
    IntVector found(switches.Size(), 0);

    for (i = 0; i < Stoch->numVariables(); i++)
      for (j = 0; j < switches.Size(); j++) {
        if (Stoch->Switches(i) == switches[j]) {
          values[j] = Stoch->Values(i);
          bestvalues[j] = Stoch->Values(i);

          lowerbds[j] = Stoch->Lower(i);
          upperbds[j] = Stoch->Upper(i);
          if (Stoch->OptGiven())
            opt[j] = Stoch->Optimise(i);

          if (isZero(initialvalues[j])) {
            if ((opt.Size() == 0) || (opt[j] == 1)) {
              //JMB - only print a warning if we are optimising this variable ...
              handle.logWarning("Warning in keeper - cannot scale switch with initial value zero", switches[j].getValue());
            }
            scaledvalues[j] = values[j];
          } else
            scaledvalues[j] = values[j] / initialvalues[j];

          match[i]++;
          found[j]++;
        }
      }

    for (i = 0; i < Stoch->numVariables(); i++)
      if (match[i] == 0)
        handle.logWarning("Warning in keeper - failed to match switch", Stoch->Switches(i).getValue());

    for (i = 0; i < switches.Size(); i++)
      if (found[i] == 0)
        handle.logWarning("Warning in keeper - using default values for switch", switches[i].getValue());

  } else {
    if (this->numVariables() != Stoch->numVariables())
      handle.logFailure("Error in keeper - received wrong number of variables to update");

    for (i = 0; i < Stoch->numVariables(); i++) {
      if (Stoch->OptGiven())
        opt[i] = Stoch->Optimise(i);

      values[i] = Stoch->Values(i);
      bestvalues[i] = Stoch->Values(i);
      if (isZero(initialvalues[i])) {
        if ((opt.Size() == 0) || (opt[i] == 1)) {
          //JMB - only print a warning if we are optimising this variable ...
          handle.logWarning("Warning in keeper - cannot scale switch with initial value zero", switches[i].getValue());
        }
        scaledvalues[i] = values[i];
      } else
        scaledvalues[i] = Stoch->Values(i) / initialvalues[i];
    }
  }

  for (i = 0; i < address.Nrow(); i++)
    for (j = 0; j < address.Ncol(i); j++)
      *address[i][j].addr = values[i];
}

void Keeper::Opt(IntVector& optimise) const {
  int i;
  for (i = 0; i < optimise.Size(); i++)
    optimise[i] = opt[i];
}

void Keeper::Switches(ParameterVector& sw) const {
  int i;
  for (i = 0; i < sw.Size(); i++)
    sw[i] = switches[i];
}

void Keeper::writeParamsInColumns(const char* const filename, int prec, int interrupt) const {

  int i, p, w, check;
  ofstream outfile;
  outfile.open(filename, ios::out);
  handle.checkIfFailure(outfile, filename);
  handle.Open(filename);

  p = prec;
  if (prec == 0)
    p = largeprecision;
  w = p + 4;

  outfile << "; ";
  RUNID.print(outfile);

  if (interrupt == 1) {
    outfile << "; Gadget was interrupted after " << EcoSystem->getFuncEval()
      << " function evaluations\n; the best likelihood value found so far is "
      << setprecision(p) << bestlikelihood << endl;

  } else if (EcoSystem->getFuncEval() == 0) {
    outfile << "; a stochastic run was performed giving a likelihood value of "
      << setprecision(p) << EcoSystem->getLikelihood() << endl;

  } else {
    if (EcoSystem->getFuncEvalSA() != 0) {
      outfile << "; the Simulated Annealing algorithm ran for " << EcoSystem->getFuncEvalSA()
        << " function evaluations\n; and stopped when the likelihood value was "
        << setprecision(p) << EcoSystem->getLikelihoodSA();
      if (EcoSystem->getConvergeSA() == 1)
        outfile << "\n; because the convergence criteria were met\n";
      else
        outfile << "\n; because the maximum number of function evaluations was reached\n";
    }

    if (EcoSystem->getFuncEvalHJ() != 0) {
      outfile << "; the Hooke & Jeeves algorithm ran for " << EcoSystem->getFuncEvalHJ()
        << " function evaluations\n; and stopped when the likelihood value was "
        << setprecision(p) << EcoSystem->getLikelihoodHJ();
      if (EcoSystem->getConvergeHJ() == 1)
        outfile << "\n; because the convergence criteria were met\n";
      else
        outfile << "\n; because the maximum number of function evaluations was reached\n";
    }

    if (EcoSystem->getFuncEvalBFGS() != 0) {
      outfile << "; the BFGS algorithm ran for " << EcoSystem->getFuncEvalBFGS()
        << " function evaluations\n; and stopped when the likelihood value was "
        << setprecision(p) << EcoSystem->getLikelihoodBFGS();
      if (EcoSystem->getConvergeBFGS() == 1)
        outfile << "\n; because the convergence criteria were met\n";
      else
        outfile << "\n; because the maximum number of function evaluations was reached\n";
    }
  }

  outfile << "switch\tvalue\t\tlower\tupper\toptimise\n";
  for (i = 0; i < bestvalues.Size(); i++) {
    //JMB - if a switch is outside the bounds, we need to reset this back to the bound
    //note that the simulation should have used the value of the bound anyway ...
    check = 0;
    if (lowerbds[i] > bestvalues[i]) {
      check++;
      outfile << switches[i] << TAB << setw(w) << setprecision(p) << lowerbds[i] << TAB;
      handle.logWarning("Warning in keeper - parameter has a final value", bestvalues[i]);
      handle.logWarning("which is lower than the corresponding lower bound", lowerbds[i]);
    } else if (upperbds[i] < bestvalues[i]) {
      check++;
      outfile << switches[i] << TAB << setw(w) << setprecision(p) << upperbds[i] << TAB;
      handle.logWarning("Warning in keeper - parameter has a final value", bestvalues[i]);
      handle.logWarning("which is higher than the corresponding upper bound", upperbds[i]);
    } else
      outfile << switches[i] << TAB << setw(w) << setprecision(p) << bestvalues[i] << TAB;

    outfile << setw(smallwidth) << setprecision(smallprecision) << lowerbds[i]
      << setw(smallwidth) << setprecision(smallprecision) << upperbds[i]
      << setw(smallwidth) << opt[i];

    if (check)
      outfile << " ; warning - parameter has been reset to bound";
    outfile << endl;
  }
  handle.Close();
  outfile.close();
  outfile.clear();
}

void Keeper::LowerBds(DoubleVector& lbs) const {
  int i;
  for (i = 0; i < lbs.Size(); i++)
    lbs[i] = lowerbds[i];
}

void Keeper::UpperBds(DoubleVector& ubs) const {
  int i;
  for (i = 0; i < ubs.Size(); i++)
    ubs[i] = upperbds[i];
}

void Keeper::LowerOptBds(DoubleVector& lbs) const {
  int i, j;
  if (lbs.Size() != this->numOptVariables())
    handle.logFailure("Error in keeper - received invalid number of optimising variables");

  if (lbs.Size() == 0)
    this->LowerBds(lbs);
  else {
    j = 0;
    for (i = 0; i < lowerbds.Size(); i++)
      if (opt[i] == 1) {
        lbs[j] = lowerbds[i];
        j++;
      }
  }
}

void Keeper::UpperOptBds(DoubleVector& ubs) const {
  int i, j;
  if (ubs.Size() != this->numOptVariables())
    handle.logFailure("Error in keeper - received invalid number of optimising variables");

  if (ubs.Size() == 0)
    this->UpperBds(ubs);
  else {
    j = 0;
    for (i = 0; i < upperbds.Size(); i++)
      if (opt[i] == 1) {
        ubs[j] = upperbds[i];
        j++;
      }
  }
}

void Keeper::checkBounds(const LikelihoodPtrVector& likevec) const {
  if (boundsgiven == 0)
    return;

  int i, count;

  //check that we have a boundlikelihood component
  count = 0;
  for (i = 0; i < likevec.Size(); i++) {
    if (likevec[i]->Type() == BOUNDLIKELIHOOD)
      count++;
  }
  if (count == 0)
    handle.logWarning("Warning in keeper - no boundlikelihood component found\nNo penalties will be applied if any of the parameter bounds are exceeded");

  //check the values of the switches are within the bounds to start with
  count = 0;
  for (i = 0; i < values.Size(); i++) {
    if (lowerbds[i] > values[i]) {
      count++;
      handle.logWarning("Error in keeper - parameter has initial value", values[i]);
      handle.logWarning("which is lower than the corresponding lower bound", lowerbds[i]);
    }
    if (upperbds[i] < values[i]) {
      count++;
      handle.logWarning("Error in keeper - parameter has initial value", values[i]);
      handle.logWarning("which is higher than the corresponding upper bound", upperbds[i]);
    }
    if (upperbds[i] < lowerbds[i]) {
      count++;
      handle.logWarning("Error in keeper - parameter has upper bound", upperbds[i]);
      handle.logWarning("which is lower than the corresponding lower bound", lowerbds[i]);
    }
  }
  if (count > 0)
    handle.logFailure("Error in keeper - failed to read parameters and bounds correctly");
}

void Keeper::addString(const string str) {
  addString(str.c_str());
}

void Keeper::StoreVariables(double likvalue, double* point) {
  int i, j;

  j = 0;
  bestlikelihood = likvalue;
  for (i = 0; i < bestvalues.Size(); i++) {
    if ((opt.Size() == 0) || (opt[i] == 1)) {
      bestvalues[i] = point[j];
      j++;
    }
  }
}
