#include "ecosystem.h"
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
#include "formatedcatchprinter.h"
#include "likelihoodprinter.h"
#include "predpreystdprinter.h"
#include "mortprinter.h"
#include "biomassprinter.h"
#include "surveyindices.h"
#include "understocking.h"
#include "catchdistribution.h"
#include "catchstatistics.h"
#include "stomachcontent.h"
#include "tagdata.h"
#include "stockpredator.h"
#include "lengthpredator.h"
#include "stockdistribution.h"
#include "predatorindex.h"
#include "migrationpenalty.h"
#include "logcatchfunction.h"
#include "catchintons.h"

void Ecosystem::Initialize(int optimize) {
  Preyptrvector preyvec;
  Predatorptrvector predvec;
  PopPredatorptrvector poppredvec;
  int preyindex = 0;
  int predindex = 0;
  int poppredindex = 0;
  int i;

  for (i = 0; i < stockvec.Size(); i++) {
    if (stockvec[i]->IsEaten()) {
      preyvec.resize(1);
      preyvec[preyindex++] = stockvec[i]->ReturnPrey();
    }
    if (stockvec[i]->DoesEat()){
      predvec.resize(1);
      poppredvec.resize(1);
      predvec[predindex++] = stockvec[i]->ReturnPredator();
      poppredvec[poppredindex++] = stockvec[i]->ReturnPredator();
    }
  }

  for (i = 0; i < otherfoodvec.Size(); i++) {
    preyvec.resize(1);
    preyvec[preyindex++] = otherfoodvec[i]->ReturnPrey();
  }

  for (i = 0; i < fleetvec.Size(); i++) {
    predvec.resize(1);
    poppredvec.resize(1);
    predvec[predindex++] = fleetvec[i]->ReturnPredator();
    poppredvec[poppredindex++] = fleetvec[i]->ReturnPredator();
  }

  for (i = 0; i < predvec.Size(); i++)
    predvec[i]->SetPrey(preyvec, keeper);
  for (i = 0; i < stockvec.Size(); i++)
    stockvec[i]->SetCatch(catchdata);
  for (i = 0; i < stockvec.Size(); i++)
    stockvec[i]->SetStock(stockvec);
  for (i = 0; i < stockvec.Size(); i++)
    stockvec[i]->SetCI();
  for (i = 0; i < tagvec.Size(); i++)
    tagvec[i]->SetStock(stockvec);

  /*JMB code removed from here - see RemovedCode.txt for details*/
  //This is a good place to initialize the printer classes.
  for (i = 0; i < printvec.Size(); i++)
    switch(printvec[i]->Type()) {
      case STOCKSTDPRINTER:
        printvec[i]->SetStock(stockvec);
        break;
      case STOCKPRINTER:
        printvec[i]->SetStock(stockvec);
        break;
      case PREDATORPRINTER:
        printvec[i]->SetPredAndPrey(predvec, preyvec);
        break;
      case PREDATOROVERPRINTER:
        printvec[i]->SetPredator(predvec);
        break;
      case PREYOVERPRINTER:
        printvec[i]->SetPrey(preyvec);
        break;
      case STOCKPREYFULLPRINTER:
        printvec[i]->SetStock(stockvec);
        break;
      case PREDPREYSTDPRINTER:
        printvec[i]->SetStocksAndPredAndPrey(stockvec, poppredvec, preyvec);
        break;
      case STOCKFULLPRINTER:
        printvec[i]->SetStock(stockvec);
        break;
      case FORMATEDSTOCKPRINTER:
        printvec[i]->SetStock(stockvec);
        break;
      case FORMATEDCHATPRINTER:
        printvec[i]->SetFleet(fleetvec);
        break;
      case FORMATEDPREYPRINTER:
        printvec[i]->SetStock(stockvec);
        break;
      case MORTPRINTER:
        printvec[i]->SetStock(stockvec);
        break;
      case BIOMASSPRINTER:
        printvec[i]->SetStock(stockvec);
        break;
      default:
        cerr << "Unrecognized printer type " << printvec[i]->Type() << " when initializing\n";
        exit(EXIT_FAILURE);
        break;
    }

  if (optimize)
    for (i = 0; i < Likely.Size(); i++)
      switch(Likely[i]->Type()) {
        case SURVEYINDICESLIKELIHOOD:
          ((SurveyIndices*)Likely[i])->SetStocks(stockvec);
          break;
        case UNDERSTOCKINGLIKELIHOOD:
          ((UnderStocking*)Likely[i])->SetFleets(fleetvec);
          break;
        case CATCHDISTRIBUTIONLIKELIHOOD:
          ((CatchDistribution*)Likely[i])->SetFleetsAndStocks(fleetvec, stockvec);
          break;
        case LOGCATCHLIKELIHOOD:
          ((LogCatches*)Likely[i])->SetFleetsAndStocks(fleetvec, stockvec);
          break;
        case CATCHSTATISTICSLIKELIHOOD:
          ((CatchStatistics*)Likely[i])->SetFleetsAndStocks(fleetvec, stockvec);
          break;
        case STOMACHCONTENTLIKELIHOOD:
          ((StomachContent*)Likely[i])->SetPredatorsAndPreys(predvec, preyvec);
          break;
        case TAGLIKELIHOOD:
          ((TagData*)Likely[i])->SetFleetsAndStocks(fleetvec, stockvec);
          break;
        case STOCKDISTRIBUTIONLIKELIHOOD:
          ((StockDistribution*)Likely[i])->SetFleetsAndStocks(fleetvec, stockvec);
          break;
        case PREDATORINDICESLIKELIHOOD:
          ((PredatorIndices*)Likely[i])->SetPredatorsAndPreys(predvec, preyvec);
          break;
        case MIGRATIONPENALTYLIKELIHOOD:
          ((MigrationPenalty*)Likely[i])->SetStocks(stockvec);
          break;
        case CATCHINTONSLIKELIHOOD:
          ((CatchInTons*)Likely[i])->SetFleetsAndStocks(fleetvec, stockvec);
          break;
        case AGGREGATEDCDLIKELIHOOD:
          break;
        case LOGSURVEYLIKELIHOOD:
          break;
        case RANDOMWALKLIKELIHOOD:
          break;
        case BOUNDLIKELIHOOD:
          break;
        default:
          cerr << "Unrecognized likelihood type " << Likely[i]->Type() << " when initializing\n";
          exit(EXIT_FAILURE);
          break;
      }

  //Initialize likelihood printers in likprintvec [10.04.00 MNAA]
  //Moved this to the end, so the likelihood classes is initialized
  //before the printer classes [12.07.00]
  for (i = 0; i < likprintvec.Size(); i++)
    switch(likprintvec[i]->Type()) {
      case FORMATEDCATCHPRINTER:
        ((FormatedCatchPrinter*)(likprintvec[i]))->SetLikely(Likely);
        break;
      case LIKELIHOODPRINTER:
        ((LikelihoodPrinter*)(likprintvec[i]))->SetLikely(Likely);
        break;
      default:
        cerr << "Unrecognized likelihood printer type " << likprintvec[i]->Type() << " when initializing\n";
        exit(EXIT_FAILURE);
        break;
    }
}
