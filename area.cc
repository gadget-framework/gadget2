#include "areatime.h"
#include "readfunc.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

AreaClass::AreaClass(CommentStream& infile, const TimeClass* const TimeInfo) {
  int i, j;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  infile >> text >> ws;
  if (strcasecmp(text, "areas") == 0) {
    i = 0;
    while (isdigit(infile.peek()) && !infile.eof()) {
      OuterAreas.resize(1);
      infile >> OuterAreas[i] >> ws;
      i++;
    }
  } else
    handle.Unexpected("areas", text);

  int noareas = OuterAreas.Size();
  size.resize(noareas);
  infile >> text;
  if (strcasecmp(text, "size") == 0) {
    if (!readVector(infile, size))
      handle.Message("Failed to read size of areas");
  } else
    handle.Unexpected("size", text);

  infile >> text >> ws;
  if (strcasecmp(text, "temperature") != 0)
    handle.Unexpected("temperature", text);

  //Now the data which is in the following format: year step area temperature.
  temperature.AddRows(TimeInfo->TotalNoSteps() + 1, noareas, 0);
  IntVector Years, Steps;
  int timeid, areaid, keepdata, year, step, area;
  double tmpnumber;

  //Check the number of columns in the inputfile
  if (countColumns(infile) != 4)
    handle.Message("Wrong number of columns in inputfile - should be 4");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> area >> tmpnumber >> ws;

    //check if the year and step are in the simulation
    timeid = -1;
    if (TimeInfo->IsWithinPeriod(year, step)) {
      //if this is a new timestep, resize to store the data
      for (i = 0; i < Years.Size(); i++)
        for (j = 0; j < Steps.Size(); j++)
          if ((Years[i] == year) && (Steps[j] == step))
            timeid = i;

      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        timeid = (Years.Size() - 1);
      }

    } else {
      //dont keep the data
      keepdata = 1;
    }

    //if area is in OuterAreas find areaid, else dont keep the data
    areaid = -1;
    for (i = 0; i < OuterAreas.Size(); i++)
      if (area == OuterAreas[i])
        areaid = i;

    if (areaid == -1)
      keepdata = 1;

    if (keepdata == 0) {
      //temperature data is required, so store it
      temperature[timeid][areaid] = tmpnumber;
    }
  }
  handle.LogMessage("Read area file - number of areas", noareas);
}

int AreaClass::InnerArea(int area) const {
  int innerarea = -1;
  int i;
  for (i = 0; i < OuterAreas.Size(); i++)
    if (area == OuterAreas[i])
      innerarea = i;
  return innerarea;
}
