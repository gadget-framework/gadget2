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
  outfile << "; Predation file by length for predator " << predname << " and prey " << preyname;

  if (printtimeid == 0)
    outfile << "\n; Printing the following information at the end of each timestep";
  else
    outfile << "\n; Printing the following information at the start of each timestep";

  outfile << "\n; year-step-area-pred length-prey length-cons number-cons biomass-mortality\n";
  outfile.flush();
}

PredPreyStdLengthPrinter::~PredPreyStdLengthPrinter() {
  delete predinfo;
}

void PredPreyStdLengthPrinter::setPopPredAndPrey(const PopPredator* pred,
  const Prey* pRey, int IsStockPredator, int IsStockPrey) {

  predator = pred;
  prey = pRey;
  if (IsStockPrey)
    predinfo = new PredStdInfoByLength(predator, (const StockPrey*)(prey), areas);
  else
    predinfo = new PredStdInfoByLength(predator, prey, areas);
}

void PredPreyStdLengthPrinter::Print(const TimeClass* const TimeInfo, int printtime) {

  if ((!AAT.AtCurrentTime(TimeInfo)) || (printtime != printtimeid))
    return;

  int a, predl, preyl, p, w;
  const LengthGroupDivision* predLgrpDiv = predinfo->returnPredLengthGroupDiv();
  const LengthGroupDivision* preyLgrpDiv = predinfo->returnPreyLengthGroupDiv();

  if (precision == 0) {
    p = printprecision;
    w = printwidth;
  } else {
    p = precision;
    w = precision + 4;
  }

  for (a = 0; a < areas.Size(); a++) {
    predinfo->Sum(TimeInfo, areas[a]);

    for (predl = 0; predl < predLgrpDiv->numLengthGroups(); predl++) {
      for (preyl = 0; preyl < preyLgrpDiv->numLengthGroups(); preyl++) {

        outfile << setw(lowwidth) << TimeInfo->CurrentYear() << sep
          << setw(lowwidth) << TimeInfo->CurrentStep() << sep
          << setw(lowwidth) << outerareas[a] << sep << setw(lowwidth)
          << predLgrpDiv->meanLength(predl) << sep << setw(lowwidth)
          << preyLgrpDiv->meanLength(preyl) << sep;

        //JMB crude filter to remove the 'silly' values from the output
        if ((predinfo->NconsumptionByLength(areas[a])[predl][preyl] < rathersmall)
           || (predinfo->BconsumptionByLength(areas[a])[predl][preyl] < rathersmall)
           || (predinfo->MortalityByLength(areas[a])[predl][preyl] < verysmall))

          outfile << setw(w) << 0 << sep << setw(w) << 0 << sep << setw(w) << 0 << endl;

        else
          outfile << setprecision(p) << setw(w) << predinfo->NconsumptionByLength(areas[a])[predl][preyl] << sep
            << setprecision(p) << setw(w) << predinfo->BconsumptionByLength(areas[a])[predl][preyl] << sep
            << setprecision(p) << setw(w) << predinfo->MortalityByLength(areas[a])[predl][preyl] << endl;

      }
    }
  }
  outfile.flush();
}
