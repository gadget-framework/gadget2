#include "formatedcatchprinter.h"
#include "conversion.h"
#include "stockaggregator.h"
#include "areatime.h"
#include "readfunc.h"
#include "readword.h"
#include "errorhandler.h"
#include "catchdistribution.h"
#include "formatedprinting.h"
#include "runid.h"
#include "gadget.h"

extern RunId RUNID;

/*  FormatedCatchPrinter
 *
 *  Purpose:  Constructor
 *
 *  In:  CommentStream& infile        :Input file
 *       AreaClass* Area              :Area definition
 *       TimeClass* TimeInfo          :Time definition
 *
 *  Usage:  FormatedCatchPrinter(infile, Area, Time)
 *
 *  Pre:  infile, Area, & Time are valid according to StockPrinter documentation.
 */
FormatedCatchPrinter::FormatedCatchPrinter(CommentStream& infile,
  const AreaClass* const AreaInfo, const TimeClass* const TimeInfo)
  : Printer(FORMATEDCATCHPRINTER), Area(AreaInfo), Time(TimeInfo) {

  ErrorHandler handle;
  char text[MaxStrLength];
  char filename[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  strncpy(filename, "", MaxStrLength);
  int i;

  ReadWordAndVariable(infile, "ageaggregate", ageagg);

  //Read in the printfile names
  i = 0;
  bioprint = 0;
  numberprint = 0;
  infile >> text >> ws;
  while ((strcasecmp(text, "catches") != 0) && !infile.eof()) {
    if ((strcasecmp(text, "printfile") == 0) && (bioprint == 0)) {
      infile >> filename >> ws;
      biofile.open(filename, ios::out);
      CheckIfFailure(biofile, filename);
      bioprint = 1;
      i++;
      biofile << "; ";
      RUNID.print(biofile);
      biofile.flush();
    } else if ((strcasecmp(text, "bioprintfile") == 0) && (bioprint == 0)) {
      infile >> filename >> ws;
      biofile.open(filename, ios::out);
      CheckIfFailure(biofile, filename);
      bioprint = 1;
      i++;
      biofile << "; ";
      RUNID.print(biofile);
      biofile.flush();
    } else if ((strcasecmp(text, "numberprintfile") == 0) && (numberprint == 0)) {
      infile >> filename >> ws;
      numberfile.open(filename, ios::out);
      CheckIfFailure(numberfile, filename);
      numberprint = 1;
      i++;
      numberfile << "; ";
      RUNID.print(numberfile);
      numberfile.flush();
    }
    infile >> text >> ws;
  }

  if (i == 0)
    handle.Message("No printfiles read in for formatedcatchprinter");

  //Read in the catchnames
  i = 0;
  if (!(strcasecmp(text, "catches") == 0))
    handle.Unexpected("catches", text);
  infile >> text >> ws;
  while (!infile.eof()) {
    catchnames.resize(1);
    catchnames[i] = new char[strlen(text) + 1];
    strcpy(catchnames[i++], text);
    infile >> text >> ws;
  }
}

void FormatedCatchPrinter::SetLikely(Likelihoodptrvector& likevec) {
  assert(likevec.Size() > 0);
  int i, j;
  int index = 0;
  for (i = 0; i < likevec.Size(); i++)
    if (likevec[i]->Type() == CATCHDISTRIBUTIONLIKELIHOOD)
      for (j = 0; j < catchnames.Size(); j++)
        if (strcasecmp(((CatchDistribution*)likevec[i])->Name(), catchnames[j]) == 0) {
          catches.resize(1);
          catches[index++] = (CatchDistribution*) likevec[i];
        }

  if (catches.Size() != catchnames.Size()) {
    cerr << "Error in printer when searching for catch(es) with name matching:\n";
    for (i = 0; i < catchnames.Size(); i++)
      cerr << (const char*)catchnames[i] << sep;
    cerr << "\nDid only find the catch(es)\n";
    for (i = 0; i < catches.Size(); i++)
      cerr << (const char*)catches[i]->Name() << sep;
    cerr << endl;
    exit(EXIT_FAILURE);
  }
  timevec.resize(1, catches[0]->getYears()[0]);
  for (i = 1; i < catches[0]->getYears().Size(); i++)
    if (timevec[timevec.Size() - 1] != catches[0]->getYears()[i])
      timevec.resize(1, catches[0]->getYears()[i]);
}

/*  Print
 *
 *  Purpose:  Print stocks according to configuration in constructor.
 *
 *  In:  TimeClass& TimeInfo           :Current time
 *
 *  Usage:  Print(TimeInfo)
 *
 *  Pre:  SetStock has been called  - JMB but this has been removed?
 */
void FormatedCatchPrinter::Print(const TimeClass* const TimeInfo) {
  if (TimeInfo->CurrentTime() < TimeInfo->TotalNoSteps())
    return;
  int b = 0, d = 0, badage = 0;
  int i;
  int yearagg = -1;
  for (i = 0; i < catches.Size(); i++) {
    b += catches[i]->aggLevel();
    d += catches[i]->ageCols();
  }
  if (b == catches.Size())
    yearagg = 1;
  else if (b == 0)
    yearagg = 0;
  else   //This should not happen!
    cerr << "Warning: Different aggregate levels in catches!\n";

  if (d != catches.Size()) {
    badage = 1;
    biofile << "(*\n"
      << "   WARNING! Atempting to print CatchDistribution(s) where\n"
      << "   ages are agregated. This is not implemented corectly.\n"
      << "   Where age groups are used, the biomass will be repeated\n"
      << "   for each individual age row belonging to that agegroup.\n"
      << "   The 'total' will be wrong, and should be disregarded.\n" << "*)\n";
  }
  //NOTE - to fix this, we should aggregate biomass to the same age division.
  if (bioprint) {
    biofile.setf(ios::fixed);
    for (i = 0; i < catches.Size(); i++)
      catches[i]->aggregateBiomass();
    biofile << "Calculated catch in Biomass";
    if (ageagg)
      printCalcBiomassByFleet(yearagg, biofile, lowprecision, printwidth);
    else
      printCalcBiomassByAgeAndFleet(yearagg, biofile, lowprecision, printwidth);

    biofile << "\n\nObserved catch in Biomass";
    if (ageagg)
      printObsBiomassByFleet(yearagg, biofile, lowprecision, printwidth);
    else
      printObsBiomassByAgeAndFleet(yearagg, biofile, lowprecision, printwidth);

    biofile << endl;
    biofile.flush();
  }

  if (numberprint) {
    numberfile.setf(ios::fixed);
    numberfile << "Calculated catch in Numbers";
    if (ageagg)
      printCalcCatchByFleet(yearagg, numberfile, lowprecision, printwidth);
    else
      printCalcCatchByAgeAndFleet(yearagg, numberfile, lowprecision, printwidth);

    numberfile << "\n\nObserved catch in Numbers";
    if (ageagg)
      printObsCatchByFleet(yearagg, numberfile, lowprecision, printwidth);
    else
      printObsCatchByAgeAndFleet(yearagg, numberfile, lowprecision, printwidth);

    numberfile << endl;
    numberfile.flush();
    /* JMB code removed from here - see RemovedCode.txt for details*/
  }
}

void FormatedCatchPrinter::printCalcCatchByFleet(int yearagg, ostream& outfile, int PREC, int WIDTH) {

  double total, b;
  int i, ar, t, f;
  outfile << " by time (year), and fleet\n\n";
  for (i = 0; i < catchnames.Size(); i++) {
    outfile.precision(PREC);
    outfile.width(WIDTH);
    outfile << catchnames[i] << sep;
  }
  outfile << " total\n";
  for (ar = 0; ar < Area->NoAreas(); ar++) {
    outfile << "area " << ar << endl;
    for (t = 0; t < catches[0]->calcCatchByTimeAge(ar).Nrow(); t++) {
      printTime(outfile, t, yearagg);
      outfile << sep;
      total = 0;
      for (f = 0; f < catches.Size(); f++) {
        b = 0;
        for (i = 0; i < catches[f]->calcCatchByTimeAge(ar).Ncol(); i++)
          b += catches[f]->calcCatchByTimeAge(ar)[t][i];
        total += b;
        outfile.precision(PREC);
        outfile.width(WIDTH);
        outfile << b << sep;
      }
      outfile.precision(PREC);
      outfile.width(WIDTH);
      outfile << total << endl;
    }
  }
  outfile.flush();
}

void FormatedCatchPrinter::printObsCatchByFleet(int yearagg, ostream& outfile, int PREC, int WIDTH) {

  double total, b;
  int i, ar, t, f;
  outfile <<" by time (year), and fleet\n\n";
  for (i = 0; i < catchnames.Size(); i++) {
    outfile.precision(PREC);
    outfile.width(WIDTH);
    outfile << catchnames[i] << sep;
  }
  outfile <<" total\n";
  for (ar = 0; ar < Area->NoAreas(); ar++) {
    outfile << "area " << ar << endl;
    for (t = 0; t < catches[0]->obsCatchByTimeAge(ar).Nrow(); t++) {
      printTime(outfile, t, yearagg);
      outfile << sep;
      total = 0;
      for (f = 0; f < catches.Size(); f++) {
        b = 0;
        for (i = 0; i < catches[f]->obsCatchByTimeAge(ar).Ncol(); i++)
          b += catches[f]->obsCatchByTimeAge(ar)[t][i];
        total += b;
        outfile.precision(PREC);
        outfile.width(WIDTH);
        outfile << b << sep;
      }
      outfile.precision(PREC);
      outfile.width(WIDTH);
      outfile << total << endl;
    }
  }
  outfile.flush();
}

void FormatedCatchPrinter::printCalcCatchByAgeAndFleet(int yearagg, ostream& outfile, int PREC, int WIDTH) {

  double total;
  int i, j, ar, t, f;
  int found, done, age;
  outfile << " by time (year), age and fleet\n";
  for (ar = 0; ar < Area->NoAreas(); ar++) {
    outfile << "area " << ar;
    for (t = 0; t < catches[0]->calcCatchByTimeAge(ar).Nrow(); t++) {
      outfile << endl;
      for (i = 0; i < catchnames.Size(); i++) {
        outfile.precision(PREC);
        outfile.width(WIDTH);
        outfile << catchnames[i] << sep;
      }
      outfile << "total\n";
      printTime(outfile, t, yearagg);
      outfile << endl;

      done = 0;
      age = 1;

      while (!done) {
        total = 0;
        outfile.precision(0);
        outfile.width(2);
        outfile << age << endl;
        for (f = 0; f < catches.Size(); f++) {
          found = 0;
          for (j = 0; j < catches[f]->getAges().Nrow(); j++) {
            for (i = 0; i < catches[f]->getAges().Ncol(); i++) {
              if (catches[f]->getAges()[j][i] == age) {
                found = 1;
                total += catches[f]->calcCatchByTimeAge(ar)[t][j];
                outfile.precision(PREC);
                outfile.width(WIDTH);
                outfile << catches[f]->calcCatchByTimeAge(ar)[t][j] << sep;
              }
            }
          }
          outfile.precision(PREC);
          outfile.width(WIDTH + 1);
          if (!found)
            outfile << "- ";
          if (catches[f]->maxAge() == age)
            done = 1;
          else
            done = 0;
        }
        outfile.precision(PREC);
        outfile.width(WIDTH);
        outfile << total << endl;
        age++;
      }
    }
  }
  outfile.flush();
}

void FormatedCatchPrinter::printObsCatchByAgeAndFleet(int yearagg, ostream& outfile, int PREC, int WIDTH) {

  double total;
  int i, j, ar, t, f;
  int found, done, age;
  outfile << " by time (year), age and fleet\n";
  for (ar = 0; ar < Area->NoAreas(); ar++) {
    outfile << "area " << ar;
    for (t = 0; t < catches[0]->obsCatchByTimeAge(ar).Nrow(); t++) {
      outfile << endl;
      for (i = 0; i < catchnames.Size(); i++) {
        outfile.precision(PREC);
        outfile.width(WIDTH);
        outfile << catchnames[i] << sep;
      }
      outfile << "total\n";
      printTime(outfile, t, yearagg);
      outfile << endl;

      done = 0;
      age = 1;

      while (!done) {
        total = 0;
        outfile.precision(0);
        outfile.width(2);
        outfile << age << sep;
        for (f = 0; f < catches.Size(); f++) {
          found = 0;
          for (j = 0; j < catches[f]->getAges().Nrow(); j++) {
            for (i = 0; i < catches[f]->getAges().Ncol(); i++) {
              if (catches[f]->getAges()[j][i] == age) {
                found = 1;
                total += catches[f]->obsCatchByTimeAge(ar)[t][j];
                outfile.precision(PREC);
                outfile.width(WIDTH);
                outfile << catches[f]->obsCatchByTimeAge(ar)[t][j] << sep;
               }
            }
          }
          outfile.precision(PREC);
          outfile.width(WIDTH + 1);
          if (!found)
            outfile << "- ";
          if (catches[f]->maxAge() == age)
            done = 1;
          else
            done = 0;
        }
        outfile.precision(PREC);
        outfile.width(WIDTH);
        outfile << total << endl;
        age++;
      }
    }
  }
  outfile.flush();
}

void FormatedCatchPrinter::printCalcBiomassByFleet(int yearagg, ostream& outfile, int PREC, int WIDTH) {

  double total, b;
  int i, ar, t, f;

  outfile << " by time (year), and fleet\n\n";
  for (i = 0; i < catchnames.Size(); i++) {
    outfile.precision(PREC);
    outfile.width(WIDTH);
    outfile << catchnames[i] << sep;
  }
  outfile << " total\n";
  for (ar = 0; ar < Area->NoAreas(); ar++) {
    outfile << "area " << ar << endl;
    for (t = 0; t < catches[0]->getAggCalcBiomass(ar).Nrow(); t++) {
      printTime(outfile, t, yearagg);
      outfile << sep;
      total = 0;
      for (f = 0; f < catches.Size(); f++) {
        b = 0;
        for (i = 0; i < catches[f]->getAggCalcBiomass(ar).Ncol(); i++)
          b += catches[f]->getAggCalcBiomass(ar)[t][i];
        total += b;
        outfile.precision(PREC);
        outfile.width(WIDTH);
        outfile << b << sep;
      }
      outfile.precision(PREC);
      outfile.width(WIDTH);
      outfile << total << endl;
    }
  }
  outfile.flush();
}

/* Warning, the following functions only print aggregated biomass, no mather what agg_lev is! Fix this! */
void FormatedCatchPrinter::printObsBiomassByFleet(int yearagg, ostream& outfile, int PREC, int WIDTH) {

  double total, b;
  int i, ar, t, f;

  outfile << " by time (year), and fleet\n\n";
  for (i = 0; i < catchnames.Size(); i++) {
    outfile.precision(PREC);
    outfile.width(WIDTH);
    outfile << catchnames[i] << sep;
  }
  outfile << " total\n";
  for (ar = 0; ar < Area->NoAreas(); ar++) {
    outfile << "area " << ar << endl;
    for (t = 0; t < catches[0]->getAggObsBiomass(ar).Nrow(); t++) {
      printTime(outfile, t, yearagg);
      outfile << sep;
      total = 0;
      for (f = 0; f < catches.Size(); f++) {
        b = 0;
        for (i = 0; i < catches[f]->getAggObsBiomass(ar).Ncol(); i++)
          b += catches[f]->getAggObsBiomass(ar)[t][i];
        total += b;
        outfile.precision(PREC);
        outfile.width(WIDTH);
        outfile << b << sep;
      }
      outfile.precision(PREC);
      outfile.width(WIDTH);
      outfile << total << endl;
    }
  }
  outfile.flush();
}

void FormatedCatchPrinter::printCalcBiomassByAgeAndFleet(int yearagg, ostream& outfile, int PREC, int WIDTH) {

  double total;
  int i, j, ar, t, f;
  int found, done, age;
  outfile <<" by time (year), age and fleet\n";
  for (ar = 0; ar < Area->NoAreas(); ar++) {
    outfile << "area " << ar;
    for (t = 0; t < catches[0]->getAggCalcBiomass(ar).Nrow(); t++) {
      outfile << endl;
      for (i = 0; i < catchnames.Size(); i++) {
        outfile.precision(PREC);
        outfile.width(WIDTH);
        outfile << catchnames[i] << sep;
      }
      outfile << "total\n";
      printTime(outfile, t, yearagg);
      outfile << endl;
      done = 0;
      age = 1;
      //intvector ageindex;
      while (!done) {
        total = 0;
        outfile.precision(0);
        outfile.width(2);
        outfile << age << sep;
        for (f = 0; f < catches.Size(); f++) {
          found = 0;
          for (j = 0; j < catches[f]->getAges().Nrow(); j++) {
            for (i = 0; i < catches[f]->getAges().Ncol(); i++) {
              if (catches[f]->getAges()[j][i] == age) {
                found = 1;
                total += catches[f]->getAggCalcBiomass(ar)[t][j];
                outfile.precision(PREC);
                outfile.width(WIDTH);
                outfile << catches[f]->getAggCalcBiomass(ar)[t][j] << sep;
              }
            }
          }
          outfile.precision(PREC);
          outfile.width(WIDTH + 1);
          if (!found)
            outfile << "- ";
          if (catches[f]->maxAge() == age)
            done = 1;
          else
            done = 0;
        }
        outfile.precision(PREC);
        outfile.width(WIDTH);
        outfile << total << endl;
        age++;
      }
    }
  }
  outfile.flush();
}

void FormatedCatchPrinter::printObsBiomassByAgeAndFleet(int yearagg, ostream& outfile, int PREC, int WIDTH) {

  double total;
  int i, j, ar, t, f;
  int found, done, age;
  outfile << " by time (year), age and fleet\n";
  for (ar = 0; ar < Area->NoAreas(); ar++) {
    for (t = 0; t < catches[0]->getAggObsBiomass(ar).Nrow(); t++) {
      outfile << "area " << ar << endl;
      for (i = 0; i < catchnames.Size(); i++) {
        outfile.precision(PREC);
        outfile.width(WIDTH);
        outfile << catchnames[i] << sep;
      }
      outfile << "total\n";
      printTime(outfile, t, yearagg);
      outfile << endl;
      done = 0;
      age = 1;

      while (!done) {
        total = 0;
        outfile.precision(0);
        outfile.width(2);
        outfile << age << sep;
        for (f = 0; f < catches.Size(); f++) {
          found = 0;
          for (j = 0; j < catches[f]->getAges().Nrow(); j++) {
            for (i = 0; i < catches[f]->getAges().Ncol(); i++) {
              if (catches[f]->getAges()[j][i] == age) {
                found = 1;
                total += catches[f]->getAggObsBiomass(ar)[t][j];
                outfile.precision(PREC);
                outfile.width(WIDTH);
                outfile << catches[f]->getAggObsBiomass(ar)[t][j] << sep;
              }
            }
          }
          outfile.precision(PREC);
          outfile.width(WIDTH + 1);
          if (!found)
            outfile << "- ";
          if (catches[f]->maxAge() == age)
            done = 1;
          else
            done = 0;
        }
        outfile.precision(PREC);
        outfile.width(WIDTH);
        outfile << total << endl;
        age++;
      }
    }
  }
  outfile.flush();
}

void FormatedCatchPrinter::printTime(ostream& out, int time, int yearagg) {
  if (yearagg) {
    out << "year " << timevec[time];
  } else {
    out << "year " << catches[0]->getYears()[time] << " step " << catches[0]->getSteps()[time];
  }
}

FormatedCatchPrinter::~FormatedCatchPrinter() {
  if (bioprint) {
    biofile.close();
    biofile.clear();
  }
  if (numberprint) {
    numberfile.close();
    numberfile.clear();
  }

  int i;
  for (i = 0; i < catchnames.Size(); i++)
    delete[] catchnames[i];
}
