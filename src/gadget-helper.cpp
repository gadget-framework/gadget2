#include "ecosystem.h"
#include "renewal.h"
#include "stockprey.h"

#include <Rcpp.h>

extern Ecosystem* EcoSystem;

// [[Rcpp::export]]
Rcpp::List getEcosystemInfo(){

   FleetPtrVector& fleetvec = EcoSystem->getModelFleetVector();

   int maxFleet = fleetvec.Size();

   Rcpp::CharacterVector infoFleet(maxFleet);

   for(int fN=0;fN<maxFleet;fN++){
      Fleet *fleet = fleetvec[fN];
      infoFleet[fN] = fleet->getName();
   }

   StockPtrVector& stockvec = EcoSystem->getModelStockVector();
   int maxStock = stockvec.Size();

   Rcpp::CharacterVector infoStock(maxStock);

   for(int sN=0;sN<maxStock;sN++){
      Stock *stock = stockvec[sN];
      infoStock[sN] = stock->getName();
   }

   return Rcpp::List::create(Rcpp::Named("fleet") = infoFleet,
                Rcpp::Named("stock") = infoStock);

}

// [[Rcpp::export]]
Rcpp::List getStockInfoC(Rcpp::IntegerVector stockNo){

   StockPtrVector stockvec = EcoSystem->getModelStockVector();
   Stock *stock = stockvec[stockNo[0]-1];

   const LengthGroupDivision* lengthGroup = stock->getLengthGroupDiv();

   int lSize = lengthGroup->numLengthGroups();

   Rcpp::NumericVector maxLength(lSize);
   Rcpp::NumericVector minLength(lSize);

   for(int i = 0; i < lSize; i++){
      maxLength[i] = lengthGroup->maxLength(i);
      minLength[i] = lengthGroup->minLength(i);
   }

   return Rcpp::List::create(Rcpp::_("lengthGroup") =
				   Rcpp::DataFrame::create( Rcpp::_("minLength") = minLength,
							    Rcpp::_("maxLength") = maxLength)
				);
}

// [[Rcpp::export]]
Rcpp::IntegerVector updateRecruitmentC(Rcpp::IntegerVector stockNo, Rcpp::IntegerVector year, Rcpp::IntegerVector step,
                Rcpp::IntegerVector area,
		Rcpp::IntegerVector age, Rcpp::IntegerVector number, Rcpp::NumericVector mean,
		Rcpp::NumericVector sdev, Rcpp::NumericVector alpha, Rcpp::NumericVector beta,
		Rcpp::NumericVector length,Rcpp::NumericVector meanWeight){

   int maxage, minage;

   TimeClass* TimeInfo = EcoSystem->getTimeInfo();
   AreaClass* Area = EcoSystem->getArea();
   StockPtrVector stockvec = EcoSystem->getModelStockVector();
   Keeper* keeper = EcoSystem->getKeeper();

   Stock *stock = stockvec[stockNo[0]-1];

   maxage = stock->maxAge();
   minage = maxage = stock->minAge();

   RenewalData* renewal = stock->getRenewalData();

   int readoption = renewal->getReadOption();

#ifdef DEBUG
   std::cout << "Renewal type: " << readoption << std::endl;

   std::cout << "print data before" << std::endl;
   std::ofstream ofs;
   ofs.open ("/tmp/testofs-before.txt", std::ofstream::out);

   renewal->Print(ofs);
   ofs.close();
#endif

   if(readoption == 0){
     //Normal condition -- not implemented yet
   }else if(readoption == 1){
     //Normal parameter
     //std::cout << year[0] << step[0] << area[0] << age[0] << number[0] << mean[0] << sdev[0] << alpha[0] << beta[0] << std::endl;
     renewal->updateNormalParameterData(year[0], step[0], area[0], age[0], number[0], mean[0], sdev[0], alpha[0], beta[0],
       keeper, TimeInfo, Area, minage, maxage);
   }else{
     //Number
     renewal->updateNumberData(year[0], step[0], area[0], age[0], length[0], number[0], meanWeight[0],
       keeper, TimeInfo, Area, minage, maxage);
   }

#ifdef DEBUG
   std::cout << "print data after" << std::endl;
   ofs.open ("/tmp/testofs-after.txt", std::ofstream::out);

   renewal->Print(ofs);
   ofs.close();
#endif

   return Rcpp::IntegerVector(1,0);

}

// [[Rcpp::export]]
Rcpp::IntegerVector updateSuitabilityC(Rcpp::IntegerVector fleetNo, Rcpp::IntegerVector stockNo, Rcpp::NumericVector len, Rcpp::NumericVector value){

   int lenIdx;

   FleetPtrVector& fleetvec = EcoSystem->getModelFleetVector();

   Fleet *fleet = fleetvec[fleetNo[0]-1];

   LengthPredator *predator = fleet->getPredator();

   DoubleMatrix *suit = predator->getSuits(stockNo[0]-1);

#ifdef DEBUG
   for(int i=0; i<suit->Nrow(); i++){
      for(int j=0; j<suit->Ncol(i); j++){
         std::cout << (*suit)[i][j] << " ";
      }
      std::cout << std::endl;
   }
#endif

   if(suit->Nrow() == 0){
     std::cout << "No suitability vector found" << std::endl;
     return Rcpp::IntegerVector(1,55);
   }

   const LengthGroupDivision* lengthGroup = predator->getLengthGroupDiv();

   lenIdx = lengthGroup->numLengthGroup(len[0]);

   if(lenIdx == -1){
     std::cout << "Invalid length" << std::endl;
   }

#ifdef DEBUG
   DoubleVector suitVal = (*suit)[0];

   std::cout << "Bef: " << suitVal[lenIdx] << " " << (*suit)[0][lenIdx] << std::endl;
#endif

   //Since predator length for fleet will always be one, pick only the first element
   (*suit)[0].set(lenIdx, value[0]);

#ifdef DEBUG
   std::cout << "Aft: " << suitVal[lenIdx] << " " << (*suit)[0][lenIdx] << std::endl;
#endif

   return Rcpp::IntegerVector(1,0);
}

// [[Rcpp::export]]
Rcpp::NumericMatrix printPredatorPrey(Rcpp::IntegerVector fleetNo, Rcpp::IntegerVector stockNo){

   int i, j, k, h, l;
   double ratio;

   const DoubleVector* suitptr;
   const AgeBandMatrix* alptr;

   const ConversionIndex *CI;

   AgeBandMatrixPtrVector total;
   AgeBandMatrixPtrVector consume;
   DoubleMatrixPtrVector mortality;

   TimeClass* TimeInfo = EcoSystem->getTimeInfo();

   AreaClass* Area = EcoSystem->getArea();
   IntVector areas = Area->getAllModelAreas();

   StockPtrVector stockvec = EcoSystem->getModelStockVector();
   Stock *stock = stockvec[stockNo[0]-1];

   FleetPtrVector& fleetvec = EcoSystem->getModelFleetVector();
   Fleet *fleet = fleetvec[fleetNo[0]-1];
   LengthPredator *predator = fleet->getPredator();

   StockPrey* prey = stock->getPrey();

   CI = new ConversionIndex(prey->getLengthGroupDiv(), prey->getLengthGroupDiv());

   if (CI->Error())
     std::cout << "Error in predatorpreyaggregator - error when checking length structure" << std::endl;

   //check that the prey is a stock
   if (prey->getType() == LENGTHPREY)
     std::cout << "Error in predatorpreyaggregator - cannot aggregate prey" << prey->getName() << std::endl;

   for (j = 0; j < areas.Size(); j++){
      int area = Area->getInnerArea(areas[j]);
      alptr = &prey->getConsumptionALK(area);
      mortality.resize(new DoubleMatrix(alptr->Nrow(), prey->getLengthGroupDiv()->numLengthGroups(), 0.0));
   }

   PopInfo tmppop;
   tmppop.N = 1.0;
   PopInfoMatrix popmatrix(alptr->Nrow(), prey->getLengthGroupDiv()->numLengthGroups(), tmppop);
   total.resize(areas.Size(), 0, 0, popmatrix);
   consume.resize(areas.Size(), 0, 0, popmatrix);

   // Set to zero
   for (i = 0; i < mortality.Size(); i++) {
     total[i].setToZero();
     consume[i].setToZero();
     (*mortality[i]).setToZero();
   }

   //Sum over the appropriate predators, preys, areas, ages and length groups
   //First calculate the prey population that is consumed by the predation
   if (predator->doesEat(prey->getName())) {
          for (j = 0; j < areas.Size(); j++) {
            int area = Area->getInnerArea(areas[j]);
            if ((prey->isPreyArea(area)) && (predator->isInArea(area))) {
              for (k = 0; k < predator->numPreys(); k++) {
                if (strcasecmp(prey->getName(), predator->getPrey(k)->getName()) == 0) {
                  alptr = &prey->getConsumptionALK(area);
                  for (h = 0; h < predator->getLengthGroupDiv()->numLengthGroups(); h++) {
                    //suitptr = &predator->getSuitability(k)[h];
                    suitptr = &predator->getUseSuitability(area, k)[h];
                    ratio = predator->getConsumptionRatio(area, k, h);
                    for (l = 0; l < alptr->Nrow(); l++){
                          int age = alptr->minAge() + l;
                          consume[j][l].Add((*alptr)[age], *CI, *suitptr, ratio);
                    }
                  }
                }
              }
            }
          }
   }

  //Then calculate the prey population before predation
  for (j = 0; j < areas.Size(); j++) {
        int area = Area->getInnerArea(areas[j]);
        if (prey->isPreyArea(area)) {
          alptr = &prey->getConsumptionALK(area);
          for (l = 0; l < alptr->Nrow(); l++){
                int age = alptr->minAge() + l;
                total[j][l].Add((*alptr)[age], *CI);
          }

        }
  }

  // Finally calculate the mortality caused by the predation
  ratio = 1.0 / TimeInfo->getTimeStepSize();
  for (i = 0; i < mortality.Size(); i++)
    for (j = 0; j < (*mortality[i]).Nrow(); j++)
      for (k = 0; k < (*mortality[i]).Ncol(j); k++)
        (*mortality[i])[j][k] = calcMortality(consume[i][j][k].N, total[i][j][k].N, ratio);

  //Print it

  int width = 0;
  int lowwidth = 0;
  int printwidth = 0;
  int precision = 4;

  int a, age, len;

  Rcpp::DataFrame df;

  for (a = 0; a < areas.Size(); a++) {
    for (age = consume[a].minAge(); age <= consume[a].maxAge(); age++) {
      for (len = consume[a].minLength(age); len < consume[a].maxLength(age); len++) {
        cout << setw(lowwidth) << TimeInfo->getYear() << sep
          << setw(lowwidth) << TimeInfo->getStep() << sep
          << setw(printwidth) << Area->getModelArea(a) << sep
          << setw(printwidth) << stock->minAge() + age << sep
          << setw(printwidth) << prey->getLengthGroupDiv()->minLength(len) << sep;

        Rcpp::NumericVector v = Rcpp::NumericVector::create(TimeInfo->getYear(), TimeInfo->getStep(),
            Area->getModelArea(a), stock->minAge() + age,
            prey->getLengthGroupDiv()->minLength(len) );

        Rcpp::NumericVector w;
        // JMB crude filter to remove the 'silly' values from the output
        if ((consume[a][age][len].N < rathersmall) || (consume[a][age][len].W < 0.0)){
          cout << setw(width) << 0 << sep << setw(width) << 0 << sep << setw(width) << 0 << endl;
          w = Rcpp::NumericVector::create(0, 0, 0);
        }else{
          cout << setprecision(precision) << setw(width) << consume[a][age][len].N
            << sep << setprecision(precision) << setw(width)
            << consume[a][age][len].N * consume[a][age][len].W
            << sep << setprecision(precision) << setw(width)
            << (*mortality[a])[age][len] << endl;
          w = Rcpp::NumericVector::create(consume[a][age][len].N,
            consume[a][age][len].N * consume[a][age][len].W,
            (*mortality[a])[age][len]);
        }

        // Do: z <- c(v,w)
        Rcpp::NumericVector z(v.size() + w.size());

        std::copy(v.begin(), v.end(), z.begin());
        std::copy(w.begin(), w.end(), z.begin() + v.size());

        // Append at the end of the DataFrame
        df.insert(df.end(), z);
      }
    }
  }

  // Give names to the columns
  Rcpp::CharacterVector namevec = Rcpp::CharacterVector::create("year", "step",
     "area", "age", "length", "numberConsumed", "biomassConsumed", "mortality");

  // Convert to matrix so that we can transpose it (nrows = 8)
  int dfsize = df.size();
  Rcpp::NumericMatrix mattemp(8, dfsize);
  for ( i = 0; i < dfsize; i++ ) {
      mattemp(Rcpp::_, i) = Rcpp::NumericVector(df[i]);
  }

  Rcpp::rownames(mattemp) = namevec;

  Rcpp::NumericMatrix mout = Rcpp::transpose(mattemp);

  return mout;
}
