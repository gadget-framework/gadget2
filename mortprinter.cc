#include "mortprinter.h"
#include "stockaggregator.h"
#include "areatime.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "stockptrvector.h"
#include "mortprey.h"
#include "stock.h"
#include "formatedprinting.h"
#include "runid.h"
#include "gadget.h"

extern RunID RUNID;
extern ErrorHandler handle;

/*  MortPrinter
 *
 *  Purpose:  Constructor
 *
 *  In:  CommentStream& infile        :Input file
 *       AreaClass* Area              :Area defenition
 *       TimeClass* TimeInfo          :Time defenition
 *
 *  Usage:  MortPrinter(infile, Area, Time)
 *
 *  Pre:  infile, Area, & Time are valid according to StockPrinter documentation.
 */
MortPrinter::MortPrinter(CommentStream& infile,
  const AreaClass* const AreaInfo, const TimeClass* const TimeInfo)
  : Printer(MORTPRINTER), Area(AreaInfo) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;

  sumF = 0;
  sumM1 = 0;
  sumM2 = 0;
  sumN = 0;
  totalNbar = 0;

  char filename[MaxStrLength];
  strncpy(filename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  //Open the printfile
  readWordAndValue(infile, "printfile", filename);
  outfile.open(filename, ios::out);
  handle.checkIfFailure(outfile, filename);

  readWordAndVariable(infile, "printf", printf);
  readWordAndVariable(infile, "printm1", printm1);
  readWordAndVariable(infile, "printm2", printm2);
  readWordAndVariable(infile, "printn", printn);

  //read in area aggregation from file
  readWordAndValue(infile, "areaaggfile", filename);
  datafile.open(filename, ios::in);
  handle.checkIfFailure(datafile, filename);
  handle.Open(filename);
  i = readAggregation(subdata, areas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //read in the stocknames
  i = 0;
  infile >> text >> ws;
  if (!(strcasecmp(text, "stocks") == 0))
    handle.Unexpected("stocks", text);
  infile >> text >> ws;
  while (!infile.eof() && !(strcasecmp(text, "yearsandsteps") == 0)) {
    stocknames.resize(1);
    stocknames[i] = new char[strlen(text) + 1];
    strcpy(stocknames[i++], text);
    infile >> text >> ws;
  }

  if (!(strcasecmp(text, "yearsandsteps") == 0))
    handle.Unexpected("yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
    handle.Message("Error in mortprinter - wrong format for yearsandsteps");

  //JMB - changed so that firstyear is not read from file
  //but calculated from the yearsandsteps read into AAT
  firstyear = TimeInfo->FirstYear();
  nrofyears = TimeInfo->LastYear() - firstyear + 1;

  //prepare for next printfile component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[component]") == 0))
      handle.Unexpected("[component]", text);
  }

  //finished initializing. Now print first line.
  outfile << "; ";
  RUNID.print(outfile);
  outfile.flush();
}

/*  setStock
 *
 *  Purpose:  Initialise vector of stocks to be printed
 *
 *  In:  StockPtrVector& stockvec     :vector of all stocks, the stock names from the
 *                                     input files are matched with these.
 *
 *  Usage:  setStock(stockvec)
 *
 *  Pre:  stockvec.Size() > 0, all stocks names in input file are in stockvec
 */
void MortPrinter::setStock(StockPtrVector& stockvec) {
  int index = 0;
  int i, j;
  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < stocknames.Size(); j++)
      if (strcasecmp(stockvec[i]->Name(), stocknames[j]) == 0) {
        stocks.resize(1);
        stocks[index++] = stockvec[i];
      }

  if (stocks.Size() != stocknames.Size()) {
    handle.logWarning("Error in mortprinter - failed to match stocks");
    for (i = 0; i < stocks.Size(); i++)
      handle.logWarning("Error in mortprinter - found stock", stocks[i]->Name());
    for (i = 0; i < stocknames.Size(); i++)
      handle.logWarning("Error in mortprinter - looking for stock", stocknames[i]);
    exit(EXIT_FAILURE);
  }
  minage = stocks[0]->minAge();
  maxage = stocks[0]->maxAge();
  for (i = 0; i < stocks.Size(); i++) {
    if (stocks[i]->minAge() < minage)
      minage = stocks[i]->minAge();
    if (stocks[i]->maxAge() > maxage)
      maxage = stocks[i]->maxAge();
  }

  if (sumF != 0)
    delete sumF;
  if (sumM1 != 0)
    delete sumM1;
  if (sumM2 != 0)
    delete sumM2;
  if (sumN != 0)
    delete sumN;
  if (totalNbar != 0)
    delete totalNbar;
  sumF = new DoubleMatrix(maxage - minage + 1, nrofyears, 0.0);
  sumM1 = new DoubleMatrix(maxage - minage + 1, nrofyears, 0.0);
  sumM2 = new DoubleMatrix(maxage - minage + 1, nrofyears, 0.0);
  sumN = new DoubleMatrix(maxage - minage + 1, nrofyears, 0.0);
  totalNbar = new DoubleMatrix(maxage - minage + 1, nrofyears, 0.0);
}

/*  Print
 *
 *  Purpose:  Print stocks according to configuration in constructor.
 *
 *  In:  TimeClass& TimeInfo         :Current time
 *
 *  Usage:  Print(TimeInfo)
 *
 *  Pre:  setStock has been called
 */
void MortPrinter::Print(const TimeClass* const TimeInfo) {
  int s, i, j, area, inarea, age, yo;
  for (area = 0; area < areas.Size(); area++)
    for (i = 0; i < stocks.Size(); i++)
      if (stocks[i]->Type() == LENSTOCKTYPE)
        ((LenStock*)stocks[i])->calcForPrinting(Area->InnerArea(areas[area]), *TimeInfo);

  if (TimeInfo->CurrentTime() == TimeInfo->TotalNoSteps())
    for (area = 0; area < areas.Size(); area++) {
      sumF->setElementsTo(0.0);
      sumM1->setElementsTo(0.0);
      sumM2->setElementsTo(0.0);
      sumN->setElementsTo(0.0);
      totalNbar->setElementsTo(0.0);
      inarea = Area->InnerArea(areas[area]);
      yo = firstyear - TimeInfo->FirstYear();
      for (s = 0; s < stocks.Size(); s++) {
        for (i = stocks[s]->minAge(); i <= stocks[s]->maxAge(); i++) {
          const DoubleMatrix& F = ((LenStock*)stocks[s])->getF(inarea);
          const DoubleMatrix& M1 = ((LenStock*)stocks[s])->getM1(inarea);
          const DoubleMatrix& M2 = ((LenStock*)stocks[s])->getM2(inarea);
          const DoubleMatrix& Nbar = ((LenStock*)stocks[s])->getNbar(inarea);
          const DoubleMatrix& N = ((LenStock*)stocks[s])->getNsum(inarea);
          age = i - stocks[s]->minAge();
          for (j = 0; j < nrofyears; j++) {
            (*sumF)[i - minage][j] += F[age][j + yo] * Nbar[age][j + yo];
            (*sumM1)[i - minage][j] += M1[age][j + yo] * Nbar[age][j + yo];
            (*sumM2)[i - minage][j] += M2[age][j + yo] * Nbar[age][j + yo];
            (*sumN)[i - minage][j] += N[age][j + yo];
            (*totalNbar)[i - minage][j] += Nbar[age][j + yo];
          }
        }
      }
      for (i = 0; i < totalNbar->Nrow(); i++) {
        for (j = 0; j < totalNbar->Ncol(i); j++) {
          (*sumF)[i][j] /= (*totalNbar)[i][j];
          (*sumM1)[i][j] /= (*totalNbar)[i][j];
          (*sumM2)[i][j] /= (*totalNbar)[i][j];
        }
      }
      outfile << "stocks";
      for (s = 0; s < stocks.Size(); s++)
        outfile << sep << stocks[s]->Name();
      outfile << "\nareas "<< area << "\n\n";
      if (printf) {
        outfile << "Total fishing mortality at age\n";
        printFbyAge(outfile, *sumF, minage, firstyear, 2);
      }
      if (printm1) {
        outfile << "mortality M1\n";
        printM1byAge(outfile, *sumM1, minage, firstyear, 2);
      }
      if (printm2) {
        outfile << "mortality M2\n";
        printM2byAge(outfile, *sumM2, minage, firstyear, 2);
      }
      if (printn) {
        outfile << "stock numbers at age (in thousands) by jan. 1\n";
        printNbyAge(outfile, *sumN, minage, firstyear, 2);
      }
      outfile.flush();
    }
}

/*  ~MortPrinter
 *
 *  Purpose:  Destructor
 */
MortPrinter::~MortPrinter() {
  if (sumF != 0)
    delete sumF;
  if (sumM1 != 0)
    delete sumM1;
  if (sumM2 != 0)
    delete sumM2;
  if (sumN != 0)
    delete sumN;
  if (totalNbar != 0)
    delete totalNbar;

  outfile.close();
  outfile.clear();
  int i;
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
}
