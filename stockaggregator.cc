#include "stockaggregator.h"
#include "conversionindex.h"
#include "stock.h"
#include "mathfunc.h"

StockAggregator::StockAggregator(const StockPtrVector& Stocks,
  const LengthGroupDivision* const LgrpDiv, const IntMatrix& Areas, const IntMatrix& Ages)
  : stocks(Stocks), areas(Areas), ages(Ages) {

  int i, j, k, l = 0;
  maxcol.resize(Ages.Nrow(), 0);
  mincol.resize(Ages.Nrow(), 9999);
  maxrow = 0;
  minrow = 9999;
  int numlengths = LgrpDiv->numLengthGroups();

  CI.resize(stocks.Size());
  for (i = 0; i < stocks.Size(); i++) {
    CI[i] = new ConversionIndex(stocks[i]->returnLengthGroupDiv(), LgrpDiv);

    //For convenience, use ap as shorthand for &(stocks[i]->getAgeLengthKeys(0))
    //Changed 25-9 2001 and the memberfunction Areas added to livesinareas
    const AgeBandMatrix* ap = &(stocks[i]->getAgeLengthKeys(stocks[i]->Areas()[0]));

    //Now, loop over all the possible ages in the Ages matrix,
    for (j = 0; j < Ages.Nrow(); j++) {
      for (k = 0; k < Ages.Ncol(j); k++) {
        l = Ages[j][k];

        if (l >= ap->minAge() && l <= ap->maxAge()) {
          //l is within the stock age range

          if (j < minrow) //Update minrow if this age is in a lower row of the Ages matrix
            minrow = j;
          if (j > maxrow) //Update maxrow if this age is in a higher row of the Ages matrix
            maxrow = j;

          //If the stock minlength is not smaller than in the CI
          if (ap->minLength(l) >= CI[i]->minLength()) {
            //update mincol if the CI minlength is smaller than mincol
            if (CI[i]->Pos(ap->minLength(l)) < mincol[j])
              mincol[j] = CI[i]->Pos(ap->minLength(l));

          } else {
            //Else the stock minlength is smaller than in CI
            mincol[j] = 0;
          }

          //If the stock maxlength is not larger than in the CI
          if (ap->maxLength(l) - 1 < CI[i]->maxLength()) {
            //update maxcol if the CI minlength is larger than mincol
            if (CI[i]->Pos(ap->maxLength(l) - 1) > maxcol[j])
              maxcol[j] = CI[i]->Pos(ap->maxLength(l) - 1);

          } else {
            //Else the stock maxlength is larger than in CI
            maxcol[j] = numlengths - 1;
          }
        }
      }
    }
  }

  //Resize total using dummy variables tmppop and popmatrix.
  //Take care to make total a full matrix
  //i.e. a rectangular matrix, with minage = 0 and minlength = 0.
  PopInfo tmppop;
  tmppop.N = 1.0;
  tmppop.W = 1.0;
  PopInfoMatrix popmatrix(ages.Nrow(), numlengths, tmppop);
  total.resize(areas.Nrow(), 0, 0, popmatrix);
  this->Reset();
}

StockAggregator::~StockAggregator() {
  int i;
  for (i = 0; i < CI.Size(); i++)
    delete CI[i];
}

void StockAggregator::Print(ofstream& outfile) const {
  int i, j, k;

  for (i = 0; i < total.Size(); i++) {
    outfile << "\tInternal areas " << i << endl;
    for (j = 0; j < total[i].Nrow(); j++) {
      outfile << TAB;
      for (k = 0; k < total[i].maxLength(j); k++) {
        outfile.width(smallwidth);
        outfile << total[i][j][k].N << sep;
      }
      outfile << endl;
    }
  }
  outfile << flush;
}

void StockAggregator::Reset() {
  int i, j, k;
  PopInfo nullpop;

  for (i = 0; i < total.Size(); i++)
    for (j = 0; j < total[i].Nrow(); j++)
      for (k = 0; k < total[i].maxLength(j); k++)
        total[i][j][k] = nullpop;
}

void StockAggregator::Sum() {
  int i, j, k;
  int aggrArea, aggrAge, area, age;

  this->Reset();
  //Sum over the appropriate stocks, areas, ages and length groups.
  for (i = 0; i < stocks.Size(); i++) {
    for (aggrArea = 0; aggrArea < areas.Nrow(); aggrArea++) {
      for (j = 0; j < areas.Ncol(aggrArea); j++) {
        area = areas[aggrArea][j];
        if (stocks[i]->isInArea(area)) {
          const AgeBandMatrix* alptr = &stocks[i]->getAgeLengthKeys(area);
          for (aggrAge = 0; aggrAge < ages.Nrow(); aggrAge++) {
            for (k = 0; k < ages.Ncol(aggrAge); k++) {
              age = ages[aggrAge][k];
              if ((alptr->minAge() <= age) && (age <= alptr->maxAge()))
                total[aggrArea][aggrAge].Add((*alptr)[age], *CI[i]);
            }
          }
        }
      }
    }
  }
}
