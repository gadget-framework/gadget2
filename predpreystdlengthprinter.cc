#include "predstdinfobylength.h"
#include "predpreystdprinter.h"
#include "predstdinfo.h"
#include "areatime.h"
#include "gadget.h"

#include "runid.h"
extern RunId RUNID;

PredPreyStdLengthPrinter::PredPreyStdLengthPrinter(CommentStream& infile,
  const AreaClass* const Area, const TimeClass* const TimeInfo)
  : PredPreyStdPrinter(infile, Area, TimeInfo), predinfo(0), predator(0), prey(0), PredLgrpDiv(0), PreyLgrpDiv(0) {

  //finished initializing. Now print first lines
  outfile << "; ";
  RUNID.print(outfile);
  outfile << "; Predation file by length for predator " << predname << " and prey " << preyname
    << "\n; year-step-area-pred length-prey length-cons number-cons biomass-mortality\n";
  outfile.flush();
}

PredPreyStdLengthPrinter::~PredPreyStdLengthPrinter() {
  delete PredLgrpDiv;
  delete PreyLgrpDiv;
  delete predinfo;
}

void PredPreyStdLengthPrinter::SetPopPredAndPrey(const PopPredator* pred,
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
  if (!aat.AtCurrentTime(TimeInfo))
    return;
  int a, predl, preyl;

  for (a = 0; a < areas.Size(); a++)
    predinfo->Sum(TimeInfo, areas[a]);

  PredLgrpDiv = predinfo->ReturnPredLengthGroupDiv();
  PreyLgrpDiv = predinfo->ReturnPreyLengthGroupDiv();

  for (a = 0; a < areas.Size(); a++) {
    for (predl = 0; predl < PredLgrpDiv->NoLengthGroups(); predl++) {
      for (preyl = 0; preyl < PreyLgrpDiv->NoLengthGroups(); preyl++) {
        outfile << setw(smallwidth) << TimeInfo->CurrentYear() << sep
          << setw(smallwidth) << TimeInfo->CurrentStep() << sep
          << setw(smallwidth) << outerareas[a] << sep << setw(smallwidth)
          << PredLgrpDiv->Meanlength(predl) << sep << setw(smallwidth)
          << PreyLgrpDiv->Meanlength(preyl) << sep;

        //JMB crude filter to remove the 'silly' values from the output
        if ((predinfo->NconsumptionByLength(areas[a])[predl][preyl] < rathersmall)
           || (predinfo->BconsumptionByLength(areas[a])[predl][preyl] < rathersmall)
           || (predinfo->MortalityByLength(areas[a])[predl][preyl] < verysmall))

          outfile << setw(largewidth) << 0 << sep << setw(largewidth) << 0
            << sep << setw(largewidth) << 0 << endl;

        else
          outfile << setw(largewidth) << predinfo->NconsumptionByLength(areas[a])[predl][preyl] << sep
            << setw(largewidth) << predinfo->BconsumptionByLength(areas[a])[predl][preyl] << sep
            << setw(largewidth) << predinfo->MortalityByLength(areas[a])[predl][preyl] << sep << endl;

      }
    }
  }
  outfile.flush();
}
