#include "stock.h"
#include "predpreystdprinter.h"
#include "errorhandler.h"
#include "stockpredator.h"
#include "prey.h"
#include "stockprey.h"
#include "readword.h"
#include "readaggregation.h"
#include "gadget.h"

PredPreyStdPrinter::PredPreyStdPrinter(CommentStream& infile,
  const AreaClass* const Area, const TimeClass* const TimeInfo)
  : Printer(PREDPREYSTDPRINTER), predname(0), preyname(0) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;

  predname = new char[MaxStrLength];
  strncpy(predname, "", MaxStrLength);
  readWordAndValue(infile, "predator", predname);

  preyname = new char[MaxStrLength];
  strncpy(preyname, "", MaxStrLength);
  readWordAndValue(infile, "prey", preyname);

  //Read in area aggregation from file
  char filename[MaxStrLength];
  strncpy(filename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  CharPtrVector areaindex;
  IntMatrix tmpareas;
  readWordAndValue(infile, "areaaggfile", filename);
  datafile.open(filename, ios::in);
  checkIfFailure(datafile, filename);
  handle.Open(filename);
  i = readAggregation(subdata, tmpareas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Check if we read correct input
  if (tmpareas.Nrow() != 1)
    handle.Message("Error - there should be only one aggregated area for predpreystdprinter");

  for (i = 0; i < tmpareas.Ncol(0); i++)
    outerareas.resize(1, tmpareas[0][i]);

  //Must change from outer areas to inner areas.
  areas.resize(outerareas.Size());
  for (i = 0; i < areas.Size(); i++)
    if ((areas[i] = Area->InnerArea(outerareas[i])) == -1)
      handle.UndefinedArea(outerareas[i]);

  //Open the printfile
  readWordAndValue(infile, "printfile", filename);
  outfile.open(filename, ios::out);
  checkIfFailure(outfile, filename);

  infile >> text >> ws;
  if (!(strcasecmp(text, "yearsandsteps") == 0))
    handle.Unexpected("yearsandsteps", text);
  if (!AAT.ReadFromFile(infile, TimeInfo))
    handle.Message("Error in predpreystdprinter - wrong format for yearsandsteps");

  //prepare for next printfile component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[component]") == 0))
      handle.Unexpected("[component]", text);
  }

  //areaindex is not required - free up memory
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
}

PredPreyStdPrinter::~PredPreyStdPrinter() {
  outfile.close();
  outfile.clear();
  delete[] predname;
  delete[] preyname;
}

void PredPreyStdPrinter::SetStocksAndPredAndPrey(const StockPtrVector& stockvec,
  const PopPredatorPtrVector& predvec, const PreyPtrVector& preyvec) {

  //First we try comparing the StockPredators and StockPreys to predname and preyname.
  int stockpred = 0;
  int stockprey = 0;
  const PopPredator* predator = 0;
  const Prey* prey = 0;
  int i;

  for (i = 0; i < stockvec.Size(); i++) {
    if (stockvec[i]->doesEat()) {
      if (strcasecmp(stockvec[i]->ReturnPredator()->Name(), predname) == 0) {
        if (predator) {
          cerr << "Error - found more than one predator with the name " << predname << endl;
          exit(EXIT_FAILURE);
        }
        stockpred = 1;
        predator = stockvec[i]->ReturnPredator();
      }
    }
    if (stockvec[i]->IsEaten()) {
      if (strcasecmp(stockvec[i]->ReturnPrey()->Name(), preyname) == 0) {
        if (prey) {
          cerr << "Error - found more than one prey with the name " << preyname << endl;
          exit(EXIT_FAILURE);
        }
        stockprey = 1;
        prey = stockvec[i]->ReturnPrey();
      }
    }
  }

  //And now check the PopPredators.
  for (i = 0; i < predvec.Size(); i++) {
    if (strcasecmp(predvec[i]->Name(), predname) == 0) {
      if (predvec[i] != predator && predator) {
        cerr << "Error - found more than one predator with the name " << predname << endl;
        exit(EXIT_FAILURE);
      }
      predator = predvec[i];
    }
  }

  //And now check the Preys.
  for (i = 0; i < preyvec.Size(); i++) {
    if (strcasecmp(preyvec[i]->Name(), preyname) == 0) {
      if (preyvec[i] != prey && prey) {
        cerr << "Error - found more than one prey with the name " << preyname << endl;
        exit(EXIT_FAILURE);
      }
      prey = preyvec[i];
    }
  }

  if (prey == 0)
    cerr << "Error - found no prey with the name " << preyname << endl;
  if (predator == 0)
    cerr << "Error - found no predator with the name " << predname << endl;
  if (prey == 0 || predator == 0)
    exit(EXIT_FAILURE);

  for (i = 0; i < areas.Size(); i++)
    if (!prey->IsInArea(areas[i])) {
      cerr << "Error - prey " << preyname << " is not defined on area " << i << endl;
      exit(EXIT_FAILURE);
    }

  for (i = 0; i < areas.Size(); i++)
    if (!predator->IsInArea(areas[i])) {
      cerr << "Error - predator " << predname << " is not defined on area " << i << endl;
      exit(EXIT_FAILURE);
    }

  //If we get here, we have found exactly one predator and one prey defined on all the areas
  //so we can call the virtual function SetPredAndPrey to set the predators and preys
  this->SetPopPredAndPrey(predator, prey, stockpred, stockprey);
}
