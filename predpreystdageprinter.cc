#include "predstdinfo.h"
#include "predpreystdprinter.h"
#include "areatime.h"
#include "stockpredstdinfo.h"
#include "gadget.h"

#include "runid.h"
extern RunID RUNID;

PredPreyStdAgePrinter::PredPreyStdAgePrinter(CommentStream& infile,
  const AreaClass* const Area, const TimeClass* const TimeInfo)
  : PredPreyStdPrinter(infile, Area, TimeInfo), predinfo(0), predator(0), prey(0) {

  //finished initializing. Now print first lines
  outfile << "; ";
  RUNID.print(outfile);
  outfile << "; Predation file by age for predator " << predname << " and prey " << preyname
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

        outfile << setw(lowwidth) << TimeInfo->CurrentYear() << sep
          << setw(lowwidth) << TimeInfo->CurrentStep() << sep
          << setw(lowwidth) << outerareas[a] << sep << setw(lowwidth)
          << predage << sep << setw(lowwidth) << preyage << sep;

        //JMB crude filter to remove the 'silly' values from the output
        if ((predinfo->NconsumptionByAge(areas[a])[predage][preyage] < rathersmall)
           || (predinfo->BconsumptionByAge(areas[a])[predage][preyage] < rathersmall)
           || (predinfo->MortalityByAge(areas[a])[predage][preyage] < verysmall))

          outfile << setw(printwidth) << 0 << sep << setw(printwidth) << 0
            << sep << setw(printwidth) << 0 << endl;

        else
          outfile << setprecision(printprecision) << setw(printwidth) << predinfo->NconsumptionByAge(areas[a])[predage][preyage] << sep
            << setprecision(printprecision) << setw(printwidth) << predinfo->BconsumptionByAge(areas[a])[predage][preyage] << sep
            << setprecision(printprecision) << setw(printwidth) << predinfo->MortalityByAge(areas[a])[predage][preyage] << endl;

      }
    }
  }
  outfile.flush();
}
