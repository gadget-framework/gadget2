#include "ecosystem.h"
#include "errorhandler.h"
#include "predator.h"
#include "prey.h"
#include "lengthprey.h"
#include "stockprey.h"
#include "preyptrvector.h"
#include "predatorptrvector.h"
#include "poppredatorptrvector.h"
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
#include "predatorindex.h"
#include "migrationpenalty.h"
#include "catchintons.h"

extern ErrorHandler handle;

void Ecosystem::Initialise(int optimize) {
  PreyPtrVector preyvec;
  PredatorPtrVector predvec;
  PopPredatorPtrVector poppredvec;
  int preyindex = 0;
  int predindex = 0;
  int poppredindex = 0;
  int i;

  for (i = 0; i < stockvec.Size(); i++) {
    if (stockvec[i]->IsEaten()) {
      preyvec.resize(1);
      preyvec[preyindex++] = stockvec[i]->returnPrey();
    }
    if (stockvec[i]->doesEat()) {
      predvec.resize(1);
      poppredvec.resize(1);
      predvec[predindex++] = stockvec[i]->returnPredator();
      poppredvec[poppredindex++] = stockvec[i]->returnPredator();
    }
  }

  for (i = 0; i < otherfoodvec.Size(); i++) {
    preyvec.resize(1);
    preyvec[preyindex++] = otherfoodvec[i]->returnPrey();
  }

  for (i = 0; i < fleetvec.Size(); i++) {
    predvec.resize(1);
    poppredvec.resize(1);
    predvec[predindex++] = fleetvec[i]->returnPredator();
    poppredvec[poppredindex++] = fleetvec[i]->returnPredator();
  }

  for (i = 0; i < predvec.Size(); i++)
    predvec[i]->setPrey(preyvec, keeper);
  for (i = 0; i < stockvec.Size(); i++)
    stockvec[i]->setStock(stockvec);
  for (i = 0; i < stockvec.Size(); i++)
    stockvec[i]->setCI();
  for (i = 0; i < tagvec.Size(); i++)
    tagvec[i]->setStock(stockvec);

  //This is a good place to initialise the printer classes.
  for (i = 0; i < printvec.Size(); i++)
    switch(printvec[i]->Type()) {
      case STOCKSTDPRINTER:
        printvec[i]->setStock(stockvec);
        break;
      case STOCKPRINTER:
        printvec[i]->setStock(stockvec);
        break;
      case PREDATORPRINTER:
        printvec[i]->setPredAndPrey(predvec, preyvec);
        break;
      case PREDATOROVERPRINTER:
        printvec[i]->setPredator(predvec);
        break;
      case PREYOVERPRINTER:
        printvec[i]->setPrey(preyvec);
        break;
      case STOCKPREYFULLPRINTER:
        printvec[i]->setStock(stockvec);
        break;
      case PREDPREYSTDPRINTER:
        printvec[i]->setStocksAndPredAndPrey(stockvec, poppredvec, preyvec);
        break;
      case STOCKFULLPRINTER:
        printvec[i]->setStock(stockvec);
        break;
      case FORMATEDSTOCKPRINTER:
        printvec[i]->setStock(stockvec);
        break;
      case FORMATEDCHATPRINTER:
        printvec[i]->setFleet(fleetvec);
        break;
      case FORMATEDPREYPRINTER:
        printvec[i]->setStock(stockvec);
        break;
      case MORTPRINTER:
        printvec[i]->setStock(stockvec);
        break;
      case BIOMASSPRINTER:
        printvec[i]->setStock(stockvec);
        break;
      default:
        handle.logFailure("Error when initialising model - unrecognized printer type", printvec[i]->Type());
        break;
    }

  if (optimize)
    for (i = 0; i < Likely.Size(); i++)
      switch(Likely[i]->Type()) {
        case SURVEYINDICESLIKELIHOOD:
          ((SurveyIndices*)Likely[i])->setStocks(stockvec);
          break;
        case UNDERSTOCKINGLIKELIHOOD:
          ((UnderStocking*)Likely[i])->setFleets(fleetvec);
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
        case PREDATORINDICESLIKELIHOOD:
          ((PredatorIndices*)Likely[i])->setPredatorsAndPreys(predvec, preyvec);
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
        case AGGREGATEDCDLIKELIHOOD:
          break;
        case LOGCATCHLIKELIHOOD:
          break;
        case LOGSURVEYLIKELIHOOD:
          break;
        case RANDOMWALKLIKELIHOOD:
          break;
        case BOUNDLIKELIHOOD:
          break;
        default:
          handle.logFailure("Error when initialising model - unrecognized likelihood type", Likely[i]->Type());
          break;
      }

  //Initialise likelihood printers in likprintvec [10.04.00 MNAA]
  //Moved this to the end, so the likelihood classes are initialised
  //before the printer classes [12.07.00]
  for (i = 0; i < likprintvec.Size(); i++)
    switch(likprintvec[i]->Type()) {
      case FORMATEDCATCHPRINTER:
        break;
      case LIKELIHOODPRINTER:
        ((LikelihoodPrinter*)(likprintvec[i]))->setLikely(Likely);
        break;
      default:
        handle.logFailure("Error when initialising model - unrecognized printer type", likprintvec[i]->Type());
        break;
    }
}
