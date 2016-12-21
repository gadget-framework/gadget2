#include "formula.h"
#include "errorhandler.h"
#include "mathfunc.h"
#include "gadget.h"
#include "global.h"

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
  switch (type) {
    case CONSTANT:
    case PARAMETER:
      return value;
      break;
    case FUNCTION:
      double v;
      v = this->evalFunction();
      return v;
      break;
    default:
      handle.logMessage(LOGFAIL, "Error in formula - unrecognised type", type);
      break;
  }
  // JMB need to return something here even though this never happens
  return value;
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

    case SIN: //JMB note that this works in radians
      if (argList.size() != 1)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for sin");
      v = sin(*(argList[0]));
      break;

    case COS: //JMB note that this works in radians
      if (argList.size() != 1)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for cos");
      v = cos(*(argList[0]));
      break;

    case LOG:
      if (argList.size() != 1)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for log");
      if (*(argList[0]) < verysmall)
        handle.logMessage(LOGWARN, "Warning in formula - cannot take log of zero");
      else
        v = log(*(argList[0]));
      break;

    case EXP:
      if (argList.size() != 1)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for exp");
      v = exp(*(argList[0]));
      break;

    case LOG10:
      if (argList.size() != 1)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for log10");
      if (*(argList[0]) < verysmall)
        handle.logMessage(LOGWARN, "Warning in formula - cannot take log10 of zero");
      else
        v = log10(*(argList[0]));
      break;

    case POWER:
      if (argList.size() == 1)
        v = pow(10.0, *(argList[0]));
      else if (argList.size() == 2)
        v = pow(*(argList[0]), *(argList[1]));
      else
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for power");
      break;

    case SQRT:
      if (argList.size() != 1)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for sqrt");
      if (*(argList[0]) < 0.0)
        handle.logMessage(LOGWARN, "Warning in formula - cannot take sqrt of negative number");
      else
        v = sqrt(*(argList[0]));
      break;

    case RAND:
      if (argList.size() != 1)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for rand");
      v = *(argList[0]);
      break;

    case LESS:
      if (argList.size() < 2)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for <");
      v = 1.0;
      for (i = 1; i < argList.size(); i++)
        if (*(argList[i - 1]) >= *(argList[i]))
            v = 0.0;
      break;

    case GREATER:
      if (argList.size() < 2)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for >");
      v = 1.0;
      for (i = 1; i < argList.size(); i++)
        if (*(argList[i - 1]) <= *(argList[i]))
          v = 0.0;
      break;

    case EQUAL:
      if (argList.size() < 2)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for =");
      v = 1.0;
      for (i = 1; i < argList.size(); i++)
        if (!isEqual(*(argList[i - 1]), *(argList[i])))
          v = 0.0;
      break;

    case AND:
      if (argList.size() < 1)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for and");
      v = 1.0;
      for (i = 0; i < argList.size(); i++)
        if (isZero(*(argList[i])))
          v = 0.0;
      break;

    case OR:
      if (argList.size() < 1)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for or");
      v = 0.0;
      for (i = 0; i < argList.size(); i++)
        if (!isZero(*(argList[i])))
          v = 1.0;
      break;

    case NOT:
      if (argList.size() != 1)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for not");
      v = 0.0;
      if (isZero(*(argList[0])))
        v = 1.0;
      break;

    case ABS:
      if (argList.size() != 1)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for abs");
      v = fabs(*(argList[0]));
      break;

    case IF:
      if (argList.size() != 3)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for if");
      v = 0.0;
      if (isZero(*(argList[0])))
        v = *(argList[2]);
      else
        v = *(argList[1]);
      break;

    case PI:
      if (argList.size() != 1)
        handle.logMessage(LOGFAIL, "Error in formula - invalid number of parameters for pi");
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
  value = initial.value;
  functiontype = initial.functiontype;
  switch (type) {
    case CONSTANT:
      break;
    case PARAMETER:
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

    } else if ((strcasecmp(text, "log") == 0) || (strcasecmp(text, "ln") == 0)) {
      F.functiontype = LOG;

    } else if (strcasecmp(text, "exp") == 0) {
      F.functiontype = EXP;

    } else if (strcasecmp(text, "log10") == 0) {
      F.functiontype = LOG10;

    } else if ((strcasecmp(text, "power") == 0) || (strcasecmp(text, "**") == 0)) {
      F.functiontype = POWER;

    } else if (strcasecmp(text, "sqrt") == 0) {
      F.functiontype = SQRT;

    } else if (strcasecmp(text, "rand") == 0) {
      F.functiontype = RAND;
      Formula* f = new Formula(randomNumber());
      F.argList.push_back(f);
      if (handle.getRunOptimise())
        handle.logMessage(LOGWARN, "Warning in formula - random function used for optimising run");

    } else if (strcasecmp(text, "<") == 0) {
      F.functiontype = LESS;

    } else if (strcasecmp(text, ">") == 0) {
      F.functiontype = GREATER;

    } else if (strcasecmp(text, "=") == 0) {
      F.functiontype = EQUAL;

    } else if (strcasecmp(text, "and") == 0) {
      F.functiontype = AND;

    } else if (strcasecmp(text, "or") == 0) {
      F.functiontype = OR;

    } else if (strcasecmp(text, "not") == 0) {
      F.functiontype = NOT;

    } else if (strcasecmp(text, "abs") == 0) {
      F.functiontype = ABS;

    } else if (strcasecmp(text, "if") == 0) {
      F.functiontype = IF;
      if (handle.getRunOptimise())
        handle.logMessage(LOGWARN, "Warning in formula - if statement used for optimising run");

    } else if (strcasecmp(text, "pi") == 0) {
      F.functiontype = PI;
      Formula* f = new Formula(pivalue);
      F.argList.push_back(f);

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

  if (!isdigit(c) && (c != '-'))  //JMB check that we actually have a number to read
    handle.logFileMessage(LOGFAIL, "failed to read formula data");

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
    F.type = PARAMETER;
    infile.get(c);
    infile >> F.name;
    if (strlen(F.name.getName()) <= 0)
      handle.logFileMessage(LOGFAIL, "failed to read parameter name");
    return infile;
  }

  // If we get to here then things have failed
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
  newF.value = value;
  newF.functiontype = functiontype;
  switch (type) {
    case CONSTANT:
      break;

    case PARAMETER:
      newF.name = name;
      keeper->changeVariable(value, newF.value);
      break;

    case FUNCTION:
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
  value = F.value;
  switch (type) {
    case CONSTANT:
      break;
    case PARAMETER:
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

