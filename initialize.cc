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
#include "predpreystdprinter.h"
#include "likelihoodprinter.h"
#include "summaryprinter.h"
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
#include "catchinkilos.h"

extern ErrorHandler handle;

void Ecosystem::Initialise() {
  PreyPtrVector preyvec;
  PredatorPtrVector predvec;
  int i, j;

  //first check that the names of the components are unique
  for (i = 0; i < fleetvec.Size(); i++)
    for (j = 0; j < fleetvec.Size(); j++)
      if ((strcasecmp(fleetvec[i]->getName(), fleetvec[j]->getName()) == 0) && (i != j))
        handle.logFailure("Error in input files - repeated fleet", fleetvec[i]->getName());

  for (i = 0; i < tagvec.Size(); i++)
    for (j = 0; j < tagvec.Size(); j++)
      if ((strcasecmp(tagvec[i]->getName(), tagvec[j]->getName()) == 0) && (i != j))
        handle.logFailure("Error in input files - repeated tagging experiment", tagvec[i]->getName());

  for (i = 0; i < otherfoodvec.Size(); i++)
    for (j = 0; j < otherfoodvec.Size(); j++)
      if ((strcasecmp(otherfoodvec[i]->getName(), otherfoodvec[j]->getName()) == 0) && (i != j))
        handle.logFailure("Error in input files - repeated other food", otherfoodvec[i]->getName());

  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < stockvec.Size(); j++)
      if ((strcasecmp(stockvec[i]->getName(), stockvec[j]->getName()) == 0) && (i != j))
        handle.logFailure("Error in input files - repeated stock", stockvec[i]->getName());

  for (i = 0; i < likevec.Size(); i++)
    for (j = 0; j < likevec.Size(); j++)
      if ((strcasecmp(likevec[i]->getName(), likevec[j]->getName()) == 0) && (i != j))
        handle.logFailure("Error in input files - repeated likelihood component", likevec[i]->getName());

  for (i = 0; i < printvec.Size(); i++)
    for (j = 0; j < printvec.Size(); j++)
      if ((strcasecmp(printvec[i]->getFileName(), printvec[j]->getFileName()) == 0) && (i != j))
        handle.logFailure("Error in input files - repeated print file", printvec[i]->getFileName());

  //OK, next create a list of all the predators and all the preys
  for (i = 0; i < stockvec.Size(); i++) {
    if (stockvec[i]->isEaten()) {
      preyvec.resize(1);
      preyvec[preyvec.Size() - 1] = stockvec[i]->returnPrey();
    }
    if (stockvec[i]->doesEat()) {
      predvec.resize(1);
      predvec[predvec.Size() - 1] = stockvec[i]->returnPredator();
    }
  }

  for (i = 0; i < otherfoodvec.Size(); i++) {
    preyvec.resize(1);
    preyvec[preyvec.Size() - 1] = otherfoodvec[i]->returnPrey();
  }

  for (i = 0; i < fleetvec.Size(); i++) {
    predvec.resize(1);
    predvec[predvec.Size() - 1] = fleetvec[i]->returnPredator();
  }

  //Now we can start initialising things
  for (i = 0; i < predvec.Size(); i++)
    predvec[i]->setPrey(preyvec, keeper);
  for (i = 0; i < stockvec.Size(); i++)
    stockvec[i]->setStock(stockvec);
  for (i = 0; i < stockvec.Size(); i++)
    stockvec[i]->setCI();
  for (i = 0; i < tagvec.Size(); i++)
    tagvec[i]->setStock(stockvec);

  //This is a good place to initialise the likelihood classes.
  for (i = 0; i < likevec.Size(); i++) {
    switch(likevec[i]->Type()) {
      case SURVEYINDICESLIKELIHOOD:
        handle.logMessage("Initialising likelihood component", likevec[i]->getName());
        ((SurveyIndices*)likevec[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case UNDERSTOCKINGLIKELIHOOD:
        handle.logMessage("Initialising likelihood component", likevec[i]->getName());
        ((UnderStocking*)likevec[i])->setPredators(predvec);
        break;
      case CATCHDISTRIBUTIONLIKELIHOOD:
        handle.logMessage("Initialising likelihood component", likevec[i]->getName());
        ((CatchDistribution*)likevec[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case CATCHSTATISTICSLIKELIHOOD:
        handle.logMessage("Initialising likelihood component", likevec[i]->getName());
        ((CatchStatistics*)likevec[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case STOMACHCONTENTLIKELIHOOD:
        handle.logMessage("Initialising likelihood component", likevec[i]->getName());
        ((StomachContent*)likevec[i])->setPredatorsAndPreys(predvec, preyvec);
        break;
      case TAGLIKELIHOOD:
        handle.logMessage("Initialising likelihood component", likevec[i]->getName());
        ((Recaptures*)likevec[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case STOCKDISTRIBUTIONLIKELIHOOD:
        handle.logMessage("Initialising likelihood component", likevec[i]->getName());
        ((StockDistribution*)likevec[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case MIGRATIONPENALTYLIKELIHOOD:
        handle.logMessage("Initialising likelihood component", likevec[i]->getName());
        ((MigrationPenalty*)likevec[i])->setStocks(stockvec);
        break;
      case CATCHINKILOSLIKELIHOOD:
        handle.logMessage("Initialising likelihood component", likevec[i]->getName());
        ((CatchInKilos*)likevec[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case RECSTATISTICSLIKELIHOOD:
        handle.logMessage("Initialising likelihood component", likevec[i]->getName());
        ((RecStatistics*)likevec[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case SURVEYDISTRIBUTIONLIKELIHOOD:
        handle.logMessage("Initialising likelihood component", likevec[i]->getName());
        ((SurveyDistribution*)likevec[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case BOUNDLIKELIHOOD:
        handle.logMessage("Initialising likelihood component", likevec[i]->getName());
        break;
      default:
        handle.logFailure("Error when initialising model - unrecognised likelihood type", likevec[i]->Type());
        break;
    }
  }

  //This is a good place to initialise the printer classes.
  for (i = 0; i < printvec.Size(); i++) {
    switch(printvec[i]->Type()) {
      case STOCKSTDPRINTER:
        handle.logMessage("Initialising printer for output file", printvec[i]->getFileName());
        ((StockStdPrinter*)(printvec[i]))->setStock(stockvec);
        break;
      case STOCKPRINTER:
        handle.logMessage("Initialising printer for output file", printvec[i]->getFileName());
        ((StockPrinter*)(printvec[i]))->setStock(stockvec);
        break;
      case PREDATORPRINTER:
        handle.logMessage("Initialising printer for output file", printvec[i]->getFileName());
        ((PredatorPrinter*)(printvec[i]))->setPredAndPrey(predvec, preyvec);
        break;
      case PREDATOROVERPRINTER:
        handle.logMessage("Initialising printer for output file", printvec[i]->getFileName());
        ((PredatorOverPrinter*)(printvec[i]))->setPredator(predvec);
        break;
      case PREYOVERPRINTER:
        handle.logMessage("Initialising printer for output file", printvec[i]->getFileName());
        ((PreyOverPrinter*)(printvec[i]))->setPrey(preyvec);
        break;
      case STOCKPREYFULLPRINTER:
        handle.logMessage("Initialising printer for output file", printvec[i]->getFileName());
        ((StockPreyFullPrinter*)(printvec[i]))->setStock(stockvec);
        break;
      case PREDPREYSTDPRINTER:
        handle.logMessage("Initialising printer for output file", printvec[i]->getFileName());
        ((PredPreyStdPrinter*)(printvec[i]))->setStocksAndPredAndPrey(stockvec, predvec, preyvec);
        break;
      case STOCKFULLPRINTER:
        handle.logMessage("Initialising printer for output file", printvec[i]->getFileName());
        ((StockFullPrinter*)(printvec[i]))->setStock(stockvec);
        break;
      case LIKELIHOODPRINTER:
        handle.logMessage("Initialising printer for output file", printvec[i]->getFileName());
        ((LikelihoodPrinter*)(printvec[i]))->setLikelihood(likevec);
        break;
      case LIKELIHOODSUMMARYPRINTER:
        handle.logMessage("Initialising printer for output file", printvec[i]->getFileName());
        ((SummaryPrinter*)(printvec[i]))->setLikelihood(likevec);
        break;
      default:
        handle.logFailure("Error when initialising model - unrecognised printer type", printvec[i]->Type());
        break;
    }
  }
}
