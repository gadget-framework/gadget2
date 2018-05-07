#include "ecosystem.h"
#include "renewal.h"
#include "stockprey.h"
#include "spawner.h"

#include <Rcpp.h>

extern Ecosystem* EcoSystem;

// [[Rcpp::export]]
Rcpp::List getEcosystemInfo() {

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
Rcpp::IntegerVector updateRenewalC(Rcpp::IntegerVector stockNo, Rcpp::IntegerVector year, Rcpp::IntegerVector step,
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

   if(!renewal){
     Rcpp::Rcout << "This stock doesn't renew" << std::endl;
     return Rcpp::IntegerVector(1,55);
   }


   int readoption = renewal->getReadOption();

#ifdef DEBUG
   Rcpp::Rcout << "Renewal type: " << readoption << std::endl;

   Rcpp::Rcout << "print data before" << std::endl;
   std::ofstream ofs;
   ofs.open ("/tmp/testofs-before.txt", std::ofstream::out);

   renewal->Print(ofs);
   ofs.close();
#endif

   if(readoption == 0){
     //Normal condition -- not implemented yet
   }else if(readoption == 1){
     //Normal parameter
     //Rcpp::Rcout << year[0] << step[0] << area[0] << age[0] << number[0] << mean[0] << sdev[0] << alpha[0] << beta[0] << std::endl;
     renewal->updateNormalParameterData(year[0], step[0], area[0], age[0], number[0], mean[0], sdev[0], alpha[0], beta[0],
       keeper, TimeInfo, Area, minage, maxage);
   }else{
     //Number
     renewal->updateNumberData(year[0], step[0], area[0], age[0], length[0], number[0], meanWeight[0],
       keeper, TimeInfo, Area, minage, maxage);
   }

#ifdef DEBUG
   Rcpp::Rcout << "print data after" << std::endl;
   ofs.open ("/tmp/testofs-after.txt", std::ofstream::out);

   renewal->Print(ofs);
   ofs.close();
#endif

   return Rcpp::IntegerVector(1, 0);
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
         Rcpp::Rcout << (*suit)[i][j] << " ";
      }
      Rcpp::Rcout << std::endl;
   }
#endif

   if(suit->Nrow() == 0){
     Rcpp::Rcout << "No suitability vector found" << std::endl;
     return Rcpp::IntegerVector(1,55);
   }

   const LengthGroupDivision* lengthGroup = predator->getLengthGroupDiv();

   lenIdx = lengthGroup->numLengthGroup(len[0]);

   if(lenIdx == -1){
     Rcpp::Rcout << "Invalid length" << std::endl;
   }

#ifdef DEBUG
   DoubleVector suitVal = (*suit)[0];

   Rcpp::Rcout << "Bef: " << suitVal[lenIdx] << " " << (*suit)[0][lenIdx] << std::endl;
#endif

   //Since predator length for fleet will always be one, pick only the first element
   (*suit)[0].set(lenIdx, value[0]);

#ifdef DEBUG
   Rcpp::Rcout << "Aft: " << suitVal[lenIdx] << " " << (*suit)[0][lenIdx] << std::endl;
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
     Rcpp::Rcout << "Error in predatorpreyaggregator - error when checking length structure" << std::endl;

   //check that the prey is a stock
   if (prey->getType() == LENGTHPREY)
     Rcpp::Rcout << "Error in predatorpreyaggregator - cannot aggregate prey" << prey->getName() << std::endl;

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

#ifdef DEBUG
  int width = 0;
  int lowwidth = 0;
  int printwidth = 0;
  int precision = 4;
#endif

  int a, age, len;

  Rcpp::DataFrame df;

  for (a = 0; a < areas.Size(); a++) {
    for (age = consume[a].minAge(); age <= consume[a].maxAge(); age++) {
      for (len = consume[a].minLength(age); len < consume[a].maxLength(age); len++) {
#ifdef DEBUG
        Rcpp::Rcout << setw(lowwidth) << TimeInfo->getPrevYear() << sep
          << setw(lowwidth) << TimeInfo->getPrevStep() << sep
          << setw(printwidth) << Area->getModelArea(a) << sep
          << setw(printwidth) << stock->minAge() + age << sep
          << setw(printwidth) << prey->getLengthGroupDiv()->minLength(len) << sep;
#endif
        Rcpp::NumericVector v = Rcpp::NumericVector::create(TimeInfo->getPrevYear(),
            TimeInfo->getPrevStep(), Area->getModelArea(a), stock->minAge() + age,
            prey->getLengthGroupDiv()->minLength(len) );

        Rcpp::NumericVector w;
        // JMB crude filter to remove the 'silly' values from the output
        if ((consume[a][age][len].N < rathersmall) || (consume[a][age][len].W < 0.0)){
#ifdef DEBUG
          Rcpp::Rcout << setw(width) << 0 << sep << setw(width) << 0 << sep << setw(width) << 0 << endl;
#endif
          w = Rcpp::NumericVector::create(0, 0, 0);
        }else{
#ifdef DEBUG
          Rcpp::Rcout << setprecision(precision) << setw(width) << consume[a][age][len].N
            << sep << setprecision(precision) << setw(width)
            << consume[a][age][len].N * consume[a][age][len].W
            << sep << setprecision(precision) << setw(width)
            << (*mortality[a])[age][len] << endl;
#endif
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

// [[Rcpp::export]]
Rcpp::NumericMatrix printStock(Rcpp::IntegerVector stockNo){

   int j, age, len;

   const AgeBandMatrix* alptr;

   TimeClass* TimeInfo = EcoSystem->getTimeInfo();

   AreaClass* Area = EcoSystem->getArea();
   IntVector areas = Area->getAllModelAreas();

   StockPtrVector stockvec = EcoSystem->getModelStockVector();
   Stock *stock = stockvec[stockNo[0]-1];

#ifdef DEBUG
   int width = 0;
   int lowwidth = 0;
   int precision = 4;
#endif

   Rcpp::DataFrame df;

   for (j = 0; j < areas.Size(); j++) {
     int a = Area->getInnerArea(areas[j]);
     alptr = &stock->getCurrentALK(a);
     for (age = alptr->minAge(); age <= alptr->maxAge(); age++) {
       for (len = alptr->minLength(age); len < alptr->maxLength(age); len++) {
#ifdef DEBUG
         Rcpp::Rcout << setw(lowwidth) << TimeInfo->getPrevYear() << sep
           << setw(lowwidth) << TimeInfo->getPrevStep() << sep
           << setw(lowwidth) << Area->getModelArea(a) << sep << setw(lowwidth)
           << age << sep << setw(lowwidth)
           << stock->getLengthGroupDiv()->minLength(len) << sep;
#endif
         Rcpp::NumericVector v = Rcpp::NumericVector::create(TimeInfo->getPrevYear(),
            TimeInfo->getPrevStep(), Area->getModelArea(a), age,
            stock->getLengthGroupDiv()->minLength(len));
         Rcpp::NumericVector w;

         //JMB crude filter to remove the 'silly' values from the output
         if (((*alptr)[age][len].N < rathersmall) || ((*alptr)[age][len].W < 0.0)){
#ifdef DEBUG
           Rcpp::Rcout << setw(width) << 0 << sep << setw(width) << 0 << endl;
#endif
           w = Rcpp::NumericVector::create(0, 0);
         }else{
#ifdef DEBUG
           Rcpp::Rcout << setprecision(precision) << setw(width) << (*alptr)[age][len].N << sep
             << setprecision(precision) << setw(width) << (*alptr)[age][len].W << endl;
#endif
           w = Rcpp::NumericVector::create((*alptr)[age][len].N, (*alptr)[age][len].W);
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
     "area", "age", "length", "number", "meanWeights");

  // Convert to matrix so that we can transpose it (nrows = 7)
  int dfsize = df.size();
  Rcpp::NumericMatrix mattemp(7, dfsize);
  for ( j = 0; j < dfsize; j++ ) {
      mattemp(Rcpp::_, j) = Rcpp::NumericVector(df[j]);
  }

  Rcpp::rownames(mattemp) = namevec;

  Rcpp::NumericMatrix mout = Rcpp::transpose(mattemp);

  return mout;
}

// [[Rcpp::export]]
Rcpp::NumericMatrix printDetailedSSB(Rcpp::IntegerVector stockNo){

   int j, age, len;

   const AgeBandMatrix* alptr;

   TimeClass* TimeInfo = EcoSystem->getTimeInfo();

   AreaClass* Area = EcoSystem->getArea();
   IntVector areas = Area->getAllModelAreas();

   StockPtrVector stockvec = EcoSystem->getModelStockVector();
   Stock *stock = stockvec[stockNo[0]-1];

   if(!stock->getSpawnData()){
	   Rcpp::NumericMatrix m(0,0);
	   return m;
   }

   const DoubleMatrixPtrVector* spawnNumbers = stock->getSpawnData()->getSpawnNumbers();

#ifdef DEBUG
   int width = 0;
   int lowwidth = 0;
   int precision = 4;
#endif

   Rcpp::DataFrame df;

   for (j = 0; j < areas.Size(); j++) {
     int a = Area->getInnerArea(areas[j]);
     alptr = &stock->getCurrentALK(a);
     for (age = alptr->minAge(); age <= alptr->maxAge(); age++) {
       for (len = alptr->minLength(age); len < alptr->maxLength(age); len++) {
#ifdef DEBUG
         Rcpp::Rcout << setw(lowwidth) << TimeInfo->getPrevYear() << sep
           << setw(lowwidth) << TimeInfo->getPrevStep() << sep
           << setw(lowwidth) << Area->getModelArea(a) << sep << setw(lowwidth)
           << age << sep << setw(lowwidth)
           << stock->getLengthGroupDiv()->minLength(len) << sep;
#endif
         Rcpp::NumericVector v = Rcpp::NumericVector::create(TimeInfo->getPrevYear(),
            TimeInfo->getPrevStep(), Area->getModelArea(a), age,
            stock->getLengthGroupDiv()->minLength(len));
         Rcpp::NumericVector w;

         if (((*(*spawnNumbers)[a])[age][len]  < rathersmall) || ((*(*spawnNumbers)[a])[age][len]< 0.0)){
#ifdef DEBUG
           Rcpp::Rcout << setw(width) << 0 << endl;
#endif
           w = Rcpp::NumericVector::create(0);
         }else{
#ifdef DEBUG
           Rcpp::Rcout << setprecision(precision) << setw(width) <<  (*(*spawnNumbers)[a])[age][len] << endl;
#endif
           w = Rcpp::NumericVector::create((*(*spawnNumbers)[a])[age][len]);
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
     "area", "age", "length", "SSB");

  // Convert to matrix so that we can transpose it (nrows = 6)
  int dfsize = df.size();
  Rcpp::NumericMatrix mattemp(6, dfsize);
  for ( j = 0; j < dfsize; j++ ) {
      mattemp(Rcpp::_, j) = Rcpp::NumericVector(df[j]);
  }

  Rcpp::rownames(mattemp) = namevec;

  Rcpp::NumericMatrix mout = Rcpp::transpose(mattemp);

  return mout;
}

// [[Rcpp::export]]
Rcpp::IntegerVector updateRecruitementC(Rcpp::IntegerVector stockNo, Rcpp::NumericVector recruitParams){

   int j;
   int functionnumber, paramsize;

   AreaClass* Area = EcoSystem->getArea();
   IntVector areas = Area->getAllModelAreas();

   StockPtrVector stockvec = EcoSystem->getModelStockVector();
   Stock *stock = stockvec[stockNo[0]-1];

   if(!stock->getSpawnData()){
	   return Rcpp::IntegerVector(1,55);
   }

   ModelVariableVector* spawnparameters = stock->getSpawnData()->getSpawnParameters();
   const char* functionname = stock->getSpawnData()->getFunctionName();

   // Determine the function type and its number of parameters
   if (strcasecmp(functionname, "simplessb") == 0) {
     functionnumber = 1;
     paramsize = 1;
   } else if (strcasecmp(functionname, "ricker") == 0) {
     functionnumber = 2;
     paramsize = 2;
   } else if (strcasecmp(functionname, "bevertonholt") == 0) {
     functionnumber = 3;
     paramsize = 2;
   } else if (strcasecmp(functionname, "fecundity") == 0) {
     functionnumber = 4;
     paramsize = 5;
   } else if (strcasecmp(functionname, "baleen") == 0) {
     functionnumber = 5;
     paramsize = 4;
   } else if (strcasecmp(functionname, "hockeystick") == 0) {
     functionnumber = 6;
     paramsize = 2;
   } else {
     Rcpp::Rcout << "unrecognised recruitment function " << functionname << std::endl;
     return Rcpp::IntegerVector(1,55);
   }

   // Check the length of user-defined parameters
   if(recruitParams.length() != paramsize){
      Rcpp::Rcout << "Recruitment function " << functionname << " requires "
         << paramsize << " parameters!" << std::endl;
      return Rcpp::IntegerVector(1,55);
   }

#ifdef DEBUG
   Rcpp::Rcout << functionname << " " << functionnumber << endl;
   for (j = 0 ; j < paramsize ; j++)
      Rcpp::Rcout << (*spawnparameters)[j] << " ";
   Rcpp::Rcout << endl;
#endif

   for (j = 0 ; j < paramsize ; j++)
      (*spawnparameters)[j].setValue(recruitParams[j]);

#ifdef DEBUG
   Rcpp::Rcout << functionname << " " << functionnumber << endl;
   for (j = 0 ; j < paramsize ; j++)
      Rcpp::Rcout << (*spawnparameters)[j] << " ";
   Rcpp::Rcout << endl;
#endif

   return Rcpp::IntegerVector(1,0);
}

// [[Rcpp::export]]
Rcpp::NumericMatrix printSSB(Rcpp::IntegerVector stockNo){

   int j;

   TimeClass* TimeInfo = EcoSystem->getTimeInfo();

   AreaClass* Area = EcoSystem->getArea();
   IntVector areas = Area->getAllModelAreas();

   StockPtrVector stockvec = EcoSystem->getModelStockVector();
   Stock *stock = stockvec[stockNo[0]-1];

   SpawnData* spawner = stock->getSpawnData();
   if(!spawner){
	   Rcpp::NumericMatrix m(0,0);
	   return m;
   }

   Rcpp::DataFrame df;

   for (j = 0; j < areas.Size(); j++) {
      int a = Area->getInnerArea(areas[j]);
      Rcpp::NumericVector v;
      if(spawner->isSpawnStepAreaPrev(a, TimeInfo)){
          v = Rcpp::NumericVector::create(TimeInfo->getPrevYear(),
            TimeInfo->getPrevStep(), Area->getModelArea(a), spawner->getSSB()[a]);
      }else{
          v = Rcpp::NumericVector::create(TimeInfo->getPrevYear(),
            TimeInfo->getPrevStep(), Area->getModelArea(a), 0.0);
      }
      // Append at the end of the DataFrame
      df.insert(df.end(), v);
   }

   // Give names to the columns
   Rcpp::CharacterVector namevec = Rcpp::CharacterVector::create("year", "step",
     "area", "SSB");

   // Convert to matrix so that we can transpose it (nrows = 4)
   int dfsize = df.size();
   Rcpp::NumericMatrix mattemp(4, dfsize);
   for ( j = 0; j < dfsize; j++ ) {
      mattemp(Rcpp::_, j) = Rcpp::NumericVector(df[j]);
   }

   Rcpp::rownames(mattemp) = namevec;

   Rcpp::NumericMatrix mout = Rcpp::transpose(mattemp);

   return mout;
}

// [[Rcpp::export]]
Rcpp::NumericMatrix printRecruitment(Rcpp::IntegerVector stockNo){

   int j;

   TimeClass* TimeInfo = EcoSystem->getTimeInfo();

   AreaClass* Area = EcoSystem->getArea();
   IntVector areas = Area->getAllModelAreas();

   StockPtrVector stockvec = EcoSystem->getModelStockVector();
   Stock *stock = stockvec[stockNo[0]-1];

   SpawnData* spawner = stock->getSpawnData();
   if(!spawner){
	   Rcpp::NumericMatrix m(0,0);
	   return m;
   }

   Rcpp::DataFrame df;

   for (j = 0; j < areas.Size(); j++) {
      int a = Area->getInnerArea(areas[j]);
      Rcpp::NumericVector v;
      if(spawner->isSpawnStepAreaPrev(a, TimeInfo)){
         v = Rcpp::NumericVector::create(TimeInfo->getPrevYear(),
            TimeInfo->getPrevStep(), Area->getModelArea(a), spawner->getRec()[a]);
      }else{
         v = Rcpp::NumericVector::create(TimeInfo->getPrevYear(),
            TimeInfo->getPrevStep(), Area->getModelArea(a), 0.0);
      }
      // Append at the end of the DataFrame
      df.insert(df.end(), v);
   }

   // Give names to the columns
   Rcpp::CharacterVector namevec = Rcpp::CharacterVector::create("year", "step",
     "area", "Rec");

   // Convert to matrix so that we can transpose it (nrows = 4)
   int dfsize = df.size();
   Rcpp::NumericMatrix mattemp(4, dfsize);
   for ( j = 0; j < dfsize; j++ ) {
      mattemp(Rcpp::_, j) = Rcpp::NumericVector(df[j]);
   }

   Rcpp::rownames(mattemp) = namevec;

   Rcpp::NumericMatrix mout = Rcpp::transpose(mattemp);

   return mout;
}
