#include "readaggregation.h"
#include "readfunc.h"
#include "errorhandler.h"
#include "gadget.h"

int ReadAggregation(CommentStream& infile, intmatrix& agg, charptrvector& aggindex) {

  int i = 0;
  infile >> ws;
  while (!infile.eof()) {
    aggindex.resize(1);
    aggindex[i] = new char[MaxStrLength];
    strncpy(aggindex[i], "", MaxStrLength);
    infile >> aggindex[i];
    agg.AddRows(1, 0);
    ReadVectorInLine(infile, agg[i]);
    infile >> ws;
    i++;
  }
  //return the number of aggregation groups read
  return(aggindex.Size());
}

int ReadAggregation(CommentStream& infile, intvector& agg, charptrvector& aggindex) {

  int i = 0;
  int tmp = 0;
  infile >> ws;
  while (!infile.eof()) {
    aggindex.resize(1);
    aggindex[i] = new char[MaxStrLength];
    strncpy(aggindex[i], "", MaxStrLength);
    infile >> aggindex[i] >> tmp >> ws;
    agg.resize(1, tmp);
    i++;
  }
  //return the number of aggregation groups read
  return(aggindex.Size());
}

int ReadLengthAggregation(CommentStream& infile, doublevector& lengths, charptrvector& lenindex) {

  ErrorHandler handle;
  int i = 0;
  double dblA, dblB;
  infile >> ws;
  while (!infile.eof()) {
    lenindex.resize(1);
    lenindex[i] = new char[MaxStrLength];
    strncpy(lenindex[i], "", MaxStrLength);
    infile >> lenindex[i] >> dblA >> dblB >> ws;

    if (i == 0)
      //first length entry to be read
      lengths.resize(1, dblA);

    if (lengths[i] == dblA)
      //add next length entry
      lengths.resize(1, dblB);
    else
      //lengths are not consecutive - throw error
      handle.Message("Error in length aggregation - lengths not consecutive");

    i++;
  }

  //return the number of length groups read
  return(lenindex.Size());
}

int ReadPreyAggregation(CommentStream& infile, charptrmatrix& preynames,
  doublematrix& preylengths, Formulamatrix& digestioncoeff,
  charptrvector& preyindex, Keeper* const keeper) {

  int i = 0, j = 0;
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
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
      handle.Unexpected("lengths", text);

    //JMB - changed so that only 2 lengths are read in
    preylengths.AddRows(1, 0);
    preylengths[i].resize(2);
    infile >> preylengths[i][0] >> preylengths[i][1] >> text >> ws;

    if (!(strcasecmp(text, "digestioncoefficients") == 0))
      handle.Unexpected("digestioncoefficients", text);

    //JMB - changed so that only 3 elements are read in - check this
    digestioncoeff.AddRows(1, 3);
    infile >> digestioncoeff[i];
    digestioncoeff[i].Inform(keeper);

    infile >> ws;
    i++;
  }
  //return the number of prey aggregation groups read
  return(preyindex.Size());
}
