#include "preyoveraggregator.h"
#include "prey.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

PreyOverAggregator::PreyOverAggregator(const PreyPtrVector& Preys,
  const IntMatrix& Areas, const LengthGroupDivision* const LgrpDiv)
  : preys(Preys), areas(Areas) {

  int i, j;
  for (i = 0; i < preys.Size(); i++) {
    if (!checkLengthGroupStructure(preys[i]->getLengthGroupDiv(), LgrpDiv))
      handle.logMessage(LOGFAIL, "Error in preyoveraggregator - invalid length group structure");
    preyConv.AddRows(1, preys[i]->getLengthGroupDiv()->numLengthGroups(), -1);
    for (j = 0; j < preyConv.Ncol(i); j++)
      preyConv[i][j] = LgrpDiv->numLengthGroup(preys[i]->getLengthGroupDiv()->meanLength(j));
  }
  total.AddRows(areas.Nrow(), LgrpDiv->numLengthGroups(), 0.0);
}

void PreyOverAggregator::Reset() {
  total.setToZero();
}

void PreyOverAggregator::Sum() {
  int i, j, k, l;

  this->Reset();
  //Sum over the appropriate preys, areas, and lengths.
  for (k = 0; k < preys.Size(); k++)
    for (i = 0; i < areas.Nrow(); i++)
      for (j = 0; j < areas.Ncol(i); j++)
        if (preys[k]->isOverConsumption(areas[i][j]))
          for (l = 0; l < preyConv.Ncol(k); l++)
            if (preyConv[k][l] >= 0)
              total[i][preyConv[k][l]] += (preys[k]->getOverConsumption(areas[i][j]))[l];

}
