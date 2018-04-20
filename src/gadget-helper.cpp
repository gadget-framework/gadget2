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
Rcpp::IntegerVector updateRecruitment(Rcpp::CharacterVector stockName){

   StockPtrVector stockvec = EcoSystem->getModelStockVector();

   // Find the stock based on the name
   int stockNo=-1;

   for(int i=0; i < stockvec.Size(); i++){
      const char* stName = stockvec[i]->getName();
      if(strcmp(stName,(const char*)stockName[0])==0){
         stockNo = i;
         break;
      }
   }

   if(stockNo < 0){
     std::cout << "No stock found" << std::endl;
     return Rcpp::IntegerVector(1,55);
   }

   Stock *stock = stockvec[stockNo];
   
   RenewalData* renewal = stock->getRenewalData();

   // Important vars:
   //
   // renewalTime[i]
   // renewalArea[i]
   // renewalAge[i]
   // renewalDistribution[i]
   // renewalMult[i]
   // *CI
   std::ofstream ofs;
   ofs.open ("/tmp/testofs.txt", std::ofstream::out);

   renewal->Print(ofs);
   ofs.close();


   return Rcpp::IntegerVector(1,0);

}

// [[Rcpp::export]]
Rcpp::IntegerVector updateSuitability(Rcpp::IntegerVector fleetNo, Rcpp::CharacterVector stockName, Rcpp::IntegerVector len, Rcpp::NumericVector value){

   int lenIdx;
 
   FleetPtrVector& fleetvec = EcoSystem->getModelFleetVector();

   Fleet *fleet = fleetvec[fleetNo[0]-1];

   LengthPredator *predator = fleet->getPredator();

   // Find the suitability for the stockName prey
 
   int preyNo=-1;

   for(int i=0; i<predator->numPreys(); i++){
      const char* preyname = predator->getPreyName(i);
      if(strcmp(preyname,(const char*)stockName[0])==0){
         preyNo = i;
         break;
      }
   }

   if(preyNo < 0){
     std::cout << "No suitable prey found" << std::endl;
     return Rcpp::IntegerVector(1,55);
   }

   DoubleMatrix *suit = predator->getSuits(preyNo);

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

   if(len[0] > suit->Ncol(0) || len[0] < 1){
     std::cout << "The length index is out of the range" << std::endl;
     return Rcpp::IntegerVector(1,55);
   }else{
     lenIdx = len[0]-1;
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

