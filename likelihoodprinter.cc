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
    handle.Unexpected("likelihood", text);
  infile >> text >> ws;
  while (!infile.eof() && !(strcasecmp(text, "printfile") == 0)) {
    likenames.resize(1);
    likenames[i] = new char[strlen(text) + 1];
    strcpy(likenames[i++], text);
    infile >> text >> ws;
  }

  //read the name of the printfile, and then open itthe printfile
  if (!(strcasecmp(text, "printfile") == 0))
    handle.Unexpected("printfile", text);
  infile >> text >> ws;
  outfile.open(text, ios::out);
  handle.checkIfFailure(outfile, text);

  infile >> text >> ws;
  if (strcasecmp(text, "printatstart") == 0)
    infile >> printtimeid >> ws >> text >> ws;
  else
    printtimeid = 0;

  if (printtimeid != 0 && printtimeid != 1)
    handle.Message("Error in likelihoodprinter - invalid value of printatend");

  if (printtimeid == 1)
    handle.Message("Error in likelihoodprinter - cannot print likelihood informtion at the start of the timestep");

  if (!(strcasecmp(text, "yearsandsteps") == 0))
    handle.Unexpected("yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
    handle.Message("Error in likelihoodprinter - wrong format for yearsandsteps");

  //prepare for next printfile component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[component]") == 0))
      handle.Unexpected("[component]", text);
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
  int index = 0;
  int i, j;

  for (i = 0; i < likevec.Size(); i++) {
    for (j = 0; j < likenames.Size(); j++) {
      if (strcasecmp(likevec[i]->Name(), likenames[j]) == 0) {
        like.resize(1);
        like[index++] = likevec[i];
      }
    }
  }

  if (like.Size() != likenames.Size()) {
    handle.logWarning("Error in likelihoodprinter - failed to match likelihood components");
    for (i = 0; i < like.Size(); i++)
      handle.logWarning("Error in likelihoodprinter - found component", like[i]->Name());
    for (i = 0; i < likenames.Size(); i++)
      handle.logWarning("Error in likelihoodprinter - looking for component", likenames[i]);
    exit(EXIT_FAILURE);
  }

  outfile << "; ";
  RUNID.print(outfile);

  for (i = 0; i < like.Size(); i++) {
    outfile << "; Likelihood output file for the likelihood component " << like[i]->Name();
    switch(like[i]->Type()) {
      case CATCHDISTRIBUTIONLIKELIHOOD:
        outfile << "\n; year-step-area-age-length-number\n";
        break;
      case CATCHSTATISTICSLIKELIHOOD:
        outfile << "\n; year-step-area-age-number-mean[-stddev]\n";
        break;
      case CATCHINTONSLIKELIHOOD:
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
        outfile << "\n; year-step-area-label-index\n";
        break;
      case TAGLIKELIHOOD:
        outfile << "\n; tagid-year-step-area-length-number\n";
        break;
      case RECSTATISTICSLIKELIHOOD:
        handle.logWarning("Error in likelihoodprinter - recstatistics not currently implemented");
        //outfile << "\n; tagid-year-step-area-number-mean\n";
        break;
      case UNDERSTOCKINGLIKELIHOOD:
        handle.logWarning("Error in likelihoodprinter - understocking not currently implemented");
        break;
      case MIGRATIONPENALTYLIKELIHOOD:
        handle.logWarning("Error in likelihoodprinter - migrationpenalty not currently implemented");
        break;
      case BOUNDLIKELIHOOD:
        handle.logWarning("Error in likelihoodprinter - boundlikelihood not currently implemented");
        break;
      default:
        handle.logFailure("Error in likelihoodprinter - unrecognised likelihood type", like[i]->Type());
        break;
    }
  }

  outfile.flush();
}

void LikelihoodPrinter::Print(const TimeClass* const TimeInfo, int printtime) {

  if ((!AAT.AtCurrentTime(TimeInfo)) || (printtime != printtimeid))
    return;

  int i;
  for (i = 0; i < like.Size(); i++)
    like[i]->LikelihoodPrint(outfile, TimeInfo);

  outfile.flush();
}
