source('stock.R')
source('function.R')
source('files.R')
source('gadgetfunctions.R')
source('mig6.R')
source('hessian.R')
##' estimate fleet example
##'
##' Optimisation run starting from a random value.
##' Only estimate the multiplicative scaling factor for the commerical fleet
##'
##' The commands in this shell script:
##'
##'   - create the directory for output
##'   - do a gadget optimisation run (-l) 
##'            with the optimisation parameters in optinfofile (-opt) 
##'            output the estimated parameters to run1/params.out (-p)
##'
##'   - then do a simulation run to print the modelled population (-s)
##'      saving the likelihood scores to run1/like.out (-o)
##'
estimate.fleet <- function(){
  ## set up the initial folders
  try(system('mkdir estimate.fleet',intern=TRUE,ignore.stderr=TRUE))
  setwd('estimate.fleet')
  try(system('mkdir Gfiles',intern=TRUE,ignore.stderr=TRUE))
  ## example specific switches
  opt <- gadget.options()
  opt$numofareas <- 2
  opt$probarea <- c(1,1)/2
  opt$spbeta <- -0.3
  opt$H <- 400
  opt$m0 <- 1e-5
  opt$Fycomm <- 0.45
  opt$Fysurv <- 0.01
  opt$doescatchsurv <- 1:opt$numofareas
  opt$doescatchcomm <- 1:opt$numofareas
  opt$bylen <- 0
  opt$commfleettype <- 'linearfleet'
  opt$commmultiplicative <- '(* 0.1 #mult)'
  opt$calcindex <- 0
  opt$calcldist.c <- 0
  opt$calcldist.s <- 0
  opt$calc.in.kilos <- 1
  opt$estimate.recruits.by.year <- 0
  opt$optim.params <- 'mult'
  ## generate dataset
  sim <- Rgadget(opt)
  makefiles(sim$opt,sim)
  ## estimation run
  setwd('Gfiles')
  try(system('mkdir run1',intern=TRUE,ignore.stderr=TRUE))
  try(system('mkdir out',intern=TRUE,ignore.stderr=TRUE))
  callGadget(l=1,i='refinputfile',opt='optinfofile',p='run1/params.out')
  ## rerun from optimum to print population details
  callGadget(s=1,i='run1/params.out',o='run1/like.out')
  printfiles <- read.printfiles('out')
  ## return to old wd
  setwd('../../')
  return(printfiles)
}

estimate.growth <- function(){
  ## setting up the workspace
  try(system('mkdir estimate.growth',intern=TRUE,ignore.stderr=TRUE))
  setwd('estimate.growth')
  try(system('mkdir Gfiles',intern=TRUE,ignore.stderr=TRUE))
  ## example switches
  opt <- gadget.options()
  opt$numofareas <- 2
  opt$doescatchsurv <- 1:opt$numofareas
  opt$doescatchcomm <- 1:opt$numofareas
  opt$probarea <- c(1,1)/2
  opt$m0 <- 10^(-5)
  opt$H <- 4e3
  opt$Fysurv <- 0.01
  opt$Fycomm <- 0.7
  opt$survstep <- 1
  opt$calcindex <- 0
  opt$calcalk.c <- 0
  opt$calcalk.s <- 1
  opt$estimate.recruits.by.year <- 0
  opt$estimate.recruits.by.year.and.area <- 0
  opt$estimate.recruits.by.year.constant.area <- 0
  opt$optim.params <- c('linf','k')#,'beta','comma','commb','surveya','surveyb')
  opt$randomise.all.initial.values <- 1
  ## create dataset
  sim <- Rgadget(opt)
  makefiles(sim$opt,sim)
  ## estimation run
  setwd('Gfiles')
  try(system('mkdir run1',intern=TRUE,ignore.stderr=TRUE))
  try(system('mkdir out',intern=TRUE,ignore.stderr=TRUE))
  callGadget(l=1,i='refinputfile',opt='optinfofile',p='run1/params.out')
  ## rerun from optimum to print population details
  callGadget(s=1,i='run1/params.out',o='run1/like.out')
}
  
standard1 <- function(){
  ## setting up the workspace
  try(system('mkdir standard1',intern=TRUE,ignore.stderr=TRUE))
  setwd('standard1')
  ## example switches
  opt <- gadget.options()
  opt$numofareas <- 2
  opt$doescatchsurv <- 1:opt$numofareas
  opt$doescatchcomm <- 1:opt$numofareas
  opt$probarea <- c(1,1)/2
  ## create dataset
  sim <- Rgadget(opt)
  opt1 <- sim$opt
  opt1$estimate.recruits.by.year.and.area <- 1
  try(system('mkdir Gfiles1',intern=TRUE,ignore.stderr=TRUE))
  makefiles(opt1,sim,location='Gfiles1')
  setwd('Gfiles1')
  try(system('mkdir run1',intern=TRUE,ignore.stderr=TRUE))
  try(system('mkdir out',intern=TRUE,ignore.stderr=TRUE))
##  callGadget(l=1,i='refinputfile',opt='optinfofile',p='run1/params.out')
  ## rerun from optimum to print population details
  callGadget(s=1,i='refinputfile',o='run1/like.out',log='run1/logfile')
  ## alternative formulation
  opt2 <- sim$opt
  opt2$estimate.recruits.by.year.constant.area <- 1
  setwd('..')
  try(system('mkdir Gfiles2',intern=TRUE,ignore.stderr=TRUE))
  makefiles(opt2,sim,location='Gfiles2')
  ## third formulation, recruitments are initialised slightly wrong
  opt3 <- opt2
  opt3$randomised.recruits <- 1
  try(system('mkdir Gfiles3',intern=TRUE,ignore.stderr=TRUE))
  makefiles(opt2,sim,location='Gfiles3')
  
}


#estimate.sel <- function

##' Estimate Migration
##'
##' For Violeta Calian
migfunc <- function(){
  ## setting up the workspace
  try(system('mkdir migfunc',intern=TRUE,ignore.stderr=TRUE))
  setwd('migfunc')
  try(system('mkdir Gfiles',intern=TRUE,ignore.stderr=TRUE))
  ## example switches
  opt <- gadget.options()
  opt$numofareas <- 2
  opt$doescatchcomm <- 1:2
  opt$doescatchsurv <- 1:2
  f1 <- Vmig6(40,26,75,65,22,5,58,55)
  f2 <- Vmig6(22,5,58,55,40,26,75,65)
  opt$migrationP1 <- rep(1-f2,4)
  opt$migrationP2 <- rep(f1,4)
  opt$murec <- 13
  opt$probarea <- c(1,1)/2
  opt$diffusion <- 0.01
  opt$driftx <- 0.01
  opt$drifty <- 0.01
  opt$lambda <- 1
  opt$doesfuncmigrate <- 1
  opt$bylen <- 1
  opt$optim.params <- c('diffusion','driftx','drifty')
  opt$estimate.recruits.by.year <- 0
  
  sim <- Rgadget(opt)
  makefiles(sim$opt,sim)
  filefuncmigrate(sim$opt,sim,recdata=c(22,5,58,55,40,26,75,65))
  ## estimation run
  setwd('Gfiles')
  try(system('mkdir run1',intern=TRUE,ignore.stderr=TRUE))
  try(system('mkdir out',intern=TRUE,ignore.stderr=TRUE))
  callGadget(l=1,i='refinputfile',opt='optinfofile',p='run1/params.out')
  ## rerun from optimum to print population details
  callGadget(s=1,i='run1/params.out',o='run1/like.out')
  printfiles <- read.printfiles('out')
  setwd('../../')
  return(printfiles)  
}

##' R-Gadget testcase
##'
##' R-Gadget test-case as an ecosystem simulator
ecoSim <- function(){
  library(lattice)
  opt <- gadget.options()
  opt$stock <- c('predator','prey')
  opt$doeseat <- 1
  opt$numofareas <- 2
  opt$doescatchcomm <- 1:2
  opt$doescatchsurv <- 1:2
  opt$doesmove <- 0
  opt$immminage <- 1
  opt$matminage <- 1
  opt$immmaxage <- 10
  opt$matmatxage <- 10
  opt$probarea <- c(1,3)/4
  opt$comm.catches <- ''
 
  sim <- Rgadget(opt)
  imm <- as.data.frame.table(sim$immNumRec)
  mat <- as.data.frame.table(sim$matNumRec)
  
}

ices.example <- function(){
  ## set up the initial folders
  try(system('mkdir ices.example',intern=TRUE,ignore.stderr=TRUE))
  setwd('ices.example')
  try(system('mkdir results',intern=TRUE,ignore.stderr=TRUE))
  
  ## example specific switches
  opt <- gadget.options()
  opt$numofareas <- 2
  opt$probarea <- c(1,1)/2
  opt$spbeta <- -0.3
  opt$H <- 400
  opt$m0 <- 1e-5
  opt$Fycomm <- 0.45
  opt$Fysurv <- 0.01
  opt$doescatchsurv <- 1:opt$numofareas
  opt$doescatchcomm <- 1:opt$numofareas
  opt$bylen <- 0
  opt$commfleettype <- 'linearfleet'
  opt$commmultiplicative <- '(* 0.1 #mult)'
  opt$calcindex <- 1
  opt$calcldist.c <- 0
  opt$calcldist.s <- 0
  opt$calc.in.kilos <- 1
  opt$calcalk.c <- 0
  opt$calcalk.s <- 0
  opt$estimate.recruits.by.year <- 1
  opt$optim.params <- c('mult','comma','commb','surveya','surveyb',
                        sprintf('rec%s',1:10))
  opt$survey.sigma <- 0.2
  ## generate dataset
  sim <- Rgadget(opt)
  run.stuff <- function(run){
    try(system(sprintf('mkdir Gfiles%s',run),intern=TRUE,ignore.stderr=TRUE))
    makefiles(sim$opt,sim,location=sprintf('Gfiles%s',run))
    ## estimation run
  
    setwd(sprintf('Gfiles%s',run))
    ## try(system('mkdir run1',intern=TRUE,ignore.stderr=TRUE))
    ## try(system('mkdir out',intern=TRUE,ignore.stderr=TRUE))
    callGadget(l=1,i='refinputfile',p=sprintf('../results/params.%s',run))
    setwd('..')
  }
  library(multicore)
  mclapply(1:100,run.stuff,mc.cores=3)
  ## rerun from optimum to print population details
#  callGadget(s=1,i='run1/params.out',o='run1/like.out')
  #printfiles <- read.printfiles('out')
  ## return to old wd
  tmp <- array(0,c(18,100))
  for(i in 1:100){
    tmp[,i] <- read.table(sprintf('results/params.%s',i),header=TRUE,skip=4)$value
  }
  dat <- read.table('results/params.1',header=TRUE,skip=4,stringsAsFactors=FALSE)
  dimnames(tmp) <- list(parameter=dat$switch,iteration=1:100)
  setwd('../')
  quants <- apply(tmp,1,function(x) quantile(x,c(0.025,0.25,0.5,0.75,0.975)))
  quants.frame <- as.data.frame.table(quants)
  xyplot(Freq~parameter,
         groups=Var1,
         quants.frame[quants.frame$parameter %in%  sprintf("rec%s", 1:10),])
  
  ## hessian matrix approximations
  
  run.hess <- function(run){
    tmp <- run.hessegadget(file.in=sprintf('results/params.%s',run),
                       result=sprintf('results/hess.%s',run),
                       location=sprintf('Gfiles%s',run))
    
    s2 <- tmp$sum.sq/(40-tmp$df)*diag(solve(tmp$hessian[4:18,4:18]))
    return(s2)
  }
  
#  s2 <- array(0,c(18,100))
#  for(i in 1:100){
#    s2[,i] <- run.hess(i)
#  }
#  save.image(file='s2.RData')
  s2 <- mclapply(1:100,run.hess,mc.cores=4)
  rec.mat <- tmp[sprintf('rec%s',1:10),]
  s2.mat <- sapply(s2,function(x){ x[sprintf('rec%s',1:10)]})
  s.mat <- sqrt(s2.mat)
#  s.mat[is.na(s.mat)] <- 0.1
  up.conf <- rec.mat+2*s.mat
  lo.conf <- rec.mat-2*s.mat
  
  for(i in 1:100){blu[i] <- sum(up.conf[,i]>1&lo.conf[,i]<1)}
  pdf('hess.pdf')
  plot(1:10,
       rec.mat[,13],
       type='l',lty=1,ylim=c(0.5,1.2),
       ylab='Recruitment',
       xlab='Year',lwd=2,
       cex.lab=1.5,
       col='red'
       )
  axis(1,lwd=2)
  axis(2,lwd=2)
  lines(1:10,rec.mat[,13]+qnorm(0.25)*s.mat[,13],lty=2,lwd=2)
  lines(1:10,rec.mat[,13]+qnorm(0.75)*s.mat[,13],lty=2,lwd=2)
  lines(1:10,rec.mat[,13]+qnorm(0.025)*s.mat[,13],lty=3,lwd=2)
  lines(1:10,rec.mat[,13]+qnorm(0.975)*s.mat[,13],lty=3,lwd=2)
  lines(1:10,rep(1,10),col='blue',lwd=2)
  legend('bottomright',inset = 0.01,legend=c('True Value','Parameter estimate','25% quantiles','2.5% quantiles'),lty=c(1,1,1,2,3),col=c('blue','red',rep('black',2)),bg='grey90',lwd=2)
  dev.off()
  pdf('mont.pdf')
  plot(1:10,
       quants[3,sprintf('rec%s',1:10)],
       type='l',lty=1,ylim=c(0.5,1.2),
       ylab='Recruitment',
       xlab='Year',lwd=2,
       cex.lab=1.5
       )
  axis(1,lwd=2)
  axis(2,lwd=2)
  for(i in 1:5){
    lines(1:10,quants[i,sprintf('rec%s',1:10)],
          type='l',lty=abs(i-3)+1,lwd=2)
  }
  lines(1:10,rep(1,10),col='blue',lwd=2)
  lines(1:10,apply(tmp,1,mean,lwd=2)[sprintf('rec%s',1:10)],col='red')
  legend('bottomright',inset = 0.01,legend=c('True Value','Mean','Median','25% quantiles','2.5% quantiles'),lty=c(1,1,1,2,3),col=c('blue','Red',rep('black',3)),bg='grey90',lwd=2)
  dev.off()
  invisible(tmp)
}



