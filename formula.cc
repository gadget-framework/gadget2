#include "formula.h"
#include "errorhandler.h"
#include "mathfunc.h"
#include "gadget.h"

extern ErrorHandler handle;

Formula::Formula() {
  value = 0.0;
  type = CONSTANT;
  functiontype = NONE;
}

Formula::Formula(double initial) {
  value = initial;
  type = CONSTANT;
  functiontype = NONE;
}

Formula::~Formula() {
  unsigned int i;
  for (i = 0; i < argList.size(); i++)
    delete argList[i];
}

Formula::Formula(FunctionType ft, vector<Formula*> formlist) {
  if (formlist.size() <= 0)
    handle.logMessage(LOGFAIL, "Error in formula - no formula given in parameter list");

  value = 0.0;
  type = FUNCTION;
  functiontype = ft;
  unsigned int i;
  for (i = 0; i < formlist.size(); i++) {
    Formula *f = new Formula(*formlist[i]);
    argList.push_back(f);
  }
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
      handle.logMessage(LOGFAIL, "Error in formula - unrecognised type", type);
      break;
  }
  return v;
}

double Formula::evalFunction() const {
  double v = 0.0;
  unsigned int i;
  switch (functiontype) {
    case NONE:
      handle.logMessage(LOGFAIL, "Error in formula - no function type found");
      break;

    case MULT:
      v = 1.0;
      for (i = 0; i < argList.size(); i++)
        v *= *(argList[i]);
      break;

    case DIV:
      if (argList.size() == 0) {
        v = 1.0;
      } else if (argList.size() == 1) {
        if (!isZero(*(argList[0]))) {
          v = 1.0 / (*(argList[0]));
        } else {
          handle.logMessage(LOGWARN, "Warning in formula - divide by zero");
        }
      } else {
        v = *(argList[0]);
        for (i = 1; i < argList.size(); i++) {
          if (!isZero(*(argList[0]))) {
            v /= *(argList[i]);
          } else {
            handle.logMessage(LOGWARN, "Warning in formula - divide by zero");
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
      if (argList.size() == 0) {
        v = 0.0;
      } else if (argList.size() == 1) {
        v = -(*(argList[0]));
      } else {
        v = *(argList[0]);
        for (i = 1; i < argList.size(); i++)
          v -= *(argList[i]);
      }
      break;

    case SIN:
      //JMB note that this works in radians
      if (argList.size() != 1)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for sin");
      v = sin(*(argList[0]));
      break;

    case COS:
      //JMB note that this works in radians
      if (argList.size() != 1)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for cos");
      v = cos(*(argList[0]));
      break;

    case LOG:
      if (argList.size() != 1)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for log");
      v = log(*(argList[0]));
      break;

    case EXP:
      if (argList.size() != 1)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for exp");
      v = exp(*(argList[0]));
      break;

    case SQRT:
      if (argList.size() != 1)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for sqrt");
      v = sqrt(*(argList[0]));
      break;

    case RAND:
      if (argList.size() != 1)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for rand");
      v = *(argList[0]);
      break;

    default:
      handle.logMessage(LOGFAIL, "Error in formula - unrecognised function type", type);
      break;
  }
  return v;
}

Formula::Formula(const Formula& initial) {
  type = initial.type;
  functiontype = initial.functiontype;
  switch (type) {
    case CONSTANT:
      value = initial.value;
      break;
    case PARAMETER:
      value = initial.value;
      name = initial.name;
      break;
    case FUNCTION:
      unsigned int i;
      for (i = 0; i < initial.argList.size(); i++) {
        Formula* f = new Formula(*initial.argList[i]);
        argList.push_back(f);
      }
      break;
    default:
      handle.logMessage(LOGFAIL, "Error in formula - unrecognised type", type);
      break;
  }
}

void Formula::setValue(double init) {
  if (type == FUNCTION)
    handle.logMessage(LOGFAIL, "Error in formula - cannot set value for function");
  value = init;
}

CommentStream& operator >> (CommentStream& infile, Formula& F) {
  if (F.type != CONSTANT)
    handle.logFileMessage(LOGFAIL, "failed to read formula data");

  if (infile.fail())
    return infile;

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
    if (strcasecmp(text, "*") == 0) {
      F.functiontype = MULT;

    } else if (strcasecmp(text, "/") == 0) {
      F.functiontype = DIV;

    } else if (strcasecmp(text, "+") == 0) {
      F.functiontype = PLUS;

    } else if (strcasecmp(text, "-") == 0) {
      F.functiontype = MINUS;

    } else if (strcasecmp(text, "sin") == 0) {
      F.functiontype = SIN;

    } else if (strcasecmp(text, "cos") == 0) {
      F.functiontype = COS;

    } else if (strcasecmp(text, "log") == 0) {
      F.functiontype = LOG;

    } else if (strcasecmp(text, "exp") == 0) {
      F.functiontype = EXP;

    } else if (strcasecmp(text, "sqrt") == 0) {
      F.functiontype = SQRT;

    } else if (strcasecmp(text, "rand") == 0) {
      F.functiontype = RAND;
      Formula* f = new Formula(randomNumber());
      F.argList.push_back(f);
      if (handle.getRunOptimise())
        handle.logMessage(LOGWARN, "Warning in formula - random function used for optimising run");

    } else {
      handle.logFileMessage(LOGFAIL, "unrecognised formula function name", text);
      return infile;
    }

    c = infile.peek();
    while (c != ')') {
      Formula* f = new Formula();
      infile >> *f;
      F.argList.push_back(f);
      infile >> ws;
      if (infile.eof()) {
        // Something has gone wrong, no closing bracket
        handle.logFileMessage(LOGFAIL, "failed to read formula data");
        return infile;
      }
      c = infile.peek();
    }
    infile.get(c);
    return infile;
  }

  // Read PARAMETER syntax (no initial value)
  if (c == '#') {
    //No initial value, parameter name only - set it to 1
    F.value = 1.0;
    F.type = PARAMETER;
    infile.get(c);
    infile >> F.name;
    if (strlen(F.name.getName()) <= 0)
      handle.logFileMessage(LOGFAIL, "failed to read parameter name");
    return infile;
  }

  // Read initial value (could be CONSTANT or PARAMETER)
  if (!(infile >> F.value))
    return infile;

  // then '#' if there is one
  c = infile.peek();
  if (infile.fail() && !infile.eof())
    return infile;

  // Read CONSTANT
  if ((c != '#') || infile.eof()) {
    //If not '#' then return
    F.type = CONSTANT;
    return infile;
  }
  // Read PARAMETER (with initial value)
  if (c == '#') {
    // Only one mark
    F.type = PARAMETER;
    infile.get(c);
    infile >> F.name;
    if (strlen(F.name.getName()) <= 0)
      handle.logFileMessage(LOGFAIL, "failed to read parameter name");
    return infile;
  }

  handle.logFileMessage(LOGFAIL, "failed to read formula data");
  return infile;
}

void Formula::Inform(Keeper* keeper) {
  // let keeper know of the marked variables
  switch (type) {
    case CONSTANT:
      break;
    case PARAMETER:
      // only one parameter
      keeper->keepVariable(value, name);
      break;
    case FUNCTION:
      // function with multiple parameters
      unsigned int i;
      for (i = 0; i < argList.size(); i++)
        (*argList[i]).Inform(keeper);
      break;
    default:
      handle.logMessage(LOGFAIL, "Error in formula - unrecognised type", type);
      break;
  }
}

void Formula::Interchange(Formula& newF, Keeper* keeper) const {
  while (newF.argList.size() > 0)
    newF.argList.pop_back();

  newF.type = type;
  switch (type) {
    case CONSTANT:
      newF.value = value;
      break;

    case PARAMETER:
      newF.value = value;
      newF.name = name;
      keeper->changeVariable(value, newF.value);
      break;

    case FUNCTION:
      newF.functiontype = functiontype;
      unsigned int i;
      for (i = 0; i < argList.size(); i++) {
        Formula* f = new Formula(*argList[i]);
        newF.argList.push_back(f);
      }
      for (i = 0; i < argList.size(); i++)
        (*argList[i]).Interchange(*newF.argList[i], keeper);
      break;

    default:
      handle.logMessage(LOGFAIL, "Error in formula - unrecognised type", type);
      break;
  }
}

void Formula::Delete(Keeper* keeper) const {
  switch (type) {
    case CONSTANT:
      break;
    case PARAMETER:
      keeper->deleteParameter(value);
      break;
    case FUNCTION:
      unsigned int i;
      for (i = 0; i < argList.size(); i++)
        (*argList[i]).Delete(keeper);
      break;
    default:
      handle.logMessage(LOGFAIL, "Error in formula - unrecognised type", type);
      break;
  }
}

Formula& Formula::operator = (const Formula& F) {
  type = F.type;
  functiontype = F.functiontype;
  switch (type) {
    case CONSTANT:
      value = F.value;
      break;
    case PARAMETER:
      value = F.value;
      name = F.name;
      break;
    case FUNCTION:
      unsigned int i;
      for (i = 0; i < argList.size(); i++)
        delete argList[i];
      argList.clear();
      for (i = 0; i < F.argList.size(); i++) {
        Formula *f = new Formula(*F.argList[i]);
        argList.push_back(f);
      }
      break;
    default:
      handle.logMessage(LOGFAIL, "Error in formula - unrecognised type", type);
      break;
  }
  return *this;
}
