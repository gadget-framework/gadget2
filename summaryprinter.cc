#include "areatime.h"
#include "errorhandler.h"
#include "summaryprinter.h"
#include "charptrvector.h"
#include "readword.h"
#include "gadget.h"
#include "runid.h"

extern RunID RUNID;
extern ErrorHandler handle;

SummaryPrinter::SummaryPrinter(CommentStream& infile)
  : Printer(LIKELIHOODSUMMARYPRINTER), like(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  printtimeid = 0;

  //Open the printfile
  char filename[MaxStrLength];
  strncpy(filename, "", MaxStrLength);
  readWordAndValue(infile, "printfile", filename);
  outfile.open(filename, ios::out);
  handle.checkIfFailure(outfile, filename);

  outfile << "; ";
  RUNID.print(outfile);
  // print header information
  outfile << "; Summary likelihood information from the current run" << endl
    << "; year-step-area-component-weight-likelihood value" << endl;
  outfile.flush();

  //prepare for next printfile component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[component]") == 0))
      handle.Unexpected("[component]", text);
  }
}

SummaryPrinter::~SummaryPrinter() {
  outfile.close();
  outfile.clear();
}

void SummaryPrinter::setLikelihood(LikelihoodPtrVector& likevec) {
  int i, index = 0;
  for (i = 0; i < likevec.Size(); i++) {
    like.resize(1);
    like[index++] = likevec[i];
  }

  for (i = 0; i < like.Size(); i++) {
    switch(like[i]->Type()) {
      case CATCHDISTRIBUTIONLIKELIHOOD:
      case CATCHSTATISTICSLIKELIHOOD:
      case CATCHINTONSLIKELIHOOD:
      case SURVEYDISTRIBUTIONLIKELIHOOD:
      case STOCKDISTRIBUTIONLIKELIHOOD:
      case STOMACHCONTENTLIKELIHOOD:
      case SURVEYINDICESLIKELIHOOD:
      case UNDERSTOCKINGLIKELIHOOD:
      case BOUNDLIKELIHOOD:
        break;
      case TAGLIKELIHOOD:
      case RECSTATISTICSLIKELIHOOD:
      case MIGRATIONPENALTYLIKELIHOOD:
        handle.logMessage("Warning in summaryprinter - printing not implemented for", like[i]->Name());
        break;
      default:
        handle.logFailure("Error in summaryprinter - unrecognised likelihood type", like[i]->Type());
        break;
    }
  }
}

void SummaryPrinter::Print(const TimeClass* const TimeInfo, int printtime) {

  if ((TimeInfo->CurrentTime() != TimeInfo->TotalNoSteps()) || (printtime != printtimeid))
    return;

  int i;
  for (i = 0; i < like.Size(); i++)
    like[i]->SummaryPrint(outfile);

  outfile.flush();
}
