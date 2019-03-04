#include "areatime.h"
#include "errorhandler.h"
#include "likelihoodprinter.h"
#include "charptrvector.h"
#include "readword.h"
#include "gadget.h"
#include "runid.h"
#include "global.h"


LikelihoodPrinter::LikelihoodPrinter(CommentStream& infile, const TimeClass* const TimeInfo)
  : Printer(LIKELIHOODPRINTER) {

  int i = 0;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  printtimeid = 0;

  //read in the names of the likelihood components
  infile >> text >> ws;
  if (strcasecmp(text, "likelihood") != 0)
    handle.logFileUnexpected(LOGFAIL, "likelihood", text);
  infile >> text >> ws;
  while (!infile.eof() && (strcasecmp(text, "printfile") != 0)) {
    likenames.resize(new char[strlen(text) + 1]);
    strcpy(likenames[i++], text);
    infile >> text >> ws;
  }
  if (likenames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "\nError in likelihoodprinter - failed to read component");

  //read the name of the printfile, and then open the printfile
  filename = new char[MaxStrLength];
  strncpy(filename, "", MaxStrLength);

  if (strcasecmp(text, "printfile") != 0)
    handle.logFileUnexpected(LOGFAIL, "printfile", text);
  infile >> filename >> ws;
  outfile.open(filename, ios::out);
  handle.checkIfFailure(outfile, filename);

  //JMB - removed the need to read in the yearsandsteps data
  infile >> ws;
  if (!infile.eof()) {
    char c = infile.peek();
    if ((c == 'y') || (c == 'Y')) {
      infile >> text >> ws;
      if (strcasecmp(text, "yearsandsteps") == 0) {
        handle.logMessage(LOGWARN, "Warning in likelihoodprinter - yearsandsteps data ignored");
        AAT.readFromFile(infile, TimeInfo);
      }
    }
  }

  //prepare for next printfile component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (strcasecmp(text, "[component]") != 0)
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
        like.resize(likevec[i]);

  if (like.Size() != likenames.Size()) {
    handle.logMessage(LOGWARN, "Error in likelihoodprinter - failed to match likelihood components");
    for (i = 0; i < like.Size(); i++)
      handle.logMessage(LOGWARN, "Error in likelihoodprinter - found component", like[i]->getName());
    for (i = 0; i < likenames.Size(); i++)
      handle.logMessage(LOGWARN, "Error in likelihoodprinter - looking for component", likenames[i]);
    handle.logMessage(LOGFAIL, ""); //JMB this will exit gadget
  }

  outfile << "; ";
  RUNID.Print(outfile);

  for (i = 0; i < like.Size(); i++) {
    outfile << "; Likelihood output file for the likelihood component " << like[i]->getName();
    switch (like[i]->getType()) {
      case CATCHDISTRIBUTIONLIKELIHOOD:
	outfile << "\n; -- data --";
        outfile << "\n; year step area age length number\n";
        break;
      case CATCHSTATISTICSLIKELIHOOD:
        outfile << "\n; -- data --";
	outfile << "\n; year step area age number mean [stddev]\n";
        break;
      case CATCHINKILOSLIKELIHOOD:
	outfile << "\n; -- data --";
        outfile << "\n; year step area fleet biomass\n";
        break;
      case SURVEYDISTRIBUTIONLIKELIHOOD:
	outfile << "\n; -- data --";
        outfile << "\n; year step area age length number\n";
        break;
      case STOCKDISTRIBUTIONLIKELIHOOD:
        outfile << "\n; -- data --";
	outfile << "\n; year step area stock age length number\n";
        break;
      case STOMACHCONTENTLIKELIHOOD:
        outfile << "\n; -- data --";
	outfile << "\n; year step area predator prey number [stddev]\n";
        break;
      case SURVEYINDICESLIKELIHOOD:
	outfile << "\n; -- data --";
        outfile << "\n; year step area label number\n";
        break;
      case TAGLIKELIHOOD:
	outfile << "\n; -- data --";
        outfile << "\n; tagid year step area length number\n";
        break;
      case RECSTATISTICSLIKELIHOOD:
	outfile << "\n; -- data --";
        outfile << "\n; tagid year step area number mean [stddev]\n";
        break;
      case MIGRATIONPROPORTIONLIKELIHOOD:
	outfile << "\n; -- data --";
        outfile << "\n; year step area ratio\n";
        break;
      case PROGLIKELIHOOD:
	outfile << "\n; -- data --";
	outfile << "\n; year reference_biomass reference_biomass_with_error trigger_biomass trigger_biomass_with_error";
	outfile << "harvest_rate tac\n";
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
  if (printtime != printtimeid)
    return;

  int i;
  for (i = 0; i < like.Size(); i++)
    like[i]->printLikelihood(outfile, TimeInfo);
  outfile.flush();
}
