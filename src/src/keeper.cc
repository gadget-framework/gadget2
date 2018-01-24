#include "keeper.h"
#include "errorhandler.h"
#include "runid.h"
#include "ecosystem.h"
#include "gadget.h"
#include "global.h"

extern Ecosystem* EcoSystem;

Keeper::Keeper() {
  stack = new StrStack();
  boundsgiven = 0;
  fileopen = 0;
  numoptvar = 0;
  bestlikelihood = 0.0;
}

void Keeper::keepVariable(double& value, Parameter& attr) {

  int i, index = -1;
  for (i = 0; i < switches.Size(); i++)
    if (switches[i] == attr)
      index = i;

  if (index == -1) {
    //attr was not found -- add it to switches and values
    index = switches.Size();
    switches.resize(attr);
    values.resize(1, value);
    bestvalues.resize(1, value);
    lowerbds.resize(1, -9999.0);  // default lower bound
    upperbds.resize(1, 9999.0);   // default upper bound
    opt.resize(1, 0);
    scaledvalues.resize(1, 1.0);
    initialvalues.resize(1, 1.0);
    address.resize();
    address[index].resize();
    address[index][0] = &value;
    if (stack->getSize() != 0)
      address[index][0] = stack->sendAll();

  } else {
    if (value != values[index]) {
      handle.logFileMessage(LOGFAIL, "read repeated switch name but different initial value", switches[index].getName());

    } else {
      i = address[index].Size();
      address[index].resize();
      address[index][i] = &value;
      if (stack->getSize() != 0)
        address[index][i] = stack->sendAll();
    }
  }
}

Keeper::~Keeper() {
  delete stack;
  if (fileopen) {
    handle.Close();
    outfile.close();
    outfile.clear();
  }
}

void Keeper::deleteParameter(const double& var) {
  int i, j, check;
  check = 0;
  for (i = 0; i < address.Nrow(); i++) {
    for (j = 0; j < address[i].Size(); j++) {
      if (address[i][j] == &var) {
        check++;
        address[i].Delete(j);
        if (address[i].Size() == 0) {
          //the variable we deleted was the only one with this switch
          address.Delete(i);
          switches.Delete(i);
          values.Delete(i);
          bestvalues.Delete(i);
          opt.Delete(i);
          lowerbds.Delete(i);
          upperbds.Delete(i);
          scaledvalues.Delete(i);
          initialvalues.Delete(i);
          i--;
        }
      }
    }
  }
  if (check != 1)
    handle.logMessage(LOGFAIL, "Error in keeper - failed to delete parameter");
}

void Keeper::changeVariable(const double& pre, double& post) {
  int i, j, check;
  check = 0;
  for (i = 0; i < address.Nrow(); i++) {
    for (j = 0; j < address.Ncol(i); j++) {
      if (address[i][j] == &pre) {
        check++;
        address[i][j] = &post;
      }
    }
  }
  if (check != 1)
    handle.logMessage(LOGFAIL, "Error in keeper - failed to change variables");
}

void Keeper::clearLast() {
  stack->clearString();
}

void Keeper::clearAll() {
  stack->clearStack();
}

void Keeper::setString(const char* str) {
  stack->clearStack();
  stack->storeString(str);
}

void Keeper::addString(const char* str) {
  stack->storeString(str);
}

void Keeper::addString(const string str) {
  this->addString(str.c_str());
}

int Keeper::numVariables() const {
  return switches.Size();
}

void Keeper::getCurrentValues(DoubleVector& val) const {
  int i;
  for (i = 0; i < values.Size(); i++)
    val[i] = values[i];
}

void Keeper::getInitialValues(DoubleVector& val) const {
  int i;
  for (i = 0; i < initialvalues.Size(); i++)
    val[i] = initialvalues[i];
}

void Keeper::getScaledValues(DoubleVector& val) const {
  int i;
  for (i = 0; i < scaledvalues.Size(); i++)
    val[i] = scaledvalues[i];
}

void Keeper::getOptScaledValues(DoubleVector& val) const {
  int i, j = 0;
  if (val.Size() != numoptvar)
    handle.logMessage(LOGFAIL, "Error in keeper - received invalid number of optimising variables");

  for (i = 0; i < scaledvalues.Size(); i++) {
    if (opt[i]) {
      val[j] = scaledvalues[i];
      j++;
    }
  }
}

void Keeper::getOptInitialValues(DoubleVector& val) const {
  int i, j = 0;
  if (val.Size() != numoptvar)
    handle.logMessage(LOGFAIL, "Error in keeper - received invalid number of optimising variables");

  for (i = 0; i < initialvalues.Size(); i++) {
    if (opt[i]) {
      val[j] = initialvalues[i];
      j++;
    }
  }
}

void Keeper::resetVariables() {
  int i;
  for (i = 0; i < values.Size(); i++) {
    initialvalues[i] = 1.0;
    scaledvalues[i] = values[i];
  }
}

void Keeper::scaleVariables() {
  int i;
  for (i = 0; i < values.Size(); i++) {
    if (isZero(values[i])) {
      if (opt[i])
        handle.logMessage(LOGWARN, "Warning in keeper - cannot scale switch with initial value zero", switches[i].getName());

      initialvalues[i] = 1.0;
      scaledvalues[i] = values[i];
      //JMB - do we want to scale the bounds here as well??
    } else {
      initialvalues[i] = values[i];
      scaledvalues[i] = 1.0;
    }
  }
}

void Keeper::Update(const DoubleVector& val) {
  int i, j;
  if (val.Size() != values.Size())
    handle.logMessage(LOGFAIL, "Error in keeper - received wrong number of variables to update");

  for (i = 0; i < address.Nrow(); i++) {
    for (j = 0; j < address.Ncol(i); j++)
      *address[i][j].addr = val[i];

    values[i] = val[i];
    if (isZero(initialvalues[i])) {
      if (opt[i])
        handle.logMessage(LOGWARN, "Warning in keeper - cannot scale switch with initial value zero", switches[i].getName());

      scaledvalues[i] = val[i];
    } else
      scaledvalues[i] = val[i] / initialvalues[i];
  }
}

void Keeper::Update(int pos, double& value) {
  int i;
  if (pos <= 0 && pos >= address.Nrow())
    handle.logMessage(LOGFAIL, "Error in keeper - received invalid variable to update");

  for (i = 0; i < address.Ncol(pos); i++)
    *address[pos][i].addr = value;

  values[pos] = value;
  if (isZero(initialvalues[pos])) {
    if (opt[pos])
      handle.logMessage(LOGWARN, "Warning in keeper - cannot scale switch with initial value zero", switches[pos].getName());

    scaledvalues[pos] = value;
  } else
    scaledvalues[pos] = value / initialvalues[pos];
}

void Keeper::writeBestValues() {
  int i, j = 0;
  DoubleVector tmpvec(numoptvar, 0.0);
  for (i = 0; i < bestvalues.Size(); i++) {
    if (opt[i]) {
      tmpvec[j] = bestvalues[i];
      j++;
    }
  }
  handle.logMessage(LOGINFO, tmpvec);
}

void Keeper::openPrintFile(const char* const filename) {
  if (fileopen)
    handle.logMessage(LOGFAIL, "Error in keeper - cannot open output file");
  fileopen = 1;
  outfile.open(filename, ios::out);
  handle.checkIfFailure(outfile, filename);
  handle.Open(filename);
  outfile << "; ";
  RUNID.Print(outfile);
}

void Keeper::writeInitialInformation(const LikelihoodPtrVector& likevec) {
  if (!fileopen)
    handle.logMessage(LOGFAIL, "Error in keeper - cannot write to output file");

  int i, j;
  outfile << "; Listing of the switches used in the current Gadget run\n";
  for (i = 0; i < address.Nrow(); i++) {
    outfile << switches[i].getName() << TAB;
    for (j = 0; j < address.Ncol(i); j++)
      outfile << address[i][j].getName() << TAB;
    outfile << endl;
  }

  outfile << ";\n; Listing of the likelihood components used in the current Gadget run\n;\n";
  outfile << "; Component\tType\tWeight\n";
  for (i = 0; i < likevec.Size(); i++)
    outfile << likevec[i]->getName() << TAB << likevec[i]->getType() << TAB << likevec[i]->getWeight() << endl;
  outfile << ";\n; Listing of the output from the likelihood components for the current Gadget run\n;\n";
}

void Keeper::writeValues(const LikelihoodPtrVector& likevec, int prec) {
  if (!fileopen)
    handle.logMessage(LOGFAIL, "Error in keeper - cannot write to output file");

  //JMB - print the number of function evaluations at the start of the line
  outfile << EcoSystem->getFuncEval() << TAB;

  int i, p, w;
  p = prec;
  if (prec == 0)
    p = printprecision;
  w = p + 4;
  for (i = 0; i < values.Size(); i++)
    outfile << setw(w) << setprecision(p) << values[i] << sep;

  if (prec == 0)
    p = smallprecision;
  w = p + 4;
  outfile << TAB << TAB;
  for (i = 0; i < likevec.Size(); i++)
    outfile << setw(w) << setprecision(p) << likevec[i]->getUnweightedLikelihood() << sep;

  if (prec == 0)
    p = fullprecision;
  w = p + 4;
  outfile << TAB << TAB << setw(w) << setprecision(p) << EcoSystem->getLikelihood() << endl;
}

void Keeper::Update(const StochasticData* const Stoch) {

  int i, j;
  if (Stoch->numSwitches() > 0) {
    if (Stoch->isOptGiven())
      boundsgiven = 1;
    else
      numoptvar = switches.Size();

    IntVector match(Stoch->numVariables(), 0);
    IntVector found(switches.Size(), 0);
    for (i = 0; i < Stoch->numVariables(); i++) {
      for (j = 0; j < switches.Size(); j++) {
        if (Stoch->getSwitch(i) == switches[j]) {
          values[j] = Stoch->getValue(i);
          bestvalues[j] = Stoch->getValue(i);

          if (!boundsgiven) {
            //JMB we are going to optimise all variables
            opt[j] = 1;
          } else {
            lowerbds[j] = Stoch->getLowerBound(i);
            upperbds[j] = Stoch->getUpperBound(i);
            opt[j] = Stoch->getOptFlag(i);
            if (opt[j])
              numoptvar++;
          }

          if (isZero(initialvalues[j])) {
            if (opt[j])
              handle.logMessage(LOGWARN, "Warning in keeper - cannot scale switch with initial value zero", switches[j].getName());

            scaledvalues[j] = values[j];
          } else
            scaledvalues[j] = values[j] / initialvalues[j];

          match[i]++;
          found[j]++;
        }
      }
    }

    if (handle.getLogLevel() >= LOGWARN) {
      for (i = 0; i < Stoch->numVariables(); i++)
        if (!match[i])
          handle.logMessage(LOGWARN, "Warning in keeper - failed to match switch", Stoch->getSwitch(i).getName());

      for (i = 0; i < switches.Size(); i++)
        if (!found[i])
          handle.logMessage(LOGWARN, "Warning in keeper - using default values for switch", switches[i].getName());
    }

  } else {
    if (this->numVariables() != Stoch->numVariables())
      handle.logMessage(LOGFAIL, "Error in keeper - received wrong number of variables to update");

    numoptvar = Stoch->numVariables();
    for (i = 0; i < numoptvar; i++) {
      opt[i] = 1;  //JMB we are going to optimise all variables
      values[i] = Stoch->getValue(i);
      bestvalues[i] = values[i];
      if (isZero(initialvalues[i])) {
        handle.logMessage(LOGWARN, "Warning in keeper - cannot scale switch with initial value zero", switches[i].getName());

        scaledvalues[i] = values[i];
      } else
        scaledvalues[i] = values[i] / initialvalues[i];
    }
  }

  for (i = 0; i < address.Nrow(); i++)
    for (j = 0; j < address.Ncol(i); j++)
      *address[i][j].addr = values[i];
}

void Keeper::getOptFlags(IntVector& optimise) const {
  int i;
  for (i = 0; i < optimise.Size(); i++)
    optimise[i] = opt[i];
}

void Keeper::getSwitches(ParameterVector& sw) const {
  int i;
  for (i = 0; i < sw.Size(); i++)
    sw[i] = switches[i];
}

void Keeper::writeParams(const OptInfoPtrVector& optvec, const char* const filename, int prec, int interrupt) {

  int i, p, w, check;
  ofstream paramfile;
  paramfile.open(filename, ios::out);
  handle.checkIfFailure(paramfile, filename);
  handle.Open(filename);

  p = prec;
  if (prec == 0)
    p = largeprecision;
  w = p + 4;

  paramfile << "; ";
  RUNID.Print(paramfile);

  if (interrupt) {
    paramfile << "; Gadget was interrupted after " << EcoSystem->getFuncEval()
      << " function evaluations\n; the best likelihood value found so far is "
      << setprecision(p) << bestlikelihood << endl;

  } else if (EcoSystem->getFuncEval() == 0) {
    paramfile << "; a simulation run was performed giving a likelihood value of "
      << setprecision(p) << EcoSystem->getLikelihood() << endl;

  } else {
    for (i = 0; i < optvec.Size(); i++)
      optvec[i]->Print(paramfile, p);
  }

  paramfile << "switch\tvalue\t\tlower\tupper\toptimise\n";
  for (i = 0; i < bestvalues.Size(); i++) {
    //JMB - if a switch is outside the bounds, we need to reset this back to the bound
    //note that the simulation should have used the value of the bound anyway ...
    check = 0;
    if (lowerbds[i] > bestvalues[i]) {
      check++;
      paramfile << switches[i].getName() << TAB << setw(w) << setprecision(p) << lowerbds[i];
      handle.logMessage(LOGWARN, "Warning in keeper - parameter has a final value", bestvalues[i]);
      handle.logMessage(LOGWARN, "which is lower than the corresponding lower bound", lowerbds[i]);
    } else if (upperbds[i] < bestvalues[i]) {
      check++;
      paramfile << switches[i].getName() << TAB << setw(w) << setprecision(p) << upperbds[i];
      handle.logMessage(LOGWARN, "Warning in keeper - parameter has a final value", bestvalues[i]);
      handle.logMessage(LOGWARN, "which is higher than the corresponding upper bound", upperbds[i]);
    } else
      paramfile << switches[i].getName() << TAB << setw(w) << setprecision(p) << bestvalues[i];

    paramfile << TAB << setw(smallwidth) << setprecision(smallprecision) << lowerbds[i]
      << sep << setw(smallwidth) << setprecision(smallprecision) << upperbds[i]
      << sep << setw(smallwidth) << opt[i];

    if (check)
      paramfile << " ; warning - parameter has been reset to bound";
    paramfile << endl;
  }
  handle.Close();
  paramfile.close();
  paramfile.clear();
}

void Keeper::getLowerBounds(DoubleVector& lbs) const {
  int i;
  for (i = 0; i < lbs.Size(); i++)
    lbs[i] = lowerbds[i];
}

void Keeper::getUpperBounds(DoubleVector& ubs) const {
  int i;
  for (i = 0; i < ubs.Size(); i++)
    ubs[i] = upperbds[i];
}

void Keeper::getOptLowerBounds(DoubleVector& lbs) const {
  int i, j = 0;
  if (lbs.Size() != numoptvar)
    handle.logMessage(LOGFAIL, "Error in keeper - received invalid number of optimising variables");

  for (i = 0; i < lowerbds.Size(); i++) {
    if (opt[i]) {
      lbs[j] = lowerbds[i];
      j++;
    }
  }
}

void Keeper::getOptUpperBounds(DoubleVector& ubs) const {
  int i, j = 0;
  if (ubs.Size() != numoptvar)
    handle.logMessage(LOGFAIL, "Error in keeper - received invalid number of optimising variables");

  for (i = 0; i < upperbds.Size(); i++) {
    if (opt[i]) {
      ubs[j] = upperbds[i];
      j++;
    }
  }
}

void Keeper::checkBounds(const LikelihoodPtrVector& likevec) const {
  if (!boundsgiven)
    return;

  int i, count;
  //check that we have a boundlikelihood component
  count = 0;
  for (i = 0; i < likevec.Size(); i++)
    if (likevec[i]->getType() == BOUNDLIKELIHOOD)
      count++;

  if ((count == 0) && (values.Size() != 0))
    handle.logMessage(LOGWARN, "Warning in keeper - no boundlikelihood component found\nNo penalties will be applied if any of the parameter bounds are exceeded");
  if (count > 1)
    handle.logMessage(LOGWARN, "Warning in keeper - repeated boundlikelihood components found");

  //check the values of the switches are within the bounds to start with
  for (i = 0; i < values.Size(); i++) {
    if ((lowerbds[i] > values[i]) || (upperbds[i] < values[i]))
      handle.logMessage(LOGFAIL, "Error in keeper - initial value outside bounds for parameter", switches[i].getName());
    if (upperbds[i] < lowerbds[i])
      handle.logMessage(LOGFAIL, "Error in keeper - upper bound lower than lower bound for parameter", switches[i].getName());
    if ((lowerbds[i] < 0.0) && (upperbds[i] > 0.0) && (opt[i]))
      handle.logMessage(LOGWARN, "Warning in keeper - bounds span zero for parameter", switches[i].getName());
  }
}

void Keeper::storeVariables(double likvalue, const DoubleVector& point) {
  int i, j = 0;
  bestlikelihood = likvalue;
  for (i = 0; i < bestvalues.Size(); i++) {
    if (opt[i]) {
      bestvalues[i] = point[j];
      j++;
    }
  }
}
