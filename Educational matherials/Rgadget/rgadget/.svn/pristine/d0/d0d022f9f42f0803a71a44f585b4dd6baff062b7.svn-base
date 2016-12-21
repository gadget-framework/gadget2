##' This function creates a list of default values of all necessary switches
##' for the RGadget simulation. The user can then change the
##' values of the switches and use the changed list as input to RGadget.
##' Here the default values for consumption (predation and fleet operations),
##' migration, maturation (via stock 'movement'), number of areas and their
##' basic properties, various attributes of the sub stocks such as age, length
##' and weight along with growth parameters. Also length of the simulation is
##' given a default value.
##' If the user wants to change the default values he will need to make the
##' changes on the resulting list.
##' @title Gagdet options
##' @return a list of swithes
##' \item{stocks}{names of the stocks in the simulation}
##' \item{doeseat}{Does the 'mature' stock eat the 'immature'}
##' \item{doescatchsurv}{Is there a survey fleet}
##' \item{doescatchcomm}{Is there a commercial fleet}
##' \item{doesmigrateimm}{Does the immature stock migrate}
##' \item{doesmigratemat}{Does the mature stock migrate}
##' \item{immMigration}{Migration matrix for the immmature substock}
##' \item{matMigration}{Migration matrix for the mature substock}
##' \item{doesfuncmigrate}{(migration) pde's used to describe migration.}
##' \item{diffusion}{(migration) diffusion parameter}
##' \item{driftx}{(migration) drift in x coordinate}
##' \item{drifty}{(migration) drift in y coordinate}
##' \item{doesmove}{Does the immature stock mature into the mature stock}
##' \item{numofareas}{Number of gadget areas}
##' \item{probarea}{A vector of proportions in a given area, assumed equal for both stocks}
##' \item{areasize}{Size of the gadget area (assumed equal for all areas}
##' \item{area.temperature}{Average temperature of the area}
##' \item{immminage}{Minimum age of the immmature stock}
##' \item{immmaxage}{Maximum age of the immmature stock}
##' \item{matminage}{Minimum age of the mature stock}
##' \item{matmaxage}{Maximum age of the mature stock}
##' \item{minlen}{Minimum length of both stocks}
##' \item{maxlen}{Maximum length of both stocks}
##' \item{lengthgrouplen}{Size of each lengthgroup. We assume the size of the lengthgroups is the same for both stocks.}
##' \item{a}{a in the length-weight relationship a*l^b}
##' \item{b}{b in the length-weight relationship a*l^b}
##' \item{sigma}{The standard deviation of length at i years old. This vector must the same length as the number of ages.}
##' \item{n}{Number of recruits per year.}
##' \item{murec}{If specified this will be the meanlength of recruits}
##' \item{lsup}{L-infinity. Bertalanffy growth parameters lsup, and k for the growth function (used for all ages > 1)}
##' \item{binn}{binn is the maximum updating length}
##' \item{beta}{Beta for beta-binomial}
##' \item{numobs}{number of years observed}
##' \item{numoftimesteps}{number of timesteps in each year}
##' \item{z}{z is the natural mortality constant used to calculate the size of the initial population for age 2 +}
##' \item{spalpha}{alpha for the predation suitability function}
##' \item{spbeta}{beta for the predation suitability function}
##' \item{spagamma}{gamma for the predation suitability function}
##' \item{spdelta}{delta for the predation suitability function}
##' \item{m0}{m0 for the maximum consumption}
##' \item{m3}{m3 for the maximum consumption}
##' \item{H}{H The density (biomass per area unit) of available food at which the predator can consume half maximum consumption}
##' \item{otherfrac}{the fraction of otherfood that is eaten}
##' \item{otherfood}{The maximum portion consumed, in Gadget it is 0.95, this is known as understocking in Gadget}
##' \item{survstep}{timestep(s) for the survey}
##' \item{commstep}{timestep(s) for the commercial effort}
##' \item{salphasurv}{for the suitability function - survey}
##' \item{sbetasurv}{for the suitability function - survey}
##' \item{survfleettype}{Fleettype for the survey}
##' \item{survmultiplicative}{For the fleettype}
##' \item{Fysurv}{Fishing effort of the survey}
##' \item{surv.catches}{What stocks does the survey fleet catch from}
##' \item{salphacomm}{for the suitability function - commerical catch}
##' \item{sbetacomm}{for the suitability function - commercial catch}
##' \item{commfleettype}{Fleettype for the commercial catch}
##' \item{comm.catches}{What stocks does the commercial fleet catch from}
##' \item{commmultiplicative}{For the fleettype}
##' \item{Fycomm}{Fishing effort of the commercial catch}
##' @author Bjarki Thor Elvarsson, Asta Jenny Sigurdardottir and Elinborg Ingunn Olafsdottir
##' @examples
##' opt <- gadget.options
##' ## change the length of the simulation to 13 years
##' opt$numobs <- 13
gadget.options <- function(){
  opt <- list(
#############################################################
#
# This file contains all variables and switches
# for a 2 stock 1 area Gadget model.
#
              stocks=c('imm','mat'),
           
#############################################################
#
# Set the switch to 1 if you want to catch, else set it to 0
#
              doeseat=1,
              
              doescatchsurv=1,
              surv.catches=c('imm','mat'),
              doescatchcomm=1,
              comm.catches=c('imm','mat'),
# Migration
              doesmigrateimm = 1,
              doesmigratemat = 1,
              immMigration = array(c(1,0,.4,.6,.6,.4,0,1,.6,.4,0,1,1,0,.4,.6),c(2,2,4)),
              matMigration = array(c(1,0,.4,.6,.6,.4,0,1,.6,.4,0,1,1,0,.4,.6),c(2,2,4)),
              doesfuncmigrate = 0,
              diffusion = NULL,
              driftx = NULL,
              drifty = NULL,
              lambda = NULL,
              
# immature fish move into mature population
              
              doesmove=1,

# Number of areas (cannot currently be more than 2)
              numofareas = 1,
#The portion of the stock in area 1.
              probarea=1,
# The temperature of the area
              area.temperature=5,
# areas assumed to be of equal size
              areasize=2*10^5,

#################################
#
# Variables for the ages
#
# Min- and maxage for immature
              immminage=1,
              immmaxage=3,
# Min- and maxage for the mature
              matminage=4,
              matmaxage=10,

#################################
#
# Variables for the lengths and weights
#
#min and max length in lengthgroups
              minlen=5,
              maxlen=90,

# Size of each lengthgroup. We assume the size of
# the lengthgroups is the same for both stocks.
              lengthgrouplen=1,

# a and b in the length-weight relationship a*l^b
              a=10^(-5),
              b=3,

# The standard deviation of length at i years old
# This vector must the same length as the number of ages.
              sigma=c(2.2472, 2.8982, 4.0705, 4.9276,
                5.5404, 5.8072, 6.0233, 8, 9, 9),
              ## number of recruits
              n = 1000000,
              ## Meanlength for the recruits
              murec=NULL,
# von Bertalanffy growth parameters for the growth function
# (used for all ages > 1)
# L-infinity
              lsup=115,
# k
              k=0.09,

# binn is maximum updating length
              binn=15,

# Beta for beta-binomial
              beta=200,

#################################
# Variables for time and area
#
# number of years observed
              numobs=10,

# numoftimesteps is a number of timesteps in each year
              numoftimesteps=4,


#################################
#
# Variables for the the initial stock
#

# z is the natural mortality constant used to calculate the size of
# the initial population for age 2 +
              z=0.2,


#################################
#
# Variables for the predation
#

# alpha and beta for the predation suitability function
              spalpha = 4.5,
              spbeta = -0.2,
              spgamma = -0.3,
              spdelta = 1,

# For the maximum consumption
              m0=10^(-2),
              m3=3,
              H=4*10^3,

# otherfrac is the fraction of otherfood that is eaten
              otherfrac=0.8,
              otherfood=50000,
# The maximum portion consumed, in Gadget it is 0.95
# this is known as understaocking in Gadget
              maxratioconsumed=0.95,

#################################
#
# Variables for the catch
#
              
# timesteps for the fleets:
              survstep = 2,
              commstep = 1:4,

#alpha and beta for the suitability function - survey
              salphasurv= -4.5,
              sbetasurv= 0.3,
              survfleettype='totalfleet',
              survmultiplicative='1',
              Fysurv=0.1,
#alpha and beta for the suitability function - catch
              salphacomm = -8,
              sbetacomm =  0.22,
              commfleettype='totalfleet',
              commmultiplicative='1',
              Fycomm=0.7


              )
  class(opt) <- c('gadget.options',class(opt))
  return(opt)
}

##' This function is a helper function for RGadget.
##' it calculates additional options and switches that can be derived from
##' the gadget options lists.
##' @title Derived options
##' @param opt gadget options list
##' @return augmented gadget options list
derivedOptions <- function(opt){
  
# Min- and maxage over both of the stocks
    
  opt$minage <- min(opt$immminage,
                    opt$matminage)
  opt$maxage <- max(opt$immmaxage,
                    opt$matmaxage)
  
#################################
# Variables for time and area
#
# lengthoftimesteps is the length of each timestep
  opt$lengthoftimesteps <- 12/opt$numoftimesteps
  if(length(opt$probarea)<opt$numofareas){
    opt$probarea <- rep(1,opt$numofareas)/opt$numofareas
    warning('length(opt$probarea)<opt$numofareas - equal initial area probabilites assumed')
  }
#################################
#
# Variables for the the initial stock
#

# n is a vector where n[i] is number of recruits in timestep i
# mod(numoftimestep) in year ([i/numoftimesteps]+1)
  if(length(opt$n)==1)
    opt$n <- rep(c(opt$n,0,0,0),opt$numobs)
  if(length(opt$n)<opt$numobs*opt$numoftimesteps)
    warning('Recruitment vector has length less the total number of timesteps. Expect errors.')
  
    
#################################
#
# Natural mortality
#
# mort[i] is the natural mortality for age i at each time step
# this assumes the same natural mortality as for the initial year
# and for each age
  opt$mort <- c(rep(opt$z,opt$maxage-1),0.5)

#################################
#
# Variables for the predation
#
# Otherfood, the amount eaten in biomass
  opt$otherfood <- rep(opt$otherfood,
                       opt$numobs*opt$numoftimesteps*opt$numofareas)
  dim(opt$otherfood) <- c(opt$numofareas,opt$numobs*opt$numoftimesteps)
#################################
#
# Variables for the catch
                                        #

    #F for the catch
  opt$Fysurv <- rep(opt$Fysurv, opt$numobs/length(opt$Fysurv))
  opt$Fycomm <- rep(opt$Fycomm, opt$numobs/length(opt$Fycomm))

  #################################
#
# Initialises the times

# Length of the timestep
  opt$dt <- 1/opt$numoftimesteps

################################
#
# Initialises the ages
#
# Dummy variables
  opt$matmax <- opt$matmaxage-opt$matminage+1
  opt$immmax <- opt$immmaxage-opt$immminage+1

################################
#
# Initialises the lengths and the weights
#
# l is a vector containing the endpoints for the lengthgroups.
# We assume that the size of the lengthgroups are all the same,
# so if this is not preferable change next line of code.
  opt$l <- seq(opt$minlen,opt$maxlen,opt$lengthgrouplen)

# The number of lengthgroups
  opt$numoflgroups <- length(opt$l)-1

#lt[i] is the mean of lengthgroup i
  opt$lt <- (opt$l[2:length(opt$l)]+opt$l[1:(length(opt$l)-1)])/2

#w[i] is the mean weight in lengthgroup i
  opt$w <- opt$a*opt$lt^opt$b

# mu[i] meanlength for i years old
  opt$mu <- opt$lsup*(1-exp(-opt$k*1:opt$maxage))
  if(!is.null(opt$murec))
    opt$mu[1] <- opt$murec
# timesteps of the survey
  opt$survtimesteps <- (1:opt$numobs-1)*opt$numoftimesteps+opt$survstep
#################################
#
# Predation
#
# Maximum consumption
  opt$maxConsumption <- opt$m0*opt$lt^opt$m3*12*opt$dt
  if(opt$numofareas==1){
    opt$doesmigratemat <- 0
    opt$doesmigrateimm <- 0
  }

  return(opt)
}
