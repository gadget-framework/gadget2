
##' For each prey an upper limit needs to be set on the total amount
##' consumed by all predators so as not to obtain more consumption than
##' available biomass.  Consumption is limited to 95\% (\eqn{R_M}) of the
##' available biomass. This is implemented by scaling target consumption by all
##' predators. Let \eqn{R_{prey}(l)}{R_prey(l)} be the Ratio consumed and
##' \eqn{R_M} be the maximum ratio consumed then
##' \deqn{R_{prey}(l)=\frac{\sum_{pred}\sum_{L}C_{pred,prey}(L,l)}{N_{prey}(l)W_{prey}(l)}}
##' If \eqn{R_{prey}(l)>R_M}{R_prey(l)>R_M} consumption is adjusted as follows
##' \deqn{C_{pred,prey}(L,l)=R_MN_{prey}(l)W_{prey}(l)\frac{C_{pred,prey}(L,l)}{\sum_{pred}C_{pred,prey}(L,l)}}
##' @title Adjust for overconsumption 
##' @param C is the commercial catch of prey
##' @param S is the survey catch of prey
##' @param E is the consumption of prey by predator
##' @param N is the total number of prey
##' @param opt gadget options list
##' @return a list with adjusted catches/consumption for C, S and E.
adjustconsumption <- function(C,
                              S=NULL,
                              E=NULL,
                              N,
                              maxratioconsumed,
                              numofareas)
{
  if(is.null(S))
    S <- array(0,dim(C))
  if(is.null(E))
    E <- array(0,dim(C))
  ## Nasty hack
  if(numofareas==1){
    dim(C) <- c(1,dim(C))
    dim(S) <- c(1,dim(S))
    dim(E) <- c(1,dim(E))
    dim(N) <- c(1,dim(N))
  }
  ratio <- apply(C+S+E,c(1,2),sum)/apply(N,c(1,2),sum)
  ratio <- ifelse(is.infinite(ratio)|is.nan(ratio),0,ratio)
  index <- ratio > maxratioconsumed
  if(sum(index)>0)
    print("Warning - understocking has occured")
  index2 <- array(index,c(dim(index),dim(C)[3]))
  C[index2] <- (maxratioconsumed/ratio[index])*C[index2]
  S[index2] <- (maxratioconsumed/ratio[index])*S[index2]
  E[index2] <- (maxratioconsumed/ratio[index])*E[index2]
  return(list(C=C,S=S,E=E))
}

##' Catch is implemented to be similar to the 'Linearfleet' in Gadget. 
##' Let \eqn{C_{fleet,prey}(l,a,t)} be the number of age \eqn{a} prey, in
##' lengthgroup \eqn{l} caught at timestep \eqn{t}, then
##' \deqn{C_{fleet,prey}(l,a,t) = F_{l,t}N_{prey}(l,a,t)\Delta t}
##' with \eqn{F_{l,t} = S_{l}F_{y}} where \eqn{F_y} is constant for each year, 
##' \deqn{S_{l} = \frac{1}{1+e^{-\alpha-\beta l}}}
##' is the suitability function and \eqn{\alpha} and \eqn{\beta} are constants. ##' @title Fleet catches 
##' @param N number of prey
##' @param timestep the timestep of the catch
##' @param Fy fishing yield
##' @param salpha suitability constant for the fleet
##' @param sbeta suitability constant for the fleet
##' @param numperyear number of catch timesteps
##' @param numobs number of observation years
##' @return Total catches of the fleet
catch <- function(N,
                  timestep,
                  Fy,
                  salpha,
                  sbeta,
                  numperyear,
                  numobs,
                  l)
{
  #The suitability for the catch
  temp<-suitability(salpha,sbeta,0,1,l)
  Sl<-temp[1,]
  Fy <- rep(Fy/numperyear,each=numobs)
  
  #Proportion caught each year
  
  Fly<-Sl*Fy[timestep]
  if(length(dimnames(N)$area)>0)
    Fly <- rep(Fly,each=dim(N)[1]) 
  C<-Fly*N 
  
  return(C)
}

whaleCatch <- function(N,NTagged,quota,salpha,sbeta){
  MaleS <- suitability(c(salpha['Male'],sbeta['Male'],
                       0,1),as.numeric(dimnames(N)$age))[1,]
  FemaleS <- suitability(c(salpha['Female'],sbeta['Female'],
                         0,1),as.numeric(dimnames(N)$age))[1,]
  sl <- cbind(MaleS,FemaleS)
  F <- min(1,quota/sum(sl*t(apply((N+NTagged),2:3,sum))))
  #Proportion caught each year
  
  Fly<-F*sl
#  if(length(dimnames(N)$stock)>0)
#    Fly <- rep(Fly,each=dim(N)[1])
#  else
#    Fly <- rep(Fly,each=2)
  C <- aaply(N,1,function(x) x*t(Fly))[dimnames(N)$stock,
                                       dimnames(N)$gender,
                                       dimnames(N)$age]
  CT <- aaply(NTagged,1,function(x) x*t(Fly))[dimnames(N)$stock,
                                       dimnames(N)$gender,
                                       dimnames(N)$age]
  
  return(list(C=C,CT=CT))
}

##' This is a helper function for the firststep function. This defines the
##' length distribution for each age group
##' @title Length distribution
##' @param mu mean length for all ages
##' @param sigma standart deviation of length for all ages
##' @param l lengthgroups
##' @return a matrix of dimension length(mu) X (length(l)-1)
distr <- function(mu,sigma,l) {
  fi <- (pnorm(rep(l[-1],each=length(sigma)),mu,sigma)-
         pnorm(rep(l[-length(l)],each=length(sigma)),mu,sigma))
  dim(fi) <- c(length(sigma),length(l)-1)
  fi[,1] <- pnorm(rep(l[2],length(sigma)),mu,sigma)
  fi[,length(l)-1] <- (1-pnorm(rep(l[length(l)-1],
                                   length(sigma)),mu,sigma))

  return(t(fi))
}


##' The following variables are used in the consumption calculations:
##' l is Lengthgroup in prey
##' L is Lengthgroup in predator
##' A is Areasize
##' H The density (biomass per area unit) of available food at
##' which the predator can consume half maximum consumption
##' \eqn{\Delta t} as Length of timestep
##' \eqn{M_{pred}(L)} as Maximum consumption
##' \eqn{\psi_{pred}(L)} as Fraction of \eqn{M_{pred}} consumed
##' N_{pred}(L) as Number of predator \eqn{pred} in lengthgroup $L
##' N_{prey}(l) as Number of prey \eqn{prey} in lengthgroup l
##' W_{prey}(l) as The mean weight of prey of length l
##' S_{pred,prey}(L,l) as Suitability of prey at length l for pred at length L
##' C_{prey,pred}(L,L) as Total weight predator of length L consumes of prey of length $l$
##' The formula for the consumption is as follows:
##' \deqn{C_{pred,prey}(L,l)=N_{pred}(L)M_{pred}(L)\Psi_{pred}(L)\frac{F_{pred,prey}(L,l)}{\sum_{l,prey}F_{pred,prey}(L,l)}}
##' \deqn{=N_{pred}(L)M_{pred}(L)\frac{\sum_{l,prey}F_{pred,prey}(L,l)}{\sum_{l,prey}F_{pred,prey}(L,l)+HA}\frac{F_{pred,prey}(L,l)}{\sum_{l,prey}F_{pred,prey}(L,l)}}
##' \deqn{=N_{pred}(L)M_{pred}(L)\frac{F_{pred,prey}(L,l)}{\sum_{l,prey}F_{pred,prey}(L,l)+HA}}
##' where
##' \deqn{F_{pred,prey}(L,l) =S_{pred,prey}(L,l)N_{prey}(l)W_{prey}(l)}
##' \deqn{ M_{pred}(L) =m_0e^{(m_1T-m_2T^3)}L_{pred}^{m_4}\Delta t}
##' The suitability function for predation used in the \R model is:
##' \deqn{S_{pred,prey}(L,l) = \frac{\delta}{1+e^{-\alpha-\beta l-\gamma L}}}
##' With one predator, one prey and otherfood the equation becomes:
##' \deqn{C_{L,l}=N_{L}M_{L}\Psi_{L}\frac{F_{L,l}}{\sum_lF_{L,l}+OA}}
##' \deqn{=N_{L}M_{L}\frac{F_{L,l}}{\sum_lF_{L,l}+OA+HA}}
##' where O is the density of otherfood.
##' @title Eat
##' @param PreyInArea Number of prey items in the area
##' @param PredInArea Number of predator items in the area
##' @param step the timestep, that is the time of the year
##' @param opt gadget options list
##' @return The total unadjusted consumption of the predator on the prey
eat <- function(PreyInArea,PredInArea,step,opt){  
  preydim <- dim(PreyInArea)
  preddim <- dim(PredInArea)
  if(preydim[2]!=preddim[2]){
    print("Error - the number of lengthgroups is not the same for predation")
  }else{
    numoflgroups <- preydim[2]
  }
  ## The amount eaten
  Eat<-array(0,preydim[1:3])
  ## The suitability for the predation
  Spred<-suitability(opt$spalpha,opt$spbeta,opt$spgamma,opt$spdelta,opt$lt,opt$lt)
  ## Food = S(L,l)*N_l
  Food<-array(0,c(numoflgroups,opt$numoflgroups,preydim[3]))
  
  for(area in 1:opt$numofareas){
    Prey <- PreyInArea[area,,,step]
    Pred <- PredInArea[area,,,step]
  # F_{Llat}
    for(i in 1:preydim[3])
      Food[,,i]<-t(Spred)*Prey[,i]*opt$w
  # loop through predators lengths
    predsum <- apply(Pred,1,sum)*opt$maxConsumption
    foodsum <- apply(Food,2,sum)
    other <- opt$H*opt$lengthoftimesteps+opt$otherfood[step]*opt$otherfrac
    for(j in 1:numoflgroups){
      Eat[area,,] <- Eat[area,,] +
        predsum[j]*Food[,j,]/(foodsum[j] + other*opt$areasize)
    }

    Eat[area,,] <- Eat[area,,]/opt$w
  }
  return(Eat)
}


##' The simulation is started with an initial population, the age of which
##' ranges from the minimum age of the immature stock to the maximum age
##' of the mature stock. This population is calculated as follows:
##' Let \eqn{n_{a}} be the number of age \eqn{a} individuals in the first
##' timestep, \eqn{\mu_{a}} the mean length at age \eqn{a} and \eqn{\sigma_{a}}
##' the standard deviation of length at age \eqn{a}. For the minimum age
##' (representing recruitment) see the recruitment function. For a given
##' constant mortality \eqn{Z_{0}} we get
##' \deqn{n_{a} = n_{a-1}e^{-Z_{0}}}
##' to calculate the number at age for all \eqn{a}.
##' The number in lengthgroup $i$ at age $a$ in timestep 1 can then be
##' calculated from:
##' \deqn{N_{i,a,1} = n_a\left(\Phi\left(\frac{l_{i+1}-\mu_a}{\sigma_a}\right)-\Phi\left(\frac{l_{i}-\mu_a}{\sigma_a}\right)\right)}{N_{i,a,1} = n_a (Phi(l_{i+1}-mu_a)/sigma_a)-Phi(l_{i}-\mu_a)/sigma_a))}
##' where \eqn{l_{i}} and \eqn{l_{i+1}} are the endpoints of lengthgroup \eqn{i},
##' \eqn{N_{l,a,t}} is the number at age \eqn{a} in lengthgroup \eqn{l} at
##' timestep \eqn{t} and \eqn{\Phi}{Phi} is the probability function for Normal
##' distribution. 
##' NB: in Gadget (which is programmed in C++) the value of \eqn{\Phi}{Phi} is
##' approximated whereas R uses integration. To ensure
##' compatibility between the models, the initial population for Gadget is
##' entered directly from the initial population file rather than
##' calculated from a Normal distribution. While this is an option
##' within Gadget, it is not the standard method.
##' @title The first timestep
##' @param n Number of age 1 individuals
##' @param mu Vector of mean length per age.
##' @param sigma Vector of standard deviation of length per age.
##' @param l Vector of length groups
##' @param z Natural mortality per age
##' @param numofareas Number of areas
##' @param probarea Vector of proportions living in an area. 
##' @param minage Minimum age of the species.
##' @param maxage Maximum age of the species.
##' @return matrix with initial distribution
firststep <- function(n,
                      mu,
                      sigma,
                      l,
                      z,
                      numofareas,
                      probarea,
                      minage,
                      maxage
                      ) {
  minlen <- min(l)
  maxlen <- max(l)
  numoflgroups <- length(l)-1
  num.agegroup <- n[1]*exp(-(minage:maxage-1)*z)
  
  if((maxage - minage +1)!=length(sigma)) {
    stop("Error - the number of age groups for sigma doesnt match the maximum age")
   # return('Error')
  }
    
  temp <- distr(mu,sigma,l)*rep(num.agegroup,each=numoflgroups)
#  if(minage>1)
#    temp[,1:minage] <- 0 
  # assign the distribution to areas according to the probability of being in
  # that area
  if(length(probarea)!=numofareas){
    stop("Error - the area probabilities do not match the number of areas")
  }
  initial.distribution <- array(rep(temp,each=numofareas),
                                c(numofareas,dim(temp)))*probarea
  dimnames(initial.distribution) <- list(area=1:numofareas,
                                         length=minlen:(maxlen-1),
                                         age=minage:maxage
                                         )
  
  return(initial.distribution)
}



##' Growth is according to a von Bertalanffy equation 
##' \deqn{\mu_{a} = L_{\infty}(1-e^{-\kappa a})}
##' with the lengthvbsimple growth function from Gadget implemented.
##' For a fish of age a and length l, mean length growth \eqn{\Delta L} is
##' then calculated as:
##' \deqn{\Delta L =L_{\infty}(1 - \frac{l}{L_{\infty}})(1 - e^{-\kappa \Delta t})}\deqn{Delta L =L_{infty}(1 - l/L_{infty})(1 - e^{-kappa Delta t})}
##' The length distribution is updated using the beta-binomial
##' distribution, ie the probability of growing x lengthgroups, given
##' maximum lengthgroupgrowth n, is
##' \deqn{P[X = x] =  \frac{\Gamma(n+1)}{\Gamma(n-x+1)\Gamma(x+1)} \frac{\Gamma(\alpha + \beta)}{\Gamma(n+\alpha+\beta)} \frac{\Gamma(n-x+\beta)}{\Gamma(\beta)} \frac{\Gamma(x+a)}{\Gamma(\alpha)}}
##' with \eqn{\alpha = \frac{\beta\Delta L}{n-\Delta L}} to preserve the mean
##' lengthgrowth according to the equation equation above. NB: the
##' expected value of \eqn{\Delta L} should be taken into consideration when
##' fixing n. 
##' Let \eqn{G = [g_{ij}]} be the length updating matrix where \eqn{g_{ij}} is the
##' probability of growing from lengthgroup i to lengthgroup j
##' obtained from the equation above.
##' \deqn{N_{l,a+1,t+\Delta t} = \sum_{l'\leq l}g_{l'l}N_{l,a,t}}
##' with \eqn{N_{l,a,t}} as described for the initial population for a >
##' min a .
##' @title Growth probability
##' @param lt Vector of midpoints for the length groups.
##' @param beta Beta for the Von Bertanlanffy curve 
##' @param lsup \eqn{l_\infty}{l_infty} for the Von Bertanlanffy.
##' @param k \eqn{\kappa}{kappa} for the Von Bertanlanffy.
##' @param dt Length of the time interval.
##' @param lengthgrouplen length of the lengthgroups.
##' @param binn is the maximum updating length.
##' @return a matrix where the index (j,i) repsents the probability of going
##' lengthgroup i to lengthgroup j
growthprob <-function(lt,
                      beta,
                      lsup,
                      k,
                      dt,
                      lengthgrouplen,
                      binn)
{
  
  prob <- function(alpha,beta,x){
    na <- length(alpha)
    n <- length(x) - 1
    alpha <- rep(alpha,n + 1)
    x <- rep(x,each=na)
    ## Create a probability matrix where the columns represent the
    ## probability of growing x lengthgroups for each lengthgroup
    ## length group jumps are distributed according to a beta-binomial
    ## distribution
    val <- exp(lgamma(n + 1)+
               lgamma(alpha + beta) +
               lgamma(n - x + beta) +
               lgamma(x + alpha) -
               lgamma(n - x + 1) -
               lgamma(x + 1) -
               lgamma(n + alpha + beta) -
               lgamma(beta) -
               lgamma(alpha))
    dim(val) <- c(na,n + 1)
    growth.matrix <- array(0,c(na,na))
    for(lg in 1:na){
      if(lg == na){
        growth.matrix[na,na] <- 1
      } else if(lg + n > na){
        growth.matrix[lg,lg:(na-1)] <- val[lg,1:(na - lg )]
        growth.matrix[lg,na] <- sum(val[lg,(na - lg + 1):(n + 1)])
      } else {
        growth.matrix[lg,lg:(n + lg)] <- val[lg,]
      }
    }
    return(growth.matrix)
  }
  ## dmu[i] is the mean growth in lengthgroup i 
  dmu <- lsup*(1-lt/lsup)*(1-exp(-k*dt))

  ## if growth>maxgrowth assume that growth is a bit smaller than maxgrowth
  dmu[dmu/lengthgrouplen >= binn] <- binn-0.1

  ## if growth is negative assume no growth
  dmu[dmu < 0] <- 0
  
  alpha <- (beta*dmu/lengthgrouplen)/(binn-dmu/lengthgrouplen)
  ## possible length growth
  length.growth <- 0:binn
  
  return(prob(alpha,beta,length.growth))
}

##' If more than one area is defined the user can define migration
##' between the areas. The migration procedure is the same as in MULTSPEC. 
##' The migration is defined and takes place in every timestep and it is
##' assumed to be constant for all years. This means that the porportion that
##' migrates from area i to j can be different between different timesteps
##' (seasons) but they are fixed between observations (years). Migration at
##' timestep t is defined by the user as an \eqn{n\times n}{nxn} transition
##' matrix P_t := [p_{ij}] where p_{ij} is the proportion moving from area j
##' to area i, ignoring other processes. For P to be a transition matrix
##' \eqn{\sum_ip_{ij} = 1}{sum_i p_ij = 1}, for each j. The vector of abundance
##' for all areas at time t is therefore:
##' \deqn{\mathbf{N_t} = P_t \mathbf{N_{t-1}}}{N_t = P_t N_{t-1}}
##' In a two area example this would mean that if N_{1,t-1} is a matrix
##' containing the abundance in numbers in area 1 at timestep t before
##' migration and N_{2,t-1} is the same number for area 2, the numbers after
##' migration will be
##' \deqn{N_{1,t} = p_{11}\cdot N_{1,t-1} + p_{12}\cdot N_{2,t-1} }
##' \deqn{N_{2,t} = p_{21}\cdot N_{2,t-1}+ p_{22}\cdot N_{2,t-1} }
##' @title Migrate
##' @param N An array containing substock abundance by area (first dimension) and other variables
##' @param M an array with migration matricies
##' @return the migrated substock abundance array
##' @author Bjarki Þór Elvarsson
migrate <- function(N,M){
  numofareas <- dim(N)[1]
  for(area in 1:numofareas){
    N[area,,] <- apply(M[area,]*N,c(2,3),sum)
  }
  return(N)
}


##' The timestep (or timesteps) on which recruitment takes place is
##' defined by the user. 
##' Given \eqn{n_{t}} recruits, at timestep t, with mean length
##' \eqn{\mu}{mu} and
##' standard deviation of length \eqn{\sigma}{sigma}, the number of recruits in
##' lengthgroup i is calculated by:
##' \deqn{N_{i,1,t} = n_t(\Phi(\frac{l_{i+1}-\mu}{\sigma})-\Phi(\frac{l_{i}-\mu}{\sigma}))}
##' As for the initial population, the number of recruits in each length
##' groups is given in the recruit input file. 
##' @title Recruitment
##' @param n Number of recruited individuals
##' @param mu Vector of mean length per age.
##' @param sigma Vector of standard deviation of length per age.
##' @param l Vector of length groups
##' @param numofareas Number of areas
##' @param probarea Vector of proportions living in an area. 
##' @param numobs Number of years.
##' @param numoftimesteps Number of observations per year.
##' @return Recruits by area, length, time
recruits <- function(n,mu,sigma,
                     l,numofareas,probarea,
                     numobs,numoftimesteps)
{
  
  Temp <- distr(mu,sigma,l)%*%t(n)
  rec <- array(rep(Temp,each=2),c(numofareas,dim(Temp)))*probarea
  dimnames(rec) <- list(area = 1:numofareas,
                        length = min(l):(max(l)-1),
                        time=paste(sprintf('Year_%s',rep(1:numobs,
                                each=numoftimesteps)
                                ),
                                sprintf('Step_%s',rep(1:numoftimesteps,
                                                      numobs)),
                                sep='_'))
  return(rec)
}



##' The suitability function for predation used in the R model is:
##' \deqn{S_{pred,prey}(L,l) = \frac{\delta}{1+e^{-\alpha-\beta l-\gamma L}}}{S_{pred,prey}(L,l) = \frac{delta}{1+e^{-alpha-beta l-gamma L}}}
##' With one predator, one prey and otherfood the equation becomes:
##' \deqn{C_{L,l}=N_{L}M_{L}\Psi_{L}\frac{F_{L,l}}{\sum_lF_{L,l}+OA}}
##' \deqn{=N_{L}M_{L}\frac{F_{L,l}}{\sum_lF_{L,l}+OA+HA}}
##' where $O$ is the density of otherfood.
##' @title Prey suitability
##' @param salpha \eqn{\alpha}{alpha} for the suitability function.
##' @param sbeta \eqn{\beta}{beta} for the suitability function.
##' @param sgamma \eqn{\gamma}{gamma} for the suitability function.
##' @param sdelta \eqn{\delta}{delta} for the suitability function.
##' @param l prey length group(s)
##' @param L predator length group(s)
##' @return matrix of suitabilities, columns prey length, lines predator length
suitability <- function(params,
                        l,
                        L=c(0),
                        type = 'exponential',
                        to.data.frame = FALSE,
                        normalize = FALSE)
{

  if(tolower(type) == 'andersenfleet'){
    type <- 'andersen'
    L <- params[6]
  }
  
  if(tolower(type) == 'constant'){
    S <- array(params[1],c(length(L),length(l)))

  } else if(tolower(type) == 'straightline') {
    S <- array(rep(l*params[1] + params[2],each = length(L)),
               c(length(L),length(l)))

  } else if(tolower(type) == 'exponential'){
    S <- array(params[4]/(1+exp(-(params[1]+params[2]*rep(l,each=length(L))+
                               params[3]*rep(L,length(l))))),
               c(length(L),length(l)))
    
  } else if(tolower(type) == 'exponentiall50'){
    S <- array(rep(1/(1+exp(-params[1]*(l - params[2]))),each = length(L)),
               c(length(L),length(l)))

  } else if(tolower(type) == 'richards') {
    S <- array(params[4]/(1+exp(-(params[1]+params[2]*rep(l,each=length(L))+
                                  params[3]*rep(L,length(l))))),
               c(length(L),length(l)))^(1/params[5])

  } else if(tolower(type) == 'andersen') {
    l.tmp <- rep(l,each=length(L))
    L.tmp <- rep(L,length(l))
    if(L==0)
      L.tmp <- median(l.tmp)
    
    S <- array(params[1] + params[3]*
               ifelse(log(L.tmp/l.tmp) < params[2],
                      exp(-(log(L.tmp/l.tmp)-params[2])^2/params[5]),
                      exp(-(log(L.tmp/l.tmp)-params[2])^2/params[4])),
               c(length(L),length(l)))
    
  } else if(tolower(type) == 'gamma'){

    S <- array(rep((l/((params[1] - 1)*params[2]*params[3]))^(params[1] -1) *
                   exp(params[1] - 1 - 1/(params[2]*params[3])),
                   each = length(L)),
               c(length(L),length(l)))
  } else {
    stop(sprintf('Error in suitability -- %s not defined',type))
  }
  if(to.data.frame){
    dimnames(S) <- list(L=L,l=l)
    S <- as.data.frame.table(S,responseName = 'suit')
    if(normalize)
      S$suit <- S$suit/max(S$suit)
    S$L <- as.numeric(S$L)
    S$l <- as.numeric(S$l)
  } else {
    dimnames(S) <- list(sprintf('Pred_length_%s',L),
                        sprintf('Prey_length_%s',l))
  }
  return(S)
}


overlap <- function(Abundance,mixing){
  stock.num <- aaply(Abundance,c(1,3),
                     function(x) sum(x))[dimnames(Abundance)$stock,
                                         dimnames(Abundance)$age]
  for(stock in dimnames(Abundance)$stock)
    Abundance[stock,,] <- t(stock.num[stock,]%o%mixing[,stock])
  return(Abundance)
}

dispersion <- function(Abundance,dispersion){
  tmp <- Abundance
  Abundance <- 0*tmp
  for(from in dimnames(Abundance)$stock){
    for(to in dimnames(Abundance)$stock){
      Abundance[to,,] <- Abundance[to,,] +
        tmp[from,,]*dispersion[from,to]
    }
  }
  return(Abundance)
}

init.pop <- function(init.abund,M,maxage,probarea){
  x <- init.abund*(1-exp(-M))/(1-exp(-M*(maxage+1)))
  array(rep(x*exp(-M*(0:maxage)),
            each=length(probarea))*probarea,
        c(length(probarea),maxage+1))
}

#tag.experiment <- function(Abundance,tag.number){
#  tag.number*Abundance/sum(Abundance)
#}

Births <- function(B,N,A,z,K){
  b <- array(0,c(dim(N)[1],2,dim(N)[2]),
             dimnames=list(stock=dimnames(N)$stock,
               gender=c('Male','Female'),
               area=dimnames(N)$area))
  for(stock in dimnames(N)$stock){
    b[stock,'Male',] <- B*N[stock,]*(1+A*(1-(sum(N[stock,])/K[stock])^z))
    b[stock,'Female',] <- B*N[stock,]*(1+A*(1-(sum(N[stock,])/K[stock])^z))
  }
  return(b)
}
