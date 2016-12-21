##' \item{calcindex}{output survey index likelihood file}
##' \item{calc.in.kilos}{output catches in kilos likelihood file}
##' \item{estimate.recruits.by.year}{should recruits be estimated by year}
##' \item{estimate.recruits.by.year.and.area}{should recruits be estimated by year and area}
##' \item{estimate.recruits.by.year.constant.area}{should recruits be estimated by year with constant by area effect (fewer degrees of freedom used compared to the one above).}
##' \item{randomised.recruits}{Should recruits be initialised randomly}
##' \item{bylen}{output survey index likelihood file by length groups, 0 for age aggregation and 1 for length aggregation}
##' \item{length.groups}{length groups for indices}
##' \item{calcldist.c}{output length distribution likelihood data for the catch}
##' \item{calcalk.c}{output age length distribution likelihood data for the catch}
##' \item{calcldist.s}{output length distribution likelihood data for the survey}
##' \item{calcalk.s}{output age length distribution likelihood data for the survey}
##' \item{file.migration.ratio}{Output migration matricies (0) or ratios (1)}
##' \item{alkagg}{aggregation level for age length distributions, will be used for both}
##' \item{survey.sigma}{sigma for error in survey indices}
##' \item{catch.sigma}{sigma for error in catchdistributions}
##' \item{frac}{fraction of catch that should be put in likelihood files}
##' \item{doesgrow}{do the stocks grow}
##' \item{growthfunction}{What growth function should be used, currently only lengthvbsimple is implemented}
##' \item{optim.params}{What parameters should Gadget optimise}
##' \item{randomise.all.initial.values}{Should initial values be randomized} 
gadget.file.options <- function(){
  ##################################
##
## Gadget likelihood files
## used when running gadget -l
## ?should file options be here?
#
## output survey index likelihood file
## this calculates 2 indices for each area
## one for recruits and another representing all age2+ fish
              calcindex = 1,
              calc.in.kilos = 0,
#                                        # should recruits be estimated
              estimate.recruits.by.year=1,
              estimate.recruits.by.year.and.area=0,
              estimate.recruits.by.year.constant.area=0,
              randomised.recruits=0,
#              
## output survey index likelihood file by length groups
## 0 for age aggregation
## 1 for length aggregation
              bylen = 1,
## length groups for indices
## only 2 allowed - unless you edit files.splus
## filesurveyindexlen()
## filelikelihood()
## fileaggregation()
              length.groups=c(4,16,90),
## output length distribution likelihood data for the catch
              calcldist.c=1,
# output age length distribution likelihood data for the catch
              calcalk.c=1,

# output length distribution likelihood data for the survey
              calcldist.s=1,
# output age length distribution likelihood data for the survey
              calcalk.s=1,
# output migration matricies or ratios
              file.migration.ratio=0,
# aggregation level for age length distributions
# will be used for both
              alkagg = 5,

#########################
#
# lognormal error can be added to the likelihood data
#

# sigma for error in survey indices
              survey.sigma=0,
# sigma for error in catchdistributions
              catch.sigma=0,

# fraction of catch that should be put in likelihood files
              frac=0.1,
# growthfunction
              doesgrow=1,
              growthfunction='lengthvbsimple',
# optimiser
              optim.params='',
              randomise.all.initial.values = 0
}


############################################
#
# This file contains functions which make the following 
# inputfiles for Gadget's two stock single area example:
#
#	function		file
#	--------		-------------
#	filerec()		example.rec
#	filerec.est()		example.rec to estimate parameters
#	fileinit()		example.imm.init, example.mat.init
#	filerefw()		example.refw
#	filearea()		area
#       filetime()              time
#	filefleet()		fleet, fleet.data
#	fileotherfood()		otherfood, otherfood.data
#       filemain()              mainx
#	filestock()		example.mat example.imm
#	filemigrate()		example.migration
#	filesurveyindex()	example.surveyindex
#	filesurveyindexlen()	example.surveyindexlen
#	fileldist.catch()	example.ldist.catch
#	fileldist.survey()	example.ldist.survey
#	filealk.catch()	        example.alk.catch
#	filealk.survey()	example.alk.survey
#
# These are the input files which are the same for all examples.
#
############################################
##' Makefiles
##'
##' Create the necessary input files for gadget
##' @param opt
##' @param sim
##' @param location
makefiles <- function(opt,sim,location='Gfiles'){
  
  filemain(opt,sim,location=location)
  filetime(opt,sim,location=location)
  filearea(opt,sim,location=location)
  fileaggregation(opt,sim,location=location)
  fileinput(opt,sim,location=location)
  filelikelihood(opt,sim,location=location)
  filerec(opt,sim,location=location)
  filerefw(opt,sim,location=location)
  filefleet(opt,sim,location=location)
  fileotherfood(opt,sim,location=location)
  filesurveyindex(opt,sim,location=location)
  filesurveyindexlen(opt,sim,location=location)
  fileldist.survey(opt,sim,location=location)
  fileldist.catch(opt,sim,location=location)
#  fileldistagg.catch(
  fileoptinfo(opt,sim,location=location)
#  filealk.survey(opt,sim)
#  filealk.catch(opt,sim)
  fileinit(opt,sim,location=location)
  filerefw(opt,sim,location=location)
  filestock(opt,sim,location=location)
  filemigrate(opt,sim,location=location)
  fileprint(opt,sim,location=location)
  if(opt$calc.in.kilos>0){
    filecatchinkilos(opt,sim,location=location)
  }
}



####################################################
#' make the main file for the example
##' @param opt Gadget options list
##' @param sim results from simulation
##' @param location output folder
##' @param file name of the output main file
filemain<-function(opt,sim,location='Gfiles',file='main')
{
  main.text <- 
    paste("; main file for the gadget example",
          "; created automatically from R-gadget",
          "timefile\ttime",
          "areafile\tarea",
          "printfiles\tprintfile",
          "[stock]",
          "stockfiles\texample.imm\texample.mat",
          "[tagging]",
          "[otherfood]",
          ifelse(opt$doeseat>0,
                 "otherfoodfiles\totherfood",""),
          "[fleet]",
          "fleetfiles\tfleet",
          "[likelihood]",
          "likelihoodfiles\tlikelihood",sep='\n')
  write(main.text,paste('.',location,file,sep='/'))
}

############################################
#' make the file time for the example
filetime<-function(opt,sim,location='Gfiles',file='time')
{
  col1 <- c("firstyear","firststep","lastyear","laststep")
  col2 <- c(1,1,opt$numobs,opt$numoftimesteps)
  time.text <-
    paste("; time file for the gadget example",
          "; created automatically from R-gadget",
          paste(col1,col2,sep='\t',collapse='\n'),
          paste("notimesteps",opt$numoftimesteps,
                paste(rep((12/opt$numoftimesteps),
                          opt$numoftimesteps),
                      collapse='\t'),
                sep='\t'),
          sep='\n')  
  write(time.text,paste('.',location,file,sep='/'))
}

############################################
#' make the file area for the 1 area example
#' the order of the file's columns is
#' year, step, area, temperature.
filearea<-function(opt,sim,location='Gfiles',file='area')
{
  area.text <-
    paste("; area file for the gadget example",
          "; created automatically from R-gadget",
          paste(c("areas",1:opt$numofareas),collapse='\t'),
          paste(c("size",rep(opt$areasize,opt$numofareas)),collapse='\t'),
          "temperature",
          sep='\n')
  write(area.text,paste('.',location,file,sep='/'))
  year<-rep(1:opt$numobs,rep(opt$numofareas*opt$numoftimesteps,opt$numobs))
  step<-(1:opt$numoftimesteps)
  area<-rep(1:opt$numofareas,each=opt$numoftimesteps)
  
  Area<-cbind(year,step,area,opt$area.temperature)
  write.table(Area,
              paste('.',location,file,sep='/'),
              sep="\t",
              row.names=F,
              col.names=F,
              quote=F,
              append=T)
}

############################################
#' make the various aggregation files for the example
fileaggregation<-function(opt,sim,location='Gfiles',file='area') {
  #first the area aggregation files
  area.agg <-
    paste("; area aggregation file - no aggregation",
          "; created automatically from R-gadget",
          paste(sprintf('area%1$s\t%1$s',1:opt$numofareas),collapse='\n'),
          sep='\n')
  write(area.agg,paste('.',location,paste(file,'.agg',sep=''),sep='/'))
  area.n.agg <-
    paste("; area aggregation file - area 1 only",
          "; created automatically from R-gadget",
          "area%1$s\t%1$s",
          sep='\n')
  for(i in 1:opt$numofareas){
    write(sprintf(area.n.agg,i),
          sprintf(paste('.',location,paste(file,'%s.agg',sep=''),sep='/'),i))
  }
  allareas.agg <-
    paste("; area aggregation file - all areas",
          "; created automatically from R-gadget",
          paste(c("allareas",1:opt$numofareas),collapse='\t'),
          sep='\n')
  write(allareas.agg,paste('.',location,'allareas.agg',sep='/'))
  #then the age aggregation files
  allage.agg <-
    paste("; age aggregation file - all age groups",
          "; created automatically from R-gadget",
          paste(c("allages",opt$minage:opt$maxage),collapse='\t'),
          sep='\n')
  write(allage.agg,paste('.',location,'allage.agg',sep='/'))
  if(opt$calcindex>0) {
    if(opt$bylen>0) {
      len.agg.file <-
        paste(
              "; length aggregation file - length group 1",
              "; created automatically from R-gadget",
              paste(
                    paste(
                          sprintf('lengp%s',opt$length.groups[-1]),
                          opt$length.groups[-length(opt$length.groups)],
                          opt$length.groups[-1],sep='\t'),
                    collapse='\n'),
              sep='\n',
              collapse='\n')
      write(len.agg.file,paste('.',location,'lensurvey.agg',sep='/'))
    } else {
      agesurv.R.agg <-
        paste("; age aggregation file - recruitment separate",
              "; created automatically from R-gadget",
              paste("age1    ",opt$minage),
              sep='\n')
      write(agesurv.R.agg,paste('.',location,"/agesurveyR.agg",sep='/'))
      agesurv.O.agg <-
        paste("; age aggregation file - recruitment separate",
              "; created automatically from R-gadget",
              paste("age1",opt$minage,sep='\t'),
              paste(c("ageother",(opt$minage+1):opt$maxage),collapse='\t'),
              sep='\n')
      write(agesurv.O.agg,paste('.',location,"agesurveyO.agg",sep='/'))
    }
  }
  age.agg <-
    paste("; age aggregation file - no aggregation",
          "; created automatically from R-gadget",
          sep='\n')
  lage <- opt$minage:opt$maxage
  write(age.agg,paste('.',location,"age.agg",sep='/'))
  write.table(cbind(sprintf("age%s",lage),lage),
              paste('.',location,"age.agg",sep='/'),
              quote=F, row.names=F, col.names=F, append=T, sep="\t")

  ##finally the length aggregation files
  filelenagg(1,opt,location,'len')
  filelenagg(5,opt,location,'len')
  filelenagg(10,opt,location,'len')
  filelenagg(opt$numoflgroups,opt,location,'alllen')
  filelenagg(opt$alkagg,opt,location,'len')
  
}


filelenagg <- function(aggl,opt,location,file){
  len.agg <-
    paste("; length aggregation file - %s cm aggregation",
          "; created automatically from R-gadget",
          sep='\n')
  inum <- floor(opt$numoflgroups/aggl)
  li <- 1:inum
  minl <- seq(opt$minlen, opt$minlen+(inum-1)*aggl,aggl)
  maxl <- seq(opt$minlen+aggl, opt$minlen+inum*aggl, aggl)
  if(aggl==1|aggl==opt$maxlen)
    aggl <- ''
  write(len.agg,paste('.',location,
                        sprintf('%s%s.agg',file,aggl),
                        sep='/'))
  write.table(cbind(paste("len",li,sep=""),minl,maxl),
              paste('.',location,
                    sprintf('%s%s.agg',file,aggl),
                    sep='/'),
              quote=F, row.names=F, col.names=F, append=T, sep="\t")

}

############################################
#' make the input file for the example with correct starting values
fileinput<-function(opt,sim,
#                    optim.params,
                    location='Gfiles',
                    file='refinputfile'
                    )
{
  ## parameters to be estimated
  param.names <- c('linf',
                   'k',
                   'beta',
                   'comma',
                   'commb',
                   'surveya',
                   'surveyb')
  initial.values <- c(opt$lsup*0.1,
                      opt$k*100,
                      opt$beta,
                      opt$salphacomm,
                      opt$sbetacomm*10,
                      opt$salphasurv,
                      opt$sbetasurv)
  lower.values <- c(opt$maxlen*0.1,
                    0.01*100,
                    1,
                    -10,
                    0.01,
                    -10,
                    0.01)
  upper.values <- c((opt$lsup + 20)*0.1,
                    0.2*100,
                    1000,
                    -0.01,
                    10,
                    -0.01,
                    10)
  if(opt$calc.in.kilos==1){
    param.names <- c(param.names,'mult')
    initial.values <- c(initial.values,'random')
    lower.values <- c(lower.values,0.01)
    upper.values <- c(upper.values,10)
  }
  if(opt$estimate.recruits.by.year==1){
##    rec <- aggregate(opt$n,
##                     by=list(rep(1:opt$numobs,each=length(opt$n)/numobs)),
##                     sum)
    param.names <- c(param.names,paste('rec',1:opt$numobs,sep=''))
    initial.values <- c(initial.values,rep(1,opt$numobs))
    lower.values <- c(lower.values,rep(0.01,opt$numobs))
    upper.values <- c(upper.values,rep(10,opt$numobs))
  } else if(opt$estimate.recruits.by.year.and.area==1){
    param.names <- c(param.names,
                     paste('rec',
                           rep(1:opt$numobs,each=opt$numofareas),
                           '.',
                           rep(1:opt$numofareas,opt$numobs),
                           sep=''))
    initial.values <- c(initial.values,rep(1,opt$numofareas*opt$numobs))
    lower.values <- c(lower.values,rep(0.01,opt$numofareas*opt$numobs))
    upper.values <- c(upper.values,rep(10,opt$numofareas*opt$numobs))
  } else if(opt$estimate.recruits.by.year.constant.area==1){
    param.names <- c(param.names,
                     paste('rec',1:opt$numobs,sep=''),
                     paste('p',1:(opt$numofareas-1),sep=''))
    initial.values <- c(initial.values,rep(1,opt$numobs),
                        opt$probarea[1:(opt$numofareas-1)])
    lower.values <- c(lower.values,rep(0.01,opt$numobs),
                      0.001)
    upper.values <- c(upper.values,rep(10,opt$numobs),1)
  }
  if(opt$doesfuncmigrate==1){
        param.names <- c(param.names,
                         'diffusion','driftx','drifty')
    initial.values <- c(initial.values,opt$diffusion,opt$driftx,opt$drifty)
    lower.values <- c(lower.values,rep(0,3))
    upper.values <- c(upper.values,rep(10,3))
  }
  
  params <-
    data.frame(switch=param.names,
               value=initial.values,
               lower=lower.values,
               upper=upper.values,
               optimise=1*(param.names %in% opt$optim.params))
  if(opt$randomised.recruits==1){
    params$value[agrep('rec',params$switch)] <- 'random'
  }

  if(opt$randomise.all.initial.values==1){
    params$value[params$optimise==1] <- 'random'
  }
    
  input.text <-
    paste("; input file for the gadget example",
          "; created automatically from R-gadget",
          paste(names(params),collapse='\t'),
          sep='\n')
  write(input.text,paste('.',location,file,sep='/'))
  write.table(params,file=paste('.',location,file,sep='/'),
              quote=FALSE, row.names=FALSE, col.names=FALSE,
              append=TRUE, sep="\t")
}

############################################
#' make the likelihood file for the example
filelikelihood<-function(opt,sim,location='Gfiles',file='likelihood')
{
  likelihood <-
    paste("; likelihood file for the gadget example",
          "; created automatically from R-gadget",
          "[component]",
          "name\t\tbounds",
          "weight\t\t1",
          "type\t\tpenalty",
          "datafile\tpenaltyfile",
          
          ";\n[component]",
          "name\t\tunderstocking",
          "weight\t\t1",
          "type\t\tunderstocking",
          sep='\n')

  if(opt$calcindex>0){
    likelihood <-
      paste(likelihood,
            paste(sprintf(paste(";\n[component]",
                                "name\t\tsi.area%1$s.R",
                                "weight\t\t1e-8",
                                "type\t\tsurveyindices",
                                ifelse(opt$bylen==1,
                                       paste("datafile\texample.surveyindexlen",
                                             "sitype\t\tlengths",
                                             "areaaggfile\tarea%1$s.agg",
                                             "ageaggfile\tlensurveyR.agg",
                                             "stocknames\texample.imm",
                                             "fittype\t\tfixedloglinearfit",
                                             sep='\n'),
                                       paste("datafile\texample.surveyindex",
                                             "sitype\t\tages",
                                             "areaaggfile\tarea%1$s.agg",
                                             "ageaggfile\tagesurveyR.agg",
                                             "stocknames\texample.imm",
                                             "fittype\t\tfixedlinearfit",
                                             sep='\n')),
                                "slope\t\t1",
                                "intercept\t0",
                                
                                ";\n[component]",
                                "name\t\tsi.area%1$s.O",
                                "weight\t\t1e-8",
                                "type\t\tsurveyindices",
                                ifelse(opt$bylen==1,
                                       paste("datafile\texample.surveyindexlen",
                                             "sitype\t\tlengths",
                                             "areaaggfile\tarea%1$s.agg",
                                             "ageaggfile\tlensurveyO.agg",
                                             "stocknames\texample.imm\texample.mat",
                                             "fittype\t\tfixedloglinearfit",
                                             sep='\n'),
                                       paste("datafile\texample.surveyindex",
                                             "sitype\t\tages",
                                             "areaaggfile\tarea%1$s.agg",
                                             "ageaggfile\tagesurveyO.agg",
                                             "stocknames\texample.imm\texample.mat",
                                             "fittype\t\tfixedlinearfit",
                                             sep='\n')),
                                "slope\t\t1",
                                "intercept\t0",
                                sep='\n')
                          ,1:opt$numofareas),
                  collapse='\n'),
            sep='\n')
  }

  
  if(sum( opt$doescatchcomm+opt$doescatchsurv)>0) {
    if(opt$calcalk.c==1){
      likelihood <-
        paste(likelihood,
              ";\n[component]",
              "name\t\talk.catch",
              "weight\t\t1",
              "type\t\tcatchdistribution",
              "datafile\texample.alk.catch",
              "function\tsumofsquares",
              "areaaggfile\tarea.agg",
              "ageaggfile\tage.agg",
              paste("lenaggfile\tlen",opt$alkagg,".agg",sep=""),
              "fleetnames\tcomm",
              "stocknames\texample.imm\texample.mat",
              sep='\n')
    }

    if(opt$calcalk.s == 1) {
      likelihood <-
        paste(likelihood,
              ";\n[component]",
              "name\t\talk.survey",
              "weight\t\t1",
              "type\t\tcatchdistribution",
              "datafile\texample.alk.survey",
              "function\tsumofsquares",
              "areaaggfile\tarea.agg",
              "ageaggfile\tage.agg",
              paste("lenaggfile\tlen",opt$alkagg,".agg",sep=""),
              "fleetnames\tsurvey",
              "stocknames\texample.imm\texample.mat",
              sep='\n')
    }    

    
    
    if(opt$calcldist.c > 0) {
      likelihood <-
        paste(likelihood,
              ";\n[component]",
              "name\t\tldist.catch",
              "weight\t\t1",
              "type\t\tcatchdistribution",
              "datafile\texample.ldist.catch",
              "function\tsumofsquares",
              "areaaggfile\tarea.agg",
              "ageaggfile\tallage.agg",
              "lenaggfile\tlen.agg",
              "fleetnames\tcomm",
              "stocknames\texample.imm\texample.mat",
              sep='\n')
    }

    if(opt$calcldist.s > 0) {
      likelihood <-
        paste(likelihood,
              ";\n[component]",
              "name\t\tldist.survey",
              "weight\t\t1",
              "type\t\tcatchdistribution",
              "datafile\texample.ldist.survey",
              "function\tsumofsquares",
              "areaaggfile\tarea.agg",
              "ageaggfile\tallage.agg",
              "lenaggfile\tlen.agg",
              "fleetnames\tsurvey",
              "stocknames\texample.imm\texample.mat",
              sep='\n')
    }
    if(opt$calc.in.kilos>0){
      likelihood <-
        paste(likelihood,
              ";\n[component]",
              "name\t\tcatch.kilos",
              "weight\t\t1e-1",
              "type\t\tcatchinkilos",
              "datafile\texample.kilos",
              "function\tsumofsquares",
              "areaaggfile\tarea.agg",
              "fleetnames\tcomm",
              "stocknames\texample.imm\texample.mat",
              sep='\n')
    }
  }
  write(likelihood,paste('.',location,file,sep='/'))
  filepenalty(opt,sim,location=location,file='penaltyfile')
}
##' Gadget Penalty file
filepenalty <- function(opt,sim,location='Gfiles',file='penaltyfile'){
  penalty <- paste("; penalty file for the gadget example",
                   "; created automatically from R-gadget",
                   "; switch - power - lower - upper",
                   "default\t2\t10000\t10000 ; defaults",
                   sep='\n')
  write(penalty,paste('.',location,file,sep='/'))
}



############################################
#' make the print file for the example
fileprint<-function(opt,sim,location='Gfiles',
                    file='printfile',output.folder='out')
{
  print.text <-
    paste("; print file for the gadget example\n",
          "; created automatically from R-gadget\n",
          "[component]\n",
          "type\t\tlikelihoodsummaryprinter\n",
          "printfile\t%1$s/summary.print\n",
          paste(sprintf(paste(
                              ";\n[component]",
                              "type\t\tstockfullprinter",
                              "stockname\texample.%1$s",
                              "printfile\t%%1$s/example.%1$s.full",
                              "yearsandsteps\tall\tall",
                              
                              ";\n[component]",
                              "type\t\tstockstdprinter",
                              "stockname\texample.%1$s",
                              "printfile\t%%1$s/example.%1$s.std",
                              "yearsandsteps\tall\tall",
                              
                              ";\n[component]",
                              "type\t\tstockprinter",
                              "stocknames\texample.%1$s",
                              "areaaggfile\tarea.agg",
                              "ageaggfile\tage.agg",
                              "lenaggfile\tlen.agg",
                              "printfile\t%%1$s/example.%1$s",
                              "yearsandsteps\tall\tall",
                              sep='\n'),
                        opt$stocks),
                collapse='\n'),
          sep = '')
  if(opt$calcindex>0){
    print.text <-
      paste(print.text,
            paste(sprintf(paste(";\n[component]",
                                "type\t\tlikelihoodprinter",
                                "likelihood\tsi.area%1$s.R",
                                "printfile\t%%1$s/siarea%1$sR.print",
                                "yearsandsteps\tall\tall",

                                ";\n[component]",
                                "type\t\tlikelihoodprinter",
                                "likelihood\tsi.area%1$s.O",
                                "printfile\t%%1$s/siarea%1$sO.print",
                                "yearsandsteps\tall\tall",
                                sep='\n'),
                          1:opt$numofareas),
                  collapse='\n'),
            sep='\n')
  }

  if(sum(opt$doescatchcomm+opt$doescatchsurv)>0) {
    if(opt$calcldist.c > 0) {
      print.text <-
        paste(print.text,              
              ";\n[component]",
              "type\t\tlikelihoodprinter",
              "likelihood\tldist.catch",
              "printfile\t%1$s/ldist.catch",
              "yearsandsteps\tall\tall",
              
              ";\n[component]",
              "type\t\tpredatorprinter",
              "predatornames\tcomm",
              "preynames\t\texample.imm\texample.mat",
              "areaaggfile\tarea.agg",
              "predlenaggfile\talllen.agg",
              "preylenaggfile\tlen.agg",
              "biomass\t\t0",
              "printfile\t%1$s/example.fleet",
              "yearsandsteps\tall\tall",
              sep='\n')
  }
    if(opt$calcldist.s > 0) {
      print.text <-
        paste(print.text,
              ";\n[component]",
              "type\t\tlikelihoodprinter",
              "likelihood\tldist.survey",
              "printfile\t%1$s/ldist.survey",
              "yearsandsteps\tall\tall",
              
              ";\n[component]",
              "type\t\tpredatorprinter",
              "predatornames\tsurvey",
              "preynames\t\texample.imm\texample.mat",
              "areaaggfile\tarea.agg",
              "predlenaggfile\talllen.agg",
              "preylenaggfile\tlen.agg",
              "biomass\t\t0",
              "printfile\t%1$s/example.survey",
              paste("yearsandsteps\tall\t",paste(opt$survstep,collapse='\t'),sep=''),
              sep='\n')
    }
  }
  if(opt$calc.in.kilos>0){
    print.text <-
      paste(print.text,
            ";\n[component]",
            "type\t\tlikelihoodprinter",
            "likelihood\tcatch.kilos",
            "printfile\t%1$s/catch.kilos",
            "yearsandsteps\tall\tall",
            sep='\n')
  }
  
  if(opt$doeseat>0) {
    print.text <-
      paste(print.text,
            ";\n[component]",
            "type\t\tpredatorprinter",
            "predatornames\texample.mat",
            "preynames\t\texample.imm",
            "areaaggfile\tarea.agg",
            "predlenaggfile\tlen.agg",
            "preylenaggfile\tlen.agg",
            "biomass\t\t1",
            "printfile\t%1$s/example.eat",
            "yearsandsteps\tall\tall",
            sep='\n')
  }
  if(opt$doeseat>0|sum(opt$doescatchcomm+opt$doescatchsurv)>0) {
    print.text <-
      paste(print.text,
            ";\n[component]",
            "type\t\tstockpreyfullprinter",
            "stockname\texample.imm",
            "printfile\tout/example.imm.prey",
            "yearsandsteps\tall\tall",
            sep='\n')
  }
  write(sprintf(print.text,output.folder),
        paste('.',location,file,sep='/'))
}


############################################
#' make the stock files for the example
filestock<-function(opt,sim,location='Gfiles',file='example.')
{
  growth <- function(type,param,binn){
    if(type=='lengthvbsimple'){
      tmp <-
        list(growthfunction=type,
             growthparameters=c('( * 10 #linf)','( * 0.01 #k)',param),
             beta='#beta',
             maxlengthgroupgrowth=binn)
      return(tmp)
    } else {
      warning('Growth function not implemented in file export')
      return('')
    }
  }
  if(opt$doesgrow!=0){
    tmp <- growth(opt$growthfunction,c(opt$a,opt$b),opt$binn)
    growth.text <-
      paste('1',
            paste(names(tmp),sapply(tmp,
                                    function(x) paste(x,collapse='\t')),
                  sep='\t',collapse='\n'),
            sep='\n')
  } else {
    growth.text <- '0'
  }

  mat.suitability <-
    paste('suitability',
          paste('example.imm','function exponential',opt$spalpha,
                opt$spbeta,opt$spgamma, opt$spdelta,sep='\t'),
          paste('other','function constant',opt$otherfrac,sep='\t'),
          'preference',
          paste('example.imm',1,sep='\t'),
          paste('other',1,sep='\t'),
          paste('maxconsumption',opt$m0,0,0,opt$m3,sep='\t'),
          paste('halffeedingvalue',opt$H*opt$areasize,sep='\t'),
          sep='\n')

  initial.imm <-
    paste(c('minage','maxage','minlength','maxlength','dl','sdev','numberfile'),
          c(opt$immminage,opt$immmaxage,opt$minlen,opt$maxlen,
            opt$lengthgrouplen,1,'example.imm.init'),
          sep='\t',collapse='\n')
  initial.mat <-
    paste(c('minage','maxage','minlength','maxlength','dl','sdev','numberfile'),
          c(opt$matminage,opt$matmaxage,opt$minlen,opt$maxlen,
            opt$lengthgrouplen,1,'example.mat.init'),
          sep='\t',collapse='\n')

  renew <-
    paste(c('minlength','maxlength','numberfile'),
          c(opt$minlen,opt$maxlen,'example.rec'),
          sep='\t',collapse='\n')
  move <- paste('1',
                paste('transitionstocksandratios','example.mat','1',sep='\t'),
                paste('transitionstep','4',sep='\t'),
                sep='\n')
  mature <- paste(c('yearstepfile','definemmatrices'),
                  c('migration.time','migration.data'),
                  sep='\t',
                  collapse='\n')
  if(opt$doesfuncmigrate==1){
    migrate <-
      paste('1',paste(c('diffusion\t','driftx\t','drifty\t','lambda\t'),
                      c(opt$diffusion,opt$driftx,opt$drifty,opt$lambda),
                      c('#diffusion','#driftx','#drifty',''),
                      sep='',
                      collapse='\n'),
            'areadefinition\tmigration.area.data',
            sep='\n') 
  } else {
    migrate <-
      paste('1',
            ifelse(opt$file.migration.ratio,
                   paste(c('yearstepfile','defineratios'),
                         c('migration.time','migration.ratio'),
                         sep='\t',collapse='\n'),
                   paste(c('yearstepfile','definematrices'),
                         c('migration.time','migration.data'),
                         sep='\t',collapse='\n')
                   ),
            sep='\n'
            )
  }
  imm.stock <-
    list(stockname='example.imm',
         livesonareas=1:opt$numofareas,
         minage=opt$immminage,
         maxage=opt$immmaxage,
         minlength=opt$minlen,
         maxlength=opt$maxlen,
         dl=opt$lengthgrouplen,
         refweightfile='example.refw',
         growthandeatlengths='len.agg',
         doesgrow=growth.text,
         naturalmortality=opt$mort[opt$immminage:opt$immmaxage],
         iseaten=ifelse(FALSE,'0',
           '1\npreylengths\tlen.agg\nenergycontent\t1'),
         doeseat=ifelse(TRUE,'0',
           paste('1',mat.suitability,sep='\n')),
         initialconditions=paste('\n',initial.imm,sep=''),
         doesmigrate=ifelse(opt$doesmigrateimm==0,'0',migrate),
         doesmature=0,
         doesmove=ifelse(FALSE,'0',move),
         doesrenew=ifelse(FALSE,'0',
           paste('1',renew,sep='\n')),
         doesspawn=0,
         doesstray=0)
  
  mat.stock <-
    list(stockname='example.mat',
         livesonareas=1:opt$numofareas,
         minage=opt$matminage,
         maxage=opt$matmaxage,
         minlength=opt$minlen,
         maxlength=opt$maxlen,
         dl=opt$lengthgrouplen,
         refweightfile='example.refw',
         growthandeatlengths='len.agg',
         doesgrow=growth.text,
         naturalmortality=opt$mort[opt$matminage:opt$matmaxage],
         iseaten=ifelse(FALSE,'0',
           '1\npreylengths\tlen.agg\nenergycontent\t1'),
         doeseat=ifelse(FALSE,'0',
           paste('1',mat.suitability,sep='\n')),
         initialconditions=paste('\n',initial.mat,sep=''),
         doesmigrate=ifelse(opt$doesmigratemat==0,'0',migrate),
         doesmature=0,
         doesmove=ifelse(TRUE,'0',move),
         doesrenew=ifelse(TRUE,'0',
           paste('1',renew,sep='\n')),
         doesspawn=0,
         doesstray=0)
  imm.text <-
    paste("; stock file for the gadget example - example.imm",
          "; created automatically from R-gadget",
          paste(names(imm.stock),sapply(imm.stock,
                                        function(x) paste(x,collapse='\t')),
                sep='\t',collapse='\n'),
          sep='\n')
  mat.text <-
    paste("; stock file for the gadget example - example.mat",
          "; created automatically from R-gadget",
          paste(names(mat.stock),sapply(mat.stock,
                                        function(x) paste(x,collapse='\t')),
                sep='\t',collapse='\n'),
          sep='\n')
  write(imm.text,paste('.',location,paste(file,'imm',sep=''),sep='/'))
  write(mat.text,paste('.',location,paste(file,'mat',sep=''),sep='/'))

}

############################################
#' make the file example.init
#' the order of the file's columns is
#' area, age, length, number in length, mean weight
fileinit<-function(opt,sim,location='Gfiles')
{
  columns <- c('area','age','length','Freq','weight')
  immInit <- as.data.frame.table(sim$immStart)
  immInit$weight <- opt$w[as.numeric(immInit$length)]
  immInit <- immInit[columns]
  matInit <- as.data.frame.table(sim$matStart)
  matInit$weight <- opt$w[as.numeric(matInit$length)]
  matInit <- matInit[columns]

  imm.text <-
    paste("; initial conditions file for example.imm",
          "; created automatically from R-gadget",
          sprintf('; %s',paste(names(immInit),collapse='-')),
          sep='\n')
  write(imm.text,
        paste('.',location,"example.imm.init",sep='/'))
  
  write.table(immInit,
              paste('.',location,"example.imm.init",sep='/'),
              sep="\t",row.names=F,col.names=F,quote=F,append=T)

  mat.text <-
    paste("; initial conditions file for example.mat",
          "; created automatically from R-gadget",
          sprintf('; %s',paste(names(matInit),collapse='-')),
          sep='\n')
  write(mat.text,paste('.',location,"example.mat.init",sep='/'))
  write.table(matInit,
              paste('.',location,"example.mat.init",sep='/'),
              sep="\t",row.names=F,col.names=F,quote=F,append=T)

}


############################################
#' make the file example.rec 
#' the order of the file's columns is
#' year, step, area, age, length, number in length, mean weight
filerec<-function(opt,sim,location='Gfiles',file='example.rec')
{
  
  Rec <- as.data.frame.table(sim$Rec,stringsAsFactors=FALSE)
  Rec$year <- sapply(strsplit(Rec$time,'_'),function(x) x[2])
  Rec$step <- sapply(strsplit(Rec$time,'_'),function(x) x[4])
  Rec$weight <- opt$w[as.numeric(Rec$length)-opt$minlen+1]
  Rec$stepmass <- rep(tapply(Rec$Freq,Rec$time,sum),
                      each=dim(Rec)[1]/length(unique(Rec$time)))
  Rec$age <- rep(opt$immminage,dim(Rec)[1])
  if(opt$estimate.recruits.by.year==1){
    Rec$number <- paste('( * ',Rec$stepmass,Rec$Freq/Rec$stepmass,
                        paste('#rec',Rec$year,sep=''),
                        ')')
    
  } else if(opt$estimate.recruits.by.year.and.area==1){
    Rec$number <- paste('( * ',Rec$stepmass,Rec$Freq/Rec$stepmass,
                        paste('#rec',paste(Rec$year,Rec$area,sep='.'),
                              sep=''),
                        ')')
  } else if(opt$estimate.recruits.by.year.constant.area==1){
    Rec$number <- paste('( * ',Rec$stepmass,Rec$Freq/Rec$stepmass,
                        paste('#rec',Rec$year,sep=''),
                        ifelse(Rec$area < max(Rec$area),
                               paste('#p',Rec$area,sep=''),
                               ''),
                        ')')
  } else {
    Rec$number <- Rec$Freq
  }
  Rec <- Rec[Rec$Freq!=0,]
  Rec <- Rec[c('year','step','area','age','length','number','weight')]
  
  header <-
    paste("; recruitment data file for the gadget example",
          "; created automatically from R-gadget",
          sprintf('; %s',paste(names(Rec),collapse='-')),
          sep='\n')
  write(header,paste('.',location,file,sep='/'))
  write.table(Rec, paste('.',location,file,sep='/'),
              sep="\t",row.names=F,col.names=F,quote=F,append=T)
}



############################################
#' make the file example.refw, length in cm and weight in kg
filerefw<-function(opt,sim,location='Gfiles',file='example.refw')
{
  len<-seq(opt$minlen+opt$lengthgrouplen/2,
           opt$maxlen-opt$lengthgrouplen/2,
           opt$lengthgrouplen)
  LW<-cbind(len,opt$w)
  refw <-
    paste("; reference weight file for the gadget example",
          "; created automatically from R-gadget",
          sep='\n')
  write(refw,paste('.',location,file,sep='/'))
  write.table(LW,paste('.',location,file,sep='/'),
              sep="\t",row.names=F,col.names=F,quote=F,append=T)
}



############################################
#' make file fleet and fleet.data containing the catch
#' the order of the file's columns is
#' year, step, area, fleet, amount
filefleet<-function(opt,sim,location='Gfiles',file='fleet')
{
  tmp <- function(mat){
    apply(apply(mat,c(1,3,4),function(x) opt$w*x),c(2,4),sum)
  }
  survamount <-
    as.data.frame.table(tmp(sim$matCsurv) + tmp(sim$immCsurv),
                        stringsAsFactors=FALSE)
  survamount$year <- sapply(strsplit(survamount$time,'_'),
                          function(x) as.numeric(x[2]))
  survamount$step <- sapply(strsplit(survamount$time,'_'),
                          function(x) as.numeric(x[4]))
  survamount$time <- NULL
  survamount$fleet <- 'survey'
  survamount <- survamount[survamount$Freq!=0,]
  commamount <-
    as.data.frame.table(tmp(sim$matCcomm) + tmp(sim$immCcomm),
                        stringsAsFactors=FALSE)
  commamount$year <- sapply(strsplit(commamount$time,'_'),
                          function(x) as.numeric(x[2]))
  commamount$step <- sapply(strsplit(commamount$time,'_'),
                          function(x) as.numeric(x[4]))
  commamount$time <- NULL
  commamount$fleet <- 'comm'
  commamount <- commamount[commamount$Freq!=0,]
  header <-
    paste("; fleet data file",
          "; created automatically from R-gadget",
          "; year-step-area-fleet-biomass",
          sep='\n')
  file.path <- paste('.',location,paste(file,'%s',sep=''),sep='/')
  survamount <- survamount[c('year','step','area','fleet','Freq')]
  commamount <- commamount[c('year','step','area','fleet','Freq')]
  if(opt$survfleettype=='totalfleet'|opt$commfleettype=='totalfleet'){
    write(header,sprintf(file.path,'.data'))
    if(opt$survfleettype=='totalfleet'){      
      write.table(survamount,
                  sprintf(file.path,'.data'),
                  sep="\t",row.names=F,col.names=F,quote=F,append=T)
    }
    if(opt$commfleettype=='totalfleet')
      write.table(commamount,
                  sprintf(file.path,'.data'),
                  sep="\t",row.names=F,col.names=F,quote=F,append=T)
  }
  if(opt$survfleettype=='linearfleet'|opt$commfleettype=='linearfleet'){
    write(header,sprintf(file.path,'.level'))
    if(opt$survfleettype=='linearfleet'){
      survamount$Freq <- 1
      write.table(survamount,
                  sprintf(file.path,'.level'),
                  sep="\t",row.names=F,col.names=F,quote=F,append=T)
    }
    if(opt$commfleettype=='linearfleet'){
      commamount$Freq <- 1
      write.table(commamount,
                  sprintf(file.path,'.level'),
                  sep="\t",row.names=F,col.names=F,quote=F,append=T)
    }
  }
  fleet.file <- 
    paste("; fleet file",
          "; created automatically from R-gadget",
          "[component]",
          sprintf("%s\tcomm",opt$commfleettype),
          paste('livesonareas',
                        paste(1:opt$numofareas,collapse='\t'),
                        sep='\t'),
          sprintf("multiplicative\t%s",opt$commmultiplicative),
          "suitability",
          "example.imm\tfunction exponential\t#comma\t( * 0.1 #commb)\t0\t1",
          "example.mat\tfunction exponential\t#comma\t( * 0.1 #commb)\t0\t1",
          sprintf("amount\t\t%s",ifelse(opt$commfleettype=='totalfleet',
                                        sprintf('%s%s',file,'.data'),
                                        sprintf('%s%s',file,'.level'))),        
          ";\n[component]",
          sprintf("%s\tsurvey",opt$survfleettype),
          paste('livesonareas',
                        paste(1:opt$numofareas,collapse='\t'),
                        sep='\t'),
          sprintf("multiplicative\t%s",opt$survmultiplicative),
          "suitability",
          "example.imm\tfunction exponential\t#surveya\t( * 0.1 #surveyb)\t0\t1",
          "example.mat\tfunction exponential\t#surveya\t( * 0.1 #surveyb)\t0\t1",
          sprintf("amount\t\t%s",ifelse(opt$survfleettype=='totalfleet',
                                        sprintf('%s%s',file,'.data'),
                                        sprintf('%s%s',file,'.level'))),
          sep='\n')
  write(fleet.file,sprintf(file.path,''))
}


############################################
#' make file otherfood and otherfood.data containing the catch
#' the order of the file's columns is
#' year, step, area, fleet, amount
fileotherfood<-function(opt,sim,location='Gfiles',file='otherfood')
{
  foodData <- data.frame(year=rep(1:opt$numoftimesteps,
                           each=opt$numofareas*opt$numobs),
                         step=rep(rep(1:opt$numobs,
                           each=opt$numofareas),
                           opt$numoftimesteps),
                         area=rep(1:opt$numofareas,
                           opt$numobs*opt$numoftimesteps),
                         food.name=rep('other',length(opt$otherfood)),
                         amount=c(opt$otherfood*opt$areasize))
  
  livesOnAreas <- paste('livesonareas',
                        paste(1:opt$numofareas,collapse='\t'),
                        sep='\t')
  file.path <- paste('.',location,paste(file,'%s',sep=''),sep='/')
  otherfood <-
    paste("; otherfood file",
          "; created automatically from R-gadget",
          "[component]",
          "foodname\tother",
          livesOnAreas,
          format(cbind(paste("lengths ",opt$minlen,opt$maxlen,sep="\t"))),
          sprintf("amount\t\t%s.data",file),
          sep='\n')
  write(otherfood,sprintf(file.path,''))

  header <-
    paste("; otherfood data file",
          "; created automatically from R-gadget",
          "; year-step-area-food-data",
          sep='\n')
  write(header,sprintf(file.path,'.data'))
  write.table(foodData,
              sprintf(file.path,'.data'),
              sep="\t",row.names=F,col.names=F,quote=F,append=T)
}

############################################
#' make the file example.surveyindex
#' the order of the file's columns is
#' year, step, area, age, index number
filesurveyindex <- function(opt,sim,location='Gfiles',
                            file='example.surveyindex') {
  #Calculates the total catch  
  
  immIndex <- as.data.frame.table(sim$immNumRec,stringsAsFactors=FALSE)
  immIndex$year <- sapply(strsplit(immIndex$time,'_'),
                          function(x) as.numeric(x[2]))
  immIndex$step <- sapply(strsplit(immIndex$time,'_'),
                          function(x) as.numeric(x[4]))
  immIndex <- immIndex[immIndex$step==opt$survstep,]
  immSurveyAgg <- aggregate(immIndex$Freq,
                            by=list(
                              year=immIndex$year,
                              step=immIndex$step,
                              area=sprintf('area%s',immIndex$area),
                              age=ifelse(immIndex$age==1,'age1','ageother')),
                            sum)
  
  matIndex <- as.data.frame.table(sim$matNumRec,stringsAsFactors=FALSE)
  matIndex$year <- sapply(strsplit(matIndex$time,'_'),
                          function(x) as.numeric(x[2]))
  matIndex$step <- sapply(strsplit(matIndex$time,'_'),
                          function(x) as.numeric(x[4]))
  matIndex <- matIndex[matIndex$step==opt$survstep,]
  matSurveyAgg <- aggregate(matIndex$Freq,
                            by=list(
                              year=matIndex$year,
                              step=matIndex$step,
                              area=sprintf('area%s',matIndex$area),
                              age=rep('ageother',dim(matIndex)[1])),
                            sum)
  
  temp <- exp(rnorm(1,0,opt$survey.sigma^2)-opt$survey.sigma^2/2)
  SurveyIndex <- immSurveyAgg
  SurveyIndex$x[SurveyIndex$age=='ageother'] <-
    SurveyIndex$x[SurveyIndex$age=='ageother'] +
      matSurveyAgg$x
  SurveyIndex$x <- SurveyIndex$x*temp
  index.file <-
    paste("; surveyindex data file for the gadget example",
          "; created automatically from R-gadget",
          sprintf('; %s',paste(names(SurveyIndex),collapse='-')),
          sep='\n')
  write(index.file,paste('.',location,file,sep='/'))
  write.table(SurveyIndex,paste('.',location,file,sep='/'),
              sep="\t",row.names=F,col.names=F,quote=F,append=T)
  invisible(SurveyIndex)
}

############################################
#' make the file example.surveyindexlen
#' the order of the file's columns is
#' year, step, area, age, index number
filesurveyindexlen <- function(opt,sim,location='Gfiles',
                               file='example.surveyindexlen') {

  immIndex <- as.data.frame.table(sim$immNumRec,stringsAsFactors=FALSE)
  immIndex$year <- sapply(strsplit(immIndex$time,'_'),
                          function(x) as.numeric(x[2]))
  immIndex$step <- sapply(strsplit(immIndex$time,'_'),
                          function(x) as.numeric(x[4]))
  immIndex <- immIndex[immIndex$step==opt$survstep,]
  immIndex$length.group <- cut(as.numeric(immIndex$length),
                               breaks=opt$length.groups,
                               labels=sprintf('lengp%s',
                                 1:(length(opt$length.groups)-1)))
  immSurveyLen <- aggregate(immIndex$Freq,
                            by=list(
                              year=immIndex$year,
                              step=immIndex$step,
                              area=sprintf('area%s',immIndex$area),
                              length.group=immIndex$length.group),
                            sum)

  matIndex <- as.data.frame.table(sim$matNumRec,stringsAsFactors=FALSE)
  matIndex$year <- sapply(strsplit(matIndex$time,'_'),
                          function(x) as.numeric(x[2]))
  matIndex$step <- sapply(strsplit(matIndex$time,'_'),
                          function(x) as.numeric(x[4]))
  matIndex <- matIndex[matIndex$step==opt$survstep,]
  matIndex$length.group <- cut(as.numeric(matIndex$length),
                               breaks=opt$length.groups,
                               labels=sprintf('lengp%s',
                                 1:(length(opt$length.groups)-1)))
  matSurveyLen <- aggregate(matIndex$Freq,
                            by=list(
                              year=matIndex$year,
                              step=matIndex$step,
                              area=sprintf('area%s',matIndex$area),
                              length.group=matIndex$length.group),
                            sum)

  SurveyIndex <- immSurveyLen
  SurveyIndex$x <- (SurveyIndex$x + matSurveyLen$x)*
    exp(rnorm(length(SurveyIndex$x),0,opt$survey.sigma^2)-
        opt$survey.sigma^2/2)
  
  index.file <-
    paste("; surveyindex data file for the gadget example",
          "; created automatically from R-gadget",
          sprintf('; %s',paste(names(SurveyIndex),collapse='-')),
          sep='\n')
  write(index.file,paste('.',location,file,sep='/'))
  write.table(SurveyIndex,paste('.',location,file,sep='/'),
              sep="\t",row.names=F,col.names=F,quote=F,append=T)
  
}


############################################
#' make the file example.ldist.catch
#' the order of the file's columns is
#' year, step, area, age, length, number
fileldist.catch<-function(opt,sim,location='Gfiles',
                          file='example.ldist.catch')
{
  
  immIndex <- as.data.frame.table(sim$immCcomm,stringsAsFactors=FALSE)
  immIndex$year <- sapply(strsplit(immIndex$time,'_'),
                          function(x) as.numeric(x[2]))
  immIndex$step <- sapply(strsplit(immIndex$time,'_'),
                          function(x) as.numeric(x[4]))
  #immIndex <- immIndex[immIndex$step==opt$survstep,]
  immIndex$length.group <- cut(as.numeric(immIndex$length),
                               breaks=opt$length.groups,
                               labels=sprintf('lengp%s',
                                 1:(length(opt$length.groups)-1)))
  immCatchLen <- aggregate(immIndex$Freq,
                           by=list(
                             year=immIndex$year,
                             step=immIndex$step,
                             area=sprintf('area%s',immIndex$area),
                             age=rep('allages',length(immIndex$year)),
                             length=immIndex$length),
                           sum)

  matIndex <- as.data.frame.table(sim$matCcomm,stringsAsFactors=FALSE)
  matIndex$year <- sapply(strsplit(matIndex$time,'_'),
                          function(x) as.numeric(x[2]))
  matIndex$step <- sapply(strsplit(matIndex$time,'_'),
                          function(x) as.numeric(x[4]))
  matCatchLen <- aggregate(matIndex$Freq,
                           by=list(
                             year=matIndex$year,
                             step=matIndex$step,
                             area=sprintf('area%s',matIndex$area),
                             age=rep('allages',length(matIndex$year)),
                             length=matIndex$length),
                           sum)

  SurveyIndex <- immCatchLen
  SurveyIndex$x <- (SurveyIndex$x + matCatchLen$x)*
    exp(rnorm(length(SurveyIndex$x),0,opt$survey.sigma^2)-
        opt$survey.sigma^2/2)

  header <-
    paste("; length distribution data file for the gadget example",
          "; created automatically from R-gadget",
          sprintf('; %s',paste(names(SurveyIndex),collapse='-')),
          sep='\n')
  write(header,paste('.',location,file,sep='/') )
  write.table(SurveyIndex,paste('.',location,file,sep='/'),
              sep="\t",row.names=F,col.names=F,quote=F,append=T)
  
}

############################################
#' make the file example.ldist.survey
#' the order of the file's columns is
#' year, step, area, age, length, number

fileldist.survey <- function(opt,sim,location='Gfiles',
                             file='example.ldist.survey') {
  immIndex <- as.data.frame.table(sim$immCsurv,stringsAsFactors=FALSE)
  immIndex$year <- sapply(strsplit(immIndex$time,'_'),
                          function(x) as.numeric(x[2]))
  immIndex$step <- sapply(strsplit(immIndex$time,'_'),
                          function(x) as.numeric(x[4]))
  immIndex <- immIndex[immIndex$step==opt$survstep,]
  immIndex$length.group <- cut(as.numeric(immIndex$length),
                               breaks=opt$length.groups,
                               labels=sprintf('lengp%s',
                                 1:(length(opt$length.groups)-1)))
  immSurveyLen <- aggregate(immIndex$Freq,
                           by=list(
                             year=immIndex$year,
                             step=immIndex$step,
                             area=sprintf('area%s',immIndex$area),
                             age=rep('allages',length(immIndex$year)),
                             length=immIndex$length),
                           sum)

  matIndex <- as.data.frame.table(sim$matCsurv,stringsAsFactors=FALSE)
  matIndex$year <- sapply(strsplit(matIndex$time,'_'),
                          function(x) as.numeric(x[2]))
  matIndex$step <- sapply(strsplit(matIndex$time,'_'),
                          function(x) as.numeric(x[4]))
  matIndex <- matIndex[matIndex$step==opt$survstep,]
  matSurveyLen <- aggregate(matIndex$Freq,
                           by=list(
                             year=matIndex$year,
                             step=matIndex$step,
                             area=sprintf('area%s',matIndex$area),
                             age=rep('allages',length(matIndex$year)),
                             length=matIndex$length),
                           sum)

  SurveyIndex <- immSurveyLen
  SurveyIndex$x <- (SurveyIndex$x + matSurveyLen$x)*
    exp(rnorm(length(SurveyIndex$x),0,opt$survey.sigma^2)-
        opt$survey.sigma^2/2)


  
  header <-
    paste("; survey length distribution data file for the gadget example",
          "; created automatically from R-gadget",
          sprintf('; %s',paste(names(SurveyIndex),collapse='-')),
          sep='\n')
  write(header,paste('.',location,file,sep='/') )
  write.table(SurveyIndex,paste('.',location,file,sep='/'),
              sep="\t",row.names=F,col.names=F,quote=F,append=T)
}

filealk <- function(len.agg,age.agg,
                    opt,sim,location='Gfiles',
                    file.prefix='example.alk'){
  ## age length table
  alk.table <- function(catch,age.agg,len.agg){
    catch.table <- as.data.frame.table(catch,stringsAsFactors=FALSE)
    catch.table$year <- sapply(strsplit(catch.table$time,'_'),
                               function(x) as.numeric(x[2]))
    catch.table$step <- sapply(strsplit(catch.table$time,'_'),
                               function(x) as.numeric(x[4]))
    catch.table$age.agg <-
      1 + round((as.numeric(catch.table$age) - opt$minage)/age.agg)
    catch.table$length.agg <-
      1 + round((as.numeric(catch.table$length) - opt$minlen)/len.agg)
    tmp <- aggregate(catch.table$Freq,
                     by=list(
                       year=catch.table$year,
                       step=catch.table$step,
                       area=paste('area',catch.table$area,sep=''),
                       age=paste('age',catch.table$age.agg,sep=''),
                       length=paste('len',catch.table$length.agg,sep='')),
                     sum)    
    if(len.agg==(opt$maxlen-opt$minlen))
      tmp$length <- 'alllen'
    if(age.agg==(opt$maxage))
      tmp$age <- 'allages'
    return(tmp)
  }

  file <- paste('.',location,file.prefix,sep='/')
  immComm <- alk.table(sim$immCcomm,
                       opt,
                       age.agg,
                       len.agg)
  matComm <- alk.table(sim$matCcomm,
                       opt,
                       age.agg,
                       len.agg)
  immSurv <- alk.table(sim$immCsurv,
                       opt,
                       age.agg,
                       len.agg)
  matSurv <- alk.table(sim$matCsurv,
                       opt,
                       age.agg,
                       len.agg)
  comm <- merge(immComm,matComm,
                by=c('year','step','area','length'),
                all=TRUE,
                suffixes=c('imm','mat'))
  comm$ximm[is.na(comm$ximm)] <- 0
  comm$xmat[is.na(comm$xmat)] <- 0
  comm$total.catch <- comm$ximm + comm$xmat
  comm <- comm[-(comm$total.catch==0),]
  comm$ximm <- NULL
  comm$xmat <- NULL
  surv <- merge(immSurv,matSurv,
                by=c('year','step','area','length','age'),
                all=TRUE,
                suffixes=c('imm','mat'))
  surv$ximm[is.na(surv$ximm)] <- 0
  surv$xmat[is.na(surv$xmat)] <- 0
  surv$total.catch <- surv$ximm + surv$xmat
  surv <- surv[-(surv$total.catch==0),]
  surv$ximm <- NULL
  surv$xmat <- NULL
  header <-
    paste("; age-length-key distribution data file for the gadget example",
          "; created automatically from R-gadget",
          sprintf('; %s',paste(names(surv),collapse='-')),
          sep='\n')
  write(header,paste(file,'survey',sep='.'))
  write(header,paste(file,'catch',sep='.'))
  write.table(comm,paste(file,'catch',sep='.'),
              sep="\t",row.names=F,col.names=F,quote=F,append=T)
  write.table(surv,paste(file,'surv',sep='.'),
              sep="\t",row.names=F,col.names=F,quote=F,append=T)
}


############################################
#' make the optinfofile file for the example
fileoptinfo<-function(opt=NULL,sim=NULL,location='Gfiles',file='optinfofile')
{
  optinfo <-
    paste("; optimisation file for the gadget example",
          "; created automatically from R-gadget",
          "; options for the Simulated Annealing optimisation",
          "[simann]",
          "simanniter\t2000",
          "simanneps\t1e-4",
          "T\t\t100",
          "rt\t\t0.85",
          "; options for the Hooke and Jeeves optimisation",
          "[hooke]",
          "hookeiter\t1000",
          "hookeeps\t1e-04",
          "rho\t\t0.5",
          "lambda\t0",
          sep='\n')
  write(optinfo,paste('.',location,file,sep='/'))
}

############################################
#' make the migration file for the example
filemigrate<-function(opt,sim,location='Gfiles',file='migration'){
  mig <-
    data.frame(year=rep(1:opt$numobs,each=opt$numoftimesteps),
               steps=rep(1:opt$numoftimesteps,opt$numobs),
               matrix=sprintf('matrix%s',rep(1:opt$numoftimesteps,opt$numobs)))
  mig.file <-
    paste("; migration time file for the gadget example",
          "; created automatically from R-gadget",
          sprintf('; %s',paste(names(mig),collapse='-')),
          sep='\n')
  write(mig.file,paste('.',location,paste(file,'.time',sep=''),sep='/')) 
  write.table(mig,paste('.',location,paste(file,'.time',sep=''),sep='/'),
              sep="\t",row.names=FALSE,col.names=FALSE,quote=FALSE,append=TRUE)
  P <- migrationProb(opt)
  if(opt$file.migration.ratio>0){
    ratio.file <-
      paste("; migration ratio file for the gadget example",
            "; created automatically from R-gadget",
            "\n[migrationmatrix]\nname\tmatrix1",
            "; from\tto\tratio",
            "2\t1\t0.4",
            "\n[migrationmatrix]\nname\tmatrix2",
            "; from\tto\tratio", 
            "1\t2\t0.4",
            "\n[migrationmatrix]\nname\tmatrix3",
            "; from\tto\tratio", 
            "1\t2\t0.4",
            "\n[migrationmatrix]\nname\tmatrix4",
            "; from\tto\tratio",
            sep='/')
    write(ratio.file, paste('.',location,paste(file,'.ratio',sep=''),sep='/'))
  } else {
    matrix.file <-
      paste("; migration data file for the gadget example",
            "; created automatically from R-gadget",
            paste(sprintf("\n[migrationmatrix]\nname\tmatrix%s\n",
                          1:opt$numoftimesteps),
                  apply(P,3,
                        function(x)
                        paste(paste(x,collapse='\t'),
                              paste(1-x,collapse='\t'),sep='\n')),
                  collapse='\n'),
            sep='\n')
    write(matrix.file,paste('.',location,paste(file,'.data',sep=''),sep='/'))
  }
}
#' catch in kilos file
filecatchinkilos<-function(opt,sim,location='Gfiles',file='example.kilos') {
  header <-
    paste("; catch in kilos datafile for the gadget example",
          "; created automatically from R-gadget",
          "; year-step-area-fleet-biomass",
          sep='\n')
  commAmount <- apply(apply(sim$immCcomm,c(1,3,4),
                            function(x) opt$w*x),
                      c(2,4),sum) +
                        apply(apply(sim$matCcomm,c(1,3,4),
                                    function(x) opt$w*x),
                              c(2,4),sum)
  commAmount <- as.data.frame.table(commAmount,stringsAsFactors=FALSE)
  commAmount$year <- sapply(strsplit(commAmount$time,'_'),
                          function(x) as.numeric(x[2]))
  commAmount$step <- sapply(strsplit(commAmount$time,'_'),
                          function(x) as.numeric(x[4]))
  commAmount$time <- NULL
  commAmount <- commAmount[commAmount$Freq!=0,]
  commAmount$area <- sprintf('area%s',commAmount$area)
  commAmount$fleet <- 'comm'
  commAmount <- commAmount[c('year','step','area','fleet','Freq')]
  write(header,paste('.',location,file,sep='/'))
  write.table(commAmount,paste('.',location,file,sep='/'),sep="\t",row.names=F,col.names=F,quote=F,append=T)
}

############################################
# make the migration file for the example
filefuncmigrate<-function(opt,sim,location='Gfiles',
                          file='migration.area.data',
                          recdata){
#  recdata <- read.table("rectangles.txt")
  
  XfU <- recdata[1]
  XfL <- recdata[2]
  YfU <- recdata[3]
  YfL <- recdata[4]
  
  coordinatesf <- c(YfL, XfL, YfU, XfU)
  
  XiU <-recdata[5]
  XiL <- recdata[6]
  YiU <- recdata[7]
  YiL <- recdata[8]
  
  coordinatesi <- c(YiL, XiL, YiU, XiU)  
  
  migration.file <-
    paste("; migration area file for the gadget example",
          "; created automatically from R-Gadget",
          ##Writing area north which is number 1
          "[area]\nname\tnorth",
          "number\t1",
          "rectangles\tmigration.rec.north", 
          ##Writing area south which is number 2
          "[area]\nname\tsouth", 
          "number\t2",
          "rectangles\tmigration.rec.south", 
          ##Writing the wall rectangle 
#          "[area]\nname\twall", 
#          "; y0\tx0\ty1\tx1",
#          "55.0\t5.0\t75.0\t45.0",
          ##Writing the land rectangle
#          "[area]\nname\tland",
#          "; y0\tx0\ty1\tx1", 
#          "64\t15\t66\t24", 
          sep='\n')
  north <-
    paste(
          ##Rectangle file for north
          "; migration rectangle data file for the gadget example",
          "; created automatically from R-Gadget",
          "; file for defning rectangles for oceanareas, every line defines two points for a\n; rectangle area (xlow, ylow) and (xupp, yupp)", 
          "; xlow\tylow\txupp\tyupp", 
          paste(coordinatesf,collapse='\t'),
          ##Rectangle file for sourth
          sep='\n')
  south <-
    paste(
          "; migration rectangle data file for the gadget example",
          "; created automatically from R-Gadget",
          "; file for defning rectangles for oceanareas, every line defines two points for a\n; rectangle area (xlow, ylow) and (xupp, yupp)",
          "; xlow\tylow\txupp\tyupp",
          paste(coordinatesi,collapse='\t'),
          sep='\n')
  write(migration.file,paste('.',location,file,sep='/'))
  write(north,paste('.',location,'migration.rec.north',sep='/'))
  write(south,paste('.',location,'migration.rec.south',sep='/'))
}
