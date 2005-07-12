#include "readaggregation.h"
#include "readfunc.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

int readAggregation(CommentStream& infile, IntMatrix& agg, CharPtrVector& aggindex) {
  int i, j;
  i = 0;
  infile >> ws;
  while (!infile.eof()) {
    aggindex.resize(1);
    aggindex[i] = new char[MaxStrLength];
    strncpy(aggindex[i], "", MaxStrLength);
    infile >> aggindex[i];
    agg.AddRows(1, 0);
    if (!readVectorInLine(infile, agg[i]))
      handle.logFileMessage(LOGFAIL, "Error in readaggregation - failed to read vector");
    infile >> ws;
    i++;
  }

  //check that the labels are unique
  for (i = 0; i < aggindex.Size(); i++)
    for (j = 0; j < aggindex.Size(); j++)
      if ((strcasecmp(aggindex[i], aggindex[j]) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in aggregation file - repeated label", aggindex[i]);

  handle.logMessage(LOGMESSAGE, "Read aggregation file - number of entries", aggindex.Size());
  return aggindex.Size();
}

int readAggregation(CommentStream& infile, IntVector& agg, CharPtrVector& aggindex) {
  int i, j, tmp;
  i = tmp = 0;
  infile >> ws;
  while (!infile.eof()) {
    aggindex.resize(1);
    aggindex[i] = new char[MaxStrLength];
    strncpy(aggindex[i], "", MaxStrLength);
    infile >> aggindex[i] >> tmp >> ws;
    agg.resize(1, tmp);
    i++;
  }

  //check that the labels are unique
  for (i = 0; i < aggindex.Size(); i++)
    for (j = 0; j < aggindex.Size(); j++)
      if ((strcasecmp(aggindex[i], aggindex[j]) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in aggregation file - repeated label", aggindex[i]);

  handle.logMessage(LOGMESSAGE, "Read aggregation file - number of entries", aggindex.Size());
  return aggindex.Size();
}

int readLengthAggregation(CommentStream& infile, DoubleVector& lengths, CharPtrVector& lenindex) {
  int i, j;
  double dblA, dblB;
  i = 0;
  infile >> ws;
  while (!infile.eof()) {
    lenindex.resize(1);
    lenindex[i] = new char[MaxStrLength];
    strncpy(lenindex[i], "", MaxStrLength);
    infile >> lenindex[i] >> dblA >> dblB >> ws;

    if (i == 0)
      lengths.resize(1, dblA); //first length entry to be read

    if (isZero(lengths[i] - dblA))
      lengths.resize(1, dblB); //add next length entry
    else
      handle.logFileMessage(LOGFAIL, "Error in length aggregation - lengths not consecutive");

    i++;
  }

  //check that the labels are unique
  for (i = 0; i < lenindex.Size(); i++)
    for (j = 0; j < lenindex.Size(); j++)
      if ((strcasecmp(lenindex[i], lenindex[j]) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in length aggregation file - repeated label", lenindex[i]);

  handle.logMessage(LOGMESSAGE, "Read length aggregation file - number of entries", lenindex.Size());
  return lenindex.Size();
}

int readPreyAggregation(CommentStream& infile, CharPtrMatrix& preynames,
  DoubleMatrix& preylengths, FormulaMatrix& digestioncoeff,
  CharPtrVector& preyindex, Keeper* const keeper) {

  int i, j;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  i = j = 0;
  infile >> ws;
  while (!infile.eof()) {
    preyindex.resize(1);
    preyindex[i] = new char[MaxStrLength];
    strncpy(preyindex[i], "", MaxStrLength);
    infile >> preyindex[i];

    j = 0;
    preynames.AddRows(1, 0);
    infile >> text >> ws;
    while (!infile.eof() && (!(strcasecmp(text, "lengths") == 0))) {
      preynames[i].resize(1);
      preynames[i][j] = new char[strlen(text) + 1];
      strcpy(preynames[i][j++], text);
      infile >> text >> ws;
    }

    if (!(strcasecmp(text, "lengths") == 0))
      handle.logFileUnexpected(LOGFAIL, "lengths", text);

    //JMB - changed so that only 2 lengths are read in
    preylengths.AddRows(1, 2);
    infile >> preylengths[i][0] >> preylengths[i][1] >> text >> ws;

    if (!(strcasecmp(text, "digestioncoefficients") == 0))
      handle.logFileUnexpected(LOGFAIL, "digestioncoefficients", text);

    //JMB - changed so that only 3 elements are read in
    digestioncoeff.AddRows(1, 3);
    infile >> digestioncoeff[i];
    digestioncoeff[i].Inform(keeper);

    infile >> ws;
    i++;
  }

  //check that the labels are unique
  for (i = 0; i < preyindex.Size(); i++)
    for (j = 0; j < preyindex.Size(); j++)
      if ((strcasecmp(preyindex[i], preyindex[j]) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in prey aggregation file - repeated label", preyindex[i]);

  handle.logMessage(LOGMESSAGE, "Read prey aggregation file - number of entries", preyindex.Size());
  return preyindex.Size();
}
