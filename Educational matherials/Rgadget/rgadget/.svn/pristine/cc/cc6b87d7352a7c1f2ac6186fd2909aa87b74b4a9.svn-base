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
gadget.simulate <- function(opt=gadget.options()){
  ## initialize the necessary variables  
  opt <- derivedOptions(opt)
  ## what areas are commercially exploited
  commAreas <- 1:opt$numofareas %in% opt$doescatchcomm
  ## define survey areas 
  surveyAreas <- 1:opt$numofareas %in% opt$doescatchsurv

################################
#
# The stocks
# preN(k)=[N_{i,1,j}] is a matrix where immN(k)_{i,1,j} 7is the number
# of recruits in lengthgroup i at timestep j, area k
#  stocks <- array(0,c(length(opt$stocks),
#                      opt$
  immNumRec <- array(0,c(opt$numofareas,
                         opt$numoflgroups,
                         opt$immmax,
                         (opt$numobs*opt$numoftimesteps)))
  dimnames(immNumRec) <- list(area=1:opt$numofareas,
                              length=opt$minlen:(opt$maxlen-1),
                              age=opt$immminage:opt$immmaxage,
                              time=paste(sprintf('Year_%s',rep(1:opt$numobs,
                                each=opt$numoftimesteps)
                                ),
                                sprintf('Step_%s',rep(1:opt$numoftimesteps,
                                                      opt$numobs)),
                                sep='_'))
  matNumRec <- array(0,c(opt$numofareas,
                         opt$numoflgroups,
                         opt$matmax,
                         (opt$numobs*opt$numoftimesteps)))
  dimnames(matNumRec) <- list(area=1:opt$numofareas,
                              length=opt$minlen:(opt$maxlen-1),
                              age=opt$matminage:opt$matmaxage,
                              time=paste(sprintf('Year_%s',rep(1:opt$numobs,
                                each=opt$numoftimesteps)
                                ),
                                sprintf('Step_%s',rep(1:opt$numoftimesteps,
                                                      opt$numobs)),
                                sep='_'))

################################
#
# Calculates natural mortality per timestep

#preM is a survival matrix, same in both areas
  M <- diag(exp(-opt$mort*opt$dt))
  immMort <- M[opt$immminage:opt$immmaxage,
               opt$immminage:opt$immmaxage]
  matMort <- M[opt$matminage:opt$matmaxage,
               opt$matminage:opt$matmaxage]

################################
#
# Defines the catch matrices
  catch.switch<- opt$doescatchcomm+opt$doescatchsurv
  
  if(sum(catch.switch)>0)
    {
      immCcomm <- array(0,c(opt$numofareas,
                            opt$numoflgroups,
                            opt$immmax,
                            (opt$numobs*opt$numoftimesteps)))
      dimnames(immCcomm) <- dimnames(immNumRec)
      matCcomm <- array(0,c(opt$numofareas,
                            opt$numoflgroups,
                            opt$matmax,
                            (opt$numobs*opt$numoftimesteps)))
      dimnames(matCcomm) <- dimnames(matNumRec)
      immCsurv <- array(0,c(opt$numofareas,
                            opt$numoflgroups,
                            opt$immmax,
                            (opt$numobs*opt$numoftimesteps)))
      dimnames(immCsurv) <- dimnames(immNumRec)
      matCsurv <- array(0,c(opt$numofareas,
                            opt$numoflgroups,
                            opt$matmax,
                            (opt$numobs*opt$numoftimesteps)))
      dimnames(matCsurv) <- dimnames(matNumRec)
    }


#################################
#
# Predation
#

# The number eaten of immature by mature, the default is zero
  Eat <- array(0,c(opt$numofareas,
                   opt$numoflgroups,
                   opt$immmax,
                   opt$numobs*opt$numoftimesteps))
  dimnames(Eat) <- dimnames(immNumRec)
  
#################################

# Assume we have one pseudo stock which splits between the 2
# areas in the portion probarea1 in area 1 and
# (1-probarea1) in area 2.
  Start<-firststep(opt$n,
                   opt$mu,
                   opt$sigma,
                   opt$l,
                   opt$z,
                   opt$numofareas,
                   opt$probarea,
                   opt$minage,
                   opt$maxage)
  
# preStart[i,j] is number in pseudo stock at the beginning of timestep 1,
# (1 year olds are not included). We use Start to make initial
# mature and immature stock such that the amount of i year old in the
# mature/immmature stock is the same as in the pseudo stock.
  immStart <- Start[,,(opt$immminage+1):opt$immmaxage]
  matStart <- Start[,,(opt$matminage:opt$matmaxage)]
  if(opt$numofareas==1){
    ## ugly hack because of destroyed array dimension
    dimnames(immStart) -> tmp
    dim(immStart) <- c(1,dim(immStart))
    dimnames(immStart) <- list(area=1,length=tmp[[1]],age=tmp[[2]])
    dimnames(matStart) -> tmp
    dim(matStart) <- c(1,dim(matStart))
    dimnames(matStart) <- list(area=1,length=tmp[[1]],age=tmp[[2]])
  }
#Matrix of length divided recruits
  Rec <- recruits(opt$n,opt$mu[opt$immminage],opt$sigma[opt$immminage],
                  opt$l,opt$numofareas, opt$probarea,
                  opt$numobs,opt$numoftimesteps)

#G[i,j] is the probability of going from lengthgroup i to lengthgroup j
#Same in both areas
  G <- growthprob(opt$lt,
                  opt$beta,
                  opt$lsup,
                  opt$k,
                  opt$dt,
                  opt$lengthgrouplen,
                  opt$binn)

####################################
#  Calculations for all timesteps  #
####################################
  for(i in 1:(opt$numobs*opt$numoftimesteps))
    {
      num<-i%%opt$numoftimesteps
      if(num==0)
        num <- opt$numoftimesteps
      if(num!=1){      ############## if we are not in timestep 1  #########
        immNumRec[,,,i] <- immNumRec[,,,i-1]
        matNumRec[,,,i] <- matNumRec[,,,i-1]
      } else if(i==1){ ### we have a special update in the 1st timestep ###
        immNumRec[,,-1,1] <- immStart
        matNumRec[,,,1] <- matStart
      } else { ###### if we are in timestep 1 we have to update age ######
        
    #############
    # Age update
    # NOTE this is the last step of the
    # calculations done in previous timestep

    # Update age for immature upto age immmaxage-1
          immNumRec[,,-1,i] <- immNumRec[,,-opt$immmax,i-1]
    # Update age for mature
          matNumRec[,,-1,i] <- matNumRec[,,-opt$matmax,i-1]
    # Adding up the maxage ones
           matNumRec[,,opt$matmax,i] <-
             matNumRec[,,opt$matmax,i] + matNumRec[,,opt$matmax,i-1]

          if(opt$doesmove==1){
      ## Adding the ones which have moved between stocks
            matimmdiff <- opt$immmaxage-opt$matminage+2
            matNumRec[,,matimmdiff,i] <-
              matNumRec[,,matimmdiff,i]+immNumRec[,,opt$immmax,i-1]
          } else {
      ## if they don't move between stocks we have
      ## a + group for the immmaxage ones
            immNumRec[,,opt$immmax,i] <-
              immNumRec[,,opt$immmax,i]+immNumRec[,,opt$immmax,i-1]
          }
        }
      ############
      # Migration Assume only two areas atm
      if(opt$doesmigrateimm==1){
        immNumRec[,,,i] <- migrate(immNumRec[,,,i],opt$immMigration[,,num])
#        immTemp<-migrate(immNumRec[1,,,i],immNumRec[2,,,i],
#                         num,P=migrationProb(opt=opt),opt=opt)
#        immNumRec[1,,,i] <- immTemp[,,1]
#        immNumRec[2,,,i] <- immTemp[,,2]
      }
      if(opt$doesmigratemat==1){
        matNumRec[,,,i] <- migrate(matNumRec[,,,i],opt$matMigration[,,num])
#        matTemp<-migrate(matNumRec[1,,,i],matNumRec[2,,,i],
#                         num,P=migrationProb(opt=opt),opt=opt)
#        matNumRec[1,,,i] <- matTemp[,,1]
#        matNumRec[2,,,i] <- matTemp[,,2]
      }


  ############
  # Consumption calculations
      if(opt$doeseat==1){
          Eat[,,,i] <- eat(immNumRec,matNumRec,i,opt)
        }
      
  ############
  # Catch calculations
      if(num %in% opt$commstep){
        if(sum(opt$doescatchcomm) > 0)
          {
            if('imm' %in% opt$comm.catches){
              immCcomm[commAreas,,,i] <-
                catch(immNumRec[commAreas,,,i],
                      i,
                      opt$Fycomm,
                      opt$salphacomm,
                      opt$sbetacomm,
                      opt$numoftimesteps,
                      opt$numobs,
                      opt$lt)
            }
            if('mat' %in% opt$comm.catches){
              matCcomm[commAreas,,,i] <-
                catch(matNumRec[commAreas,,,i],
                      i,
                      opt$Fycomm,
                      opt$salphacomm,
                      opt$sbetacomm,
                      opt$numoftimesteps,
                      opt$numobs,
                      opt$lt)
            }
          }
      }
      if(num %in% opt$survstep) 
        {
          if(sum(opt$doescatchsurv) > 0)
            {
              if('imm' %in% opt$surv.catches){
                immCsurv[surveyAreas,,,i] <-
                  catch(immNumRec[surveyAreas,,,i],
                        i,
                        opt$Fysurv,
                        opt$salphasurv,
                        opt$sbetasurv,
                        1,
                        opt$numobs,
                        opt$lt)
              }
              if('mat' %in% opt$surv.catches){
                matCsurv[surveyAreas,,,i] <-
                  catch(matNumRec[surveyAreas,,,i],
                        i,
                        opt$Fysurv,
                        opt$salphasurv,
                        opt$sbetasurv,
                        1,
                        opt$numobs,
                        opt$lt)
              }
            }
        }

  #############
  # Overconsumption check
      tempimmC<-adjustconsumption(C=immCcomm[,,,i],
                                  S=immCsurv[,,,i],
                                  E=Eat[,,,i],
                                  N=immNumRec[,,,i],
                                  opt$maxratioconsumed,
                                  opt$numofareas)
      tempmatC<-adjustconsumption(C=matCcomm[,,,i],
                                  S=matCsurv[,,,i],
                                  ,
                                  N=matNumRec[,,,i],
                                  opt$maxratioconsumed,
                                  opt$numofareas)

  #############
  # Subtract Consumption from stock
      if(opt$doeseat==1)
        {
          immNumRec[,,,i] <- immNumRec[,,,i] - tempimmC$E[,,]
        }

  ##########
  # Subtract Catch from stock
      if(num==opt$survstep) # only have survey in one timestep of the year
        {
          surveyAreas <- 1:opt$numofareas %in% opt$doescatchsurv
          immNumRec[surveyAreas,,,i] <- immNumRec[surveyAreas,,,i]-
            tempimmC$S[surveyAreas,,]
          matNumRec[surveyAreas,,,i] <- matNumRec[surveyAreas,,,i]-
            tempmatC$S[surveyAreas,,]
        }
      immNumRec[commAreas,,,i] <- immNumRec[commAreas,,,i]-
        tempimmC$C[commAreas,,]
      matNumRec[commAreas,,,i] <- matNumRec[commAreas,,,i]-
        tempmatC$C[commAreas,,]



  ###########
  # Length update and natural mortality
      for(area in 1:opt$numofarea){
        immNumRec[area,,,i] <- t(G)%*%immNumRec[area,,,i]%*%immMort
        matNumRec[area,,,i] <- t(G)%*%matNumRec[area,,,i]%*%matMort
      }
  ###########
  # Recruits
      if(opt$doesmove!=1){
        matNumRec[,,1,i]<-matNumRec[,,1,i]+Rec[,,i]
      }
      immNumRec[,,1,i]<-immNumRec[,,1,i]+Rec[,,i]
    }

  
  sim <- list(Rec=Rec,
              immStart=immStart,
              matStart=matStart,
              immNumRec=immNumRec,
              matNumRec=matNumRec,
              immCsurv=immCsurv,
              matCsurv=matCsurv,
              immCcomm=immCcomm,
              matCcomm=matCcomm,
              Eat=Eat,
              GrowthProb=G,
              immMort=immMort,
              matMort=matMort,
              opt=opt)
  class(sim) <- c('gadget.sim',class(sim))
  return(sim)
}
