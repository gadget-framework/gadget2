#include "predstdinfobylength.h"
#include "predpreystdprinter.h"
#include "predstdinfo.h"
#include "areatime.h"
#include "gadget.h"

#include "runid.h"
extern RunID RUNID;

PredPreyStdLengthPrinter::PredPreyStdLengthPrinter(CommentStream& infile,
  const AreaClass* const Area, const TimeClass* const TimeInfo)
  : PredPreyStdPrinter(infile, Area, TimeInfo), predinfo(0), predator(0), prey(0) {

  //finished initializing. Now print first lines
  outfile << "; ";
  RUNID.print(outfile);
  outfile << "; Predation file by length for predator " << predname << " and prey " << preyname
    << "\n; year-step-area-pred length-prey length-cons number-cons biomass-mortality\n";
  outfile.flush();
}

PredPreyStdLengthPrinter::~PredPreyStdLengthPrinter() {
  delete predinfo;
}

void PredPreyStdLengthPrinter::setPopPredAndPrey(const PopPredator* pred,
  const Prey* pRey, int IsStockPredator, int IsStockPrey) {

  assert(!predinfo);
  predator = pred;
  prey = pRey;
  if (IsStockPrey)
    predinfo = new PredStdInfoByLength(predator, (const StockPrey*)(prey), areas);
  else
    predinfo = new PredStdInfoByLength(predator, prey, areas);
}

void PredPreyStdLengthPrinter::Print(const TimeClass* const TimeInfo) {
  assert(predinfo);
  if (!AAT.AtCurrentTime(TimeInfo))
    return;
  int a, predl, preyl;

  for (a = 0; a < areas.Size(); a++)
    predinfo->Sum(TimeInfo, areas[a]);

  const LengthGroupDivision* PredLgrpDiv = predinfo->returnPredLengthGroupDiv();
  const LengthGroupDivision* PreyLgrpDiv = predinfo->returnPreyLengthGroupDiv();

  for (a = 0; a < areas.Size(); a++) {
    for (predl = 0; predl < PredLgrpDiv->NoLengthGroups(); predl++) {
      for (preyl = 0; preyl < PreyLgrpDiv->NoLengthGroups(); preyl++) {

        outfile << setw(lowwidth) << TimeInfo->CurrentYear() << sep
          << setw(lowwidth) << TimeInfo->CurrentStep() << sep
          << setw(lowwidth) << outerareas[a] << sep << setw(lowwidth)
          << PredLgrpDiv->Meanlength(predl) << sep << setw(lowwidth)
          << PreyLgrpDiv->Meanlength(preyl) << sep;

        //JMB crude filter to remove the 'silly' values from the output
        if ((predinfo->NconsumptionByLength(areas[a])[predl][preyl] < rathersmall)
           || (predinfo->BconsumptionByLength(areas[a])[predl][preyl] < rathersmall)
           || (predinfo->MortalityByLength(areas[a])[predl][preyl] < verysmall))

          outfile << setw(printwidth) << 0 << sep << setw(printwidth) << 0
            << sep << setw(printwidth) << 0 << endl;

        else
          outfile << setprecision(printprecision) << setw(printwidth) << predinfo->NconsumptionByLength(areas[a])[predl][preyl] << sep
            << setprecision(printprecision) << setw(printwidth) << predinfo->BconsumptionByLength(areas[a])[predl][preyl] << sep
            << setprecision(printprecision) << setw(printwidth) << predinfo->MortalityByLength(areas[a])[predl][preyl] << endl;

      }
    }
  }
  outfile.flush();
}
