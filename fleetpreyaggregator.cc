#include "fleetpreyaggregator.h"
#include "areatime.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "checkconversion.h"
#include "stock.h"
#include "stockprey.h"
#include "mortprey.h"
#include "lengthpredator.h"
#include "fleet.h"
#include "readword.h"
#include "extravector.h"
#include "gadget.h"

FleetPreyAggregator::FleetPreyAggregator(const Fleetptrvector& Fleets,
  const Stockptrvector& Stocks, LengthGroupDivision* const Lgrpdiv,
  const intmatrix& Areas, const intmatrix& Ages, const int overcons)
  : fleets(Fleets), stocks(Stocks), LgrpDiv(Lgrpdiv),
    areas(Areas), ages(Ages), overconsumption(overcons) {

  int i, j, k, l = 0;
  maxcol.resize(ages.Nrow(), 0);
  mincol.resize(ages.Nrow(), 99999);
  maxrow = 0;
  minrow = ages.Nrow();
  numlengths = LgrpDiv->NoLengthGroups();

  for (i = 0; i < stocks.Size(); i++) {
    CheckLengthGroupIsFiner(stocks[i]->ReturnPrey()->ReturnLengthGroupDiv(),
      LgrpDiv, stocks[i]->Name(), "fleet consumption");
    CI.resize(1);
    CI[i] = new ConversionIndex(stocks[i]->ReturnPrey()->ReturnLengthGroupDiv(), LgrpDiv);

    //For convinience, use ap as shorthand for &(stocks[i]->Agelengthkeys(0))
    //Changed 25-9 2001 and the memberfunction Areas added to livesinareas
    const Agebandmatrix* ap = &(stocks[i]->Agelengthkeys(stocks[i]->Areas()[0]));

    //Now, loop over all the possible ages in the Ages matrix,
    for (j = 0; j < ages.Nrow(); j++) {
      for (k = 0; k < ages.Ncol(j); k++) {
        l = ages[j][k];

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

  //Resize totalcatch using dummy variables tmppop and popmatrix. Take care
  //to make totalcatch a full matrix, i.e. a rectangular matrix, with
  //minage = 0 and minlength = 0.
  popinfo tmppop;
  tmppop.N = 1.0;
  tmppop.W = 1.0;
  popinfomatrix popmatrix(ages.Nrow(), numlengths, tmppop);
  totalcatch.resize(areas.Nrow(), 0, 0, popmatrix);

  //Resize catchratios using dummy variables tmpband. Take care
  //to make catchratios a full matrix, i.e. a rectangular matrix, with
  //minage = 0 and minlength = 0.
  const bandmatrix tmpband(0, numlengths, 0, ages.Nrow(), 1);
  catchratios.resize(areas.Nrow(), tmpband);
}

FleetPreyAggregator::~FleetPreyAggregator() {
  int i;
  for (i = 0; i < CI.Size(); i++)
    delete CI[i];
  //delete LgrpDiv;
}

void FleetPreyAggregator::Print(ofstream& outfile) const {
  int a, i, j;

  for (a = 0; a < areas.Nrow(); a++) {
    outfile << "\tareas " << a << endl;
    for (j = 0; j < ages.Nrow(); j++) {
      outfile << TAB;
      for (i = 0; i < NoLengthGroups(); i++) {
        outfile.width(printwidth);
        outfile << totalcatch[a][j][i].N;
        outfile << sep;
      }
      outfile << endl;
    }
  }
  outfile << flush;
}

void FleetPreyAggregator::Sum(const TimeClass* const TimeInfo) {
  agebandmatrixptrvector totalpop;
  popinfo tmppop;
  tmppop.N = 1.0;
  tmppop.W = 1.0;
  popinfomatrix popmatrix(ages.Nrow(), NoLengthGroups(), tmppop);
  totalpop.resize(areas.Nrow(), 0, 0, popmatrix);

  int i, j, k, f, h, z;
  int aggrArea, aggrAge, area, age;
  popinfo nullpop;

  //Initialize totalcatch and totalpop to 0
  for (i = 0; i < totalcatch.Size(); i++)
    for (j = 0; j < totalcatch[i].Nrow(); j++)
      for (k = 0; k < totalcatch[i].Maxlength(j); k++) {
        totalcatch[i][j][k] = nullpop;
        totalpop[i][j][k] = nullpop;
        catchratios[i][j][k] = 0.0;
      }

  //Sum over the appropriate fleets, stocks, areas, ages and length groups.
  //The index aggrArea is for the dummy area in totalcatch.
  //The index aggrAge is for the dummy age in totalcatch.
  for (f = 0; f < fleets.Size(); f++) {
    LengthPredator* pred = fleets[f]->ReturnPredator();
    for (h = 0; h < stocks.Size(); h++){
      //AJ 06.06.00 Typecast Prey which is returned from ReturnPrey to (StockPrey*)
      StockPrey* prey = (StockPrey*)stocks[h]->ReturnPrey();
      for (aggrArea = 0; aggrArea < areas.Nrow(); aggrArea++) {
        for (j = 0; j < areas.Ncol(aggrArea); j++) {
          //All the areas in areas[aggrArea] will be aggregated to the
          //area aggrArea in totalcatch.
          area = areas[aggrArea][j];
          if (prey->IsInArea(area) && fleets[f]->IsInArea(area)) {
            for (i = 0; i < pred->NoPreys(); i++) {
              if (prey->Name() == pred->Preys(i)->Name()) {
                const doubleindexvector* suitptr = &pred->Suitability(i)[0];
                const Agebandmatrix* alptr = &prey->AlkeysPriorToEating(area);
                for (aggrAge = 0; aggrAge < ages.Nrow(); aggrAge++) {
                  for (k = 0; k < ages.Ncol(aggrAge); k++) {
                    age = ages[aggrAge][k];
                    if ((alptr->Minage() <= age) && (age <= alptr->Maxage())) {

                      //to get the real catch obtained by the model, we
                      //have to multiply the vector suits by
                      //(prey->Ratio(area,l) > 1 ? 1/Prey->Ratio(area, l) : 1)
                      doubleindexvector Ratio = *suitptr;
                      if (overconsumption)
                        for (z = Ratio.Mincol(); z < Ratio.Maxcol(); z++)
                          Ratio[z] *= (prey->Ratio(area, z) > 1 ? 1 / prey->Ratio(area, z) : 1);

                      PopinfoAdd(totalcatch[aggrArea][aggrAge], (*alptr)[age], *CI[h],
                        fleets[f]->Amount(area, TimeInfo)*pred->Scaler(area), Ratio);

                      PopinfoAdd(totalpop[aggrArea][aggrAge], (*alptr)[age], *CI[h]);
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  //Set the "fishing mortalities"
  for (i = 0; i < totalcatch.Size(); i++)
    for (j = 0; j < totalcatch[i].Nrow(); j++)
      for (k = 0; k < totalcatch[i].Maxlength(j); k++)
        if (totalpop[i][j][k].N > 0)
          catchratios[i][j][k] = totalcatch[i][j][k].N / totalpop[i][j][k].N;
}

void FleetPreyAggregator::Sum(const TimeClass* const TimeInfo, int dummy) {
  //A new aggregator function to sum up the calculated catches from
  //the mortality model. The results are to be used in the second likelihood
  //function in CatchDistribution.
  //written by kgf 16/9 98
  agebandmatrixptrvector totalpop;
  popinfo tmppop;
  tmppop.N = 1.0;
  tmppop.W = 1.0;
  popinfomatrix popmatrix(ages.Nrow(), NoLengthGroups(), tmppop);
  totalpop.resize(areas.Nrow(), 0, 0, popmatrix);

  int i, j, k, f, h;
  int aggrArea, aggrAge, area, age;
  popinfo nullpop;

  //Initialize totalcatch and totalpop to 0
  for (i = 0; i < totalcatch.Size(); i++)
    for (j = 0; j < totalcatch[i].Nrow(); j++)
      for (k = 0; k < totalcatch[i].Maxlength(j); k++) {
        totalcatch[i][j][k] = nullpop;
        totalpop[i][j][k] = nullpop;
        catchratios[i][j][k] = 0.0;
      }

  //Sum over the appropriate fleets, stocks, areas, ages and length groups.
  //The index aggrArea is for the dummy area in totalcatch.
  //The index aggrAge is for the dummy age in totalcatch.
  for (f = 0; f < fleets.Size(); f++) {
    LengthPredator* pred = fleets[f]->ReturnPredator();
    for (h = 0; h < stocks.Size(); h++) {
      MortPrey* prey = (MortPrey*)stocks[h]->ReturnPrey();
      for (aggrArea = 0; aggrArea < areas.Nrow(); aggrArea++) {
        for (j = 0; j < areas.Ncol(aggrArea); j++) {

          //All the areas in areas[aggrArea] will be aggregated to the
          //area aggrArea in totalcatch.
          area = areas[aggrArea][j];
          if (prey->IsInArea(area) && fleets[f]->IsInArea(area)) {
            for (i = 0; i < pred->NoPreys(); i++) {
              if (prey->Name() == pred->Preys(i)->Name()) {
                const doubleindexvector* suitptr = &pred->Suitability(i)[0];
                const Agebandmatrix* alptr = &prey->getMeanN(area);
                for (aggrAge = 0; aggrAge < ages.Nrow(); aggrAge++) {
                  for (k = 0; k < ages.Ncol(aggrAge); k++) {
                    age = ages[aggrAge][k];
                    if ((alptr->Minage() <= age) && (age <= alptr->Maxage())) {
                      doubleindexvector Ratio = *suitptr;
                      PopinfoAdd(totalcatch[aggrArea][aggrAge],
                        (*alptr)[age], *CI[h], pred->getFlevel(area, TimeInfo), Ratio);
                      PopinfoAdd(totalpop[aggrArea][aggrAge],
                        (*alptr)[age], *CI[h]);
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  //Set the "fishing percentage"
  for (i = 0; i < totalcatch.Size(); i++)
    for (j = 0; j < totalcatch[i].Nrow(); j++)
      for (k = 0; k < totalcatch[i].Maxlength(j); k++)
        if (totalpop[i][j][k].N > 0)
          catchratios[i][j][k] = totalcatch[i][j][k].N / totalpop[i][j][k].N;
}
