#include "cannibalism.h"
#include "readfunc.h"
#include "readword.h"
#include "areatime.h"
#include "keeper.h"
#include "errorhandler.h"
#include "gadget.h"

Cannibalism::Cannibalism(CommentStream& infile, const LengthGroupDivision* preylen,
  const TimeClass* const TimeInfo, Keeper* const keeper)
  : altfood(TimeInfo->TotalNoSteps(), TimeInfo->FirstStep(), 0), params(3),
    cann_lev(TimeInfo->TotalNoSteps()), cannibalism(preylen->NoLengthGroups(), 0) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j, k, numstocks = 0;

  keeper->AddString("cannibalism");
  preylgp = new LengthGroupDivision(*preylen);

  //reading predator names
  infile >> text >> ws;
  if (strcasecmp(text, "predators") != 0)
    handle.Unexpected("predators", text);

  infile >> text >> ws;
  while ((strcasecmp(text, "predminages") != 0) && !infile.eof()) {
    predatornames.resize(1);
    i = predatornames.Size() - 1;
    predatornames[i] = new char[strlen(text) + 1];
    strcpy(predatornames[i], text);
    infile >> text >> ws;
  }

  nrofpredators = predatornames.Size();
  if (nrofpredators == 0)
    handle.Message("Error in cannibalism - no predators");

  //reading predator ages
  if (strcasecmp(text, "predminages") == 0) {
    minage.resize(nrofpredators);
    for (i = 0; i < nrofpredators; i++)
      infile >> minage[i];
    infile >> ws;
  } else
    handle.Unexpected("predminages", text);

  infile >> text >> ws;
  if (strcasecmp(text, "predmaxages") == 0) {
    maxage.resize(nrofpredators);
    for (i = 0; i < nrofpredators; i++)
      infile >> maxage[i];
    infile >> ws;
  } else
    handle.Unexpected("predmaxages", text);

  for (i = 0; i < nrofpredators; i++)
    agegroups.AddRows(1, maxage[i] - minage[i] + 1, 0);

  //reading delta
  infile >> text >> ws;
  if (strcasecmp(text, "delta") == 0) {
    infile >> delta;
    delta.Inform(keeper);
  } else
    handle.Unexpected("delta", text);

  //reading suitability parameters
  infile >> text >> ws;
  if (strcasecmp(text, "parameters") == 0) {
    infile >> params;
    params.Inform(keeper);
  } else
    handle.Unexpected("parameters", text);

  //read cannibalism from data file
  char filename[MaxStrLength];
  strncpy(filename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);
  readWordAndValue(infile, "canndatafile", filename);
  datafile.open(filename, ios::in);
  checkIfFailure(datafile, filename);
  handle.Open(filename);

  //reading cannibalism level
  subdata >> text >> ws;
  if (strcasecmp(text, "cannlev") == 0) {
    subdata >> cann_lev;
    cann_lev.Inform(keeper);
  } else
    handle.Unexpected("cannlev", text);

  //reading alternative food
  subdata >> text >> ws;
  if (strcasecmp(text, "altfood") == 0) {
    if (!(readIndexVector(subdata, altfood)))
      handle.Message("Error in cannibalism - wrong format for altfood");
  } else
    handle.Unexpected("altfood", text);

  handle.Close();
  datafile.close();
  datafile.clear();

  //read overlap data
  infile >> text >> ws;
  overlap.AddRows(nrofpredators, TimeInfo->StepsInYear(), 0);
  if (strcasecmp(text, "overlap") == 0) {
    for (i = 0; i < overlap.Nrow(); i++) {
      for (j = 0; j < overlap.Ncol(i); j++) {
        infile >> k >> ws;
        if ((k != 0) && (k != 1))
          handle.Message("Error in cannibalism - wrong format for overlap");
        overlap[i][j] = k;
      }
    }
  } else
    handle.Unexpected("overlap", text);

  //set dimension on the consumption matrices
  consumption.resize(nrofpredators);
  for (i = 0; i < nrofpredators; i++)
    consumption[i] = new BandMatrix(0, preylgp->NoLengthGroups(), minage[i], maxage[i] - minage[i] + 1, 0.0);

  keeper->ClearLast();
}

Cannibalism::~Cannibalism() {
  int i;
  for (i = 0; i < predatornames.Size(); i++)
    delete[] predatornames[i];
  for (i = 0; i < consumption.Size(); i++)
    delete consumption[i];
  delete preylgp;
}

const DoubleVector& Cannibalism::Mortality(const AgeBandMatrix& alk_prey,
  const AgeBandMatrix& alk_pred, const LengthGroupDivision* len_prey,
  const LengthGroupDivision* len_pred, const TimeClass* const TimeInfo,
  int pred_no, const DoubleVector& natm) {

  int prey_size, pred_is_pres;
  int i, preyl, predl, minl, maxl;
  int minage, maxage;
  double biomass, otherbiomass_factor, len, mort_fact, mortality;

  prey_size = len_prey->NoLengthGroups();
  //assert(alk_pred.Minage() == minage[pred_no]);
  assert(alk_pred.Nrow() == (*consumption[pred_no]).Nrow());
  assert(prey_size == (*consumption[pred_no]).Ncol());
  assert(prey_size == cannibalism.Size());

  PopInfoVector pop_prey(prey_size);
  alk_prey.Colsum(pop_prey); //biomass of prey's length groups
  ConversionIndex length_conversion(len_prey, len_pred, 0);

  //set cannibalism to zero
  for (i = 0; i < prey_size; i++)
    cannibalism[i] = 0.0;

  minage = alk_pred.Minage();
  maxage = alk_pred.Maxage();
  for (i = minage; i <= maxage; i++)
    for (preyl = 0; preyl < prey_size; preyl++)
      (*consumption[pred_no])[i][preyl] = 0.0;

  assert(altfood[TimeInfo->CurrentTime()] > 0.0);
  otherbiomass_factor = 1.0 / pow(altfood[TimeInfo->CurrentTime()], delta);

  for (i = 0; i < agegroups.Ncol(pred_no); i++)
    agegroups[pred_no][i] = 0.0;

  //check if the predator is present on the current timestep
  pred_is_pres = overlap[pred_no][TimeInfo->CurrentStep() - 1];
  if (!(pred_is_pres))
    return cannibalism;

  //OK, the predator is present
  for (preyl = 0; preyl < prey_size; preyl++) {
    len = len_prey->Meanlength(preyl);
    mort_fact = cann_lev[TimeInfo->CurrentTime() - 1] * otherbiomass_factor;
    for (i = minage; i <= maxage; i++) {
      maxl = alk_pred.Maxlength(i);
      if (preyl < length_conversion.Minlength())
        minl = alk_pred.Minlength(i);
      else
        minl = max(length_conversion.Pos(preyl) + 1, alk_pred.Minlength(i));

      for (predl = minl; predl < maxl; predl++) {
        biomass = alk_pred[i][predl].N * alk_pred[i][predl].W;
        mortality = biomass * mort_fact * suitfunc(len_pred->Meanlength(predl), len);
        cannibalism[preyl] += mortality;
        (*consumption[pred_no])[i][preyl] += mortality;
      }
    }
  }

  //Calculate predator's consumption of prey
  //Note that zero fishing mortality is assumed for prey
  for (i = alk_pred.Minage(); i <= alk_pred.Maxage(); i++) {
    for (predl = alk_pred.Minlength(i); predl < alk_pred.Maxlength(i); predl++)
      agegroups[pred_no][i - alk_pred.Minage()] += alk_pred[i][predl].N;

    for (preyl = 0; preyl < prey_size; preyl++) {
      mortality = (*consumption[pred_no])[i][preyl];

      if (mortality > 0)
        (*consumption[pred_no])[i][preyl] = ((mortality / (cannibalism[preyl] + natm[preyl])) *
              (1.0 - exp(-cannibalism[preyl] - natm[preyl])) * pop_prey[preyl].N * pop_prey[preyl].W);
      else
        (*consumption[pred_no])[i][preyl] = 0.0;

    }
  }
  return cannibalism;
}

int Cannibalism::getMinPredAge() {
  //written by kgf 2/3 99
  int i;
  int min = 200;
  for (i = 0; i < minage.Size(); i++) {
    if (minage[i] < min)
      min = minage[i];
  }
  return min;
}

int Cannibalism::getMaxPredAge() {
  //written by kgf 2/3 99
  int i;
  int max = -1;
  for (i = 0; i < maxage.Size(); i++) {
    if (maxage[i] > max)
      max = maxage[i];
  }
  return max;
}

double Cannibalism::suitfunc(double predlength, double preylength) {
  //based on the Andersen suitability function

  double l = log(predlength / preylength);
  double e, q = 0.0;
  double check = 0.0;

  if (l > params[0])
    q = params[2];
  else
    q = params[1];

  if (isZero(q))
    q = 1.0;   //We do not want to divide with 0, no matter what.
  if (q < 0)
    q = -q;    //To avoid getting a big positive number as an argument for exp

  e = (l - params[0]) * (l - params[0]);
  check = exp(-e / q);
  if ((check < 0.0) || (check > 1.0)) {
    cerr << "Error in cannibalism - function outside bounds " << check << endl;
    exit(EXIT_FAILURE);
  } else
    return check;
}

void Cannibalism::Print(ofstream& outfile) {
  int i;
  outfile << "Cannibalism\nParams\n";
  for (i = 0; i < params.Size(); i++)
    outfile << params[i] << sep;
  outfile << "\nAltfood\n";
  for (i = altfood.Mincol(); i < altfood.Maxcol(); i++)
    outfile << altfood[i] << sep;
  outfile << "\nMortality\n";
  for (i = 0; i < cannibalism.Size(); i++)
    outfile << cannibalism[i] << sep;
  outfile << endl;
}
