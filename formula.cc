#include "formula.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

Formula::Formula() {
  value = 0.0;
  name = NULL;
  type = CONSTANT;
  functiontype = NONE;
}

Formula::~Formula() {
  unsigned int i;
  for (i = 0; i < argList.size(); i++)
    delete argList[i];
}

Formula::operator double() const {
  double v = 0.0;
  switch (type) {
    case CONSTANT:
    case PARAMETER:
      v = value;
      break;
    case FUNCTION:
      v = this->evalFunction();
      break;
    default:
      handle.logFailure("Error in formula - unrecognised type", type);
      break;
  }
  return v;
}

double Formula::evalFunction() const {
  double v = 1.0;
  unsigned int i;
  switch (functiontype) {
    case NONE:
      handle.logFailure("Error in formula - no function type found");
      break;

    case MULT:
      for (i = 0; i < argList.size(); i++)
        v *= *(argList[i]);
      break;

    case DIV:
      if (argList.size() == 1) {
        if (!isZero(*(argList[0]))) {
          v = 1.0 / (*(argList[0]));
        } else {
          handle.logWarning("Warning in formula - divide by zero");
        }
      } else {
        v = *(argList[0]);
        for (i = 1; i < argList.size(); i++) {
          if (!isZero(*(argList[0]))) {
            v = v /= *(argList[i]);
          } else {
            handle.logWarning("Warning in formula - divide by zero");
          }
        }
      }
      break;

    case PLUS:
      v = 0.0;
      for (i = 0; i < argList.size(); i++)
        v += *(argList[i]);
      break;

    case MINUS:
      if (argList.size() == 1) {
        v = -(*(argList[0]));
      } else {
        v = *(argList[0]);
        for (i = 1; i < argList.size(); i++)
          v -= *(argList[i]);
      }
      break;

    case PRINT:
      if (argList.size() == 1)
        v = *(argList[0]);
      break;

    default:
      handle.logFailure("Error in formula - unrecognised function type", type);
      break;
  }
  return v;
}

Formula::Formula(const Formula& form) {
  type = form.type;
  functiontype = form.functiontype;
  switch (type) {
    case CONSTANT:
      value = form.value;
      break;
    case PARAMETER:
      value = form.value;
      name = form.name;
      break;
    case FUNCTION:
      unsigned int i;
      for (i = 0; i < form.argList.size(); i++) {
        Formula *f = new Formula(*form.argList[i]);
        argList.push_back(f);
      }
      break;
    default:
      handle.logFailure("Error in formula - unrecognised type", type);
      break;
  }
}

void Formula::setValue(double initValue) {
  if (type == FUNCTION)
    handle.logFailure("Error in formula - cannot set value for function");
  value = initValue;
}

CommentStream& operator >> (CommentStream& infile, Formula& F) {
  assert(F.argList.size() == 0);
  assert(F.type == CONSTANT);
  assert(F.name == NULL);

  if (infile.fail()) {
    infile.makebad();
    return infile;
  }

  /* Read formula expression. Syntax:
       3.14        - Constant                             (CONSTANT)
       #param      - Single parameter, no initial value   (PARAMETER)
       1.0#param   - Single value, initial value
       (* #p1 #p2) - Function call                        (FUNCTION)
  */

  infile >> ws;
  char c = infile.peek();

  // Read FUNCTION
  if (c == '(') {
    infile.get(c);
    F.type = FUNCTION;
    // Read function 'name'
    char text[MaxStrLength];
    strncpy(text, "", MaxStrLength);
    infile >> ws >> text >> ws;
    if (strcasecmp(text,"*") == 0)
      F.functiontype = MULT;
    else if (strcasecmp(text,"/") == 0)
      F.functiontype = DIV;
    else if (strcasecmp(text,"+") == 0)
      F.functiontype = PLUS;
    else if (strcasecmp(text,"-") == 0)
      F.functiontype = MINUS;
    else if (strcasecmp(text,"print") == 0)
      F.functiontype = PRINT;
    else {
      infile.makebad();
      handle.Message("Error in formula - unrecognised function name", text);
      return infile;
    }

    c = infile.peek();
    while (c != ')') {
      Formula* f = new Formula();
      infile >> *f;
      F.argList.push_back(f);
      infile >> ws;
      c = infile.peek();
    }
    infile.get(c);
    return infile;
  }

  // Read PARAMETER syntax (No initial value)
  if (c == '#') {
    //No initial value, parameter name only - set it to 1
    F.value = 1.0;
    F.type = PARAMETER;
    infile.get(c);
    infile >> F.name;
    if (F.name.Size() <= 0)
      handle.logWarning("Warning in formula - failed to read parameter name");
    return infile;
  }

  //Read initial value (Could be CONSTANT or PARAMETER)
  if (!(infile >> F.value)) {
    infile.makebad();
    return infile;
  }
  //then '#' if there is one
  c = infile.peek();
  if (infile.fail() && !infile.eof()) {
    infile.makebad();
    return infile;
  }
  // Read CONSTANT
  if ((c != '#') || infile.eof()) {
    //If not '#' then return
    F.type = CONSTANT;
    return infile;
  }
  // Read PARAMETER (with initial value)
  if (c == '#') {
    //Only one mark
    F.type = PARAMETER;
    infile.get(c);
    infile >> F.name;
    if (F.name.Size() <= 0)
      handle.logWarning("Warning in formula - failed to read parameter name");
    return infile;
  }

  infile.makebad();
  handle.Message("Error in formula - failed to read parameter data");
  return infile;
}

void Formula::Inform(Keeper* keeper) {
  //let keeper know of the marked variables
  switch (type) {
    case CONSTANT:
      break;
    case PARAMETER:
      // only one parameter
      keeper->KeepVariable(value, name);
      break;
    case FUNCTION:
      // function with multiple parameters
      unsigned int i;
      for (i = 0; i < argList.size(); i++)
        (*argList[i]).Inform(keeper);
      break;
    default:
      handle.logFailure("Error in formula - unrecognised type", type);
      break;
  }
}

void Formula::Interchange(Formula& NewF, Keeper* keeper) const {
  if (this == &NewF)
    return;

  while (NewF.argList.size() > 0)
    NewF.argList.pop_back();

  assert(NewF.argList.size() == 0);
  NewF.type = type;
  switch (type) {
    case CONSTANT:
      NewF.value = value;
      break;

    case PARAMETER:
      NewF.value = value;
      NewF.name = name;
      keeper->ChangeVariable(value, NewF.value);
      break;

    case FUNCTION:
      NewF.functiontype = functiontype;
      unsigned int i;
      for (i = 0; i < argList.size(); i++) {
        Formula *f = new Formula(*argList[i]);
        NewF.argList.push_back(f);
      }
      for (i = 0; i < argList.size(); i++)
        (*argList[i]).Interchange(*NewF.argList[i], keeper);
      break;

    default:
      handle.logFailure("Error in formula - unrecognised type", type);
      break;
  }
  assert(NewF.argList.size() == argList.size());
}

void Formula::Delete(Keeper* keeper) const {
  switch (type) {
    case CONSTANT:
      break;
    case PARAMETER:
      keeper->DeleteParam(value);
      break;
    case FUNCTION:
      unsigned int i;
      for (i = 0; i < argList.size(); i++)
        (*argList[i]).Delete(keeper);
      break;
    default:
      handle.logFailure("Error in formula - unrecognised type", type);
      break;
  }
}
