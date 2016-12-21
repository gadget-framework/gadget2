#include "migration.h"
#include "readfunc.h"
#include "readword.h"
#include "errorhandler.h"
#include "mathfunc.h"
#include "migrationarea.h"
#include "gadget.h"
#include "global.h"

// ********************************************************
// Functions for base Migration
// ********************************************************
Migration::Migration(const IntVector& Areas, const char* givenname)
  : HasName(givenname), LivesOnAreas(Areas) {

  if (areas.Size() == 1)
    handle.logMessage(LOGWARN, "Warning in migration - only one area defined");
}

// ********************************************************
// Functions for MigrationNumbers
// ********************************************************
MigrationNumbers::MigrationNumbers(CommentStream& infile, const IntVector& Areas,
  const AreaClass* const Area, const TimeClass* const TimeInfo,
  const char* givenname, Keeper* const keeper)
  : Migration(Areas, givenname) {

  int i, j;
  ifstream subfile;
  CommentStream subcomment(subfile);
  char text[MaxStrLength];
  char filename[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  strncpy(filename, "", MaxStrLength);
  keeper->addString("migrationmatrix");

  // Open and read year and step information
  readWordAndValue(infile, "yearstepfile", filename);
  subfile.open(filename, ios::in);
  handle.checkIfFailure(subfile, filename);
  handle.Open(filename);
  this->readTimeStepData(subcomment, TimeInfo);
  handle.Close();
  subfile.close();
  subfile.clear();

  // Open and read information about how the matrices are defined
  infile >> text >> filename >> ws;
  subfile.open(filename, ios::in);
  handle.checkIfFailure(subfile, filename);
  handle.Open(filename);

  if (strcasecmp(text, "definematrices") == 0) {
    this->readGivenMatrices(subcomment, keeper);
  } else if (strcasecmp(text, "defineratios") == 0) {
    this->readGivenRatios(subcomment, keeper, Area);
  } else
    handle.logFileUnexpected(LOGFAIL, "definematrices or defineratios", text);

  handle.Close();
  subfile.close();
  subfile.clear();

  this->checkMatrixIndex();
  handle.logMessage(LOGMESSAGE, "Read migration numbers file - number of migration matrices", readMigration.Size());
  keeper->clearLast();
}

MigrationNumbers::~MigrationNumbers() {
  int i;
  for (i = 0; i < timeindex.Size(); i++)
    if (timeindex[i] != -1)
      delete[] allmatrixnames[i];
  for (i = 0; i < usedmatrixnames.Size(); i++) {
    delete[] usedmatrixnames[i];
    delete readMigration[i];
    delete calcMigration[i];
  }
}

void MigrationNumbers::readTimeStepData(CommentStream& infile, const TimeClass* const TimeInfo) {

  int year, step, timeid, count;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  // check the number of columns in the inputfile
  infile >> ws;
  if (countColumns(infile) != 3)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 3");

  year = step = count = 0;
  allmatrixnames.resizeBlank(TimeInfo->numTotalSteps() + 1);
  timeindex.resize(TimeInfo->numTotalSteps() + 1, -1);
  while (!infile.eof()) {
    infile >> year >> step >> text >> ws;
    if (TimeInfo->isWithinPeriod(year, step)) {
      timeid = TimeInfo->calcSteps(year, step);
      timeindex[timeid] = 0;
      allmatrixnames[timeid] = new char[strlen(text) + 1];
      strcpy(allmatrixnames[timeid], text);
      count++;
    }
  }

  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in migration - found no migration data");
  if (count != TimeInfo->numTotalSteps())
    handle.logMessage(LOGWARN, "Warning in migration - migration data doesnt span time range");
  handle.logMessage(LOGMESSAGE, "Read migration time data - number of entries", count);
}

void MigrationNumbers::readGivenRatios(CommentStream& infile, Keeper* const keeper, const AreaClass* const Area) {

  int innerrow, innercol, fromArea, toArea;
  int i, j, k, count;
  int numAreas = areas.Size();
  char text[MaxStrLength];
  char name[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  strncpy(name, "", MaxStrLength);

  /* File format:
     [migrationmatrix]
     name   matrixname
     fromarea toarea  ratio
     [migrationmatrix]
     ...
  */

  toArea = fromArea = 0;
  // Read ratio information from file
  while (!infile.eof() && !infile.fail()) {
    infile >> text >> ws;
    if (strcasecmp(text, "[migrationmatrix]") != 0)
      handle.logFileUnexpected(LOGFAIL, "[migrationmatrix]", text);

    readWordAndValue(infile, "name", name);
    if (this->useMatrix(name)) {
      this->setMatrixName(name);
      count = readMigration.Size();
      checkvalues.AddRows(1, numAreas, 0);
      readMigration.resize(new FormulaMatrix(numAreas, numAreas, 0.0));

      infile >> ws;
      while (!infile.eof() && !infile.fail() && (infile.peek() != '[')) {
        if (!(isdigit(infile.peek()))) {
          infile >> text >> ws;
          handle.logFileUnexpected(LOGFAIL, "[migrationmatrix] or area number", text);
        }

        infile >> fromArea >> toArea >> ws;
        innerrow = Area->getInnerArea(toArea);
        innercol = Area->getInnerArea(fromArea);
        if (!this->isInArea(innerrow) || !this->isInArea(innercol))
          handle.logMessage(LOGFAIL, "Error in migration - invalid area for matrix", name);

        toArea = this->areaNum(innerrow);
        fromArea = this->areaNum(innercol);
        infile >> (*readMigration[count])[toArea][fromArea] >> ws;
        checkvalues[count][fromArea]++;
      }

    } else {
      handle.logMessage(LOGWARN, "Warning in migration - matrix not used in the simulation", name);
      infile >> ws;
      while (!infile.eof() && !infile.fail() && infile.peek() != '[')
        infile >> text >> ws;
    }
  }

  // Finished reading from file, now check the values
  Formula missing;
  missing.setValue(1.0);
  for (i = 0; i < readMigration.Size(); i++) {
    for (j = 0; j < readMigration[i]->Nrow(); j++) {
      if (checkvalues[i][j] == 0) {
        // no data read for migration from this area
        (*readMigration[i])[j][j].setValue(1.0);

      } else if (checkvalues[i][j] == numAreas) {
        // nothing to do - data read for migration from this area to every other area

      } else if (isZero((*readMigration[i])[j][j])) {
        // read data for migration from this area to other areas
        // so simply subtract the migration from 1 to find those that are left
        count = 0;
        vector<Formula*> checklist;
        for (k = 0; k < readMigration[i]->Ncol(); k++) {
          if (!(isZero((*readMigration[i])[k][j]))) {
            checklist.push_back(&(*readMigration[i])[k][j]);
            count++;
          }
        }

        if (count == 0)
          handle.logFileMessage(LOGFAIL, "missing data for migration matrix", usedmatrixnames[i]);
        checklist.insert(checklist.begin(), &missing);
        Formula* missingValue = new Formula(MINUS, checklist);
        (*readMigration[i])[j][j] = *missingValue;
        delete missingValue;

      } else if (checkvalues[i][j] == (numAreas - 1)) {
        // read migration data from this area to all other areas but 1
        count = -1;
        vector<Formula*> checklist;
        for (k = 0; k < readMigration[i]->Ncol(); k++) {
          if (isZero((*readMigration[i])[k][j]))
            count = k;
          else
            checklist.push_back(&(*readMigration[i])[k][j]);
        }

        if (count == -1)
          handle.logFileMessage(LOGFAIL, "missing data for migration matrix", usedmatrixnames[i]);
        checklist.insert(checklist.begin(), &missing);
        Formula* missingValue = new Formula(MINUS, checklist);
        (*readMigration[i])[count][j] = *missingValue;
        delete missingValue;

      } else {
        // havent read enough information to reliably construct matrix
        handle.logFileMessage(LOGFAIL, "missing data for migration matrix", usedmatrixnames[i]);

      }
    }
  }

  // Inform keeper of the values and resize
  for (i = 0; i < readMigration.Size(); i++) {
    (*readMigration[i]).Inform(keeper);
    calcMigration.resize(new DoubleMatrix(numAreas, numAreas, 0.0));
  }
}

void MigrationNumbers::readGivenMatrices(CommentStream& infile, Keeper* const keeper) {

  int i, j, count;
  int numAreas = areas.Size();
  char text[MaxStrLength];
  char name[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  strncpy(name, "", MaxStrLength);

  /* File format:
     [migrationmatrix]
     name   matrixname
     x11 ... x1n
     ...
     xn1 ... xnn
     [migrationmatrix]
     ...
  */

  // Read matrix information from file
  while (!infile.eof() && !infile.fail()) {
    infile >> text >> ws;
    if (strcasecmp(text, "[migrationmatrix]") != 0)
      handle.logFileUnexpected(LOGFAIL, "[migrationmatrix]", text);

    readWordAndValue(infile, "name", name);
    if (this->useMatrix(name)) {
      this->setMatrixName(name);
      count = readMigration.Size();
      readMigration.resize(new FormulaMatrix(numAreas, numAreas, 0.0));

      infile >> ws;
      if (infile.peek() == '[' || infile.eof()) {
        for (i = 0; i < numAreas; i++)
          (*readMigration[count])[i][i].setValue(1.0);

      } else if (isdigit(infile.peek()) || infile.peek() == '#' || infile.peek() == '(') {
        // look for a number or a formula as the next entry to be read
        for (i = 0; i < numAreas; i++)
          for (j = 0; j < numAreas; j++)
            if (!(infile >> (*readMigration[count])[i][j]))
              handle.logMessage(LOGFAIL, "Error in migration - failed to read matrix", name);
        infile >> ws;  //JMB strip whitespace

      } else {
        infile >> text;
        handle.logFileUnexpected(LOGFAIL, "[migrationmatrix] or matrix value", text);
      }

    } else {
      handle.logMessage(LOGWARN, "Warning in migration - matrix not used in the simulation", name);
      infile >> ws;
      while (!infile.eof() && !infile.fail() && infile.peek() != '[')
        infile >> text >> ws;
    }
  }

  // Inform keeper of the values and resize
  for (i = 0; i < readMigration.Size(); i++) {
    (*readMigration[i]).Inform(keeper);
    calcMigration.resize(new DoubleMatrix(numAreas, numAreas, 0.0));
  }
}

void MigrationNumbers::checkMatrixIndex() {
  int i, j, check;

  for (i = 0; i < timeindex.Size(); i++) {
    if (timeindex[i] == 0) {
      check = -1;
      for (j = 0; j < usedmatrixnames.Size(); j++)
        if (strcasecmp(allmatrixnames[i], usedmatrixnames[j]) == 0)
          check = j;

      if (check == -1)
        handle.logMessage(LOGFAIL, "Error in migration - failed to read matrix", allmatrixnames[i]);
      timeindex[i] = check;

    } else if (timeindex[i] != -1) {
      handle.logMessage(LOGFAIL, "Error in migration - repeated migration matrix", i);
    }
  }
}

void MigrationNumbers::Print(ofstream& outfile) {
  int i, j, k;
  outfile << "\nMigration\n\tNames of migration matrices:\n\t";
  for (i = 0; i < timeindex.Size(); i++)
    if (timeindex[i] != -1)
      outfile << allmatrixnames[i] << sep;
  outfile << "\n\n\tMigration matrices";
  for (i = 0; i < calcMigration.Size(); i++) {
    outfile << "\n\tMatrix name: " << usedmatrixnames[i] << "\n\t";
    for (j = 0; j < calcMigration[i]->Nrow(); j++) {
      for (k = 0; k < calcMigration[i]->Ncol(j); k++)
        outfile << setw(smallwidth) << (*calcMigration[i])[j][k] << sep;
      outfile << "\n\t";
    }
  }
  outfile.flush();
}

const DoubleMatrix& MigrationNumbers::getMigrationMatrix(const TimeClass* const TimeInfo) {
  return (*calcMigration[timeindex[TimeInfo->getTime()]]);
}

void MigrationNumbers::setMatrixName(char* name) {
  int i;
  // check to ensure that this matrix name is unique
  for (i = 0; i < usedmatrixnames.Size(); i++)
    if (strcasecmp(usedmatrixnames[i], name) == 0)
      handle.logMessage(LOGFAIL, "Error in migration - repeated matrix", name);

  // store the new matrix name
  char* tempname = new char[strlen(name) + 1];
  strcpy(tempname, name);
  usedmatrixnames.resize(tempname);
}

void MigrationNumbers::Reset() {
  //JMB need to reset the penalty vector first
  penalty.Reset();

  int i, j, k;
  double colsum;
  for (i = 0; i < readMigration.Size(); i++) {
    for (k = 0; k < readMigration[i]->Ncol(); k++) {
      colsum = 0.0;
      for (j = 0; j < readMigration[i]->Nrow(); j++) {
        if (isZero((*readMigration[i])[j][k])) {
          (*calcMigration[i])[j][k] = 0.0;
        } else if ((*readMigration[i])[j][k] < 0.0) {
          penalty.resize(1, (*readMigration[i])[j][k]);
          (*calcMigration[i])[j][k] = 0.0;
          handle.logMessage(LOGWARN, "Warning in migration - value outside bounds", (*readMigration[i])[j][k]);
        } else if ((*readMigration[i])[j][k] > 1.0) {
          penalty.resize(1, (*readMigration[i])[j][k]);
          (*calcMigration[i])[j][k] = 1.0;
          handle.logMessage(LOGWARN, "Warning in migration - value outside bounds", (*readMigration[i])[j][k]);
        } else
          (*calcMigration[i])[j][k] = (*readMigration[i])[j][k];

        colsum += (*calcMigration[i])[j][k];
      }

      if (isZero(colsum)) {
        handle.logMessage(LOGWARN, "Warning in migration - column sum is zero");
        // just putting anything here ... AJ 23.06.05, Needs to be changed!!!!!
        penalty.resize(1, 1.0);
        for (j = 0; j < readMigration[i]->Nrow(); j++) {
          if (k == j)
            (*calcMigration[i])[j][k] = 1.0;
          else
            (*calcMigration[i])[j][k] = 0.0;
        }

      } else if (isEqual(colsum, 1.0)) {
        // everything is OK ...

      } else {
        penalty.resize(1, colsum);
        colsum = 1.0 / colsum;
        for (j = 0; j < readMigration[i]->Nrow(); j++)
          (*calcMigration[i])[j][k] *= colsum;
      }
    }
  }

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset migration data for stock", this->getName());
}

int MigrationNumbers::useMatrix(char* name) {
  int i;
  for (i = 0; i < timeindex.Size(); i++)
    if ((timeindex[i] != -1) && (strcasecmp(allmatrixnames[i], name) == 0))
      return 1;
  return 0;
}

int MigrationNumbers::isMigrationStep(const TimeClass* const TimeInfo) {
  if (timeindex[TimeInfo->getTime()] == -1)
    return 0;
  return 1;
}

// ********************************************************
// Functions for MigrationFunction
// ********************************************************
MigrationFunction::MigrationFunction(CommentStream& infile, const IntVector& Areas,
  const AreaClass* const Area, const TimeClass* const TimeInfo,
  const char* givenname, Keeper* const keeper)
  : Migration(Areas, givenname) {

  int i;
  ifstream subfile;
  CommentStream subcomment(subfile);
  char text[MaxStrLength];
  char filename[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  strncpy(filename, "", MaxStrLength);
  keeper->addString("migrationfunction");

  readWordAndModelVariable(infile, "diffusion", diffusion, TimeInfo, keeper);
  readWordAndModelVariable(infile, "driftx", driftx, TimeInfo, keeper);
  readWordAndModelVariable(infile, "drifty", drifty, TimeInfo, keeper);
  readWordAndVariable(infile, "lambda", lambda);
  delta = TimeInfo->getTimeStepLength() / TimeInfo->numSubSteps();

  // Open and read given year and step information
  readWordAndValue(infile, "areadefinition", filename);
  subfile.open(filename, ios::in);
  handle.checkIfFailure(subfile, filename);
  handle.Open(filename);
  this->readAreaData(subcomment, Area, TimeInfo, keeper);
  handle.Close();
  subfile.close();
  subfile.clear();

  calcMigration.AddRows(oceanareas.Size(), oceanareas.Size(), 0.0);
  if (oceanareas.Size() != areas.Size())
    handle.logMessage(LOGWARN, "Warning in migration - migration data doesnt cover all areas");

  handle.logMessage(LOGMESSAGE, "Read migration function file - number of migration areas", oceanareas.Size());
  keeper->clearLast();
}

void MigrationFunction::readAreaData(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper) {

  int area, inarea;
  ifstream subfile;
  CommentStream subcomment(subfile);
  char text[MaxStrLength];
  char filename[MaxStrLength];
  char areaname[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  strncpy(filename, "", MaxStrLength);
  strncpy(areaname, "", MaxStrLength);

  /* File format:
     [area]
     name        areaname
     number      areanumber
     rectangles  filename containing the rectangle information
     [area]
     ...
  */

  // Read area information from file
  infile >> ws;
  while (!infile.eof() && !infile.fail()) {
    infile >> text >> ws;
    if (strcasecmp(text, "[area]") != 0)
      handle.logFileUnexpected(LOGFAIL, "[area]", text);

    readWordAndValue(infile, "name", areaname);
    readWordAndVariable(infile, "number", area);
    readWordAndValue(infile, "rectangles", filename);

    inarea = Area->getInnerArea(area);
    if (!this->isInArea(inarea))
      handle.logMessage(LOGFAIL, "Error in migration - invalid area", area);

    subfile.open(filename, ios::in);
    handle.checkIfFailure(subfile, filename);
    handle.Open(filename);
    oceanareas.resize(new MigrationArea(subcomment, areaname, inarea));
    handle.Close();
    subfile.close();
    subfile.clear();
  }
}

MigrationFunction::~MigrationFunction() {
  int i;
  for (i = 0; i < oceanareas.Size(); i++)
    delete oceanareas[i];
}

void MigrationFunction::Print(ofstream& outfile) {
  int i, j;
  outfile << "\nMigration\n\n\tMigration matrix\n\t";
  for (i = 0; i < calcMigration.Nrow(); i++) {
    for (j = 0; j < calcMigration.Ncol(i); j++)
      outfile << setw(smallwidth) << calcMigration[i][j] << sep;
    outfile << "\n\t";
  }
  outfile.flush();
}

const DoubleMatrix& MigrationFunction::getMigrationMatrix(const TimeClass* const TimeInfo) {
  if (this->updateVariables(TimeInfo))
    this->recalcMatrix();
  return calcMigration;
}

void MigrationFunction::recalcMatrix() {
  int from, to, idfrom, idto, i, j;
  double mig, sa;
  double colsum, sum;

  for (from = 0; from < oceanareas.Size(); from++) {
    idfrom = this->areaNum(oceanareas[from]->getAreaID());
    colsum = 0.0;
    for (to = 0; to < oceanareas.Size(); to++) {
      idto = this->areaNum(oceanareas[to]->getAreaID());
      sum = 0.0;
      for (i = 0; i < oceanareas[from]->getNumRectangles(); i++) {
        sa = (oceanareas[from]->getRectangles()[i])->getArea();
        if (!(isZero(sa))) {
          for (j = 0; j < oceanareas[to]->getNumRectangles(); j++) {
            mig = this->getMigrationFunction(oceanareas[from]->getRectangles()[i], oceanareas[to]->getRectangles()[j]);
            sum += (mig * sa);
          }
        }
      }

      calcMigration[idto][idfrom] = sum / oceanareas[from]->getArea();
      colsum += calcMigration[idto][idfrom];
    }

    if (isZero(colsum)) {
      handle.logMessage(LOGWARN, "Warning in migration - column sum is zero");
      penalty.resize(1, 1.0);
      calcMigration[idfrom][idfrom] = 1.0;

    } else if (isEqual(colsum, 1.0)) {
      // everything is OK ...

    } else {
      handle.logMessage(LOGWARN, "Warning in migration - column sum", colsum);
      penalty.resize(1, colsum);
      colsum = 1.0 / colsum;
      for (i = 0; i < calcMigration.Ncol(); i++)
        calcMigration[i][idfrom] *= colsum;
    }
  }
}

void MigrationFunction::Reset() {
  //JMB need to reset the penalty vector
  penalty.Reset();
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset migration data for stock", this->getName());
}

int MigrationFunction::isMigrationStep(const TimeClass* const TimeInfo) {
  return 1;
}

int MigrationFunction::updateVariables(const TimeClass* const TimeInfo) {
  //update the values of the variables that can change
  delta = TimeInfo->getTimeStepLength() / TimeInfo->numSubSteps();
  diffusion.Update(TimeInfo);
  driftx.Update(TimeInfo);
  drifty.Update(TimeInfo);

  if ((TimeInfo->didStepSizeChange()) || (diffusion.didChange(TimeInfo))
      || (driftx.didChange(TimeInfo)) || (drifty.didChange(TimeInfo)))
    return 1;
  return 0;
}

double MigrationFunction::getMigrationFunction(Rectangle* fromRec, Rectangle* toRec) {
  double fx, fy;
  double dx, dy;
  double xiL, yiL, xiU, yiU;
  double xfL, yfL, xfU, yfU;

  if (isZero(diffusion) || isZero(lambda))  // prevent divide by zero errors ...
    return 0.0;

  xiL = fromRec->getLowerX();
  yiL = fromRec->getLowerY();
  xiU = fromRec->getUpperX();
  yiU = fromRec->getUpperY();
  xfL = toRec->getLowerX();
  yfL = toRec->getLowerY();
  xfU = toRec->getUpperX();
  yfU = toRec->getUpperY();

  dx = delta * diffusion;
  dy = dx * lambda;

  // functions from Violeta ...
  fx = -f1x(xfU,xiU,dx,driftx) + f1x(xfU,xiL,dx,driftx) + f1x(xfL,xiU,dx,driftx) - f1x(xfL,xiL,dx,driftx) - f2x(xfU,xiU,dx,driftx) + f2x(xfU,xiL,dx,driftx) + f2x(xfL,xiU,dx,driftx) - f2x(xfL,xiL,dx,driftx);

  if (fx < verysmall)
    return 0.0;

  fy = -f1x(yfU,yiU,dy,drifty) + f1x(yfU,yiL,dy,drifty) + f1x(yfL,yiU,dy,drifty) - f1x(yfL,yiL,dy,drifty) - f2x(yfU,yiU,dy,drifty) + f2x(yfU,yiL,dy,drifty) + f2x(yfL,yiU,dy,drifty) - f2x(yfL,yiL,dy,drifty);

  if (fy < verysmall)
    return 0.0;

  return (0.5 * fx * fy);
}

double MigrationFunction::f1x(double w, double u, double D, double beta) {
  return ((2/sqrt(pivalue))*sqrt(D)*exp(-(w-(beta*delta)-u)*(w-(beta*delta)-u)/(4*D)));
}

double MigrationFunction::f2x(double w, double u, double D, double beta) {
  return ((w-(beta*delta)-u)*erf((w-(beta*delta)-u)/(2*sqrt(D))));
}
