#include "predstdinfo.h"
#include "predpreystdprinter.h"
#include "areatime.h"
#include "stockpredstdinfo.h"
#include "gadget.h"

#include "runid.h"
extern RunID RUNID;

PredPreyStdAgePrinter::PredPreyStdAgePrinter(CommentStream& infile, const TimeClass* const TimeInfo)
  : PredPreyStdPrinter(infile, TimeInfo), predinfo(0), predator(0), prey(0) {

  //finished initializing. Now print first lines
  outfile << "; ";
  RUNID.print(outfile);
  outfile << "; Predation file by age for predator " << predname << " and prey " << preyname;

  if (printtimeid == 0)
    outfile << "\n; Printing the following information at the end of each timestep";
  else
    outfile << "\n; Printing the following information at the start of each timestep";

  outfile << "\n; year-step-area-pred age-prey age-cons number-cons biomass-mortality\n";
  outfile.flush();
}

PredPreyStdAgePrinter::~PredPreyStdAgePrinter() {
  delete predinfo;
}

void PredPreyStdAgePrinter::setPopPredAndPrey(const PopPredator* pred,
  const Prey* pRey, int IsStockPredator, int IsStockPrey) {

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

void PredPreyStdAgePrinter::Print(const TimeClass * const TimeInfo, int printtime) {

  if ((!AAT.AtCurrentTime(TimeInfo)) || (printtime != printtimeid))
    return;

  int a, predage, preyage, p, w;

  if (precision == 0) {
    p = printprecision;
    w = printwidth;
  } else {
    p = precision;
    w = precision + 4;
  }

  for (a = 0; a < areas.Size(); a++) {
    predinfo->Sum(TimeInfo, areas[a]);
    for (predage = predinfo->NconsumptionByAge(areas[a]).minAge();
        predage <= predinfo->NconsumptionByAge(areas[a]).maxAge(); predage++) {
      for (preyage = predinfo->NconsumptionByAge(areas[a]).minCol(predage);
          preyage < predinfo->NconsumptionByAge(areas[a]).maxCol(predage); preyage++) {

        outfile << setw(lowwidth) << TimeInfo->CurrentYear() << sep
          << setw(lowwidth) << TimeInfo->CurrentStep() << sep
          << setw(lowwidth) << outerareas[a] << sep << setw(lowwidth)
          << predage << sep << setw(lowwidth) << preyage << sep;

        //JMB crude filter to remove the 'silly' values from the output
        if ((predinfo->NconsumptionByAge(areas[a])[predage][preyage] < rathersmall)
           || (predinfo->BconsumptionByAge(areas[a])[predage][preyage] < rathersmall)
           || (predinfo->MortalityByAge(areas[a])[predage][preyage] < verysmall))

          outfile << setw(w) << 0 << sep << setw(w) << 0 << sep << setw(w) << 0 << endl;

        else
          outfile << setprecision(p) << setw(w) << predinfo->NconsumptionByAge(areas[a])[predage][preyage] << sep
            << setprecision(p) << setw(w) << predinfo->BconsumptionByAge(areas[a])[predage][preyage] << sep
            << setprecision(p) << setw(w) << predinfo->MortalityByAge(areas[a])[predage][preyage] << endl;

      }
    }
  }
  outfile.flush();
}
