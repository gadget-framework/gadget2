#include "predstdinfo.h"
#include "predpreystdprinter.h"
#include "areatime.h"
#include "stockpredstdinfo.h"
#include "gadget.h"

#include "runid.h"
extern RunId RUNID;

PredPreyStdAgePrinter::PredPreyStdAgePrinter(CommentStream& infile,
  const AreaClass* const Area, const TimeClass* const TimeInfo)
  : PredPreyStdPrinter(infile, Area, TimeInfo), predinfo(0), predator(0), prey(0) {

  //finished initializing. Now print first lines
  outfile << "; ";
  RUNID.print(outfile);
  outfile << "; Predation file by age - predator " << predname << " - prey " << preyname
    << "\n; year-step-area-pred age-prey age-cons number-cons biomass-mortality\n";
  outfile.flush();
}

PredPreyStdAgePrinter::~PredPreyStdAgePrinter() {
  delete predinfo;
}

void PredPreyStdAgePrinter::SetPopPredAndPrey(const PopPredator* pred,
  const Prey* pRey, int IsStockPredator, int IsStockPrey) {

  assert(!predinfo);
  predator = pred;
  prey = pRey;
  if (IsStockPredator) {
    if (IsStockPrey)
      predinfo = new StockPredStdInfo((const StockPredator*)(predator), (const StockPrey*)(prey), areas);
    else
      predinfo = new StockPredStdInfo((const StockPredator*)(predator), prey, areas);
  } else
    if (IsStockPrey)
      predinfo = new PredStdInfo(predator, (const StockPrey*)(prey), areas);
    else
      predinfo = new PredStdInfo(predator, prey, areas);
}

void PredPreyStdAgePrinter::Print(const TimeClass * const TimeInfo) {
  assert(predinfo);
  if (!aat.AtCurrentTime(TimeInfo))
    return;
  int a, predage, preyage;

  for (a = 0; a < areas.Size(); a++)
    predinfo->Sum(TimeInfo, areas[a]);

  for (a = 0; a < areas.Size(); a++) {
    for (predage = predinfo->NconsumptionByAge(areas[a]).Minage();
        predage <= predinfo->NconsumptionByAge(areas[a]).Maxage(); predage++) {
      for (preyage = predinfo->NconsumptionByAge(areas[a]).Mincol(predage);
          preyage < predinfo->NconsumptionByAge(areas[a]).Maxcol(predage); preyage++) {
        outfile << setw(smallwidth) << TimeInfo->CurrentYear() << sep
          << setw(smallwidth) << TimeInfo->CurrentStep() << sep
          << setw(smallwidth) << areas[a] << sep << setw(smallwidth)
          << predage << sep << setw(smallwidth) << preyage << sep << setprecision(smallprecision)
          << predinfo->NconsumptionByAge(areas[a])[predage][preyage] << sep
          << predinfo->BconsumptionByAge(areas[a])[predage][preyage] << sep
          << predinfo->MortalityByAge(areas[a])[predage][preyage] << endl;
      }
    }
  }
  outfile.flush();
}
