#include "keeper.h"
#include "errorhandler.h"
#include "runid.h"
#include "ecosystem.h"
#include "gadget.h"

extern RunID RUNID;
extern Ecosystem* EcoSystem;

Keeper::Keeper() {
  stack = new StrStack();
  likcompnames = new StrStack();
}

void Keeper::KeepVariable(double& value, const Parameter& attr) {
  ErrorHandler handle;

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
    lowerbds.resize(1, -9999);  // default lower bound
    upperbds.resize(1, 9999);   // default upper bound
    opt.resize(1, 1);
    scaledvalues.resize(1, 1.0);
    initialvalues.resize(1, 1.0);
    address.AddRows(1, 1);
    switches[index] = attr;
    values[index] = value;
    address[index][0] = &value;
    address[index][0] = stack->SendAll();

  } else {
    if (value != values[index]) {
      handle.Message("Error in keeper - received a switch with a repeated name but different value");

    } else {
      //Everything is ok, just add the address &value to address.
      int secindex = address[index].Size();
      address[index].resize(1);
      address[index][secindex] = &value;
      address[index][secindex] = stack->SendAll();
    }
  }
}

Keeper::~Keeper() {
  ClearAll();
  delete stack;
  ClearComponents();
  delete likcompnames;
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

  if (found == 1 && pre != post) {
    cerr << "Error: Trying to put variable with value " << post
      << " instead of variable " << address[--i][--j].name << endl
      << "which has the value " << pre << endl;
    exit(EXIT_FAILURE);
  }
}

void Keeper::ClearLast() {
  stack->OutOfStack();
}

void Keeper::ClearLastAddString(const char* str) {
  stack->OutOfStack();
  stack->PutInStack(str);
}

void Keeper::ClearAll() {
  stack->ClearStack();
}

void Keeper::SetString(const char* str) {
  stack->ClearStack();
  stack->PutInStack(str);
}

void Keeper::AddString(const char* str) {
  stack->PutInStack(str);
}

void Keeper::AddComponent(const char* name) {
  likcompnames->PutInStack("\t");
  likcompnames->PutInStack(name);
}

char* Keeper::SendComponents() const {
  return likcompnames->SendAll();
}

void Keeper::ClearComponents() {
  likcompnames->ClearStack();
}

int Keeper::NoVariables() const {
  return switches.Size();
}

int Keeper::NoOptVariables() const {
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

void Keeper::ValuesOfVariables(DoubleVector& val) const {
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
  if (val.Size() != this->NoOptVariables()) {
    cerr << "Keeper gets wrong number of optimizing variables";
    return;
  }
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

void Keeper::OptValues(DoubleVector& val) const {
  int i, k;
  if (val.Size() != this->NoOptVariables()) {
    cerr << "Keeper gets wrong number of optimizing variables";
    return;
  }
  if (opt.Size() == 0)
    this->ValuesOfVariables(val);
  else {
    k = 0;
    for (i = 0; i < values.Size(); i++)
      if (opt[i] == 1) {
        val[k] = values[i];
        k++;
      }
  }
}

void Keeper::InitialOptValues(DoubleVector& val) const {
  int i, k;
  if (val.Size() != this->NoOptVariables()) {
    cerr << "Keeper gets wrong number of optimizing variables";
    return;
  }
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
  if (sw.Size() != this->NoOptVariables()) {
    cerr << "Keeper gets wrong number of optimizing variables";
    return;
  }
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
    if (values[i] != 0)
      initialvalues[i] = values[i];
    else
      initialvalues[i] = 1;
    scaledvalues[i] = values[i] / initialvalues[i];
  }
}

void Keeper::Update(const DoubleVector& val) {
  int i, j;
  if (val.Size() != values.Size()) {
    error = 1;
    return;
  }
  for (i = 0; i < address.Nrow(); i++) {
    for (j = 0; j < address.Ncol(i); j++)
      *address[i][j].addr = val[i];

    values[i] = val[i];
    scaledvalues[i] = val[i] / initialvalues[i];
  }
}

//Memberfunction added so boundlikelihood could change variables
void Keeper::Update(int pos, double& value) {
  int i;
  if (pos <= 0 && pos >= address.Nrow()) {
    error = 1;
    return;
  }
  for (i = 0; i < address.Ncol(pos); i++)
    *address[pos][i].addr = value;

  values[pos] = value;
  scaledvalues[pos] = value / initialvalues[pos];
}

void Keeper::WriteOptValues(double functionValue, const LikelihoodPtrVector& Likely) const {
  int i;
  for (i = 0; i < values.Size(); i++)
    if (opt[i] == 1 || opt.Size() == 0)
      cout << values[i] << sep;

  cout << "\n\nThe likelihood components are\n";
  for (i = 0; i < Likely.Size(); i++)
    cout << Likely[i]->UnweightedLikelihood() << sep;

  cout << "\n\nThe overall likelihood score is " << functionValue << endl;
}

void Keeper::WriteInitialInformation(const char* const filename, const LikelihoodPtrVector& Likely) {
  ofstream outfile;
  outfile.open(filename, ios::out);
  checkIfFailure(outfile, filename);
  int i, j, k;

  outfile << "; ";
  RUNID.print(outfile);
  outfile << "; Listing of the switches used in the current Gadget run\n";
  for (i = 0; i < address.Nrow(); i++) {
    outfile << switches[i] << TAB;
    for (j = 0; j < address.Ncol(i); j++)
      outfile << address[i][j].name << TAB;
    outfile << endl;
  }

  CharPtrVector tmpLikely;
  tmpLikely.resize(Likely.Size());

  char* strLikely = SendComponents();
  char* strTemp = new char[MaxStrLength];
  strncpy(strTemp, "", MaxStrLength);

  j = k = 0;
  for (i = 1; i < strlen(strLikely); i++) {
    if (strLikely[i] == TAB) {
      strTemp[j] = '\0';
      tmpLikely[k] = new char[strlen(strTemp) + 1];
      strcpy(tmpLikely[k], strTemp);
      strncpy(strTemp, "", MaxStrLength);
      j = 0;
      k++;
    } else {
      strTemp[j] = strLikely[i];
      j++;
    }
  }
  //copy the last component into tmpLikely
  strTemp[j] = '\0';
  tmpLikely[k] = new char[strlen(strTemp) + 1];
  strcpy(tmpLikely[k], strTemp);

  outfile << ";\n; Listing of the likelihood components used in the current Gadget run\n;\n";
  outfile << "; Component\tType\tWeight\n";
  for (i = 0; i < tmpLikely.Size(); i++)
    outfile << tmpLikely[i] << TAB << Likely[i]->Type() << TAB << Likely[i]->Weight() << endl;
  outfile << ";\n; Listing of the output from the likelihood components for the current Gadget run\n;\n";
  outfile.close();
  outfile.clear();

  //tmpLikely is not required - free up memory
  for (i = 0; i < tmpLikely.Size(); i++)
    delete[] tmpLikely[i];
  delete[] strLikely;
  delete[] strTemp;
}

void Keeper::WriteValues(const char* const filename,
  double functionValue, const LikelihoodPtrVector& Likely, int prec) const {

  int i, p, w;
  ofstream outfile;
  outfile.open(filename, ios::app);
  checkIfFailure(outfile, filename);

  p = prec;
  if (prec == 0)
    p = printprecision;
  w = p + 2;
  for (i = 0; i < values.Size(); i++)
    outfile << setw(w) << setprecision(p) << values[i] << sep;

  if (prec == 0) {
    p = smallprecision;
    w = p + 2;
  }
  outfile << TAB << TAB;
  for (i = 0; i < Likely.Size(); i++)
    outfile << setw(w) << setprecision(p) << Likely[i]->UnweightedLikelihood() << sep;

  if (prec == 0) {
    p = fullprecision;
    w = p + 2;
  }
  outfile << TAB << TAB << setw(w) << setprecision(p) << functionValue << endl;
  outfile.close();
  outfile.clear();
}

void Keeper::WriteInitialInformationInColumns(const char* const filename) const {
  ofstream outfile;
  outfile.open(filename, ios::out);
  checkIfFailure(outfile, filename);

  outfile << "; ";
  RUNID.print(outfile);
  outfile.close();
  outfile.clear();
}

void Keeper::WriteValuesInColumns(const char* const filename,
  double functionValue, const LikelihoodPtrVector& Likely, int prec) const {

  int i, p, w;
  ofstream outfile;
  outfile.open(filename, ios::app);
  checkIfFailure(outfile, filename);

  p = prec;
  if (prec == 0)
    p = largeprecision;
  w = p + 2;

  outfile << ";\n; the optimisation has run for " << EcoSystem->GetFuncEval()
    << " function evaluations\n; the current likelihood value is "
    << setprecision(p) << functionValue << "\nswitch\tvalue\t\tlower\tupper\toptimize\n";

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
  outfile.close();
  outfile.clear();
}

void Keeper::Update(const StochasticData* const Stoch) {
  ErrorHandler handle;
  int i, j;
  if (Stoch->SwitchesGiven()) {
    IntVector match(Stoch->NoVariables(), 0);
    for (i = 0; i < Stoch->NoVariables(); i++)
      for (j = 0; j < switches.Size(); j++)
        if (Stoch->Switches(i) == switches[j]) {
          values[j] = Stoch->Values(i);
          scaledvalues[j] = Stoch->Values(i) / initialvalues[j];
          lowerbds[j] = Stoch->Lower(i);
          upperbds[j] = Stoch->Upper(i);
          match[i]++;
          if (Stoch->OptGiven())
            opt[j] = Stoch->Optimize(i);
        }

    for (i = 0; i < Stoch->NoVariables(); i++) {
      if (match[i] == 0)
        cerr << "Warning - unmatched switch " << Stoch->Switches(i) << endl;
    }

  } else {
    if (this->NoVariables() != Stoch->NoVariables()) {
      cerr << "Number of variables in data files " << this->NoVariables()
        << "\nNumber of variables in parameter file " << Stoch->NoVariables() << endl;
      exit(EXIT_FAILURE);
    }
    for (i = 0; i < Stoch->NoVariables(); i++) {
      if (Stoch->OptGiven())
        opt[i] = Stoch->Optimize(i);
      values[i] = Stoch->Values(i);
      scaledvalues[i] = Stoch->Values(i) / initialvalues[i];
    }
  }

  for (i = 0; i < address.Nrow(); i++)
    for (j = 0; j < address.Ncol(i); j++)
      *address[i][j].addr = values[i];
}

void Keeper::Opt(IntVector& optimize) const {
  int i;
  for (i = 0; i < optimize.Size(); i++)
    optimize[i] = opt[i];
}

void Keeper::Switches(ParameterVector& sw) const {
  int i;
  for (i = 0; i < sw.Size(); i++)
    sw[i] = switches[i];
}

void Keeper::WriteParamsInColumns(const char* const filename,
  double functionValue, const LikelihoodPtrVector& Likely, int prec) const {

  int i, p, w;
  ofstream outfile;
  outfile.open(filename, ios::out);
  checkIfFailure(outfile, filename);

  p = prec;
  if (prec == 0)
    p = largeprecision;
  w = p + 2;

  outfile << "; ";
  RUNID.print(outfile);
  outfile << "; optimisation ran for " << EcoSystem->GetFuncEval()
    << " function evaluations\n; the final likelihood value was "
    << setprecision(p) << functionValue << "\nswitch\tvalue\t\tlower\tupper\toptimize\n";

  for (i = 0; i < values.Size(); i++) {
    outfile << switches[i] << TAB << setw(w) << setprecision(p) << values[i] << TAB;
    outfile << setw(smallwidth) << setprecision(smallprecision) << lowerbds[i] << TAB
      << setw(smallwidth) << setprecision(smallprecision) << upperbds[i] << TAB << opt[i] << endl;
  }
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
  if (lbs.Size() != this->NoOptVariables()) {
    cerr << "Keeper gets wrong number of optimizing variables";
    return;
  }
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
  if (ubs.Size() != this->NoOptVariables()) {
    cerr << "Keeper gets wrong number of optimizing variables";
    return;
  }
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

void Keeper::AddString(const string str) {
  AddString(str.c_str());
}
