#include "formatedchatprinter.h"
#include "conversionindex.h"
#include "areatime.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "fleet.h"
#include "formatedprinting.h"
#include "runid.h"
#include "gadget.h"

extern RunID RUNID;

/*  FormatedCHatPrinter
 *
 *  Purpose:  Constructor
 *
 *  In:  CommentStream& infile       :Input file
 *       AreaClass* Area             :Area definition
 *       TimeClass* TimeInfo         :Time definition
 *
 *  Usage:  FormatedCHatPrinter(infile, Area, Time)
 *
 *  Pre:  infile, Area, & Time are valid according to StockPrinter documentation.
 */

FormatedCHatPrinter::FormatedCHatPrinter(CommentStream& infile,
  const AreaClass* const AreaInfo, const TimeClass* const TimeInfo)
  : Printer(FORMATEDCHATPRINTER), Area(AreaInfo) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;

  //read in the fleetnames
  i = 0;
  infile >> text >> ws;
  if (!(strcasecmp(text, "fleetnames") == 0))
    handle.Unexpected("fleetnames", text);
  infile >> text >> ws;
  while (!infile.eof() && !(strcasecmp(text, "areaaggfile") == 0)) {
    fleetnames.resize(1);
    fleetnames[i] = new char[strlen(text) + 1];
    strcpy(fleetnames[i++], text);
    infile >> text >> ws;
  }

  //read in area aggregation from file
  char filename[MaxStrLength];
  strncpy(filename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  infile >> filename >> ws;
  datafile.open(filename, ios::in);
  checkIfFailure(datafile, filename);
  handle.Open(filename);
  i = readAggregation(subdata, areas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Must change from outer areas to inner areas.
  for (i = 0; i < areas.Size(); i++)
    if ((areas[i] = Area->InnerArea(areas[i])) == -1)
      handle.UndefinedArea(areas[i]);

  //Open the printfile
  readWordAndValue(infile, "printfile", filename);
  outfile.open(filename, ios::out);
  checkIfFailure(outfile, filename);
  outfile << "; ";
  RUNID.print(outfile);
  outfile.flush();

  infile >> text >> ws;
  if (!(strcasecmp(text, "yearsandsteps") == 0))
    handle.Unexpected("yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
    handle.Message("Error in formatedchatprinter - wrong format for yearsandsteps");

  //prepare for next printfile component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[component]") == 0))
      handle.Unexpected("[component]", text);
  }
}

void FormatedCHatPrinter::setFleet(FleetPtrVector& fleetvec) {
  int i, j;
  int index = 0;
  for (i = 0; i < fleetvec.Size(); i++)
    for (j = 0; j < fleetnames.Size(); j++)
      if (strcasecmp(fleetvec[i]->Name(), fleetnames[j]) == 0) {
        fleets.resize(1);
        fleets[index] = fleetvec[i];
        index++;
      }
}

void FormatedCHatPrinter::Print(const TimeClass* const TimeInfo) {
  if (!AAT.AtCurrentTime(TimeInfo))
    return;
  int i, j;
  printTime(outfile, *TimeInfo);
  for (i = 0; i < areas.Size(); i++) {
    for (j = 0; j < fleets.Size(); j++) {
      ((MortPredator*)fleets[j]->returnPredator())->calcCHat(0, TimeInfo);
      printc_hat(outfile, *(MortPredator*)fleets[j]->returnPredator(), *Area);
    }
  }
}

FormatedCHatPrinter::~FormatedCHatPrinter() {
  outfile.close();
  outfile.clear();
  int i = 0;
  for (i = 0; i < fleetnames.Size(); i++)
    delete[] fleetnames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
}
