#include "pionstep.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "predatoraggregator.h"
#include "stock.h"
#include "gadget.h"

extern ErrorHandler handle;

PIOnStep::PIOnStep(CommentStream& infile, const IntMatrix& areas,
  const DoubleVector& predatorlengths, const DoubleVector& preylengths,
  const TimeClass* const TimeInfo, int biomass, const CharPtrVector& areaindex,
  const CharPtrVector& preylenindex, const CharPtrVector& predlenindex,
  const char* datafilename, const char* name)
  : SIOnStep(infile, datafilename, areaindex, TimeInfo, predlenindex.Size() * preylenindex.Size(), areas, predlenindex, preylenindex),
    PredatorLgrpDiv(0), PreyLgrpDiv(0), Biomass(biomass), aggregator(0) {

  PredatorLgrpDiv = new LengthGroupDivision(predatorlengths);
  if (PredatorLgrpDiv->Error())
    handle.Message("Error in predatorindex - failed to create predator length group");
  PreyLgrpDiv = new LengthGroupDivision(preylengths);
  if (PreyLgrpDiv->Error())
    handle.Message("Error in predatorindex - failed to create prey length group");

  //read the predator indices data from the datafile
  ifstream datafile;
  CommentStream subdata(datafile);
  datafile.open(datafilename, ios::in);
  handle.checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  readPredatorData(subdata, areaindex, predlenindex, preylenindex, TimeInfo, name);
  handle.Close();
  datafile.close();
  datafile.clear();
}

void PIOnStep::readPredatorData(CommentStream& infile, const CharPtrVector& areaindex,
  const CharPtrVector& predlenindex, const CharPtrVector& preylenindex,
  const TimeClass* TimeInfo, const char* name) {

  int i;
  int year, step;
  double tmpnumber;
  char tmparea[MaxStrLength], tmppred[MaxStrLength], tmpprey[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmppred, "", MaxStrLength);
  strncpy(tmpprey, "", MaxStrLength);
  int keepdata, timeid, areaid, predid, preyid, colid;
  int numprey = preylenindex.Size();
  int count = 0;

  //Check the number of columns in the inputfile
  if (countColumns(infile) != 6)
    handle.Message("Wrong number of columns in inputfile - should be 6");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> tmparea >> tmppred >> tmpprey >> tmpnumber >> ws;

    //check if the year and step are in the simulation
    timeid = -1;
    if (TimeInfo->IsWithinPeriod(year, step)) {
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;
    } else
      keepdata = 1;

    //if tmparea is in areaindex keep data, else dont keep the data
    areaid = -1;
    for (i = 0; i < areaindex.Size(); i++)
      if (strcasecmp(areaindex[i], tmparea) == 0)
        areaid = i;

    if (areaid == -1)
      keepdata = 1;

    //if tmppred is in predlenindex keep data, else dont keep the data
    predid = -1;
    for (i = 0; i < predlenindex.Size(); i++)
      if (strcasecmp(predlenindex[i], tmppred) == 0)
        predid = i;

    if (predid == -1)
      keepdata = 1;

    //if tmpprey is in preylenindex keep data, else dont keep the data
    preyid = -1;
    for (i = 0; i < preylenindex.Size(); i++)
      if (strcasecmp(preylenindex[i], tmpprey) == 0)
        preyid = i;

    if (preyid == -1)
      keepdata = 1;

    if (keepdata == 0) {
      //survey indices data is required, so store it
      count++;
      colid = preyid + (numprey * predid);
      //JMB - this should really be stored as [time][prey][pred]
      Indices[timeid][colid] = tmpnumber;
    }
  }
  if (count == 0)
    handle.LogWarning("Warning in predatorindex - found no data in the data file for", name);
  handle.LogMessage("Read predatorindex data file - number of entries", count);
}

void PIOnStep::setPredatorsAndPreys(const PredatorPtrVector& predators, const PreyPtrVector& preys) {
  aggregator = new PredatorAggregator(predators, preys, Areas, PredatorLgrpDiv, PreyLgrpDiv);
}

void PIOnStep::Sum(const TimeClass* const TimeInfo) {
  if (!(this->IsToSum(TimeInfo)))
    return;
  if (Biomass)
    aggregator->Sum();
  else
    aggregator->NumberSum();

  const BandMatrixVector* cons = &(aggregator->returnSum());
  int ncol = (*cons)[0].Nrow() * (*cons)[0].Ncol();
  DoubleVector numbers(ncol);
  int k = 0;
  int predlen, preylen;
  for (predlen = 0; predlen < (*cons)[0].Nrow(); predlen ++)
    for (preylen = 0; preylen < (*cons)[0].Ncol(); preylen++) {
      numbers[k] = (*cons)[0][predlen][preylen];
      k++;
    }
  this->KeepNumbers(numbers);
}

PIOnStep::~PIOnStep() {
  delete aggregator;
  delete PredatorLgrpDiv;
  delete PreyLgrpDiv;
}
