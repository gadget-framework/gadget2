#include "stock.h"
#include "predpreystdprinter.h"
#include "errorhandler.h"
#include "stockpredator.h"
#include "prey.h"
#include "stockprey.h"
#include "readword.h"
#include "readaggregation.h"
#include "gadget.h"

extern ErrorHandler handle;

PredPreyStdPrinter::PredPreyStdPrinter(CommentStream& infile, const TimeClass* const TimeInfo)
  : Printer(PREDPREYSTDPRINTER), predname(0), preyname(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  predname = new char[MaxStrLength];
  strncpy(predname, "", MaxStrLength);
  readWordAndValue(infile, "predator", predname);

  preyname = new char[MaxStrLength];
  strncpy(preyname, "", MaxStrLength);
  readWordAndValue(infile, "prey", preyname);

  //JMB - removed the need to read in the area aggregation file
  infile >> text >> ws;
  if (strcasecmp(text, "areaaggfile") == 0) {
    infile >> text >> ws;
    handle.logMessage(LOGWARN, "Warning in predpreystdprinter - area aggreagtion file ignored");
    infile >> text >> ws;
  }

  //open the printfile
  filename = new char[MaxStrLength];
  strncpy(filename, "", MaxStrLength);
  //readWordAndValue(infile, "printfile", filename);
  if (strcasecmp(text, "printfile") == 0)
    infile >> filename >> ws >> text >> ws;
  else
    handle.logFileUnexpected(LOGFAIL, "printfile", text);

  outfile.open(filename, ios::out);
  handle.checkIfFailure(outfile, filename);

  //infile >> text >> ws;
  if (strcasecmp(text, "precision") == 0) {
    infile >> precision >> ws >> text >> ws;
    width = precision + 4;
  } else {
    // use default values
    precision = printprecision;
    width = printwidth;
  }

  if (precision < 0)
    handle.logFileMessage(LOGFAIL, "\nError in predpreystdprinter - invalid value of precision");

  if (strcasecmp(text, "printatstart") == 0)
    infile >> printtimeid >> ws >> text >> ws;
  else
    printtimeid = 0;

  if (printtimeid != 0 && printtimeid != 1)
    handle.logFileMessage(LOGFAIL, "\nError in predpreystdprinter - invalid value of printatstart");

  if (!(strcasecmp(text, "yearsandsteps") == 0))
    handle.logFileUnexpected(LOGFAIL, "yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
    handle.logFileMessage(LOGFAIL, "\nError in predpreystdprinter - wrong format for yearsandsteps");

  //prepare for next printfile component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[component]") == 0))
      handle.logFileUnexpected(LOGFAIL, "[component]", text);
  }
}

PredPreyStdPrinter::~PredPreyStdPrinter() {
  outfile.close();
  outfile.clear();
  delete[] predname;
  delete[] preyname;
}

void PredPreyStdPrinter::setStocksAndPredAndPrey(const StockPtrVector& stockvec,
  const PredatorPtrVector& predvec, const PreyPtrVector& preyvec, const AreaClass* const Area) {

  //First we try comparing the StockPredators and StockPreys to predname and preyname.
  int stockpred = 0;
  int stockprey = 0;
  const PopPredator* predator = 0;
  const Prey* prey = 0;
  int i, j;

  for (i = 0; i < stockvec.Size(); i++) {
    if (stockvec[i]->doesEat()) {
      if (strcasecmp(stockvec[i]->getPredator()->getName(), predname) == 0) {
        if (predator)
          handle.logMessage(LOGFAIL, "Error in predpreystdprinter - repeated predator", predname);

        stockpred = 1;
        predator = stockvec[i]->getPredator();

        if (areas.Size() == 0) {
          areas = stockvec[i]->getAreas();
          outerareas.resize(areas.Size(), 0);
          for (j = 0; j < outerareas.Size(); j++)
            outerareas[j] = Area->OuterArea(areas[j]);
        }
      }
    }
    if (stockvec[i]->isEaten()) {
      if (strcasecmp(stockvec[i]->getPrey()->getName(), preyname) == 0) {
        if (prey)
          handle.logMessage(LOGFAIL, "Error in predpreystdprinter - repeated prey", preyname);

        stockprey = 1;
        prey = stockvec[i]->getPrey();

        if (areas.Size() == 0) {
          areas = stockvec[i]->getAreas();
          outerareas.resize(areas.Size(), 0);
          for (j = 0; j < outerareas.Size(); j++)
            outerareas[j] = Area->OuterArea(areas[j]);
        }
      }
    }
  }

  //And now check the predators.
  for (i = 0; i < predvec.Size(); i++) {
    if (strcasecmp(predvec[i]->getName(), predname) == 0) {
      if (((PopPredator*)predvec[i] != predator) && (predator))
        handle.logMessage(LOGFAIL, "Error in predpreystdprinter - repeated predator", predname);

      predator = (PopPredator*)predvec[i];
    }
  }

  //And now check the preys.
  for (i = 0; i < preyvec.Size(); i++) {
    if (strcasecmp(preyvec[i]->getName(), preyname) == 0) {
      if (preyvec[i] != prey && prey)
        handle.logMessage(LOGFAIL, "Error in predpreystdprinter - repeated prey", preyname);

      prey = preyvec[i];
    }
  }

  if (prey == 0)
    handle.logMessage(LOGFAIL, "Error in predpreystdprinter - failed to match prey", preyname);
  if (predator == 0)
    handle.logMessage(LOGFAIL, "Error in predpreystdprinter - failed to match predator", predname);
  if (areas.Size() == 0)
    handle.logMessage(LOGFAIL, "Error in predpreystdprinter - failed to match areas");

  for (i = 0; i < areas.Size(); i++)
    if (!prey->isInArea(areas[i]))
      handle.logMessage(LOGFAIL, "Error in predpreystdprinter - prey isnt defined on all areas");

  for (i = 0; i < areas.Size(); i++)
    if (!predator->isInArea(areas[i]))
      handle.logMessage(LOGFAIL, "Error in predpreystdprinter - predator isnt defined on all areas");

  //If we get here, we have found exactly one predator and one prey defined on all the areas
  //so we can call the virtual function setPredAndPrey to set the predators and preys
  this->setPopPredAndPrey(predator, prey, stockpred, stockprey);
}
