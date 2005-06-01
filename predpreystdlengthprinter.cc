#include "predstdinfobylength.h"
#include "predpreystdprinter.h"
#include "predstdinfo.h"
#include "areatime.h"
#include "gadget.h"

#include "runid.h"
extern RunID RUNID;

PredPreyStdLengthPrinter::PredPreyStdLengthPrinter(CommentStream& infile, const TimeClass* const TimeInfo)
  : PredPreyStdPrinter(infile, TimeInfo), predinfo(0), predator(0), prey(0) {

  //finished initializing. Now print first lines
  outfile << "; ";
  RUNID.Print(outfile);
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

  if ((!AAT.atCurrentTime(TimeInfo)) || (printtime != printtimeid))
    return;

  int a, predl, preyl;
  const LengthGroupDivision* predLgrpDiv = predinfo->getPredLengthGroupDiv();
  const LengthGroupDivision* preyLgrpDiv = predinfo->getPreyLengthGroupDiv();

  for (a = 0; a < areas.Size(); a++) {
    predinfo->Sum(TimeInfo, areas[a]);

    for (predl = 0; predl < predLgrpDiv->numLengthGroups(); predl++) {
      for (preyl = 0; preyl < preyLgrpDiv->numLengthGroups(); preyl++) {

        outfile << setw(lowwidth) << TimeInfo->getYear() << sep
          << setw(lowwidth) << TimeInfo->getStep() << sep
          << setw(lowwidth) << outerareas[a] << sep << setw(lowwidth)
          << predLgrpDiv->meanLength(predl) << sep << setw(lowwidth)
          << preyLgrpDiv->meanLength(preyl) << sep;

        //JMB crude filter to remove the 'silly' values from the output
        if ((predinfo->NconsumptionByLength(areas[a])[predl][preyl] < rathersmall)
           || (predinfo->BconsumptionByLength(areas[a])[predl][preyl] < rathersmall)
           || (predinfo->MortalityByLength(areas[a])[predl][preyl] < verysmall))

          outfile << setw(width) << 0 << sep << setw(width) << 0 << sep << setw(width) << 0 << endl;

        else
          outfile << setprecision(precision) << setw(width)
            << predinfo->NconsumptionByLength(areas[a])[predl][preyl] << sep
            << setprecision(precision) << setw(width)
            << predinfo->BconsumptionByLength(areas[a])[predl][preyl] << sep
            << setprecision(precision) << setw(width)
            << predinfo->MortalityByLength(areas[a])[predl][preyl] << endl;

      }
    }
  }
  outfile.flush();
}
