#include "catch.h"
#include "stock.h"
#include "readfunc.h"
#include "readword.h"
#include "formatedprinting.h"
#include "stockaggregator.h"
#include "errorhandler.h"
#include "gadget.h"

CatchData::CatchData(CommentStream& infile, const char* givenname,
  const AreaClass* const Area, const TimeClass* const TimeInfo)
  : HasName(givenname), LgrpDiv(0) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int minage;
  int maxage;
  double minlength;
  double maxlength;
  double dl;
  ReadWordAndVariable(infile, "minage", minage);
  ReadWordAndVariable(infile, "maxage", maxage);
  ReadWordAndVariable(infile, "minlength", minlength);
  ReadWordAndVariable(infile, "maxlength", maxlength);
  ReadWordAndVariable(infile, "dl", dl);

  LgrpDiv = new LengthGroupDivision(minlength, maxlength, dl);
  if (LgrpDiv->Error())
    LengthGroupPrintError(minlength, maxlength, dl, "??");
  int nolengthgroups = LgrpDiv->NoLengthGroups();
  ReadWordAndVariable(infile, "numberofstocks", numberofstocks);
  if (numberofstocks != 1 && numberofstocks != 2)
    handle.Message("Error in CatchData - number of stocks should be 1 or 2");

  infile >> text >> ws;
  int i, tmpint;
  i = 0;
  intvector Areas;
  if (strcasecmp(text, "areas") == 0)
    while (isdigit(infile.peek()) && !infile.eof() && (i < Area->NoAreas())) {
      Areas.resize(1);
      infile >> tmpint >> ws;
      if ((Areas[i] = Area->InnerArea(tmpint)) == -1)
        handle.UndefinedArea(tmpint);
      i++;
    }
  else
    handle.Unexpected("areas", text);
  this->LetLiveOnAreas(Areas);

  infile >> text;
  if (strcasecmp(text, "filenames") != 0)
    handle.Unexpected("filenames", text);
  ifstream catchfile;
  CommentStream catchcomment(catchfile);

  //JMB - need to cast minlength to an integer - use tmpint
  tmpint = int(minlength);
  Catchmatrix.AddRows(areas.Size(), TimeInfo->TotalNoSteps() + 1,
    new bandmatrix(tmpint, nolengthgroups, minage, maxage - minage + 1, 0));

  int year, step, abstep;
  while (!infile.eof()) {
    //First we read the name of the files that we use and open them.
    //Then we read their content.
    for (i = 0; i < areas.Size(); i++) {
      infile >> ws;
      if (infile.eof())
        handle.Eof("CatchData");

      infile >> text;
      catchfile.open(text);
      CheckIfFailure(catchfile, text);
      handle.Open(text);

      ReadWordAndVariable(catchcomment, "year", year);
      ReadWordAndVariable(catchcomment, "step", step);

      while (TimeInfo->IsWithinPeriod(year, step) && !catchcomment.eof()) {
        if (year == -1 || step == -1)
          handle.Message("Was reading either year or step");
        doublematrix* tmp = ReadMatrix(catchcomment, maxage - minage + 1, nolengthgroups);
        if (tmp == 0)
          handle.Message("Error in CatchData - failed to read CatchData matrix");

        bandmatrix tmpb(*tmp, minage, tmpint);
        abstep = TimeInfo->CalcSteps(year, step);
        tmpb += *(Catchmatrix[i][abstep]);
        Catchmatrix[i][abstep] = new bandmatrix(tmpb);
        delete tmp;
        catchcomment >> ws;
        if (!catchcomment.eof()) {
          ReadWordAndVariable(catchcomment, "year", year);
          ReadWordAndVariable(catchcomment, "step", step);
        } else
          break;
      } //End of while (TimeInfo) loop

    handle.Close();
    catchfile.close();
    catchfile.clear();
    infile >> ws;
    }
  }
}

CatchData::~CatchData() {
  delete LgrpDiv;
  int i, j;
  for (i = 0; i < Catchmatrix.Nrow(); i++)
    for (j = 0; j < Catchmatrix.Ncol(i); j++)
      delete Catchmatrix[i][j];
}

int CatchData::FindStock(const Stock* stock) {
  if (strncasecmp(this->Name(), stock->Name(), strlen(this->Name())) == 0 && stock->IsCaught())
    return 1;
  return 0;
}

const bandmatrix* CatchData::GetCatch(int area, const TimeClass* const TimeInfo) const {
  if (this->IsInArea(area))
    return Catchmatrix[AreaNr[area]][TimeInfo->CurrentTime()];
  return 0;
}

const bandmatrix* CatchData::GetCatch(Stock& stock) {
  int index = stockIndex(stock);
  if (index == -1)
    return 0;
  else
    return stockCatch[index];
}

void CatchData::Recalc(int area, const TimeClass* const TimeInfo) {
  int i, j, k;
  int minl, maxl, minage, maxage;
  bandmatrix* c = Catchmatrix[AreaNr[area]][TimeInfo->CurrentTime()];

  if (stocks.Size() == 1) {
    stockCatch[0] = c;
    return;
  } else if (stocks.Size() == 0)
    return;

  for (i = 0; i < stocks.Size(); i++) {
    const Agebandmatrix* fine = &(stocks[i]->Agelengthkeys(area));
    ConversionIndex CI(stocks[i]->ReturnLengthGroupDiv(), LgrpDiv, 0);
    intvector sizes;
    intvector minlengths;
    minage = fine->Minage();
    maxage = fine->Maxage();
    for (j = minage; j <= maxage; j++) {
      minlengths.resize(1, CI.Pos(fine->Minlength(j)));
      sizes.resize(1, CI.Pos(fine->Maxlength(j) - 1) - CI.Pos(fine->Minlength(j)) + 1);
    }
    coarseN[i] = new bandmatrix(minlengths, sizes, minage, 0);
    for (j = minage; j <= maxage; j++) {
      minl = fine->Minlength(j);
      maxl = fine->Maxlength(j);
      for (k = minl; k < maxl; k++)
        (*coarseN[i])[j][CI.Pos(k)] += (*fine)[j][k].N;
    }
  }

  maxage = 0;
  minage = stocks[0]->Minage();
  for (i = 0; i < stocks.Size(); i++) {
    if (stocks[i]->Minage() < minage)
      minage = stocks[i]->Minage();
    if (stocks[i]->Maxage() > maxage)
      maxage = stocks[i]->Maxage();
  }

  intmatrix ages;
  intvector agesv;
  for (i = minage; i <= maxage; i++) {
    ages.AddRows(1, 1, i);
    agesv.resize(1, i);
  }

  StockAggregator agr(stocks, LgrpDiv , intmatrix(1, 1, area), ages);
  agr.Sum();
  total = &(agr.ReturnSum()[area]);
  int ageoffset = ages[0][0];
  for (i = 0; i < stocks.Size(); i++) {
    stockCatch[i] = new bandmatrix(*c);
    for (j = c->Minage(); j <= c->Maxage(); j++)
      for (k = c->Minlength(j); k < c->Maxlength(j); k++)
        (*stockCatch[i])[j][k] = 0;

    minage = max(c->Minage(), coarseN[i]->Minage());
    maxage = min(c->Maxage(), coarseN[i]->Maxage());
    for (j = minage; j <= maxage; j++) {
      minl = max(c->Minlength(j), coarseN[i]->Minlength(j));
      maxl = min(c->Maxlength(j), coarseN[i]->Maxlength(j));
      for (k = minl; k < maxl; k++)
        (*stockCatch[i])[j][k] = ((*coarseN[i])[j][k]) * ((*c)[j][k]);
    }

    minage = max(stockCatch[i]->Minage(), ageoffset);
    maxage = min(stockCatch[i]->Maxage(), ages[ages.Nrow() - 1][0]);
    for (j = minage; j <= maxage; j++) {
      minl = max(stockCatch[i]->Minlength(j), total->Minlength(j - ageoffset));
      maxl = min(stockCatch[i]->Maxlength(j), total->Maxlength(j - ageoffset));
      for (k = minl; k < maxl; k++)
        if ((*total)[j - ageoffset][k].N != 0)
          (*stockCatch[i])[j][k] /= (*total)[j - ageoffset][k].N;
        else
          (*stockCatch[i])[j][k] = 0;
    }
  }
}

void CatchData::AddStock(Stock& stock) {
  stocks.resize(1, &stock);
  stockCatch.resize(1, 0);
  coarseN.resize(1, 0);
}

int CatchData::stockIndex(Stock& stock) {
  int i, found = -1;
  for (i = 0; i < stocks.Size(); i++)
    if (strcasecmp(stocks[i]->Name(), stock.Name()) == 0)
      found = i;
  return found;
}
