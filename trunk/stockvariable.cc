#include "stockvariable.h"
#include "errorhandler.h"
#include "ecosystem.h"
#include "readword.h"
#include "gadget.h"
#include "global.h"

extern Ecosystem* EcoSystem;


void StockVariable::read(CommentStream& infile) {

  int i;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  infile >> ws;
  char c = infile.peek();
  biomass = 1;
  if ((c == 'b') || (c == 'B'))
    readWordAndVariable(infile, "biomass", biomass);
  if (biomass != 0 && biomass != 1)
    handle.logFileMessage(LOGFAIL, "\nError in stockvariable - biomass must be 0 or 1");

  i = 0;
  infile >> ws;
  while (!infile.eof()) {
    infile >> text >> ws;
    stocknames.resize(new char[strlen(text) + 1]);
    strcpy(stocknames[i++], text);
  }
  if (stocknames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "\nError in stockvariable - failed to read stocks");

  handle.logMessage(LOGMESSAGE, "Read stockvariable data - number of stocks", stocknames.Size());
}

StockVariable::~StockVariable() {
  int i;
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
}

void StockVariable::Update() {
  int i,j;

  //JMB need to find the stocks - only need to do this once
  if (stocks.Size() == 0) {

    StockPtrVector stockvec;
    stockvec = EcoSystem->getModelStockVector();

    for (i = 0; i < stockvec.Size(); i++)
      for (j = 0; j < stocknames.Size(); j++)
        if (strcasecmp(stockvec[i]->getName(), stocknames[j]) == 0)
          stocks.resize(stockvec[i]);

    if (stocks.Size() != stocknames.Size()) {
      handle.logMessage(LOGWARN, "Error in stockvariable - failed to match stocks");
      for (i = 0; i < stocks.Size(); i++)
        handle.logMessage(LOGWARN, "Error in stockvariable - found stock", stocks[i]->getName());
      for (i = 0; i < stocknames.Size(); i++)
        handle.logMessage(LOGWARN, "Error in stockvariable - looking for stock", stocknames[i]);
      handle.logMessage(LOGFAIL, ""); //JMB this will exit gadget
    }
  }

  value = 0.0;
  for (i = 0; i < stocks.Size(); i++) {
    if (biomass == 1)
      value += stocks[i]->getTotalStockBiomassAllAreas();
    else
      value += stocks[i]->getTotalStockNumberAllAreas();
  }
}

void StockVariable::Delete() const {
  int i;
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
}

void StockVariable::Interchange(StockVariable& newSV) const {
  newSV.value = value;
  newSV.biomass = biomass;
  newSV.stocks = stocks;
  newSV.stocknames = stocknames;
}
