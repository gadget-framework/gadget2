#include "areatime.h"
#include "errorhandler.h"
#include "likelihoodprinter.h"
#include "charptrvector.h"
#include "readword.h"
#include "gadget.h"
#include "runid.h"

extern RunID RUNID;
extern ErrorHandler handle;

LikelihoodPrinter::LikelihoodPrinter(CommentStream& infile, const TimeClass* const TimeInfo)
  : Printer(LIKELIHOODPRINTER), like(0) {

  int i = 0;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  //read in the names of the likelihood components
  infile >> text >> ws;
  if (!(strcasecmp(text, "likelihood") == 0))
    handle.logFileUnexpected(LOGFAIL, "likelihood", text);
  infile >> text >> ws;
  while (!infile.eof() && !(strcasecmp(text, "printfile") == 0)) {
    likenames.resize(1, new char[strlen(text) + 1]);
    strcpy(likenames[i++], text);
    infile >> text >> ws;
  }
  if (likenames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "\nError in likelihoodprinter - failed to read component");

  //read the name of the printfile, and then open the printfile
  filename = new char[MaxStrLength];
  strncpy(filename, "", MaxStrLength);

  if (!(strcasecmp(text, "printfile") == 0))
    handle.logFileUnexpected(LOGFAIL, "printfile", text);
  infile >> filename >> ws;
  outfile.open(filename, ios::out);
  handle.checkIfFailure(outfile, filename);

  infile >> text >> ws;
  if (strcasecmp(text, "printatstart") == 0)
    infile >> printtimeid >> ws >> text >> ws;
  else
    printtimeid = 0;

  if (printtimeid != 0 && printtimeid != 1)
    handle.logFileMessage(LOGFAIL, "\nError in likelihoodprinter - invalid value of printatstart");

  if (printtimeid == 1)
    handle.logFileMessage(LOGFAIL, "\nError in likelihoodprinter - cannot print likelihood informtion at the start of the timestep");

  if (!(strcasecmp(text, "yearsandsteps") == 0))
    handle.logFileUnexpected(LOGFAIL, "yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
    handle.logFileMessage(LOGFAIL, "\nError in likelihoodprinter - wrong format for yearsandsteps");

  //prepare for next printfile component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[component]") == 0))
      handle.logFileUnexpected(LOGFAIL, "[component]", text);
  }
}

LikelihoodPrinter::~LikelihoodPrinter() {
  outfile.close();
  outfile.clear();
  int i;
  for (i = 0; i < likenames.Size(); i++)
    delete[] likenames[i];
}

void LikelihoodPrinter::setLikelihood(LikelihoodPtrVector& likevec) {
  int i, j;

  for (i = 0; i < likevec.Size(); i++)
    for (j = 0; j < likenames.Size(); j++)
      if (strcasecmp(likevec[i]->getName(), likenames[j]) == 0)
        like.resize(1, likevec[i]);

  if (like.Size() != likenames.Size()) {
    handle.logMessage(LOGWARN, "Error in likelihoodprinter - failed to match likelihood components");
    for (i = 0; i < like.Size(); i++)
      handle.logMessage(LOGWARN, "Error in likelihoodprinter - found component", like[i]->getName());
    for (i = 0; i < likenames.Size(); i++)
      handle.logMessage(LOGWARN, "Error in likelihoodprinter - looking for component", likenames[i]);
    exit(EXIT_FAILURE);
  }

  outfile << "; ";
  RUNID.Print(outfile);

  for (i = 0; i < like.Size(); i++) {
    outfile << "; Likelihood output file for the likelihood component " << like[i]->getName();
    switch (like[i]->getType()) {
      case CATCHDISTRIBUTIONLIKELIHOOD:
        outfile << "\n; year-step-area-age-length-number\n";
        break;
      case CATCHSTATISTICSLIKELIHOOD:
        outfile << "\n; year-step-area-age-number-mean[-stddev]\n";
        break;
      case CATCHINKILOSLIKELIHOOD:
        outfile << "\n; year-step-area-fleet-number\n";
        break;
      case SURVEYDISTRIBUTIONLIKELIHOOD:
        outfile << "\n; year-step-area-age-length-number\n";
        break;
      case STOCKDISTRIBUTIONLIKELIHOOD:
        outfile << "\n; year-step-area-stock-age-length-number\n";
        break;
      case STOMACHCONTENTLIKELIHOOD:
        outfile << "\n; year-step-area-predator-prey-number\n";
        break;
      case SURVEYINDICESLIKELIHOOD:
        outfile << "\n; year-step-area-label-number\n";
        break;
      case TAGLIKELIHOOD:
        outfile << "\n; tagid-year-step-area-length-number\n";
        break;
      case RECSTATISTICSLIKELIHOOD:
        outfile << "\n; tagid-year-step-area-number-mean[-stddev]\n";
        break;
      case BOUNDLIKELIHOOD:
      case UNDERSTOCKINGLIKELIHOOD:
      case MIGRATIONPENALTYLIKELIHOOD:
        handle.logMessage(LOGWARN, "Warning in likelihoodprinter - printing not implemented for", like[i]->getName());
        break;
      default:
        handle.logMessage(LOGFAIL, "Error in likelihoodprinter - unrecognised likelihood type", like[i]->getType());
        break;
    }
  }
  outfile.flush();
}

void LikelihoodPrinter::Print(const TimeClass* const TimeInfo, int printtime) {
  if ((!AAT.atCurrentTime(TimeInfo)) || (printtime != printtimeid))
    return;

  int i;
  for (i = 0; i < like.Size(); i++)
    like[i]->printLikelihood(outfile, TimeInfo);
  outfile.flush();
}
