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
#include "stockpreyprinter.h"
#include "predatorprinter.h"
#include "predatorpreyprinter.h"
#include "predatoroverprinter.h"
#include "preyoverprinter.h"
#include "stockpreyfullprinter.h"
#include "stockfullprinter.h"
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
#include "migrationproportion.h"
#include "catchinkilos.h"
#include "proglikelihood.h"
#include "global.h"

void Ecosystem::Initialise() {
  PreyPtrVector preyvec;
  PredatorPtrVector predvec;
  int i, j, count;

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
      preyvec.resize(stockvec[i]->getPrey());
    if (stockvec[i]->doesEat())
      predvec.resize(stockvec[i]->getPredator());
  }

  for (i = 0; i < otherfoodvec.Size(); i++)
    preyvec.resize(otherfoodvec[i]->getPrey());
  for (i = 0; i < fleetvec.Size(); i++)
    predvec.resize(fleetvec[i]->getPredator());

  //Now we can start initialising things
  handle.logMessage(LOGMESSAGE, "");  //write blank line to log file
  for (i = 0; i < stockvec.Size(); i++) {
    handle.logMessage(LOGMESSAGE, "Initialising stock", stockvec[i]->getName());
    stockvec[i]->setStock(stockvec);
  }
  for (i = 0; i < predvec.Size(); i++) {
    handle.logMessage(LOGMESSAGE, "Initialising predator", predvec[i]->getName());
    ((PopPredator*)predvec[i])->setPrey(preyvec, keeper);
  }
  for (i = 0; i < tagvec.Size(); i++) {
    handle.logMessage(LOGMESSAGE, "Initialising tagging experiment", tagvec[i]->getName());
    tagvec[i]->setStock(stockvec);
  }

  //If there are any tagging experiments we need to resize objects to store them
  //This has to take place last, since some tagged stocks might not be set explicitly
  if (tagvec.Size() != 0)
    for (i = 0; i < stockvec.Size(); i++)
      if (stockvec[i]->isTagged())
        stockvec[i]->setTagged();

  for (i = 0; i < stockvec.Size(); i++)
    basevec.resize(stockvec[i]);
  for (i = 0; i < otherfoodvec.Size(); i++)
    basevec.resize(otherfoodvec[i]);
  for (i = 0; i < fleetvec.Size(); i++)
    basevec.resize(fleetvec[i]);

  //Next we initialise the likelihood classes
  count = 0;  //JMB count the number of understocking classes
  for (i = 0; i < likevec.Size(); i++) {
    handle.logMessage(LOGMESSAGE, "Initialising likelihood component", likevec[i]->getName());
    switch (likevec[i]->getType()) {
      case SURVEYINDICESLIKELIHOOD:
        ((SurveyIndices*)likevec[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case UNDERSTOCKINGLIKELIHOOD:
        ((UnderStocking*)likevec[i])->setPredatorsAndPreys(predvec, preyvec, Area);
        count++;
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
        ((MigrationPenalty*)likevec[i])->setFleetsAndStocks(fleetvec, stockvec);
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
      case MIGRATIONPROPORTIONLIKELIHOOD:
        ((MigrationProportion*)likevec[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      case BOUNDLIKELIHOOD:
        break;
      case PROGLIKELIHOOD:
        ((ProgLikelihood*)likevec[i])->setFleetsAndStocks(fleetvec, stockvec);
        break;
      default:
        handle.logMessage(LOGFAIL, "Error when initialising model - unrecognised likelihood type", likevec[i]->getType());
        break;
    }
  }

  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in input files - no understocking likelihood component found");
  if (count > 1)
    handle.logMessage(LOGWARN, "Warning in input files - repeated understocking components found");

  //Finally we initialise the printer classes
  for (i = 0; i < printvec.Size(); i++) {
    handle.logMessage(LOGMESSAGE, "Initialising printer for output file", printvec[i]->getFileName());
    switch (printvec[i]->getType()) {
      case STOCKSTDPRINTER:
        ((StockStdPrinter*)printvec[i])->setStock(stockvec, Area);
        break;
      case STOCKPRINTER:
        ((StockPrinter*)printvec[i])->setStock(stockvec, Area);
        break;
      case PREDATORPRINTER:
        ((PredatorPrinter*)printvec[i])->setPredAndPrey(predvec, preyvec, Area);
        break;
      case PREDATOROVERPRINTER:
        ((PredatorOverPrinter*)printvec[i])->setPredator(predvec, Area);
        break;
      case PREYOVERPRINTER:
        ((PreyOverPrinter*)printvec[i])->setPrey(preyvec, Area);
        break;
      case STOCKPREYPRINTER:
        ((StockPreyPrinter*)printvec[i])->setPrey(preyvec, Area);
        break;
      case STOCKPREYFULLPRINTER:
        ((StockPreyFullPrinter*)printvec[i])->setPrey(preyvec, Area);
        break;
      case PREDATORPREYPRINTER:
        ((PredatorPreyPrinter*)printvec[i])->setPredAndPrey(predvec, preyvec, Area);
        break;
      case STOCKFULLPRINTER:
        ((StockFullPrinter*)printvec[i])->setStock(stockvec, Area);
        break;
      case LIKELIHOODPRINTER:
        ((LikelihoodPrinter*)printvec[i])->setLikelihood(likevec);
        break;
      case LIKELIHOODSUMMARYPRINTER:
        ((SummaryPrinter*)printvec[i])->setLikelihood(likevec);
        break;
      default:
        handle.logMessage(LOGFAIL, "Error when initialising model - unrecognised printer type", printvec[i]->getType());
        break;
    }
  }
}
