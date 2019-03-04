#include "proglikelihood.h"
#include "readfunc.h"
#include "readword.h"
#include "areatime.h"
#include "keeper.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"


ProgLikelihood::ProgLikelihood(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper, double weight, const char* name)
  : Likelihood(PROGLIKELIHOOD, weight, name) {

  //  exploitbio.Read(infile,keeper);
  ///triggerbio.Read(infile,keeper);
  int i;
  int j;
  char text[MaxStrLength];
  
  strncpy(text, "", MaxStrLength);
  i = 0;    
  infile >> text >> ws;
  if (strcasecmp(text, "fleetnames") != 0)
    handle.logFileUnexpected(LOGFAIL, "fleetnames", text);
  infile >> text >> ws;
  while (!infile.eof() && (strcasecmp(text, "stocknames") != 0)) {
    fleetnames.resize(new char[strlen(text) + 1]);
    strcpy(fleetnames[i], text);
    i++;
    infile >> ws >> text >> ws;
  }

  if (fleetnames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "\nError in proglikelihood - failed to read fleets");
  handle.logMessage(LOGMESSAGE, "Read fleet data - number of fleets", fleetnames.Size());
  //read in the stocknames

  i = 0;
  if (strcasecmp(text, "stocknames") != 0)
    handle.logFileUnexpected(LOGFAIL, "biocoeffs", text);
  infile >> ws >> text >> ws;
  while (!infile.eof() && (strcasecmp(text, "biocoeffs") != 0)) {
    stocknames.resize(new char[strlen(text) + 1]);
    strcpy(stocknames[i], text);
    i++;
    infile >> ws >> text >> ws;

  }

  if (stocknames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "\nError in proglikelihood - failed to read stocks");
  handle.logMessage(LOGMESSAGE, "Read stock data - number of stocks", stocknames.Size());

  i = 0;
  for(i = 0; i < stocknames.Size() ; i++){
    biocoeffs.AddRows(1,3,0.0);
    for(j = 0; j < biocoeffs[i].Size(); j++){
      infile >> biocoeffs[i][j];
    }
    biocoeffs[i].Inform(keeper);
  } 

  i = 0;
  infile >> ws >> text >> ws;
  if (strcasecmp(text, "triggercoeffs") != 0)
     handle.logFileMessage(LOGFAIL, "\nError in proglikelihood - failed to read triggercoeffs");
  for(i = 0; i < stocknames.Size() ; i++){
    triggercoeffs.AddRows(1,3,0.0);
    for(j = 0; j < triggercoeffs[i].Size(); j++){
      infile >> triggercoeffs[i][j];
    }
    triggercoeffs[i].Inform(keeper);
  }
  

  infile >> text >> ws;
  if (strcasecmp(text, "triggervalues") != 0)
    handle.logFileUnexpected(LOGFAIL, "triggervalues", text);
  // Read the triggers 
  i = 0;
  while ((infile.peek() == '(' || infile.peek()== '#' || isdigit(infile.peek())) && !infile.eof()) { 
    triggervalues.resize(1,keeper);
    infile >> triggervalues[i] >> ws;
    triggervalues[i].Inform(keeper);
    i++;
  }

  infile >> text >> ws;


  if (strcasecmp(text, "harvestrates") != 0)
    handle.logFileUnexpected(LOGFAIL, "harvestrates", text);

  i = 0;
  while ((infile.peek() == '(' || infile.peek()== '#' || isdigit(infile.peek())) && !infile.eof()) {
    harvestrates.resize(1,keeper);
    infile >> harvestrates[i] >> ws;
    harvestrates[i].Inform(keeper);
    i++;
  }




  if(!((triggervalues.Size() == 2 && harvestrates.Size()==2) || (triggervalues.Size() == 4 && harvestrates.Size()==3)))
    handle.logFileUnexpected(LOGFAIL, "wrong size of harvestrates and triggervalues 2-2 and 3-4 legal"," ");
  // checks on size of different vectors required.  
  // quotasteps are the steps where the quota is allocated 
  // 3 4 5 6  means 3-6 timesteps from current timestep i.e the quotacalcstep
  i = 0;
  infile >> text >> ws;
  if (strcasecmp(text, "quotasteps") != 0)
    handle.logFileUnexpected(LOGFAIL, "quotasteps", text);
  while (isdigit(infile.peek()) && !infile.eof()) {
    quotasteps.resize(1,0);
    infile >> quotasteps[i] >> ws;
    i++;
  }
  infile >> text >> ws;
  // Could be made more complicated and the quotaproportions could be a matrix by fleet and quotastep.  
  i = 0;
  if (strcasecmp(text, "quotaproportions") != 0)
    handle.logFileUnexpected(LOGFAIL, "quotaproportions", text);
  while (isdigit(infile.peek()) && !infile.eof()) {
    quotaproportions.resize(1,0.0);
    infile >> quotaproportions[i] >> ws;
    i++;
  }
  if(quotaproportions.Size() != quotasteps.Size()) 
    handle.logFileMessage(LOGFAIL, "\nError in proglikelihood - size of quotaseps and quotaproportions do not match");
  double quotacheck = 0;
  for(i = 0; i < quotaproportions.Size() ; i++) 
    quotacheck += quotaproportions[i];
  if(quotacheck < 0.9999 || quotacheck > 1.0001) 
    handle.logFileMessage(LOGFAIL, "\nError in proglikelihood - sum of quotaproportions on timesteps is not 1");   
  // Read the proportion of catch by fleet
  infile >> text >> ws;
  i = 0;
  if (strcasecmp(text, "fleetproportions") != 0)
    handle.logFileUnexpected(LOGFAIL, "fleetproportions", text);
  while ((infile.peek() == '(' || infile.peek()== '#' || isdigit(infile.peek())) && !infile.eof()) {
    fleetproportions.resize(1,keeper);
    infile >> fleetproportions[i] >> ws;
    fleetproportions[i].Inform(keeper);
    i++;
  }
  if(fleetproportions.Size() != fleetnames.Size()) 
    handle.logFileMessage(LOGFAIL, "\nError in proglikelihood - size of fleets and fleetproportions do not match");
  
  /*double fleetcheck = 0;
  for(i = 0; i < fleetproportions.Size() ; i++) 
    fleetcheck += fleetproportions[i];
  if(fleetcheck < 0.9999 || fleetcheck > 1.0001) 
  handle.logFileMessage(LOGFAIL, "\nError in proglikelihood - sum of fleetproportion on fleets is not 1");   */
  //readWordAndVariable(infile,"weightoflastyearstac",weightoflastyearstac);
  
  infile >> text >> ws;
  i = 0;
  if (strcasecmp(text, "weightoflastyearstac") != 0)
    handle.logFileUnexpected(LOGFAIL, "weightoflastyearstac", text);
  //while ((infile.peek() == '(' || infile.peek()== '#' || isdigit(infile.peek())) && !infile.eof()) {
  //weightoflastyearstac.resize(1,keeper);
  infile >> weightoflastyearstac >> ws;
  weightoflastyearstac.Inform(keeper);
    //  i++;
    //}
  
  
  readWordAndVariable(infile,"maxchange",maxchange);
  readWordAndVariable(infile,"functionnumber",functionnumber);
  readWordAndVariable(infile,"firsttacyear",firsttacyear);
  readWordAndVariable(infile,"assessmentstep",assessmentstep);

  infile >> ws >> text >> ws;
  if (strcasecmp(text, "asserr") != 0)
    handle.logFileUnexpected(LOGFAIL, "asserr", text);
  asserr.read(infile,TimeInfo,keeper);
  
  infile >> ws; 
  if(infile.peek() == 'i' || infile.peek() == 'I'){
    infile >> text >> ws;
    if (strcasecmp(text, "implerr") != 0)
      handle.logFileUnexpected(LOGFAIL, "implerr", text);
    implerr.read(infile,TimeInfo,keeper);
  }
  
  infile >> ws;
  // Setting vectors
  tachistory.resize(TimeInfo->getLastYear() - TimeInfo->getFirstYear() +1,0.0);
  tachistory[firsttacyear- TimeInfo->getFirstYear()-1] = lastyearstac;  // year febore firsttacyear;
  bio1.resize(TimeInfo->getLastYear() - TimeInfo->getFirstYear() +1,0.0);
  bio2.resize(TimeInfo->getLastYear() - TimeInfo->getFirstYear() +1,0.0);
  bio1werr.resize(TimeInfo->getLastYear() - TimeInfo->getFirstYear() +1,0.0);
  bio2werr.resize(TimeInfo->getLastYear() - TimeInfo->getFirstYear() +1,0.0);
  historicalhr.resize(TimeInfo->getLastYear() - TimeInfo->getFirstYear() +1,0.0);
  // should perhaps have a vecor of assyears to use for biannual assessments etc.  
  firstyear = TimeInfo->getFirstYear(); 
 // For agreement with fleets the indexing here should perhaps be one higher.  
}

void ProgLikelihood::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int s, i, j, found;

  for (i = 0; i < fleetnames.Size(); i++) {
    found = 0;
    for (j = 0; j < Fleets.Size(); j++) {
      if (strcasecmp(fleetnames[i], Fleets[j]->getName()) == 0) {
        found++;
        fleets.resize(Fleets[j]);
      }
    }
    if (found == 0)
      handle.logMessage(LOGFAIL, "Error in proglikelihood - unrecognised fleet", fleetnames[i]);
  }
  for (s = 0; s < stocknames.Size(); s++) {
    found = 0;
    for (j = 0; j < Stocks.Size(); j++) {
      if (strcasecmp(stocknames[s], Stocks[j]->getName()) == 0) {
	found++;
	stocks.resize(Stocks[j]);
      }
    }
    if (found == 0)
      handle.logMessage(LOGFAIL, "Error in proglikelihood - unrecognised stock", stocknames[s]);
  }
}



void ProgLikelihood::PrintLog(ofstream& outfile) const {
  int i;
  int j;

  outfile << "biocoeffs "; 
  for(i = 0; i < biocoeffs.Nrow(); i++) {
    for(j = 0; j < biocoeffs[i].Size(); j++) outfile << biocoeffs[i][j] << " ";
    outfile << endl;
  }
  outfile << endl;

  outfile << "triggercoeffs "; 
  for(i = 0; i < triggercoeffs.Nrow(); i++) {
    for(j = 0; j < triggercoeffs[i].Size(); j++) outfile << triggercoeffs[i][j] << " ";
    outfile << endl;
  }
  outfile << endl;

  outfile << "fleetnames "; 
  for(i = 0; i < fleetnames.Size(); i++) outfile << fleetnames[i] << " ";
  outfile << endl;

  outfile << "stocknames "; 
  for(i = 0; i < stocknames.Size(); i++) outfile << stocknames[i] << " ";
  outfile << endl;

  outfile << "triggervalues "; 
  for(i = 0; i < triggervalues.Size(); i++) outfile << triggervalues[i] << " ";
  outfile << endl;

  outfile << "harvestrates "; 
  for(i = 0; i < harvestrates.Size(); i++) outfile << harvestrates[i] << " ";
  outfile << endl;

  outfile << "quotasteps "; 
  for(i = 0; i < quotasteps.Size(); i++) outfile << quotasteps[i] << " ";
  outfile << endl;

  outfile << "quotaproportions "; 
  for(i = 0; i < quotaproportions.Size(); i++) outfile << quotaproportions[i] << " ";
  outfile << endl;

  outfile << "fleetproportions "; 
  for(i = 0; i < fleetproportions.Size(); i++) outfile << fleetproportions[i] << " ";
  outfile << endl;

  outfile << "weightoflastyearstac" << " " << weightoflastyearstac << endl;
  
  if(weightoflastyearstac>1 | weightoflastyearstac<0)
    handle.logMessage(LOGFAIL, "Error in proglikelihood - value of weightoflastyearstac should be between 0 and 1, value supplied is: ", weightoflastyearstac);

  outfile <<  "maxchange"<< " " << maxchange << endl;
  outfile <<  "functionnumber"<< " " << functionnumber << endl;
  outfile <<  "firsttacyear" << " " << firsttacyear << endl;
  outfile <<  "assessmentstep"<< " " << assessmentstep  << endl;

}

void ProgLikelihood::Print(ofstream& outfile) const {}
// biomasses in first yearsare zero if we start after quotastep in the first year
// still to see how this function is called.  
void ProgLikelihood::printLikelihood(ofstream& outfile,const TimeClass* const TimeInfo) {
  int i,year;
  if(TimeInfo->getTime()==TimeInfo->calcSteps(TimeInfo->getLastYear(),TimeInfo->getLastStep())){
    outfile << "\n; -- data --";
    outfile << "\n; year reference_biomass reference_biomass_with_error trigger_biomass trigger_biomass_with_error";
    outfile << "harvest_rate tac\n";
      for(i = 0 ; i <= (bio1.Size()-1) ; i++) {
	outfile << i+firstyear << "\t" << bio1[i] << "\t" << bio1werr[i] << "\t" << bio2[i] << "\t" << bio2werr[i] << 
	  "\t" << historicalhr[i] << "\t" << tachistory[i] << endl;
      }
    }
}

void ProgLikelihood::Reset(const TimeClass* const TimeInfo) {
  asserr.Update(TimeInfo); 
  implerr.Update(TimeInfo); 
}





void ProgLikelihood::Reset(const Keeper* const keeper) { 
}



// Distribute the quota on the specified fleets.  Have to take care of not going beyond last timestep.  
// setTimeMultiplier takes care of that.  Check if SetTimeMultiplier and timemultiplier should be on lengthpredator.  
void ProgLikelihood::AllocateTac(const TimeClass* const TimeInfo){
  int i,j;
  double tmptac;
  int currenttime = TimeInfo->getTime();
  for(i = 0; i < quotasteps.Size(); i++){
    for(j = 0; j < fleets.Size(); j++){
      
      tmptac =  calculatedtac*quotaproportions[i]* fleetproportions[j]*implerr;
      fleets[j]->getPredator()->setTimeMultiplier(TimeInfo,quotasteps[i],tmptac);
    }
  }
}  
					    
// Get  the two biomasses.  They are baed on a logitfunction with 3 parameters where the first one is the slope, second
// one l50 and third one level.  Level should not be higher than 1.  Trigger based on mature stock would 
// put level of immature as 0.  
 
void ProgLikelihood::CalcBiomass(const TimeClass* const TimeInfo){
  int i;
  int index = TimeInfo->getYear() - TimeInfo->getFirstYear();
  bio1[index] = bio2[index] = 0;
  for (i = 0; i < stocks.Size(); i++) {
    bio1[index] += stocks[i]->getWeightedStockBiomassAllAreas(biocoeffs[i]);
    bio2[index] += stocks[i]->getWeightedStockBiomassAllAreas(triggercoeffs[i]);
  }
  bio1werr[index] = bio1[index]*asserr;
  bio2werr[index] = bio2[index]*asserr;
}

// TimeInfo might be used if we need assessment error, else it is not needed.  This function is extremely simple here 
// (3 triggers+2 harvestrates) but can be changed if it works.  

void ProgLikelihood::CalcTac(const TimeClass* const TimeInfo) {  // can use function number later to get variants
  double hr;
  // Number of triggers it either 2 or 4.  
  // Number of harvest rates is either 2 or 3
  
  // 2 triggers and two harvest rations example trigger 0,Bpa HR 0,0.4  
  int index = TimeInfo->getYear() - TimeInfo->getFirstYear();
  if(bio2werr[index] < triggervalues[0]) 
    hr = harvestrates[0];
  
  if((bio2werr[index] >  triggervalues[0]) & (bio2werr[index] < triggervalues[1])) 
    hr = harvestrates[0]+(bio2werr[index]-triggervalues[0])/(triggervalues[1]-triggervalues[0])*(harvestrates[1]-harvestrates[0]);
  
  if(bio2werr[index] > triggervalues[1]) 
    hr = harvestrates[1];
  
  // 2 more triggers+1 harvestratio
  if(triggervalues.Size() == 4) {
    if(bio2werr[index] > triggervalues[1] & bio2werr[index] < triggervalues[2]) 
      hr = harvestrates[1];
    
    if((bio2werr[index] >  triggervalues[2]) & (bio2werr[index] < triggervalues[3])) 
      hr = harvestrates[1]+(bio2werr[index]-triggervalues[2])/(triggervalues[3]-triggervalues[2])*(harvestrates[2]-harvestrates[1]);
    
    if(bio2werr[index] > triggervalues[3]) 
      hr = harvestrates[2];
  }  
  
  calculatedtac = hr*bio1werr[index];
  historicalhr[index] = hr;
  if(TimeInfo->getYear() == firsttacyear)
    tachistory[index] = calculatedtac;  // not set historically
    
  if(TimeInfo->getYear() > firsttacyear){
    tachistory[index] = (1-weightoflastyearstac)*calculatedtac + weightoflastyearstac*tachistory[index-1];  // not set historically
    calculatedtac = tachistory[index];
  }
}


void ProgLikelihood::addLikelihood(const TimeClass* const TimeInfo) {
  if(TimeInfo->getStep() == assessmentstep){ 
    // this is the only step we do anything , but we never change likelihood
    this->CalcBiomass(TimeInfo);  // Calculate fishable (bio1) and target (bio2) biomass.  
    this->CalcTac(TimeInfo);  // Calculate Tac
    // This is not done historically
    if(TimeInfo->getYear() >= firsttacyear){
      this->AllocateTac(TimeInfo); // Change the Tac for the targetfleets
    }
  }
}
