#include "biomassprinter.h"
#include "conversionindex.h"
#include "stockaggregator.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "stock.h"
#include "lenstock.h"
#include "formatedprinting.h"
#include "gadget.h"
#include "runid.h"

extern RunID RUNID;
extern ErrorHandler handle;

/*  BiomassPrinter
 *
 *  Purpose: Constructor
 *
 *  In: CommentStream& infile      :Input file
 *      AreaClass* Area            :Area definition
 *      TimeClass* TimeInfo        :Time definition
 *
 *  Usage: BiomassPrinter(infile, Area, Time)
 *
 *  Pre: infile, Area, & Time are valid according to StockPrinter documentation
 */

BiomassPrinter::BiomassPrinter(CommentStream& infile, const AreaClass* const AreaInfo,
  const TimeClass* const TimeInfo) : Printer(BIOMASSPRINTER), Area(AreaInfo) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;

  printtimeid = 1; //not used
  //read in the immatrure stock name
  immname = new char[MaxStrLength];
  strncpy(immname, "", MaxStrLength);
  readWordAndValue(infile, "immature", immname);

  //read in the mature stock names
  i = 0;
  infile >> text >> ws;
  if (!(strcasecmp(text, "mature") == 0))
    handle.Unexpected("mature", text);
  infile >> text >> ws;
  while (!infile.eof() && !(strcasecmp(text, "areaaggfile") == 0)) {
    matnames.resize(1);
    matnames[i] = new char[strlen(text) + 1];
    strcpy(matnames[i++], text);
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

  for (i = 0; i < areas.Size(); i++)
    if ((areas[i] = Area->InnerArea(areas[i])) == -1)
      handle.UndefinedArea(areas[i]);

  //Open the printfile
  readWordAndValue(infile, "printfile", filename);
  printfile.open(filename, ios::out);
  handle.checkIfFailure(printfile, filename);
  printfile << "; ";
  RUNID.print(printfile);

  //prepare for next printfile component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[component]") == 0))
      handle.Unexpected("[component]", text);
  }

  nrofyears = TimeInfo->LastYear() - TimeInfo->FirstYear() + 1;
  firstyear = TimeInfo->FirstYear();
}

/*  setStock
 *
 *  Purpose: Initialise vector of stocks to be printed
 *
 *  In: StockPtrVector& stockvec     :vector of all stocks, the stock
 *                                    names from the input files are
 *                                    matched with these.
 *
 *  Usage: setStock(stockvec)
 *
 *  Pre: stockvec.Size() > 0, all stocks names in input file are in stockvec
 */

void BiomassPrinter::setStock(StockPtrVector& stockvec) {
  int index = 0;
  immindex = -1;
  int i, j;
  for (i = 0; i < stockvec.Size(); i++) {
    if (strcasecmp(stockvec[i]->Name(), immname) == 0) {
      immindex = i;
      stocks.resize(1);
      stocks[index++] = stockvec[i];
    } else
      for (j = 0; j < matnames.Size(); j++)
        if (strcasecmp(stockvec[i]->Name(), matnames[j]) == 0) {
          stocks.resize(1);
          stocks[index++] = stockvec[i];
    }
  }

  if (immindex == -1)
    handle.logFailure("Error in biomassprinter - no immature stock");

  if (stocks.Size() != matnames.Size() + 1) {
    handle.logWarning("Error in biomassprinter - failed to match mature stocks");
    for (i = 0; i < stocks.Size(); i++)
      handle.logWarning("Error in biomassprinter - found stock", stocks[i]->Name());
    for (i = 0; i < matnames.Size(); i++)
      handle.logWarning("Error in biomassprinter - looking for stock", matnames[i]);
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
}

/*  Print
 *
 *  Purpose: Print stocks according to configuration in constructor.
 *
 *  In: TimeClass& TimeInfo          :Current time
 *
 *  Usage: Print(TimeInfo)
 *
 *  Pre: setStock has been called
 */

void BiomassPrinter::Print(const TimeClass* const TimeInfo, int printtime) {

  if (printtime == 0)
    return;

  int i, a, y, area, minagem, maxagem;
  if (TimeInfo->CurrentTime() == TimeInfo->TotalNoSteps()) {
    printfile << "Stock biomass at 1/1 in tons\n";

    minagem = minage;
    maxagem = maxage;
    if (stocks[immindex]->maxAge() == maxage) {
      maxagem = minage;
      for (i = 0; i < stocks.Size(); i++)
        if (i != immindex && stocks[i]->maxAge() > maxagem)
          maxagem = stocks[i]->maxAge();
    }
    if (stocks[immindex]->minAge() == minage) {
      minagem = maxage;
      for (i = 0; i < stocks.Size(); i++)
        if (i != immindex && stocks[i]->minAge() < minagem)
          minagem = stocks[i]->minAge();
    }

    for (area = 0; area < areas.Size(); area++) {
      DoubleMatrix mat, tot, rel;
      mat.AddRows(maxagem - minagem + 1, nrofyears, 0.0);
      tot.AddRows(maxage - minage + 1, nrofyears, 0.0);
      rel.AddRows(maxage - minage + 1, nrofyears, 0.0);
      for (i = 0; i < stocks.Size(); i++)
        if (i != immindex)
          for (a = minagem; a <=  maxagem; a++)
            if (a >= stocks[i]->minAge() && a <= stocks[i]->maxAge())
              for (y = 0; y < nrofyears; y++)
                mat[a - minagem][y] += ((LenStock*)stocks[i])->getBiomass(area)[a - stocks[i]->minAge()][y];

      DoubleMatrix& bio = (DoubleMatrix&)((LenStock*)stocks[immindex])->getBiomass(area);
      for (y = 0; y < nrofyears; y++) {
        for (a = minagem; a <= maxagem; a++)
          tot[a - minage][y] = mat[a - minagem][y];
        for (a = stocks[immindex]->minAge(); a <= stocks[immindex]->maxAge(); a++)
          tot[a - minage][y] += bio[a - stocks[immindex]->minAge()][y];
      }
      for (y = 0; y < nrofyears; y++) {
        for (a = minagem; a <= maxagem; a++)
          if (isZero(mat[a - minagem][y]))
            rel[a - minage][y] = 0.0;
          else
            rel[a - minage][y] = mat[a - minagem][y] / tot[a - minage][y];
      }

      printfile << "Area " << Area->InnerArea(areas[area]) << endl;
      printfile << "Immature stock biomass at 1/1 in tons\nstock "
        << stocks[immindex]->Name() << endl;
      printByAgeAndYear(printfile, bio, stocks[immindex]->minAge(), firstyear, 2);
      printfile << "Mature stock biomass at 1/1 in tons\nstocks";
      for (i = 0; i<stocks.Size(); i++)
        if (i != immindex)
          printfile << sep << stocks[i]->Name();

      printfile << endl;
      printByAgeAndYear(printfile, mat, minagem, firstyear, 2);
      printfile << "Total stock biomass at 1/1 in tons\nstocks";
      for (i = 0; i <stocks.Size(); i++)
        printfile << sep << stocks[i]->Name();

      printfile << endl;
      printByAgeAndYear(printfile, tot, minage, firstyear, 2);
      printfile << "Fraction of mature stock biomass to total biomass\n";
      printByAgeAndYear(printfile, rel, minage, firstyear, 2);
    }
    printfile.flush();
  }
}

BiomassPrinter::~BiomassPrinter() {
  int i;
  printfile.close();
  printfile.clear();
  for (i = 0; i < matnames.Size(); i++)
    delete[] matnames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  delete immname;
}
