#include "formatedpreyprinter.h"
#include "conversionindex.h"
#include "areatime.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "mortprey.h"
#include "stock.h"
#include "formatedprinting.h"
#include "runid.h"
#include "gadget.h"

extern RunID RUNID;
extern ErrorHandler handle;

/*  FormatedPreyPrinter
 *
 *  Purpose:  Constructor
 *
 *  In:  CommentStream& infile      :Input file
 *       AreaClass* Area            :Area defenition
 *       TimeClass* TimeInfo        :Time defenition
 *
 *  Usage:  FormatedPreyPrinter(infile, Area, Time)
 *
 *  Pre:  infile, Area, & Time are valid according to StockPrinter documentation.
 */

FormatedPreyPrinter::FormatedPreyPrinter(CommentStream& infile,
  const AreaClass* const AreaInfo, const TimeClass* const TimeInfo)
  : Printer(FORMATEDPREYPRINTER), Area(AreaInfo) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;

  //read in the stocknames
  i = 0;
  infile >> text >> ws;
  if (!(strcasecmp(text, "stocknames") == 0))
    handle.Unexpected("stocknames", text);
  infile >> text >> ws;
  while (!infile.eof() && !(strcasecmp(text, "areaaggfile") == 0)) {
    stocknames.resize(1);
    stocknames[i] = new char[strlen(text) + 1];
    strcpy(stocknames[i++], text);
    infile >> text >> ws;
  }

  //read in area aggregation from file
  char filename[MaxStrLength];
  strncpy(filename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  infile >> filename >> ws;
  datafile.open(filename, ios::in);
  handle.checkIfFailure(datafile, filename);
  handle.Open(filename);
  i = readAggregation(subdata, areas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Must change from outer areas to inner areas.
  for (i = 0; i < areas.Size(); i++)
    if ((areas[i] = Area->InnerArea(areas[i])) == -1)
      handle.UndefinedArea(areas[i]);

  i = 0;
  printzp = 0;
  printnp = 0;
  printcp = 0;
  infile >> text >> ws;
  while ((strcasecmp(text, "yearsandsteps") != 0) && !infile.eof()) {
    if (strcasecmp(text, "zprintfile") == 0) {
      infile >> filename >> ws;
      zoutfile.open(filename, ios::out);
      handle.checkIfFailure(zoutfile, filename);
      printzp = 1;
      i++;
      zoutfile << "; ";
      RUNID.print(zoutfile);
    } else if (strcasecmp(text, "nprintfile") == 0) {
      infile >> filename >> ws;
      noutfile.open(filename, ios::out);
      handle.checkIfFailure(noutfile, filename);
      printnp = 1;
      i++;
      noutfile << "; ";
      RUNID.print(noutfile);
    } else if (strcasecmp(text, "cprintfile") == 0) {
      infile >> filename >> ws;
      coutfile.open(filename, ios::out);
      handle.checkIfFailure(coutfile, filename);
      printcp = 1;
      i++;
      coutfile << "; ";
      RUNID.print(coutfile);
    }
    infile >> text >> ws;
  }

  if (i == 0)
    handle.Message("No printfiles read in for formatedpreyprinter");

  if (strcasecmp(text, "printatstart") == 0)
    infile >> printtimeid >> ws >> text >> ws;
  else
    printtimeid = 0;

  if (printtimeid != 0 && printtimeid != 1)
    handle.Message("Error in formatedpreyprinter - invalid value of printatend");

  if (!(strcasecmp(text, "yearsandsteps") == 0))
    handle.Unexpected("yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
    handle.Message("Error in formatedpreyprinter - wrong format for yearsandsteps");

  //prepare for next printfile component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[component]") == 0))
      handle.Unexpected("[component]", text);
  }
}

void FormatedPreyPrinter::setStock(StockPtrVector& stockvec) {
  int i, j;
  int index = 0;
  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < stocknames.Size(); j++)
      if (strcasecmp(stockvec[i]->Name(), stocknames[j]) == 0) {
        stocks.resize(1);
        stocks[index++] = stockvec[i];
      }
  if (stocks.Size() != stocknames.Size()) {
    handle.logWarning("Error in formatedpreyprinter - failed to match stocks");
    for (i = 0; i < stocks.Size(); i++)
      handle.logWarning("Error in formatedpreyprinter - found stock", stocks[i]->Name());
    for (i = 0; i < stocknames.Size(); i++)
      handle.logWarning("Error in formatedpreyprinter - looking for stock", stocknames[i]);
    exit(EXIT_FAILURE);
  }
}

void FormatedPreyPrinter::Print(const TimeClass* const TimeInfo, int printtime) {
  if ((!AAT.AtCurrentTime(TimeInfo)) || (printtime != printtimeid))
    return;

  if (printzp)
    printTime(zoutfile, *TimeInfo);
  if (printnp)
    printTime(noutfile, *TimeInfo);
  if (printcp)
    printTime(coutfile, *TimeInfo);

  IntVector ages;
  int i;
  for (i = 0; i<stocks.Size(); i++)
    if (stocks[i]->isEaten()) {
      if (printzp)
        printz(zoutfile, *(MortPrey*)stocks[i]->returnPrey(), *Area);
      if (printnp)
        printmean_n(noutfile, *(MortPrey*)stocks[i]->returnPrey(), *Area);
      if (printcp)
        printcannibalism(coutfile, *(MortPrey*)stocks[i]->returnPrey(), *Area);
    }

  if (printzp)
    zoutfile.flush();
  if (printnp)
    noutfile.flush();
  if (printcp)
    coutfile.flush();
}

FormatedPreyPrinter::~FormatedPreyPrinter() {
  if (printzp) {
    zoutfile.close();
    zoutfile.clear();
  }
  if (printnp) {
    noutfile.close();
    coutfile.clear();
  }
  if (printcp) {
    coutfile.close();
    coutfile.clear();
  }

  int i;
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
}
