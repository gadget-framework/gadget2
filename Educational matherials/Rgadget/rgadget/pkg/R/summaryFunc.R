##' Calculates the survey indices for the simulated stocks. 
##' @title Survey indices
##' @param sim Results from a Rgadget simulation
##' @param sigma sigma for a log-normal noise for the indicies
##' @return Dataframe with the survey indices 
##' @author Bjarki Þór Elvarsson
survey.index <- function(sim,sigma=0){
  ##Calculates the total catch  
  opt <- sim$opt
  Index <- rbind(as.data.frame.table(sim$immNumRec,stringsAsFactors=FALSE),
                 as.data.frame.table(sim$matNumRec,stringsAsFactors=FALSE))
  Index$year <- sapply(strsplit(Index$time,'_'),
                       function(x) as.numeric(x[2]))
  Index$step <- sapply(strsplit(Index$time,'_'),
                          function(x) as.numeric(x[4]))
  Index <- Index[Index$step==opt$survstep,]
  SurveyAgg <- aggregate(Index$Freq,
                         by=list(
                           year=Index$year,
                           step=Index$step,
                           area=sprintf('area%s',Index$area),
                           age=ifelse(Index$age==1,'age1','ageother')),
                         sum)
  
  names(SurveyAgg)[5] <- 'index'
  temp <- exp(rnorm(opt$numobs,0,sigma^2)-sigma^2/2)
  SurveyAgg$index <- SurveyAgg$index*temp
  
  SurveyAgg$time <- SurveyAgg$year+(SurveyAgg$step - 1)/opt$numoftimesteps
  
  class(SurveyAgg) <- c('Rgadget',class(SurveyAgg))
  attr(SurveyAgg,'formula') <- index~time|area
  attr(SurveyAgg,'plotGroups') <- 'age'
  attr(SurveyAgg,'plotType') <- 'l'
  attr(SurveyAgg,'xaxis') <- 'Year'
  attr(SurveyAgg,'yaxis') <- 'Survey Index'
  attr(SurveyAgg,'plotFun') <- 'xyplot'  
  return(SurveyAgg)
}
##' Calculate the survey length index based on the provided lengthgroups
##' @title Survey length index
##' @param sim Results from a Rgadget simulation
##' @param length.groups A vector of cutoff points for the length groups
##' @param sigma sigma for a log-normal error
##' @return Dataframe containing the length index from the 
##' @author Bjarki Þór Elvarsson
survey.indexlen <- function(sim,length.groups=c(4,14,90),sigma=0){
  opt <- sim$opt
  Index <- rbind(as.data.frame.table(sim$immNumRec,stringsAsFactors=FALSE),
                 as.data.frame.table(sim$matNumRec,stringsAsFactors=FALSE))
  Index$year <- sapply(strsplit(Index$time,'_'),
                       function(x) as.numeric(x[2]))
  Index$step <- sapply(strsplit(Index$time,'_'),
                       function(x) as.numeric(x[4]))
  Index <- Index[Index$step==opt$survstep,]
  Index$length <- as.numeric(Index$length)
  Index$length.group <- cut(Index$length,
                            breaks=length.groups,
                            labels=sprintf('lengp%s',
                              1:(length(length.groups)-1)))
  IndexLen <- aggregate(Index$Freq,
                        by=list(
                          year=Index$year,
                          step=Index$step,
                          area=sprintf('area%s',Index$area),
                          length.group=Index$length.group),
                        sum)
  names(IndexLen)[5] <- 'index'
  IndexLen$x <- IndexLen$index*exp(rnorm(length(IndexLen$index),0,
                                     sigma^2) - sigma^2/2)
  
  IndexLen$time <- IndexLen$year+(IndexLen$step - 1)/opt$numoftimesteps
  
  class(IndexLen) <- c('Rgadget',class(IndexLen))
  attr(IndexLen,'formula') <- index~time|area
  attr(IndexLen,'plotGroups') <- 'length.group'
  attr(IndexLen,'plotType') <- 'l'
  attr(IndexLen,'xaxis') <- 'Year'
  attr(IndexLen,'yaxis') <- 'Survey Length Index'
  attr(IndexLen,'plotFun') <- 'xyplot'
  return(IndexLen)
}

##' Calculate the length distribution from the fleet by length groups and time.
##' @title Length Dist 
##' @param sim Results from a Rgadget simulation
##' @param sigma sigma for a lognormal noise
##' @return Dataframe containing the fleet length distribution.
##' @author Bjarki Þór Elvarsson
lengthDist <- function(sim,sigma=0){
  opt <- sim$opt
  tmp.surv <- rbind(as.data.frame.table(sim$immCsurv,stringsAsFactors=FALSE),
                    as.data.frame.table(sim$matCsurv,stringsAsFactors=FALSE))
  tmp.comm <- rbind(as.data.frame.table(sim$immCcomm,stringsAsFactors=FALSE),
                    as.data.frame.table(sim$matCcomm,stringsAsFactors=FALSE))
  tmp.surv$fleet <- 'surv'
  tmp.comm$fleet <- 'comm'
  Index <- rbind(tmp.surv,tmp.comm)
  Index$year <- sapply(strsplit(Index$time,'_'),
                          function(x) as.numeric(x[2]))
  Index$step <- sapply(strsplit(Index$time,'_'),
                          function(x) as.numeric(x[4]))
  Index <- Index[Index$fleet=='comm'|Index$step==opt$survstep,]
  IndexLen <- aggregate(Index$Freq,
                         by=list(
                           year=Index$year,
                           step=Index$step,
                           area=sprintf('area%s',Index$area),
                           age=rep('allages',length(Index$year)),
                           length=as.numeric(Index$length),
                           fleet=Index$fleet),                           
                         sum)
  IndexLen$x <- IndexLen$x*exp(rnorm(length(IndexLen$x),0,
                                     sigma^2) - sigma^2/2)
  IndexLen$time <- IndexLen$year+(IndexLen$step - 1)/opt$numoftimesteps
  class(IndexLen) <- c('Rgadget',class(IndexLen))
  attr(IndexLen,'formula') <- x~length|as.ordered(year)+as.factor(area):as.factor(fleet)
  attr(IndexLen,'plotGroups') <- 'step'
  attr(IndexLen,'plotType') <- 'l'
  attr(IndexLen,'xaxis') <- 'Year'
  attr(IndexLen,'yaxis') <- 'Length Index'
  attr(IndexLen,'plotFun') <- 'xyplot'
  
  return(IndexLen)
}
##' Calculates the age-length-key for the survey and commercial fleet.
##' @title Age length key
##' @param sim Results from a Rgadget simulation
##' @param age.agg The desired age aggregation
##' @param len.agg The desired length aggregation
##' @return Dataframe containing the age - length key
##' @author Bjarki Þór Elvarsson
age.length.key <- function(sim,age.agg,len.agg){
   ## age length table
  opt <- sim$opt
  alk.table <- function(catch,age.agg,len.agg){
    catch.table <- as.data.frame.table(catch,stringsAsFactors=FALSE)
    catch.table$year <- sapply(strsplit(catch.table$time,'_'),
                               function(x) as.numeric(x[2]))
    catch.table$step <- sapply(strsplit(catch.table$time,'_'),
                               function(x) as.numeric(x[4]))
    catch.table$age.agg <-
      ordered(1 + round((as.numeric(catch.table$age) - opt$minage)/age.agg))
    levels(catch.table$age.agg) <- paste('age',
                                         levels(catch.table$age.agg),
                                         sep='')
    catch.table$length.agg <-
      ordered(1 + round((as.numeric(catch.table$length) - opt$minlen)/len.agg))
    levels(catch.table$length.agg) <- paste('len',
                                            levels(catch.table$length.agg),
                                            sep='')
    tmp <- aggregate(catch.table$Freq,
                     by=list(
                       year=catch.table$year,
                       step=catch.table$step,
                       area=paste('area',catch.table$area,sep=''),
                       age=catch.table$age.agg,
                       length=catch.table$length.agg),
                     sum)    
    if(len.agg==(opt$maxlen-opt$minlen))
      tmp$length <- ordered('alllen')
    if(age.agg==(opt$maxage))
      tmp$age <- ordered('allages')
    return(tmp)
  }

  immComm <- alk.table(sim$immCcomm,
                       age.agg,
                       len.agg)
  matComm <- alk.table(sim$matCcomm,
                       age.agg,
                       len.agg)
  immSurv <- alk.table(sim$immCsurv,
                       age.agg,
                       len.agg)
  matSurv <- alk.table(sim$matCsurv,
                       age.agg,
                       len.agg)
  comm <- merge(immComm,matComm,
                by=c('year','step','area','length','age'),
                all=TRUE,
                suffixes=c('imm','mat'))
  comm$ximm[is.na(comm$ximm)] <- 0
  comm$xmat[is.na(comm$xmat)] <- 0
  comm$total.catch <- comm$ximm + comm$xmat
  comm <- comm[!(comm$total.catch==0),]
  comm$ximm <- NULL
  comm$xmat <- NULL
  comm$fleet <- 'comm'
  surv <- merge(immSurv,matSurv,
                by=c('year','step','area','length','age'),
                all=TRUE,
                suffixes=c('imm','mat'))
  surv$ximm[is.na(surv$ximm)] <- 0
  surv$xmat[is.na(surv$xmat)] <- 0
  surv$total.catch <- surv$ximm + surv$xmat
  surv <- surv[!(surv$total.catch==0),]
  surv$ximm <- NULL
  surv$xmat <- NULL
  surv$fleet <- 'surv'
  alk <- rbind(surv,comm)

  alk$time <- alk$year + (alk$step-1)/4
  class(alk) <- c('Rgadget',class(alk))
  attr(alk,'formula') <- total.catch~as.numeric(age)+as.numeric(length)|as.ordered(time) + as.factor(area):as.factor(fleet)
  attr(alk,'plotGroups') <- ''
  attr(alk,'plotType') <- ''
  attr(alk,'xaxis') <- 'Year'
  attr(alk,'yaxis') <- 'Age - Length - Key'
  attr(alk,'plotFun') <- 'contour'
  attr(alk,'layout') <- ''
  return(alk)

}
##' Calculates the overall weigth of the catches by time step and area.
##' @title Catch in Kilos 
##' @param sim Results from a Rgadget simulation
##' @return Dataframe with the catch in kilos by timestep and ared.
##' @author Bjarki Þór Elvarsson
catch.in.kilos <- function(sim){
  opt <- sim$opt
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
  commAmount$time <- commAmount$year+(commAmount$step - 1)/opt$numoftimesteps
  commAmount <- commAmount[commAmount$Freq!=0,]
  commAmount$area <- sprintf('area%s',commAmount$area)
  commAmount$fleet <- 'comm'
  commAmount <- commAmount[c('year','step','area','fleet','Freq','time')]
  names(commAmount)[5] <- 'catch.in.kilos'

  class(commAmount) <- c('Rgadget',class(commAmount))
  attr(commAmount,'formula') <- catch.in.kilos~time|area
  attr(commAmount,'plotGroups') <- ''
  attr(commAmount,'plotType') <- 'l'
  attr(commAmount,'xaxis') <- 'Year'
  attr(commAmount,'yaxis') <- 'Catch in kilos'
  attr(commAmount,'plotFun') <- 'xyplot'
  attr(commAmount,'layout') <- ''
  return(commAmount)
}
##' Plot the results from the summary functions of the Rgadget simulation.
##' @title Plot Rgadget
##' @param dat A Rgadget object
##' @author Bjarki Þór Elvarsson
plot.Rgadget <- function(dat,compare.alk=TRUE){
  if(attr(dat,'plotFun')=='contour'){
    plot <- contourplot(attr(dat,'formula'),
                        labels=FALSE,
                        data=dat,
                        ylab=attr(dat,'yaxis'),
                        xlab=attr(dat,'xaxis'),
                        cuts=15,
                        scales=list(x=list(rot=45),y=list(rot=45)),
                        layout=attr(dat,'layout'),
                        strip=FALSE)
  } else {
    if(attr(dat,'plotGroups')!=''){
      dat$plotGroups <- dat[[attr(dat,'plotGroups')]]
      key <- list(points=FALSE,lines=TRUE,
                     title=attr(dat,'plotGroups'),space='right')
    } else {
      dat$plotGroups <- ''
      key <- FALSE
    }
    plot <- xyplot(attr(dat,'formula'),
                   data=dat,
                   groups=plotGroups,
                   type=attr(dat,'plotType'),
                   plot.points=FALSE,
                   auto.key = key,
                   ylab=attr(dat,'yaxis'),
                   xlab=attr(dat,'xaxis'),
                   scales=list(x=list(rot=45),y=list(rot=45)),
                   ref=TRUE,
                   strip=FALSE) 
  }
  if(length(dim(plot))==2){
    print(useOuterStrips(plot))
  } else {
    print(plot)
  }
    
}

##' summary of the simulation defined by gadget.options
##' @title Summary of gadget.options
##' @param opt gadget.options list
summary.gadget.options <- function(opt){
  summary.text <- paste('Summary of gadget options:',
                        '',
                        sprintf('There are %s years split into %s steps',
                                opt$numobs,opt$numoftimesteps),
                        sprintf('Area(s):\t%s',paste(1:opt$numofareas,
                                                     collapse=' ')),
                        sprintf('Areasize:\t%s',opt$areasize),
                        '',
                        'Stocks',
                        '',
                        '- Immature stock',
                        sprintf('- Ages between %s and %s',
                                opt$immminage, opt$immmaxage),
                        sprintf('- Lengths between %s and %s',
                                opt$minlen,opt$maxlen),
                        sprintf('- Length-weight relationship %s*l^%s',opt$a,opt$b),
                        sprintf('- Von Bertanlanffy growth parameters: lsup = %s and k = %s',
                                opt$lsup, opt$k),
                        sprintf('- Beta for the beta-binomial length update: %s',
                                opt$beta),
                        sprintf('- Maximum length update %s',opt$binn),
                        '',
                        '- Mature stock',
                        sprintf('- Ages between %s and %s',
                                opt$matminage, opt$matmaxage),
                        sprintf('- Lengths between %s and %s',
                                opt$minlen,opt$maxlen),
                        sprintf('- Length-weight relationship %s*l^%s',opt$a,opt$b),
                        sprintf('- Von Bertanlanffy growth parameters: lsup = %s and k = %s',
                                opt$lsup, opt$k),
                        sprintf('- Beta for the beta-binomial length update: %s',
                                opt$beta),
                        sprintf('- Maximum length update %s',opt$binn),
                        '',
                        'Fleets:',
                        '',
                        '- Survey fleet:',
                        sprintf('- suitability parameters\talpha:%s\tbeta%s',
                                opt$salphasurv,opt$sbetasurv),
                        sprintf('- Effort:\t%s',opt$Fysurv),
                        sprintf('- Timestep(s):\t%s',
                                paste(opt$survstep,collapse=' ')),
                        sprintf('- Harvests in area(s) %s',
                                paste(opt$doescatchsurv,collapse=' ')),
                        '',
                        '- Commercial fleet:',
                        sprintf('- suitability parameters\talpha:%s\tbeta%s',
                                opt$salphacomm,opt$sbetacomm),
                        sprintf('- Effort:\t%s',opt$Fycomm),
                        sprintf('- Timestep(s):\t%s',
                                paste(opt$survstep,collapse=' ')),
                        sprintf('- Harvests in area(s) %s',
                                paste(opt$doescatchcomm,collapse=' ')),
                        '\n',
                        sep='\n')
  cat(summary.text)
  invisible(summary.text)
}
                               
##' This function formats the output from RGadget to a dataframe and adds some 
##' trivial calculated values
##' @title as.data.frame.gadget.sim
##' @param sim the results from RGadget
##' @return A dataframe 
as.data.frame.gadget.sim <- function(sim){
  imm <- as.data.frame.table(sim$immNumRec,stringsAsFactors=FALSE)
  names(imm)[length(names(imm))] <- 'Num.indiv'
  catch.C.imm <- as.data.frame.table(sim$immCcomm,stringsAsFactors=FALSE)
  names(catch.C.imm)[length(names(catch.C.imm))] <- 'Commercial.catch'
  catch.S.imm <- as.data.frame.table(sim$immCsurv,stringsAsFactors=FALSE)
  names(catch.S.imm)[length(names(catch.S.imm))] <- 'Survey.catch'
  tmp.imm <- merge(imm,catch.C.imm,all=TRUE)
  tmp.imm <- merge(tmp.imm,catch.S.imm,all=TRUE)
  tmp.imm$year <- sapply(strsplit(tmp.imm$time,'_'),
                         function(x) as.numeric(x[2]))
  tmp.imm$step <- sapply(strsplit(tmp.imm$time,'_'),
                         function(x) as.numeric(x[4]))
  tmp.imm$length <- as.numeric(tmp.imm$length)
  tmp.imm$age <- as.numeric(tmp.imm$age)
  tmp.imm$weight <- opt$a*tmp.imm$length^opt$b
  tmp.imm$stock <- 'imm'
  
  mat <- as.data.frame.table(sim$matNumRec,stringsAsFactors=FALSE)
  names(mat)[length(names(mat))] <- 'Num.indiv'
  catch.C.mat <- as.data.frame.table(sim$matCcomm,stringsAsFactors=FALSE)
  names(catch.C.mat)[length(names(catch.C.mat))] <- 'Commercial.catch'
  catch.S.mat <- as.data.frame.table(sim$matCsurv,stringsAsFactors=FALSE)
  names(catch.S.mat)[length(names(catch.S.mat))] <- 'Survey.catch'
  tmp.mat <- merge(mat,catch.C.mat,all=TRUE)
  tmp.mat <- merge(tmp.mat,catch.S.mat,all=TRUE)
  tmp.mat$year <- sapply(strsplit(tmp.mat$time,'_'),
                         function(x) as.numeric(x[2]))
  tmp.mat$step <- sapply(strsplit(tmp.mat$time,'_'),
                         function(x) as.numeric(x[4]))
  tmp.mat$length <- as.numeric(tmp.mat$length)
  tmp.mat$age <- as.numeric(tmp.mat$age)
  tmp.mat$weight <- opt$a*tmp.mat$length^opt$b
  tmp.mat$stock <- 'mat'

  tmp <- rbind(tmp.imm,tmp.mat)
  tmp$time <- NULL
  return(tmp)
}
##' Plots the suitability functions, the length weight relationship and the Von Bertalanffy growth curve.
##' @title Plot gadget options
##' @param opt Gadget options object
##' @author Bjarki Þór Elvarsson
plot.gadget.options <- function(opt){
  ## suitability plot for the simulation
  l <- opt$minlen:opt$maxlen
  tmp <- data.frame(fleet=rep(c('Survey fleet','Commercial fleet'),
                      each=(opt$maxlen - opt$minlen+1)),
                    length=rep(l,2),
                    weight=opt$a*l^opt$b,
                    suitability=c(suitability(opt$salphasurv,
                      opt$sbetasurv,0,1,l),
                      suitability(opt$salphacomm,opt$sbetacomm,0,1,l)))
  suit <- xyplot(suitability~length,groups=fleet,tmp,type='l',
                 auto.key=list(lines=TRUE,points=FALSE),
                 main='Suitability')
  ## length weight relationship
  weight <- xyplot(weight~length,tmp[1:length(l),],main='Length-Weight',
                   type='l')
 
  tmp.imm <- data.frame(age=opt$immminage:opt$immmaxage,
                        stock='immature',
                        growth=vonB(opt$lsup,opt$k,
                          opt$immminage:opt$immmaxage))
  tmp.mat <- data.frame(age=opt$matminage:opt$matmaxage,
                        stock='mat',
                        growth=vonB(opt$lsup,opt$k,
                          opt$matminage:opt$matmaxage))
  tmp.age <- rbind(tmp.imm,tmp.mat)
  vonB.plot <- xyplot(growth~age,tmp.age,groups=stock,type='l',
                      auto.key=list(lines=TRUE,points=FALSE),
                      main='Growth curve')
  
  print(suit,position = c(0,0,.33,1),
        more=TRUE)
  print(weight,position = c(.33,0,.66,1),
        more=TRUE)
  print(vonB.plot,position = c(.66,0,1,1))
}
##' Calculate the Von Bertanlaffy curve according to the formula
##' \deqn{L(a) = L_\infty (1-e^{-\kappa a})}{L(a) = L_infty (1-e^{-kappa a})}
##' @title Von Bertalanffy 
##' @param lsup \eqn{L_\infty }{L_infty} terminal length
##' @param k \eqn{\kappa}{kappa}, the growth parameter
##' @param a age of the individual
##' @return a vector of length(a) with the calculated VB curve at age(s) a.
##' @author Bjarki Þór Elvarsson
vonB <- function(lsup,k,a){
  lsup <- lsup*(1-exp(-k*a))
}
##' Simulated length at age
##' @title Length at age
##' @param sim The results from a Rgadget simulation
##' @return a dataframe containin the length at age for both fleets
##' @author Bjarki Þór Elvarsson
length.at.age <- function(sim){
  sim.dat <- as.data.frame(sim)
  cols <- c('age','year','step','length.at.age','fleet')
  comm.lat <- aggregate(cbind(Commercial.catch,Commercial.catch*length)~
                        age+year+step,sim.dat[sim.dat$step %in% sim$opt$commstep,],sum)
  comm.lat$length.at.age <-  comm.lat[,5]/comm.lat[,4]
  comm.lat$fleet <- 'comm'
  comm.lat <- comm.lat[cols]
  
  surv.lat <- aggregate(cbind(Survey.catch,Survey.catch*length)~
                        age+year+step,sim.dat[sim.dat$step %in% sim$opt$survstep,],sum)
  surv.lat$length.at.age <-  surv.lat[,5]/surv.lat[,4]
  surv.lat$fleet <- 'surv'
  surv.lat <- surv.lat[cols]
  lat <- rbind(surv.lat,comm.lat)
  class(lat) <- c('Rgadget',class(lat))
  attr(lat,'formula') <- length.at.age~age|year+step
  attr(lat,'plotGroups') <- 'fleet'
  attr(lat,'plotType') <- 'l'
  attr(lat,'xaxis') <- 'Age'
  attr(lat,'yaxis') <- 'Length'
  attr(lat,'plotFun') <- 'xyplot'
  

  return(lat)
  
}
