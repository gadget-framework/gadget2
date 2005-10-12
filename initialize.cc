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
        handle.logMessage(LOGFAIL, "Error in input files - repeated fleet", fleetvec[i]->getName());

  for (i = 0; i < tagvec.Size(); i++)
    for (j = 0; j < tagvec.Size(); j++)
      if ((strcasecmp(tagvec[i]->getName(), tagvec[j]->getName()) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in input files - repeated tagging experiment", tagvec[i]->getName());

  for (i = 0; i < otherfoodvec.Size(); i++)
    for (j = 0; j < otherfoodvec.Size(); j++)
      if ((strcasecmp(otherfoodvec[i]->getName(), otherfoodvec[j]->getName()) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in input files - repeated other food", otherfoodvec[i]->getName());

  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < stockvec.Size(); j++)
      if ((strcasecmp(stockvec[i]->getName(), stockvec[j]->getName()) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in input files - repeated stock", stockvec[i]->getName());

  for (i = 0; i < likevec.Size(); i++)
    for (j = 0; j < likevec.Size(); j++)
      if ((strcasecmp(likevec[i]->getName(), likevec[j]->getName()) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in input files - repeated likelihood component", likevec[i]->getName());

  for (i = 0; i < printvec.Size(); i++)
    for (j = 0; j < printvec.Size(); j++)
      if ((strcasecmp(printvec[i]->getFileName(), printvec[j]->getFileName()) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in input files - repeated print file", printvec[i]->getFileName());

  //OK, next create a list of all the predators and all the preys
  for (i = 0; i < stockvec.Size(); i++) {
    if (stockvec[i]->isEaten())
      preyvec.resize(1, stockvec[i]->getPrey());
    if (stockvec[i]->doesEat())
      predvec.resize(1, stockvec[i]->getPredator());
  }

  for (i = 0; i < otherfoodvec.Size(); i++)
    preyvec.resize(1, otherfoodvec[i]->getPrey());
  for (i = 0; i < fleetvec.Size(); i++)
    predvec.resize(1, fleetvec[i]->getPredator());

  //Now we can start initialising things
  for (i = 0; i < predvec.Size(); i++) {
    handle.logMessage(LOGMESSAGE, "Initialising predator", predvec[i]->getName());
    ((PopPredator*)predvec[i])->setPrey(preyvec, keeper);
  }
  for (i = 0; i < stockvec.Size(); i++) {
    handle.logMessage(LOGMESSAGE, "Initialising stock", stockvec[i]->getName());
    stockvec[i]->setStock(stockvec);
  }
  for (i = 0; i < tagvec.Size(); i++) {
    handle.logMessage(LOGMESSAGE, "Initialising tagging experiment", tagvec[i]->getName());
    tagvec[i]->setStock(stockvec);
  }

  basevec.resize(stockvec.Size() + otherfoodvec.Size() + fleetvec.Size(), 0);
  for (i = 0; i < stockvec.Size(); i++)
    basevec[i] = stockvec[i];
  for (i = 0; i < otherfoodvec.Size(); i++)
    basevec[i + stockvec.Size()] = otherfoodvec[i];
  for (i = 0; i < fleetvec.Size(); i++)
    basevec[i + stockvec.Size() + otherfoodvec.Size()] = fleetvec[i];

  //This is a good place to initialise the likelihood classes.
  for (i = 0; i < likevec.Size(); i++) {
    handle.logMessage(LOGMESSAGE, "Initialising likelihood component", likevec[i]->getName());
    switch (likevec[i]->getType()) {
      case SURVEYINDICESLIKELIHOOD:
        ((SurveyIndices*)likevec[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case UNDERSTOCKINGLIKELIHOOD:
        ((UnderStocking*)likevec[i])->setPredators(predvec);
        break;
      case CATCHDISTRIBUTIONLIKELIHOOD:
        ((CatchDistribution*)likevec[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case CATCHSTATISTICSLIKELIHOOD:
        ((CatchStatistics*)likevec[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case STOMACHCONTENTLIKELIHOOD:
        ((StomachContent*)likevec[i])->setPredatorsAndPreys(predvec, preyvec);
        break;
      case TAGLIKELIHOOD:
        ((Recaptures*)likevec[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case STOCKDISTRIBUTIONLIKELIHOOD:
        ((StockDistribution*)likevec[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case MIGRATIONPENALTYLIKELIHOOD:
        ((MigrationPenalty*)likevec[i])->setStocks(stockvec);
        break;
      case CATCHINKILOSLIKELIHOOD:
        ((CatchInKilos*)likevec[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case RECSTATISTICSLIKELIHOOD:
        ((RecStatistics*)likevec[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case SURVEYDISTRIBUTIONLIKELIHOOD:
        ((SurveyDistribution*)likevec[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case BOUNDLIKELIHOOD:
        break;
      default:
        handle.logMessage(LOGFAIL, "Error when initialising model - unrecognised likelihood type", likevec[i]->getType());
        break;
    }
  }

  //This is a good place to initialise the printer classes.
  for (i = 0; i < printvec.Size(); i++) {
    handle.logMessage(LOGMESSAGE, "Initialising printer for output file", printvec[i]->getFileName());
    switch (printvec[i]->getType()) {
      case STOCKSTDPRINTER:
        ((StockStdPrinter*)(printvec[i]))->setStock(stockvec, Area);
        break;
      case STOCKPRINTER:
        ((StockPrinter*)(printvec[i]))->setStock(stockvec, Area);
        break;
      case PREDATORPRINTER:
        ((PredatorPrinter*)(printvec[i]))->setPredAndPrey(predvec, preyvec, Area);
        break;
      case PREDATOROVERPRINTER:
        ((PredatorOverPrinter*)(printvec[i]))->setPredator(predvec, Area);
        break;
      case PREYOVERPRINTER:
        ((PreyOverPrinter*)(printvec[i]))->setPrey(preyvec, Area);
        break;
      case STOCKPREYFULLPRINTER:
        ((StockPreyFullPrinter*)(printvec[i]))->setStock(stockvec, Area);
        break;
      case PREDPREYSTDPRINTER:
        ((PredPreyStdPrinter*)(printvec[i]))->setStocksAndPredAndPrey(stockvec, predvec, preyvec, Area);
        break;
      case STOCKFULLPRINTER:
        ((StockFullPrinter*)(printvec[i]))->setStock(stockvec, Area);
        break;
      case LIKELIHOODPRINTER:
        ((LikelihoodPrinter*)(printvec[i]))->setLikelihood(likevec);
        break;
      case LIKELIHOODSUMMARYPRINTER:
        ((SummaryPrinter*)(printvec[i]))->setLikelihood(likevec);
        break;
      default:
        handle.logMessage(LOGFAIL, "Error when initialising model - unrecognised printer type", printvec[i]->getType());
        break;
    }
  }
}
