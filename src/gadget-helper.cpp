#include "ecosystem.h"
#include "renewal.h"

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

