#include "ecosystem.h"
#include "errorhandler.h"
#include "predator.h"
#include "prey.h"
#include "lengthprey.h"
#include "stockprey.h"
#include "preyptrvector.h"
#include "predatorptrvector.h"
#include "stockprinter.h"
#include "stockstdprinter.h"
#include "predatorprinter.h"
#include "predatoroverprinter.h"
#include "preyoverprinter.h"
#include "stockpreyfullprinter.h"
#include "stockfullprinter.h"
#include "formatedstockprinter.h"
#include "formatedchatprinter.h"
#include "formatedpreyprinter.h"
#include "likelihoodprinter.h"
#include "predpreystdprinter.h"
#include "mortprinter.h"
#include "biomassprinter.h"
#include "surveyindices.h"
#include "understocking.h"
#include "catchdistribution.h"
#include "catchstatistics.h"
#include "recstatistics.h"
#include "stomachcontent.h"
#include "recapture.h"
#include "stockpredator.h"
#include "lengthpredator.h"
#include "stockdistribution.h"
#include "surveydistribution.h"
#include "migrationpenalty.h"
#include "catchintons.h"

extern ErrorHandler handle;

void Ecosystem::Initialise() {
  PreyPtrVector preyvec;
  PredatorPtrVector predvec;
  int preyindex = 0;
  int predindex = 0;
  int i;

  for (i = 0; i < stockvec.Size(); i++) {
    if (stockvec[i]->isEaten()) {
      preyvec.resize(1);
      preyvec[preyindex++] = stockvec[i]->returnPrey();
    }
    if (stockvec[i]->doesEat()) {
      predvec.resize(1);
      predvec[predindex++] = stockvec[i]->returnPredator();
    }
  }

  for (i = 0; i < otherfoodvec.Size(); i++) {
    preyvec.resize(1);
    preyvec[preyindex++] = otherfoodvec[i]->returnPrey();
  }

  for (i = 0; i < fleetvec.Size(); i++) {
    predvec.resize(1);
    predvec[predindex++] = fleetvec[i]->returnPredator();
  }

  for (i = 0; i < predvec.Size(); i++)
    predvec[i]->setPrey(preyvec, keeper);
  for (i = 0; i < stockvec.Size(); i++)
    stockvec[i]->setStock(stockvec);
  for (i = 0; i < stockvec.Size(); i++)
    stockvec[i]->setCI();
  for (i = 0; i < tagvec.Size(); i++)
    tagvec[i]->setStock(stockvec);

  //This is a good place to initialise the likelihood classes.
  for (i = 0; i < Likely.Size(); i++) {
    switch(Likely[i]->Type()) {
      case SURVEYINDICESLIKELIHOOD:
        ((SurveyIndices*)Likely[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case UNDERSTOCKINGLIKELIHOOD:
        ((UnderStocking*)Likely[i])->setPredators(predvec);
        break;
      case CATCHDISTRIBUTIONLIKELIHOOD:
        ((CatchDistribution*)Likely[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case CATCHSTATISTICSLIKELIHOOD:
        ((CatchStatistics*)Likely[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case STOMACHCONTENTLIKELIHOOD:
        ((StomachContent*)Likely[i])->setPredatorsAndPreys(predvec, preyvec);
        break;
      case TAGLIKELIHOOD:
        ((Recaptures*)Likely[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case STOCKDISTRIBUTIONLIKELIHOOD:
        ((StockDistribution*)Likely[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case MIGRATIONPENALTYLIKELIHOOD:
        ((MigrationPenalty*)Likely[i])->setStocks(stockvec);
        break;
      case CATCHINTONSLIKELIHOOD:
        ((CatchInTons*)Likely[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case RECSTATISTICSLIKELIHOOD:
        ((RecStatistics*)Likely[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case SURVEYDISTRIBUTIONLIKELIHOOD:
        ((SurveyDistribution*)Likely[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case BOUNDLIKELIHOOD:
        break;
      default:
        handle.logFailure("Error when initialising model - unrecognised likelihood type", Likely[i]->Type());
        break;
    }
  }

  //This is a good place to initialise the printer classes.
  for (i = 0; i < printvec.Size(); i++) {
    switch(printvec[i]->Type()) {
      case STOCKSTDPRINTER:
        ((StockStdPrinter*)(printvec[i]))->setStock(stockvec);
        break;
      case STOCKPRINTER:
        ((StockPrinter*)(printvec[i]))->setStock(stockvec);
        break;
      case PREDATORPRINTER:
        ((PredatorPrinter*)(printvec[i]))->setPredAndPrey(predvec, preyvec);
        break;
      case PREDATOROVERPRINTER:
        ((PredatorOverPrinter*)(printvec[i]))->setPredator(predvec);
        break;
      case PREYOVERPRINTER:
        ((PreyOverPrinter*)(printvec[i]))->setPrey(preyvec);
        break;
      case STOCKPREYFULLPRINTER:
        ((StockPreyFullPrinter*)(printvec[i]))->setStock(stockvec);
        break;
      case PREDPREYSTDPRINTER:
        ((PredPreyStdPrinter*)(printvec[i]))->setStocksAndPredAndPrey(stockvec, predvec, preyvec);
        break;
      case STOCKFULLPRINTER:
        ((StockFullPrinter*)(printvec[i]))->setStock(stockvec);
        break;
      case FORMATEDSTOCKPRINTER:
        ((FormatedStockPrinter*)(printvec[i]))->setStock(stockvec);
        break;
      case FORMATEDCHATPRINTER:
        ((FormatedCHatPrinter*)(printvec[i]))->setFleet(fleetvec);
        break;
      case FORMATEDPREYPRINTER:
        ((FormatedPreyPrinter*)(printvec[i]))->setStock(stockvec);
        break;
      case MORTPRINTER:
        ((MortPrinter*)(printvec[i]))->setStock(stockvec);
        break;
      case BIOMASSPRINTER:
        ((BiomassPrinter*)(printvec[i]))->setStock(stockvec);
        break;
      case LIKELIHOODPRINTER:
        ((LikelihoodPrinter*)(printvec[i]))->setLikely(Likely);
        break;
      default:
        handle.logFailure("Error when initialising model - unrecognised printer type", printvec[i]->Type());
        break;
    }
  }
}
