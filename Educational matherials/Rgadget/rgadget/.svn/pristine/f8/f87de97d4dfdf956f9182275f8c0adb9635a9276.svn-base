##' Rgadget is the driver function for the ecosystem simulation. It takes a
##' gadget options list, where all parameters of the simulation are set, as
##' input and runs the simulation for a set period of time (the length of the
##' timesteps and number of observations (years) are also defined in gadget
##' options).
##'
##' The Rgadget simulator replicates some of the more commonly used features of
##' Gadget, with use of some of these features optional. The most complex model
##' possible consists of two substocks, with the younger substock optionally
##' maturing into the older substock at a given age and timestep, ie the
##' maturation process is not modelled. Both substocks are subject to growth
##' and natural mortality and live on a number of areas. Two fleets are possible, eg
##' one representing the commercial fleet and the other an annual survey, with
##' the timesteps on which these fleets operate optional. There can be
##' migration between the areas. The mature substock can also predate upon the
##' immature stock. As with Gadget, the fleets are modelled as predators. The
##' order in which the population processes are modelled on each timestep is
##' identical to that in Gadget.
##' 
##' As Gadget is a forward simulation model, it is started with an initial
##' population. The length distribution of each age group of the population
##' is assumed to be normally distributed with a given mean and standard
##' deviation (specified by the user). The youngest age group in each year
##' is entered into the population in a similar manner.
##' Fleets are modelled as predators, as in Gadget, and operate on some or all
##' areas and at some or all timesteps which are defined by the user.
##' Consumption (predation or harvesting) implemented through length based
##' suitability functions of the form:
##'  \deqn{S_{pred,prey}(L,l) = \frac{\delta}{1+e^{-\alpha-\beta l-\gamma L}}}
##' where of l is the prey length and L predator length. For fleets L should be
##' irrelevant and therefore \eqn{\gamma = 0}{gamma  = 0} for fleets.
##' 
##' Maturation from stock A to stock B is modelled by moving the oldest
##' agegroup of A into with the age increasing if done on the last timestep
##' of the year. This replicates the Gadget process \emph{doesmove}.
##'
##' Growth follows a beta-binomial distribution with mean update length as
##' stipulated by the von Bertanlanffy curve.
##' 
##' The order of calculations is the same as in Gadget and is as follows:
##'
##' 1. Migration between areas
##' 
##' 2. Consumption, including catch by the fleets
##' 
##' 3. Natural mortality
##' 
##' 4. Growth
##' 
##' 5. Recruitment
##' 
##' 6. Remove the stock, here immature, that will move
##' 
##' 7. Increase the age
##' 
##' 8. Replace the stock, here to the mature, that has moved and increase the age. 
##' @title Rgadget
##' @param opt gadget options list, as defined by 'gadget.options'
##' @return a list of arrays:
##' \item{Rec}{Recruits for all years}
##' \item{immStart}{Initial starting population for the immature stock age 2 and older}
##' \item{matStart}{Initial starting population for the mature stock age 2 and older}
##' \item{immNumRec}{Immature stock population for all timesteps, areas, ages and lengths}
##' \item{matNumRec}{Mature stock population for all timesteps, areas, ages and lengths}
##' \item{immCsurv}{Survey catches of the immature stock}
##' \item{matCsurv}{Survey catches of the mature stock}
##' \item{immCcomm}{Commercial catches of the immature stock}
##' \item{matCcomm}{Commercial catches of the mature stock}
##' \item{Eat}{Amount consumed of immatures by matures}
##' \item{GrowthProb}{Growthprobability matrix}
##' \item{immMort}{Natural mortality for the immature stock}
##' \item{matMort}{Natural mortality for the mature stock}
##' \item{opt}{Gadget options list used in the simulation}
##'  @author Bjarki Thor Elvarsson, Asta Jenny Sigurdardottir and Elinborg Ingunn Olafsdottir
##' @examples
##' opt <- gadget.options()
##' sim <- Rgadget(sim)
Rgadget <- function(opt=gadget.options()){
  ## initialize the necessary variables  
#  opt <- derivedOptions(opt)
  ## what areas are commercially exploited
  #commAreas <- opt$areas %in% opt$doescatchcomm
  ## define survey areas 
#  surveyAreas <- 1:opt$numofareas %in% opt$doescatchsurv

################################
#
# The stocks
# preN(k)=[N_{i,1,j}] is a matrix where immN(k)_{i,1,j} 7is the number
# of recruits in lengthgroup i at timestep j, area k
  Abundance <- array(0,c(length(opt$stocks),
                         2,
                         opt$numofareas,
                         opt$numoflgroups,
                         opt$maxage+1,
                         (opt$numobs*opt$numoftimesteps)))
  dimnames(Abundance) <- list(stock=opt$stocks,
                              gender=c('Male','Female'),
                              area=opt$areas,
                              length=opt$minlen:(opt$maxlen-1),
                              age=0:opt$maxage,
                              time=paste(sprintf('Year_%s',rep(1:opt$numobs,
                                each=opt$numoftimesteps)
                                ),
                                sprintf('Step_%s',rep(1:opt$numoftimesteps,
                                                      opt$numobs)),
                                sep='_'))
  ## Tagged population
  Tagged <- Abundance
    
################################
#
# Calculates natural mortality per timestep

#preM is a survival matrix, same in both areas
  Mort <- diag(exp(-opt$mort*opt$dt))
#  immMort <- M[opt$immminage:opt$immmaxage,
#               opt$immminage:opt$immmaxage]
#  matMort <- M[opt$matminage:opt$matmaxage,
#               opt$matminage:opt$matmaxage]

################################
#
# Defines the catch matrices
  catch.switch<- unique(c(opt$doescatchcomm,opt$doescatchsurv))
  
  if(length(catch.switch)>0){
    Catches <- array(0,c(length(opt$fleets),
                         length(opt$stocks),
                         2,
                         length(catch.switch),
                         opt$numoflgroups,
                         opt$numofagegroups,
                         (opt$numobs*opt$numoftimesteps)))
    dimnames(Catches) <- list(fleets=opt$fleets,
                              stock=opt$stocks,
                              gender=c('Male','Female'),
                              area=catch.switch,
                              length=opt$minlen:(opt$maxlen-1),
                              age=0:opt$maxage,
                              time=paste(sprintf('Year_%s',
                                rep(1:opt$numobs,
                                    each=opt$numoftimesteps)
                                ),
                                sprintf('Step_%s',
                                        rep(1:opt$numoftimesteps,
                                            opt$numobs)),
                                sep='_'))
    Tagged.C <- Catches
  } else {
    Catches <- NULL
  }

 
#################################
#
# Predation
#

# The number eaten of immature by mature, the default is zero
#  if(opt$doeseat==1){
#    Eat <- array(0,c(opt$numofareas,
#                     opt$numoflgroups,
#                     opt$numofagegroups,
#                     opt$numobs*opt$numoftimesteps))
#    dimnames(Eat) <- dimnames(immNumRec)
#  } else {
#    Eat <- NULL
#  }
  
#################################

# Assume we have one pseudo stock which splits between the 2
# areas in the portion probarea1 in area 1 and
# (1-probarea1) in area 2.
#  Start<-firststep(opt$n,
#                   opt$mu,
#                   opt$sigma,
#                   opt$l,
#                   opt$z,
#                   opt$numofareas,
#                   opt$probarea,
#                   opt$minage,
#                   opt$maxage)
  
# preStart[i,j] is number in pseudo stock at the beginning of timestep 1,
# (1 year olds are not included). We use Start to make initial
# mature and immature stock such that the amount of i year old in the
# mature/immmature stock is the same as in the pseudo stock.
#  immStart <- Start[,,(opt$immminage+1):opt$immmaxage]
#  matStart <- Start[,,(opt$matminage:opt$matmaxage)]
#  if(opt$numofareas==1){
#    ## ugly hack because of destroyed array dimension
#    dimnames(immStart) -> tmp
#    dim(immStart) <- c(1,dim(immStart))
#    dimnames(immStart) <- list(area=1,length=tmp[[1]],age=tmp[[2]])
#    dimnames(matStart) -> tmp
#    dim(matStart) <- c(1,dim(matStart))
#    dimnames(matStart) <- list(area=1,length=tmp[[1]],age=tmp[[2]])
#  }
#Matrix of length divided recruits
#  Rec <- recruits(opt$n,opt$mu[opt$immminage],opt$sigma[opt$immminage],
#                  opt$l,opt$numofareas, opt$probarea,
#                  opt$numobs,opt$numoftimesteps)

#G[i,j] is the probability of going from lengthgroup i to lengthgroup j
#Same in both areas
#  if(opt$doesgrow==1){
#    
#    G <- growthprob(opt$lt,
#                    opt$beta,
#                    opt$lsup,
#                    opt$k,
#                    opt$dt,
#                    opt$lengthgrouplen,
#                    opt$binn)
#  } else {
#    G <- diag(opt$numoflgroups)
#  }

 
  
####################################
#  Calculations for all timesteps  #
####################################
  for(i in 1:(opt$numobs*opt$numoftimesteps)){        
    #print(sprintf('simulating year %s', i))
    if(i==1){
      for(stock in opt$stocks){
        tmp <- init.pop(opt$init.abund[stock],opt$z,
                        opt$maxage,
                        opt$mixing[,stock])
        Abundance[stock,'Male',,,,1] <- opt$gender.division['Male']*tmp
        Abundance[stock,'Female',,,,1] <- opt$gender.division['Female']*tmp
        
      }
      ## ASUMES ONE TAGGING AREA ONLY!!!!!!!!!!!!!!!!!!!!!!!!!!
      Tagged[,,names(opt$num.tags),,,1] <-
        whaleCatch(Abundance[,,names(opt$num.tags),,,i],
                   Tagged[,,names(opt$num.tags),,,i],
                   opt$num.tags,
                   opt$salpha,
                   opt$sbeta                    
                   )$C
                                        #        opt$num.tags*Abundance[,,names(opt$num.tags),,,1]/
#          sum(Abundance[,,names(opt$num.tags),,,1])
    } else {
      ## ###########
      ## Age update
      ## NOTE this is the last step of the
      ## calculations done in previous timestep
      
      ## Update age for mature
      Abundance[,,,,-1,i] <- Abundance[,,,,-opt$numofagegroups,i-1]
      ## Adding up the maxage ones
      Abundance[,,,,opt$numofagegroups,i] <-
        Abundance[,,,,opt$numofagegroups,i] +
          Abundance[,,,,opt$numofagegroups,i-1]
      
      ## do the same for the tagged population
      Tagged[,,,,-1,i] <- Tagged[,,,,-opt$numofagegroups,i-1]
      ## Adding up the maxage ones
      Tagged[,,,,opt$numofagegroups,i] <-
        Tagged[,,,,opt$numofagegroups,i] +
          Tagged[,,,,opt$numofagegroups,i-1]

      if(!is.null(opt$dispersion)){
        Abundance[,'Male',,,,i] <-
          dispersion(Abundance[,'Male',,,,i],opt$dispersion)
        Abundance[,'Female',,,,i] <-
          dispersion(Abundance[,'Female',,,,i],opt$dispersion)
        Tagged[,'Male',,,,i] <-
          dispersion(Tagged[,'Male',,,,i],opt$dispersion)
        Tagged[,'Female',,,,i] <-
          dispersion(Tagged[,'Female',,,,i],opt$dispersion)
      }
      
      if(!is.null(opt$mixing)){
        Abundance[,'Male',,,,i] <-
          overlap(Abundance[,'Male',,,,i],opt$mixing)
        Abundance[,'Female',,,,i] <-
          overlap(Abundance[,'Female',,,,i],opt$mixing)
        Tagged[,'Male',,,,i] <-
          overlap(Tagged[,'Male',,,,i],opt$mixing)
        Tagged[,'Female',,,,i] <-
          overlap(Tagged[,'Female',,,,i],opt$mixing)
      }
      
    }



    ## ##########
    ## Catch calculations    
    if(length(catch.switch)>0){
      tmp <- whaleCatch(Abundance[,,catch.switch,,,i],
                        Tagged[,,catch.switch,,,i],
                        opt$quota[catch.switch],
                        opt$salpha,
                        opt$sbeta                    
                        )
      Catches['Comm',,,catch.switch,,,i] <- tmp$C
      Tagged.C['Comm',,,catch.switch,,,i] <- tmp$CT
    }
    
    ## ########
    ## Subtract Catch from stock
    Abundance[,,catch.switch,,,i] <-
      Abundance[,,catch.switch,,,i]-Catches['Comm',,,,,,i]
    Tagged[,,catch.switch,,,i] <-
      Tagged[,,catch.switch,,,i]-Tagged.C['Comm',,,,,,i]
    
    ## #########
    ## Length update and natural mortality
    for(stock in opt$stocks){
      for(area in opt$areas){
        Abundance[stock,,area,,,i] <-
          Abundance[stock,,area,,,i]%*%Mort +
            (1-opt$tag.loss)*Tagged[stock,,area,,,i]%*%Mort
        Tagged[stock,,area,,,i] <-
          opt$tag.loss*Tagged[stock,,area,,,i]%*%Mort
      }
    }
    
    ## #########
    ## Recruits
    N <- apply(Abundance[,'Female',,,
                         opt$age.of.parturation:opt$maxage + 1,i],
               1:2,sum)
    Abundance[,,,,'0',i] <-
      Births(opt$avg.B,N,opt$resiliance.a['r1'],
             opt$density.z,opt$init.abund)
    
  }

  
  sim <- list(Abundance=Abundance,
              Tagged=Tagged,
              Catches=Catches,
              Tagged.C=Tagged.C,
              opt=opt)
  class(sim) <- c('gadget.sim',class(sim))
  return(sim)
}
