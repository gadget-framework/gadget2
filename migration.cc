#include "migration.h"
#include "readfunc.h"
#include "errorhandler.h"
#include "binarytree.h"
#include "mathfunc.h"
#include "vectorusingkeeper.h"
#include "variableinfo.h"
#include "gadget.h"

Migration::Migration(CommentStream& infile, int AgeDepMig, const intvector&  Areas,
  const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper)
  : LivesOnAreas(Areas), AgeDepMigration(AgeDepMig), error(0) {

  int i, j;
  ErrorHandler handle;
  const int noareas = areas.Size();
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  keeper->AddString("migration");
  if (AgeDepMigration) {                   //File format:
    infile >> text;                        //ages a1 ... am
    if (!(strcasecmp(text, "ages") == 0))  //. . .
      handle.Unexpected("ages", text);     //k1 ... kn
    i = 0;                                 //the line with a1, ..., am is in
    infile >> ws;                          //ages[0] etc.
    while (isdigit(infile.peek())) {
      //Next we want to add a line of length 0 to ages.
      ages.AddRows(1, 0);
      ReadVectorInLine(infile, ages[i]);
      infile >> ws;
    }
  }

  if (AgeDepMigration)
    MatrixNumbers.AddRows(ages.Nrow(), TimeInfo->TotalNoSteps() + 1);
  else
    MatrixNumbers.AddRows(1, TimeInfo->TotalNoSteps() + 1);
  infile >> text;

  //Read the numbers of the migration matrices we are going to use.
  //File format: matrixnumbers
  //and then the file format for ReadNoMigrationMatrices
  keeper->AddString("matrixnumbers");
  if (!(strcasecmp(text, "matrixnumbers") == 0))
    handle.Unexpected("matrixnumbers", text);
  else
    this->ReadNoMigrationMatrices(infile, TimeInfo, keeper);

  //Now we have read the matrix numbers.
  //Skip input from infile until we find a nonnumeric character.
  char c = infile.peek();
  while (!infile.eof() && (isdigit(c) || isspace(c))) {
    infile.get(c);
    c = infile.peek();
  }

  //Proceed to read the variables.
  //File format: variables
  //and then the file format for ReadOptVariables
  keeper->ClearLastAddString("variables");
  strcpy(text, " ");
  infile >> text;
  if (!(strcasecmp(text, "variables") == 0))
    handle.Unexpected("variables", text);
  intvector novariables;
  this->ReadOptVariables(infile, novariables, TimeInfo, keeper);

  //Read the coefficients for the variables.
  //File format: coefficients
  //and then the file format for ReadCoefficients
  keeper->ClearLastAddString("coefficients");
  infile >> text;
  if (!(strcasecmp(text, "coefficients") == 0))
    handle.Unexpected("coefficients", text);
  this->ReadCoefficients(infile, Area, keeper);

  int maxim = 0;
  int age = 0;
  int time = 0;
  for (age = 0; age < MatrixNumbers.Nrow(); age++)
    for (time = 1; time < MatrixNumbers.Ncol(age); time++)
      if (MatrixNumbers[age][time] > maxim)
        maxim  = MatrixNumbers[age][time];

  //Now we can read the migration matrices.
  //File format: migrationmatrices
  //n1 Matrix # n1
  //...
  //nm Matrix # nm
  keeper->ClearLastAddString("migrationmatrices");
  ReadMigList.resize(maxim + 1, 0);
  infile >> text >> ws;
  if (!(strcasecmp(text, "migrationmatrices") == 0))
    handle.Unexpected("migrationmatrices", text);
  int no, find;
  while (!infile.eof() && !infile.fail()) {
    infile >> no;
    doublematrix* matptr = ReadMatrix(infile, noareas, noareas);
    //Now we must look if the matrix number no is in MatrixNumbers. If so,
    //we must keep the matrix for future use, else we can discard it.
    find = 0;
    for (age = 0; age < MatrixNumbers.Nrow(); age++)
      for (time = 1; time < MatrixNumbers.Ncol(age); time++)
        find += (MatrixNumbers[age][time] == no);

    if (find != 0)
      ReadMigList[no] = matptr;
    else
      delete matptr;

    infile >> ws;
  }

  //Now we have read all the matrices from the input file - Clean up.
  if (AgeDepMigration)
    for (i = 0; i < ages.Nrow(); i++)
      for (j = 0; j < ages.Ncol(i); j++) {
        if (ages[i][j] >= AgeNr.Size())
          AgeNr.resize(ages[i][j] - AgeNr.Size() + 1);
        AgeNr[ages[i][j]] = i;
      }

  //Now we must check whether we read sufficient information from infile
  //and what information we may delete
  CheckInfoAndDelete(novariables, keeper);
  CalcMigList.resize(ReadMigList.Size(), 0);
  AdjustMigListAndCheckIfError(ReadMigList);
  CopyFromReadToCalc();
  keeper->ClearLast();
  keeper->ClearLast();
}

Migration::~Migration() {
  int i;
  for (i = 0; i < ReadMigList.Size(); i++)
    delete ReadMigList[i];
  for (i = 0; i < CalcMigList.Size(); i++)
    delete CalcMigList[i];
  for (i = 0; i < OptInfo.Size(); i++)
    delete OptInfo[i];
  for (i = 0; i < OptVar.Size(); i++)
    delete OptVar[i];
}

const doublematrix& Migration::Migrationmatrix(const TimeClass* const TimeInfo, int age) {
  if (AgeDepMigration) {
    if (age >= 0 && age < AgeNr.Size())
      if (AgeNr[age] >= 0)
        return *CalcMigList[MatrixNumbers[AgeNr[age]][TimeInfo->CurrentTime()]];

    cerr << "Received illegal age in migration -age was " << age << endl;
    exit(EXIT_FAILURE);
  } else
    return *CalcMigList[ MatrixNumbers[0][TimeInfo->CurrentTime()]];
}

void Migration::MigrationRecalc(int year) {
  if (OptInfo.Size() == 0)
    return;
  int i, j;
  if (OptVariables.Size() > 0)
    for (i = 0; i < OptVariables.Size(); i++)
      OptVariables[i] = OptVar[i]->ValueOfVariable(year);

  CopyFromReadToCalc(); //Copy from ReadMigList to CalcMigList
  double add;
  for (i = 0; i < OptInfo.Size(); i++) {
    const int col = OptInfo[i]->column;
    const int row = OptInfo[i]->row;
    const int no = OptInfo[i]->NoMatrix;
    add = 0.0;
    for (j = 0; j < OptInfo[i]->indices.Size(); j++)
      add += OptInfo[i]->coefficients[j] * OptVariables[OptInfo[i]->indices[j]];

    (*CalcMigList[no])[row][col] += add;
  }
  AdjustMigListAndCheckIfError(CalcMigList);
}

int Migration::Error() const {
  return error;
}

void Migration::Clear() {
  error = 0;
}

void Migration::Print(ofstream& outfile) const {
  int i, j, k;
  outfile << "Migration\nAge dependent migration " << AgeDepMigration << endl;
  if (AgeDepMigration) {
    outfile << "Ages\n";
    for (i = 0; i < ages.Nrow(); i++) {
      for (j = 0; j < ages.Ncol(i); j++)
        outfile << ages[i][j] << sep;
      outfile << endl;
    }
  }
  outfile << "Numbers of migration matrices:\n";
  for (i = 0; i < MatrixNumbers.Nrow(); i++) {
    for (j = 1; j < MatrixNumbers.Ncol(i); j++)
      outfile << MatrixNumbers[i][j] << sep;
    outfile << endl;
  }

  outfile << "Values of migration variables:\n";
  for (i = 0; i < OptVariables.Size(); i++)
    outfile << "Variable " << i << sep << OptVariables[i] << endl;
  outfile << "Optimization:\n";
  for (i = 0; i < OptInfo.Size(); i++)
    outfile << *OptInfo[i];
  outfile << "\nMigration matrices\n";
  for (i = 0; i < CalcMigList.Size(); i++)
    if (CalcMigList[i] != 0) {
      outfile << "Migration matrix number " << i << endl;
      for (j = 0; j < CalcMigList[i]->Nrow(); j++) {
        for (k = 0; k < CalcMigList[i]->Ncol(j); k++) {
          outfile.width(printwidth);
          outfile.precision(smallprecision);
          outfile << (*CalcMigList[i])[j][k] << sep;
        }
        outfile << endl;
      }
    }
}

void Migration::Print(int nr, ofstream& outfile) const {
  int i, j, k;
  outfile << "Migration.\nAge dependent migration " << AgeDepMigration << endl;
  if (AgeDepMigration) {
    outfile << "Ages\n";
    for (i = 0; i < ages.Nrow(); i++) {
      for (j = 0; j < ages.Ncol(i); j++)
        outfile  << ages[i][j] << sep;
      outfile << endl;
    }
  }
  outfile << "Numbers of migration matrices:\n";
  for (i = 0; i < MatrixNumbers.Nrow(); i++) {
    for (j = 1; j < MatrixNumbers.Ncol(i); j++)
      outfile  << MatrixNumbers[i][j] << sep;
    outfile << endl;
  }

  outfile << "Values of migration variables:\n";
  for (i = 0; i < OptVariables.Size(); i++)
    outfile << "Variable " << i << sep << OptVariables[i] << endl;
  outfile << "Optimization:\n";
  for (i = 0; i < OptInfo.Size(); i++)
    if (OptInfo[i]->NoMatrix == nr)
      outfile << *OptInfo[i];

  outfile << "\nMigration matrices\n";
  if (CalcMigList[nr] != 0) {
    outfile << "Migration matrix number " << nr << endl;
    for (j = 0; j < CalcMigList[nr]->Nrow(); j++) {
      for (k = 0; k < CalcMigList[nr]->Ncol(j); k++) {
        outfile.width(printwidth);
        outfile.precision(smallprecision);
        outfile << (*CalcMigList[nr])[j][k] << sep;
      }
      outfile << endl;
    }
  }
}

void Migration::CheckInfoAndDelete(intvector& novariables, Keeper* const keeper) {
  ErrorHandler handle;
  int i, j, age, time;

  //Check if all the matrices we need were found.
  for (age = 0; age < MatrixNumbers.Nrow(); age++)
    for (time = 1; time < MatrixNumbers.Ncol(age); time++)
      if (ReadMigList[MatrixNumbers[age][time]] == 0)
        handle.Message("Failure in migration - unable to read specified migration matrix");

  //Delete information for the matrices not used in the simulation.
  for (i = 0; i < OptInfo.Size(); i++)
    if (!(OptInfo[i]->NoMatrix < ReadMigList.Size())) {
      delete OptInfo[i];
      OptInfo.Delete(i);
      i--;
    } else {
      if (ReadMigList[OptInfo[i]->NoMatrix] == 0) {
        delete OptInfo[i];
        OptInfo.Delete(i);
        i--;
      }
    }

  BinarySearchTree tree;
  BinarySearchTree duplicates;

  //Have to check that novariables contains only unique elements.
  for (i = 0; i < novariables.Size(); i++)
    if (tree.IsIn(novariables[i]))
      duplicates.Insert(novariables[i]);
    else
      tree.Insert(novariables[i]);

  if (!duplicates.IsEmpty()) {
    handle.Message("Failure in migration - read migration variables twice");
    while (!duplicates.IsEmpty())
      duplicates.DeleteSmallest();
  }

  while (!tree.IsEmpty())
    tree.DeleteSmallest();

  //Now we keep only those of the variables that are used in the simulation
  //and check if they do not surely cover all the variables we need.
  //First, insert the numbers of the variables we need.
  for (i = 0; i < OptInfo.Size(); i++)
    for (j = 0; j < OptInfo[i]->indices.Size(); j++)
      tree.Insert(OptInfo[i]->indices[j]);

  //Then we delete the numbers of variables we have read from file.
  for (i = 0; i < novariables.Size(); i++)
    tree.Delete(novariables[i]);

  if (!tree.IsEmpty()) {
    handle.Message("Failure in migration - unable to read specified migration variables");
    while (!tree.IsEmpty())
      tree.DeleteSmallest();
  }

  //Now we want to delete unused variables. We know that now tree is empty.
  for (i = 0; i < novariables.Size(); i++)
    tree.Insert(novariables[i]);
  for (i = 0; i < OptInfo.Size(); i++)
    for (j = 0; j < OptInfo[i]->indices.Size(); j++)
      tree.Delete(OptInfo[i]->indices[j]);

  //tree contains only the numbers of unused variables.
  int unused;
  if (!tree.IsEmpty()) {
    //Print warning messages and delete unused variables at the same time
    handle.Message("Failure in migration - read unused migration variables");
    while (!tree.IsEmpty()) {
      unused = tree.DeleteSmallest();
      for (i = 0; i < novariables.Size(); i++)
        if (novariables[i] == unused) {
          DeleteElementUsingKeeper(OptVariables, keeper, i);
          novariables.Delete(i);
          i--;
        }
    }
  }

  //Now we have deleted the unused variables.
  //We want to convert from the numbers of migration variables read from
  //CommentStream to inner numbers that are from 0 to novariables.Size() - 1.
  int k = 0;
  int found = 0;
  for (i = 0; i < OptInfo.Size(); i++)
    for (j = 0; j < OptInfo[i]->indices.Size(); j++) {
      found = 0;
      k = -1;
      while (!found) {
        k++;
        if (novariables[k] == OptInfo[i]->indices[j])
          found = 1;
      }
      OptInfo[i]->indices[j] = k;
    }
}

void Migration::CopyFromReadToCalc() {
  int i, j, k;

  for (i = 0; i < ReadMigList.Size(); i++) {
    doublematrix* const dmptr = ReadMigList[i];
    if (dmptr != 0) {
      if (CalcMigList[i] == 0)
        CalcMigList[i] = new doublematrix(*dmptr);
      else
        for (j = 0; j < dmptr->Nrow(); j++)
          for (k = 0; k < dmptr->Ncol(j); k++)
            (*CalcMigList[i])[j][k] = (*dmptr)[j][k];
    }
  }
}

void Migration::AdjustMigListAndCheckIfError(MigrationList& MigList) {
  //(1) Every element in every matrix in MigList has to be >= 0.
  //(2) The sum in every column in every matrix in MigList has to equal 1.
  //If it is not possible to enforce (2), then the error bit is set.
  //(Will happen if the sum in a column is 0 (or very close to)).
  //However, it is not obvious what to do about (1). Three obvious alternatives:
  //(i) Replace x(i,j) with absolute_value(x(i, j))
  //(ii) Replace x(i,j) with max(0, x(i, j))
  //(iii) Set error bit if x(i, j) < 0.
  //Now we choose (iii) and to return immediately.
  int i, j, k;
  double colsum, colsum1;
  for (i = 0; i < MigList.Size(); i++) {
    doublematrix* const dmptr = MigList[i];
    if (dmptr != 0) {
      for (k = 0; k < dmptr->Ncol(); k++) {
        colsum = 0.0;
        colsum1 = 0.0;
        for (j = 0; j < dmptr->Nrow(); j++) {
          colsum += (*dmptr)[j][k];
          if ((*dmptr)[j][k] < 0) {
            penalty.resize(1);
            penalty[penalty.Size() - 1] = (*dmptr)[j][k];
            return;
          }
          colsum1 += (*dmptr)[j][k];
        }

        if (iszero(colsum)) {
          cerr << "Error in migration: column " << k + 1 << " in migration matrix "
            << i << " sums up to " << colsum << " instead of 1\n";
          error = 1;
          return;
        }

        for (j = 0; j < dmptr->Nrow(); j++)
          (*dmptr)[j][k] /= colsum1;

      }
    }
  }
}

//File format either:
//year, step, number of migration matrix
//...
//or
//year, step, n1 ... nl
//...
//The latter being the file format if migration is age dependent.
void Migration::ReadNoMigrationMatrices(CommentStream& infile,
  const TimeClass* const TimeInfo, Keeper* const keeper) {

  ErrorHandler handle;
  int year, step, time, age;

  if (!FindContinuousYearAndStepWithNoText(infile, TimeInfo->FirstYear(), TimeInfo->FirstStep()))
    handle.Message("Failure in migration - unable to find the start of the migration data");

  for (time = 1; time <= TimeInfo->TotalNoSteps(); time++) {
    infile >> year >> step;
    if (infile.eof())
      handle.Eof();
    if (time != TimeInfo->CalcSteps(year, step))
      handle.Message("Failure in migration - failed to read migration data");

    if (AgeDepMigration)
      for (age = 0; age < ages.Nrow(); age++)
        infile >> MatrixNumbers[age][time];
    else
      infile >> MatrixNumbers[0][time];
  }
}

//File format:
//n1 v1
//.  .
//nm vm
//where the ni's are integer numbers and vi's doubles.
void Migration::ReadOptVariables(CommentStream& infile, intvector& novariables,
  const TimeClass* const TimeInfo, Keeper* const keeper) {

  //They are in two columns, first is number of variable, next is its value.
  //Have to be a bit careful, not to inform keeper of the variables in
  //ReadDoubleWithSwitches, and then change their addresses using resize.

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  while (novariables.Size() != 0)
    novariables.Delete(0);

  infile >> ws;
  char c = infile.peek();
  int i = 0;
  ifstream subfile;
  CommentStream subcomment(subfile);
  int firstyear = TimeInfo->FirstYear();
  int lastyear = TimeInfo->LastYear();
  streampos readPos;

  keeper->AddString("variable");
  while (!infile.eof() && isdigit(c)) {
    novariables.resize(1);
    infile >> novariables[i];
    readPos = infile.tellg();
    infile >> text;
    subfile.open(text);

    if (subfile.fail()) {
      Formula* number = new Formula;
      infile.seekg(readPos);
      if (!(infile >> *number))
        handle.Message(text);
      number->Inform(keeper);
      OptVar.resize(1);
      OptVariables.resize(1);
      OptVar[i] = new MigVariable(number, firstyear, lastyear, keeper);
      OptVariables[i] = OptVar[i]->ValueOfVariable(firstyear);
      delete number;
    } else {
      OptVar.resize(1);
      OptVariables.resize(1);
      OptVar[i] = new MigVariable(subcomment, firstyear, lastyear, keeper);
      OptVariables[i] = OptVar[i]->ValueOfVariable(firstyear);
      subfile.close();
      subfile.clear();
    }
    infile >> ws;
    c = infile.peek();
    i++;
  }
}

//File format: File format for VarInfo.
void Migration::ReadCoefficients(CommentStream& infile,
  const AreaClass* const Area, Keeper* const keeper) {

  //Now we read the coefficients. The operator >> for VariableInfo does almost
  //all the work. We only need to convert the areas read to inner areas.
  ErrorHandler handle;
  int i;
  infile >> ws;

  char c = infile.peek();
  const int totalnoareas = Area->NoAreas();
  while (!infile.eof() && isdigit(c)) {
    VariableInfo* var = new VariableInfo(keeper, totalnoareas);
    infile >> *var;
    if (var->error)
      handle.Message("Failure in migration - unable to read migration matrix");

    const int innerrow = Area->InnerArea(var->row);
    const int innercol = Area->InnerArea(var->column);
    if (!this->IsInArea(innerrow) || !this->IsInArea(innercol))
      handle.Message("Failure in migration - invalid migration areas");

    var->row = AreaNr[innerrow];
    var->column = AreaNr[innercol];
    OptInfo.resize(1);
    OptInfo[OptInfo.Size() - 1] = var;
    infile >> ws;
    c = infile.peek();
  }
}

void Migration::Reset() {
  penalty.Reset();
}

const doublevector& Migration::Penalty() const {
  return penalty;
}
