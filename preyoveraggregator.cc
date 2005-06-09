#include "preyoveraggregator.h"
#include "prey.h"

PreyOverAggregator::PreyOverAggregator(const PreyPtrVector& Preys,
  const IntMatrix& Areas, const LengthGroupDivision* const LgrpDiv)
  : preys(Preys), areas(Areas) {

  int i, j;
  for (i = 0; i < preys.Size(); i++)
    checkLengthGroupIsFiner(preys[i]->getLengthGroupDiv(), LgrpDiv);

  total.AddRows(areas.Nrow(), LgrpDiv->numLengthGroups(), 0.0);
  for (i = 0; i < preys.Size(); i++) {
    preyConv.AddRows(1, preys[i]->getLengthGroupDiv()->numLengthGroups(), 0);
    for (j = 0; j < preyConv.Ncol(i); j++)
      preyConv[i][j] = LgrpDiv->numLengthGroup(preys[i]->getLengthGroupDiv()->meanLength(j));
  }
}

void PreyOverAggregator::Reset() {
  int i, j;
  for (i = 0; i < total.Nrow(); i++)
    for (j = 0; j < total.Ncol(i); j++)
      total[i][j] = 0.0;
}

void PreyOverAggregator::Sum() {
  int i, j, h, l;

  this->Reset();
  //Sum over the appropriate preys, areas, and lengths.
  for (h = 0; h < preys.Size(); h++)
    for (i = 0; i < areas.Nrow(); i++)
      for (j = 0; j < areas.Ncol(i); j++)
        if (preys[h]->isInArea(areas[i][j]))
          for (l = 0; l < preyConv.Ncol(h); l++)
            if (preyConv[h][l] >= 0)
              total[i][preyConv[h][l]] += (preys[h]->getOverConsumption(areas[i][j]))[l];

}
