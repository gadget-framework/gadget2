#include "stockaggregator.h"
#include "conversion.h"
#include "stock.h"
#include "mathfunc.h"
#include "checkconversion.h"
#include "extravector.h" //for PopInfoAdd

StockAggregator::StockAggregator(const StockPtrVector& Stocks,
  const LengthGroupDivision* const LgrpDiv, const IntMatrix& Areas, const IntMatrix& Ages)
  : stocks(Stocks), areas(Areas), ages(Ages) {

  int i, j, k, l = 0;
  maxcol.resize(Ages.Nrow(), 0);
  mincol.resize(Ages.Nrow(), 99999);
  maxrow = 0;
  minrow = Ages.Nrow();
  numlengths = LgrpDiv->NoLengthGroups();

  for (i = 0; i < stocks.Size(); i++) {
    CheckLengthGroupIsFiner(stocks[i]->ReturnLengthGroupDiv(),
      LgrpDiv, stocks[i]->Name(), "stock abundance numbers");
    CI.resize(1);
    CI[i] = new ConversionIndex(stocks[i]->ReturnLengthGroupDiv(), LgrpDiv);

    //For convenience, use ap as shorthand for &(stocks[i]->Agelengthkeys(0))
    //Changed 25-9 2001 and the memberfunction Areas added to livesinareas
    const AgeBandMatrix* ap = &(stocks[i]->Agelengthkeys(stocks[i]->Areas()[0]));

    //Now, loop over all the possible ages in the Ages matrix,
    for (j = 0; j < Ages.Nrow(); j++) {
      for (k = 0; k < Ages.Ncol(j); k++) {
        l = Ages[j][k];

        if (l >= ap->Minage() && l <= ap->Maxage()) {
          //l is within the stock age range

          if (j < minrow) //Update minrow if this age is in a lower row of the Ages matrix
            minrow = j;
          if (j > maxrow) //Update maxrow if this age is in a higher row of the Ages matrix
            maxrow = j;

          //If the stock minlength is not smaller than in the CI
          if (ap->Minlength(l) >= CI[i]->Minlength()) {
            //update mincol if the CI minlength is smaller than mincol
            if (CI[i]->Pos(ap->Minlength(l)) < mincol[j])
              mincol[j] = CI[i]->Pos(ap->Minlength(l));

          } else {
            //Else the stock minlength is smaller than in CI
            mincol[j] = 0;
          }

          //If the stock maxlength is not larger than in the CI
          if (ap->Maxlength(l) - 1 < CI[i]->Maxlength()) {
            //update maxcol if the CI minlength is larger than mincol
            if (CI[i]->Pos(ap->Maxlength(l) - 1) > maxcol[j])
              maxcol[j] = CI[i]->Pos(ap->Maxlength(l) - 1);

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
  meanTotal.resize(areas.Nrow(), 0, 0, popmatrix);
}

StockAggregator::~StockAggregator() {
  int i;
  for (i = 0; i < CI.Size(); i++)
    delete CI[i];
}

void StockAggregator::Sum() {
  int i, j, k;
  int aggrArea, aggrAge, area, age;
  PopInfo nullpop;

  //Initialize total to 0.
  for (i = 0; i < total.Size(); i++)
    for (j = 0; j < total[i].Nrow(); j++)
      for (k = 0; k < total[i].Maxlength(j); k++)
        total[i][j][k] = nullpop;

  //Sum over the appropriate stocks, areas, ages and length groups.
  //The index aggrArea is for the dummy area in total.
  //The index aggrAge is for the dummy age in total.
  for (i = 0; i < stocks.Size(); i++) {
    for (aggrArea = 0; aggrArea < areas.Nrow(); aggrArea++) {
      for (j = 0; j < areas.Ncol(aggrArea); j++){
        //All the areas in areas[aggrArea] will be aggregated to the area aggrArea in total.
        area = areas[aggrArea][j];
        if (stocks[i]->IsInArea(area)) {
          const AgeBandMatrix* alptr = &stocks[i]->Agelengthkeys(area);
          for (aggrAge = 0; aggrAge < ages.Nrow(); aggrAge++) {
            for (k = 0; k < ages.Ncol(aggrAge); k++) {
              age = ages[aggrAge][k];
              if ((alptr->Minage() <= age) && (age <= alptr->Maxage()))
                PopinfoAdd(total[aggrArea][aggrAge], (*alptr)[age], *CI[i]);
            }
          }
        }
      }
    }
  }
}

void StockAggregator::MeanSum() {
  int i, j, k;
  int aggrArea, aggrAge, area, age;
  PopInfo nullpop;

  //Initialize meanTotal to 0.
  for (i = 0; i < meanTotal.Size(); i++)
    for (j = 0; j < meanTotal[i].Nrow(); j++)
      for (k = 0; k < meanTotal[i].Maxlength(j); k++)
        meanTotal[i][j][k] = nullpop;

  //Sum over the appropriate stocks, areas, ages and length groups.
  //The index aggrArea is for the dummy area in meanTotal.
  //The index aggrAge is for the dummy age in meanTotal.
  for (i = 0; i < stocks.Size(); i++) {
    for (aggrArea = 0; aggrArea < areas.Nrow(); aggrArea++) {
      for (j = 0; j < areas.Ncol(aggrArea); j++) {
        //All the areas in areas[aggrArea] will be aggregated to the area aggrArea in meanTotal.
        area = areas[aggrArea][j];
        if (stocks[i]->IsInArea(area)) {
          const AgeBandMatrix* alptr = &stocks[i]->getMeanN(area);
          for (aggrAge = 0; aggrAge < ages.Nrow(); aggrAge++) {
            for (k = 0; k < ages.Ncol(aggrAge); k++) {
              age = ages[aggrAge][k];
              if ((alptr->Minage() <= age) && (age <= alptr->Maxage()))
                PopinfoAdd(meanTotal[aggrArea][aggrAge], (*alptr)[age], *CI[i]);
            }
          }
        }
      }
    }
  }
}

const AgeBandMatrixPtrVector& StockAggregator::ReturnSum() const {
  return total;
}

const AgeBandMatrixPtrVector& StockAggregator::ReturnMeanSum() const {
  return meanTotal;
}
