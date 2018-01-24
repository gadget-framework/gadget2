#include "areatime.h"
#include "errorhandler.h"
#include "summaryprinter.h"
#include "charptrvector.h"
#include "readword.h"
#include "gadget.h"
#include "runid.h"
#include "global.h"

SummaryPrinter::SummaryPrinter(CommentStream& infile)
  : Printer(LIKELIHOODSUMMARYPRINTER) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  printtimeid = 0;

  //Open the printfile
  filename = new char[MaxStrLength];
  strncpy(filename, "", MaxStrLength);
  readWordAndValue(infile, "printfile", filename);
  outfile.open(filename, ios::out);
  handle.checkIfFailure(outfile, filename);

  outfile << "; ";
  RUNID.Print(outfile);
  // print header information
  outfile << "; Summary likelihood information from the current run" << endl
    << "; year-step-area-component-weight-likelihood value" << endl;
  outfile.flush();

  //prepare for next printfile component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (strcasecmp(text, "[component]") != 0)
      handle.logFileUnexpected(LOGFAIL, "[component]", text);
  }
}

SummaryPrinter::~SummaryPrinter() {
  outfile.close();
  outfile.clear();
}

// proglikelihood not working for this printer
void SummaryPrinter::setLikelihood(LikelihoodPtrVector& likevec) {
  int i;
  for (i = 0; i < likevec.Size(); i++)
    like.resize(likevec[i]);
  for (i = 0; i < like.Size(); i++) {
    switch (like[i]->getType()) {
      case CATCHDISTRIBUTIONLIKELIHOOD:
      case CATCHSTATISTICSLIKELIHOOD:
      case CATCHINKILOSLIKELIHOOD:
      case SURVEYDISTRIBUTIONLIKELIHOOD:
      case STOCKDISTRIBUTIONLIKELIHOOD:
      case STOMACHCONTENTLIKELIHOOD:
      case SURVEYINDICESLIKELIHOOD:
      case UNDERSTOCKINGLIKELIHOOD:
      case BOUNDLIKELIHOOD:
      case PROGLIKELIHOOD:
      case MIGRATIONPENALTYLIKELIHOOD:
      case MIGRATIONPROPORTIONLIKELIHOOD:
        break;
      case TAGLIKELIHOOD:
      case RECSTATISTICSLIKELIHOOD:
        handle.logMessage(LOGWARN, "Warning in summaryprinter - printing incomplete for", like[i]->getName());
        break;
      default:
        handle.logMessage(LOGFAIL, "Error in summaryprinter - unrecognised likelihood type", like[i]->getType());
        break;
    }
  }
}

void SummaryPrinter::Print(const TimeClass* const TimeInfo, int printtime) {
  if ((TimeInfo->getTime() != TimeInfo->numTotalSteps()) || (printtime != printtimeid))
    return;
  int i;
  for (i = 0; i < like.Size(); i++)
    like[i]->printSummary(outfile);
  outfile.flush();
}
