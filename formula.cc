#include "formula.h"
#include "gadget.h"

Formula::Formula(const Formula& form) {
  init = form.init;
  inattr = form.inattr;
  multipliers = form.multipliers;
  attributes = form.attributes;
}

void Formula::setValue(double initValue) {
  init = initValue;
}

int Formula::isMultipleValue() {
  return (multipliers.Size() > 0);
}

CommentStream& operator>>(CommentStream& infile, Formula& F) {
  assert(F.inattr.Size() == 0);
  assert(F.multipliers.Size() == 0);
  assert(F.attributes.Size() == 0);

  if (infile.fail()) {
    infile.makebad();
    return infile;
  }

  //JMB - changed so that the initial value is optional
  infile >> ws;
  char c = infile.peek();
  if (c == '#') {
    //No initial value specified - set it to 1
    F.init = 1.0;
    infile.get(c);
    infile >> F.inattr;
    if (F.inattr.Size() <= 0)
      cerr << "Error in formula - not read entry correctly\n";
    return infile;   //success or not if could not read from infile into F.inattr
  }

  //First we read the initial value
  if (!(infile >> F.init)) {
    infile.makebad();
    return infile;
  }
  //then either '*' or '#' if there is one
  c = infile.peek();
  if (infile.fail() && !infile.eof()) {
    infile.makebad();
    return infile;
  }
  if ((c != '#' && c != '*') || infile.eof())
    //If not '#' or '*' then return
    return infile; //success

  if (c == '#') {
    //Only one mark
    infile.get(c);
    infile >> F.inattr;
    if (F.inattr.Size() <= 0)
      cerr << "Error in formula - not read entry correctly\n";
    return infile;   //success or not if could not read from infile into F.inattr.
  }

  //If here, then c == '*'
  Parameter attrb;
  double mult;
  while ((c = infile.peek()) == '*') {
    infile.get(c);
    if (!(infile >> mult)) {
      infile.makebad();
      return infile;
    }
    infile.get(c);
    if (c != '#') {
      infile.makebad();
      return infile;
    }
    if (!(infile >> attrb)) {
      infile.makebad();
      return infile;
    }
    F.multipliers.resize(1, mult);
    F.attributes.resize(1, attrb);
  }
  return infile;   //success.
}

void Formula::Inform(Keeper* keeper) {
  //let keeper know of the marked variables
  int i;

  if (inattr.Size() > 0)
    keeper->KeepVariable(init, inattr);
  else {
    for (i = 0; i < multipliers.Size(); i++) {
      ostringstream ostr;
      ostr << "*" << i + 1 << ends;
      assert(i < attributes.Size());
      keeper->AddString(ostr.str());
      keeper->KeepVariable(multipliers[i], attributes[i]);
      keeper->ClearLast();
    }
  }
}

void Formula::Interchange(Formula& NewF, Keeper* keeper) const {
  if (this == &NewF) { //mnaa 20.12.00
    return;
  }
  int i;
  NewF.init = init;
  NewF.inattr = inattr;
  if (inattr.Size() > 0)
    keeper->ChangeVariable(init, NewF.init);
  for (i = 0; i < multipliers.Size(); i++)
    (NewF.multipliers).resize(1, multipliers[i]);
  for (i = 0; i < multipliers.Size(); i++)
    keeper->ChangeVariable(multipliers[i], NewF.multipliers[i]);
}

void Formula::Interchange(Formula& NewF, Keeper* keeper) {
  if (this == &NewF) { //mnaa 20.12.00
    return;
  }
  int i;
  NewF.init = init;
  NewF.inattr = inattr;
  if (inattr.Size() > 0)
    keeper->ChangeVariable(init, NewF.init);
  for (i = 0; i < multipliers.Size(); i++)
    (NewF.multipliers).resize(1, multipliers[i]);
  for (i = 0; i < multipliers.Size(); i++)
    keeper->ChangeVariable(multipliers[i], NewF.multipliers[i]);
}

void Formula::Delete(Keeper* keeper) const {
  int i;
  for (i = 0; i < multipliers.Size(); i++)
    keeper->DeleteParam(multipliers[i]);
}
