#include "predstdinfobylength.h"
#include "predpreystdprinter.h"
#include "predstdinfo.h"
#include "areatime.h"
#include "gadget.h"

#include "runid.h"
extern RunId RUNID;

PredPreyStdLengthPrinter::PredPreyStdLengthPrinter(CommentStream& infile,
  const AreaClass* const Area, const TimeClass* const TimeInfo)
  : PredPreyStdPrinter(infile, Area, TimeInfo), predinfo(0), predator(0), prey(0) {

  //finished initializing. Now print first lines
  outfile << "; ";
  RUNID.print(outfile);
  outfile << "; Predation file by length - predator " << predname << " - prey " << preyname
    << "\n; year-step-area-pred length-prey length-cons number-cons biomass-mortality\n";
  outfile.flush();
}

PredPreyStdLengthPrinter::~PredPreyStdLengthPrinter() {
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

  for (a = 0; a < areas.Size(); a++) {
    for (predl = 0; predl < predinfo->NconsumptionByLength(areas[a]).Nrow(); predl++) {
      for (preyl = 0; preyl < predinfo->NconsumptionByLength(areas[a]).Ncol(predl); preyl++) {
        outfile << setw(smallwidth) << TimeInfo->CurrentYear() << sep
          << setw(smallwidth) << TimeInfo->CurrentStep() << sep
          << setw(smallwidth) << areas[a] << sep << setw(smallwidth)
          << predl + 1 << sep << setw(smallwidth) << preyl + 1 << sep << setprecision(smallprecision)
          << predinfo->NconsumptionByLength(areas[a])[predl][preyl] << sep
          << predinfo->BconsumptionByLength(areas[a])[predl][preyl] << sep
          << predinfo->MortalityByLength(areas[a])[predl][preyl] << sep << endl;
      }
    }
  }
  outfile.flush();
}
