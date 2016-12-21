#' Migration matrix
#'
#' Helper function for migrate
#' @param opt gadget options list
#' @return Migration array
migMat <- function(P1,P2,numofareas,numoftimesteps){
  P <- array(rbind(P1,1-P1,1-P2,P2),
             c(numofareas,numofareas,numoftimesteps))
  return(P)
}


setClass(
         Class = "Gadget.setup",
         representation = representation(
           name = "character",
           areas = "integer",
           areasize = "numeric",
           numobs = "integer",
           numoftimesteps = "integer",
           current.time.step = "integer",
           dt = "numeric",
           numofareas = "integer",
           area.temperature = "numeric",
           otherfood = "array",
           otherfrac = "numeric",
           stocknames = "character",
           fleetnames = "character",
           lengthoftimesteps = "numeric",
           H = "numeric"
           )
         #package="rgadget"
         )
setMethod(f = "initialize",
          signature = "Gadget.setup",
          definition = function(.Object, name, areas, areasize,
            numobs, numoftimesteps, 
            area.temperature, otherfood,otherfrac,
            stocknames, fleetnames,H){
            .Object@name <-  name
            .Object@areas <- as.integer(areas)
            .Object@numofareas <- as.integer(length(areas))
            .Object@numobs <- as.integer(numobs)
            .Object@numoftimesteps <- as.integer(numoftimesteps)
            .Object@area.temperature <- area.temperature
            .Object@current.time.step <- as.integer(0)
            .Object@otherfood <-
              array(rep(otherfood,
                        numobs*numoftimesteps*.Object@numofareas),
                    c(.Object@numofareas,numobs*numoftimesteps))
            .Object@otherfrac <- otherfrac
            .Object@stocknames <- stocknames
            .Object@fleetnames <- fleetnames
            .Object@dt <- 1/numoftimesteps
            .Object@lengthoftimesteps <- 12/numoftimesteps
            .Object@H <- H
            if(length(areasize) != .Object@numofareas){
              warning("Gadget.setup - length(areasize)!=numofares")
              .Object@areasize <- rep(areasize[1],.Object@numofareas)
            } else { 
              .Object@areasize <- areasize
            }
            return(.Object)
          }
          
          )


setClass(
         Class = "Fleet",
         representation = representation(
           name = "character",  
           timesteps = "integer",
           suitability = "data.frame",
           fleettype = "character",
           fy = "numeric",
           livesonareas = 'numeric'
           ),
         #package="rgadget"
         )

setMethod(
          f = "initialize",
          signature = "Fleet",
          definition = function(.Object,
            name,
            timesteps,
            fleettype,
            fy,
            livesonareas,
            stocks.caught,
            alpha,
            beta){
            .Object@name <- name
            .Object@timesteps <- as.integer(timesteps)
            .Object@fleettype <- fleettype
            .Object@fy <- fy
            .Object@livesonareas <- as.integer(livesonareas)
            tmp <- data.frame(stocks=stocks.caught,
                              alpha=alpha,
                              beta=beta,
                              suitability='exponential')
            .Object@suitability <- tmp
            return(.Object)
          }
          )
##' S4 class Stock
##'
##' 
setClass(
         Class = 'Stock',
         representation=representation(
           name = "character",
           livesonareas = "integer",
           probarea = "numeric",
           minage = "integer",
           maxage = "integer",
           minlength = "integer",
           maxlength = "integer",
           l = "integer",
           lt = "numeric",
           lengthgrouplen = "integer",
           numoflgroups = "integer",
           weight  = "numeric",
           mu = "numeric",
           sigma = "numeric",
           ## migration
           doesmigrate = "integer",
           migrationP = "array",
           doesfuncmigrate = "integer",
           diffusion = "numeric",
           driftx = "numeric",
           lambda = "numeric",
           ## recruitment
           doesmove = "integer",
           movesTo = "character",
           doesrenew = "integer",
           recruitment.step = "integer",
           BH.R_0 = "numeric", ## Beverton Holt recruitment
           n = "integer",
           ## growth parameters
           doesgrow = "integer",
           growthfunction = "character",
           #lsup = "numeric",
           #k = "numeric",
           #binn = "integer",
           #beta = "numeric",
           G = "array",
           ## consumption
           doeseat = "integer",
           suitability = "data.frame",
           ## maximum consumption
           #m0 = "numeric",
           #m3 = "numeric",
           #H = "numeric",
           maxratioconsumed = "numeric",
           maxConsumption = "numeric",
           ## number of indivuals consumed and alive
           Eat = "array",
           consumed = "array",
           stock = "array",
           ## natural mortality
           z = "numeric",
           M = "array"
           ),
                                        #package="rgadget"
         )
setMethod(
          f = "initialize",
          signature = "Stock",
          definition = function(.Object,
            name,
            livesonareas,
            ## other setup
            opt,
            probarea = 1,
            minage = 1,
            maxage = 10,
            minlength = 5,
            maxlength = 90,
            lengthgrouplen = 1,
            ## migration
            doesmigrate = 1,
            migrationP = migMat(c(1,0.6,0.6,1),c(0.6,1,1,0.6),2,4),
            doesfuncmigrate = 0,
            diffusion = 0,
            driftx = 0,
            lambda = 0,
            ## recruitment
            doesmove = 0,
            movesTo = "",
            doesrenew = 1,
            recruitment.step = 1,
            n = 1e6,
            ## growth parameters
            doesgrow = 1,
            growthfunction = 'lengthvbsimple',
            lsup = 115,
            k = 0.09,
            binn = 15,
            beta = 200,
            recruitment.length = NULL,
            sigma = c(2.2472, 2.8982, 4.0705, 4.9276,
              5.5404, 5.8072, 6.0233, 8, 9, 9),
            ## consumption
            doeseat = 0,
            stocks.eaten = '',
            s.alpha = 0,
            s.beta = 0,
            s.delta = 0,
            s.gamma = 0,
            ## maximum consumption
            m0 = 10^(-2),
            m3 = 3,
            H = 4000,
            maxratioconsumed = 0.95,
            ## weight parameters
            a = 10^(-5),
            b = 3,
            ## natural mortality
            z=0.2
            ){
            .Object@name <- name
            .Object@livesonareas <- livesonareas
            if(length(probarea) < length(livesonareas)){
              warning('length(probarea) < length(livesonareas) - even distribution assumed')
              probarea <- rep(1,length(livesonareas))/length(livesonareas)
              }
            .Object@probarea <- probarea
            .Object@minage <- as.integer(minage)
            .Object@maxage <- as.integer(maxage)
            .Object@minlength <- as.integer(minlength)
            .Object@maxlength <- as.integer(maxlength)
            .Object@lengthgrouplen <- as.integer(lengthgrouplen)
            .Object@numoflgroups <- length(.Object@l)
            .Object@doesmigrate <- as.integer(doesmigrate)
            .Object@doesfuncmigrate <- as.integer(doesfuncmigrate)
            .Object@diffusion <- diffusion
            .Object@driftx <- driftx
            .Object@lambda <- lambda
            .Object@migrationP <- migrationP
            .Object@doesmove <- as.integer(doesmove)
            .Object@movesTo <- movesTo
            .Object@doesrenew <- as.integer(doesrenew)
            .Object@recruitment.step <- as.integer(recruitment.step)
            .Object@BH.R_0 <- (1+n/exp(-2*z))
            .Object@n <- as.integer(n)
            .Object@doesgrow <- as.integer(doesgrow)
            .Object@growthfunction <- growthfunction
            .Object@doeseat <- as.integer(doeseat)
            tmp <- data.frame(stocks=stocks.eaten,
                              s.alpha=s.alpha,
                              s.beta=s.beta,
                              s.gamma=s.gamma,
                              s.delta=s.delta,
                              suitability='exponential')
            .Object@suitability <- tmp
            .Object@z <- z # c(rep(z,maxage-1),0.5)
            if(doesmove == 0)
              mort <- c(rep(z,maxage-minage),0.5)
            else
              mort <- rep(z,maxage - minage + 1)
            .Object@M <- diag(exp(-mort*opt@dt))
            .Object@l <-  as.integer(seq(minlength,maxlength,lengthgrouplen))
            .Object@lt <- (.Object@l[2:length(.Object@l)]+
                           .Object@l[1:(length(.Object@l)-1)])/2
            .Object@numoflgroups <- as.integer(length(.Object@l) - 1)
            .Object@maxConsumption <- m0*.Object@lt^m3*12*opt@dt
            .Object@maxratioconsumed <- maxratioconsumed
            .Object@weight <- a*.Object@lt^b
            ## mu[i] mean length at age i
            .Object@mu <- lsup*(1-exp(-k*minage:maxage))
            if(!is.null(recruitment.length))
              .Object@mu[1] <- recruitment.length
            .Object@sigma <- sigma
            .Object@G <- growthprob(.Object@lt,
                                    beta,
                                    lsup,
                                    k,
                                    opt@dt,
                                    .Object@lengthgrouplen,
                                    binn)
            ## stock arrays
            

            .Object@stock <- array(0,c(opt@numofareas,
                                       .Object@numoflgroups,
                                       maxage - minage +1,
                                       (opt@numobs*opt@numoftimesteps)))
            dimnames(.Object@stock) <-
              list(area=opt@areas,
                   length=.Object@l[-1],
                   age=minage:maxage,
                   time=paste(sprintf('Year_%s',rep(1:opt@numobs,
                     each=opt@numoftimesteps)
                     ),
                     sprintf('Step_%s',rep(1:opt@numoftimesteps,
                                           opt@numobs)),
                     sep='_'))
            .Object@consumed <- array(0,c(opt@numofareas,
                                          .Object@numoflgroups,
                                          maxage - minage +1,
                                          (opt@numobs*opt@numoftimesteps),
                                          length(opt@fleetnames)))
            dimnames(.Object@consumed) <-
              list(area=opt@areas,
                   length=.Object@l[-1],
                   age=minage:maxage,
                   time=paste(sprintf('Year_%s',rep(1:opt@numobs,
                     each=opt@numoftimesteps)
                     ),
                     sprintf('Step_%s',rep(1:opt@numoftimesteps,
                                           opt@numobs)),
                     sep='_'),
                   fleet=opt@fleetnames)
            .Object@Eat <- .Object@stock
            return(.Object)
          }
          
          )

##' Initial Population
##'
##' Initial population for class Stock.
##' @param .Object a Stock object
##' @param n Initial number of recruits
##' @return A stock object with updated initial population (at time 0)
setGeneric('firststep.Stock',
           function(.Object){standardGeneric('firststep.Stock')}
           )
setMethod(f = "firststep.Stock",
          signature = "Stock",
          definition = function(.Object){
            start <- firststep(.Object@n,
                               .Object@mu,
                               .Object@sigma,
                               .Object@l,
                               .Object@z,
                               length(.Object@livesonareas),
                               .Object@probarea,
                               .Object@minage,
                               .Object@maxage
                      )
            if(length(.Object@livesonareas)==1){
              ## ugly hack because of destroyed array dimension
              dimnames(start) -> tmp
              dim(start) <- c(1,dim(start))
              dimnames(start) <- list(area=1,length=tmp[[1]],age=tmp[[2]])
            }
            if(.Object@doesmove == 1){
              .Object@stock[.Object@livesonareas,,-1,1] <-
                start[,,as.character((.Object@minage+1):.Object@maxage)]
            } else { 
              .Object@stock[.Object@livesonareas,,,1] <-
                start[,,as.character(.Object@minage:.Object@maxage)]
            }
            return(.Object)
          }
          )


setGeneric('as.data.frame.Stock',
           function(.Object){standardGeneric('as.data.frame.Stock')}
           )
setMethod("as.data.frame.Stock",
          signature = "Stock",
          function(.Object){
            stock.table <- as.data.frame.table(.Object@stock,
                                               stringsAsFactors=FALSE)
            names(stock.table)[length(names(stock.table))] <- 'Num.indiv'
            catch <- as.data.frame.table(.Object@consumed,
                                         stringsAsFactors=FALSE)
            names(catch)[length(names(catch))] <- 'Catch'
            catch <- reshape(catch,timevar='fleet',
                             direction='wide',
                             idvar=names(catch)[1:4])
            stock.table <- merge(stock.table,catch,all=TRUE)
            stock.table$year <- sapply(strsplit(stock.table$time,'_'),
                                       function(x) as.numeric(x[2]))
            stock.table$step <- sapply(strsplit(stock.table$time,'_'),
                                       function(x) as.numeric(x[4]))
            stock.table$length <- as.numeric(stock.table$length)
            stock.table$age <- as.numeric(stock.table$age)
            stock.table$weight <- .Object@weight[stock.table$length]  
            return(stock.table)
          }
)

setGeneric('get.Stock',
           function(.Object,year,step){standardGeneric('get.Stock')}
           )
setMethod(f = 'get.Stock',
          signature = 'Stock',
          function(.Object,year,step){
            query <- sprintf('Year_%s_Step_%s',year,step)
            return(.Object@stock[,,,query])
          }
          )

setGeneric('recruits.Stock',
           function(.Object,year,step){standardGeneric('recruits.Stock')}
           )
setMethod(f = "recruits.Stock",
          signature = "Stock",
          definition = function(.Object,year,step){
            ## n_{t+1} = \frac{R_0 n_t}{1+ n_t/M}
            tmp <- get.Stock(.Object,year,step)
            n <- apply(tmp,3,sum)[2]
            n <- .Object@BH.R_0*n/(1+n/exp(-.Object@z))
            #rec <- recruits(n,.Object@mu,.Object@sigma,.Object@l,
            #                length(.Object@l),.Object@probarea)
            Temp <- distr(.Object@mu[1],.Object@sigma[1],.Object@l)*n
            numofareas <- length(.Object@livesonareas)
            query <- sprintf('Year_%s_Step_%s',year,step)
            .Object@stock[,,as.character(.Object@minage),query] <-
              array(rep(Temp,each=numofareas),
                    c(numofareas,dim(Temp)))*.Object@probarea
            return(.Object)
          }
          )

setGeneric('nextstep.Stock',
           function(.Object,i){standardGeneric('nextstep.Stock')}
           )
setMethod(f = 'nextstep.Stock',
          signature = "Stock",
          definition = function(.Object,i){
            .Object@stock[,,,i] <- .Object@stock[,,,i-1]
            return(.Object)
          }
         )

setGeneric('updateAge.Stock',
           function(.Object,i){standardGeneric('updateAge.Stock')}
           )
setMethod(f = 'updateAge.Stock',
          signature = "Stock",
          definition = function(.Object,i){
            minage.res <- !(.Object@minage:.Object@maxage %in% .Object@minage)
            maxage.res <- !(.Object@minage:.Object@maxage %in% .Object@maxage)
            .Object@stock[,,minage.res,i] <-
              .Object@stock[,,maxage.res,i-1]
            if(.Object@doesmove!=1){
              .Object@stock[,,as.character(.Object@maxage),i] <-
                .Object@stock[,,as.character(.Object@maxage),i] +
                  .Object@stock[,,as.character(.Object@maxage),i-1]
            }
            return(.Object)
          }
          )
setGeneric('move.Stock',
           function(mat,imm,i){standardGeneric('move.Stock')}
           )
setMethod(f = 'move.Stock',
          signature = c("Stock","Stock"),
          definition = function(mat,imm,i){
            mat@stock[,,as.character(mat@minage),i] <-
              mat@stock[,,as.character(mat@minage),i] +
                imm@stock[,,as.character(imm@maxage),i-1]
            return(mat)
          }
          )


setGeneric('migrate.Stock',
           function(.Object,num,i){standardGeneric('migrate.Stock')}
           )
setMethod(f = 'migrate.Stock',
          signature = "Stock",
          definition = function(.Object,num,i){
            mig <- .Object@migrationP[,,num]
            tmp <- 0*.Object@stock[,,,i]
            for(area1 in .Object@livesonareas){
              for(area2 in .Object@livesonareas){
                tmp[area1,,] <- tmp[area1,,] +
                  mig[area1,area2]*.Object@stock[area2,,,i]
              }
            }
            .Object@stock[,,,i] <- tmp
            return(.Object)
          }
          )


setGeneric('eat.Stock',
           function(Prey,Pred,opt,step){standardGeneric('eat.Stock')}
           )
setMethod(f = 'eat.Stock',
          signature = "Stock",
          definition = function(Prey,Pred,opt,step){
            Eat <- array(0,c(opt@numofareas,Prey@numoflgroups,
                             Prey@maxage - Prey@minage +1))
            tmp <- Pred@suitability[Pred@suitability$stocks==Prey@name,] 
            Spred<-suitability(tmp$s.alpha,tmp$s.beta,tmp$s.gamma,
                              tmp$s.delta,Prey@lt,Pred@lt)
            ## Food = S(L,l)*N_l
            Food<-array(0,c(Prey@numoflgroups,Pred@numoflgroups,
                            Prey@maxage - Prey@minage +1))
            for(area in Prey@livesonareas){
                  prey <- Prey@stock[area,,,step]
                  pred <- Pred@stock[area,,,step]
  # F_{Llat}
                  for(i in 1:(Prey@maxage - Prey@minage +1))
                    Food[,,i]<-t(Spred)*prey[,i]*Prey@weight
  # loop through predators lengths
                  predsum <- apply(pred,1,sum)*Pred@maxConsumption
                  foodsum <- apply(Food,2,sum)
                  other <- opt@H*opt@lengthoftimesteps +
                    opt@otherfood[step]*opt@otherfrac
                  for(j in 1:Prey@numoflgroups){
                    Eat[area,,] <- Eat[area,,] +
                      predsum[j]*Food[,j,]/(foodsum[j] +
                                            other*opt@areasize[area])
                  }
                  Eat[area,,] <- Eat[area,,]/Prey@weight
                }
            Prey@Eat[,,,step] <- Prey@Eat[,,,step] + Eat
            return(Prey)
          }
          
          )
setGeneric('catch.Stock',
           function(Fleet,Prey,opt,num,step){standardGeneric('catch.Stock')}
           )
setMethod(f = 'catch.Stock',
          signature = c("Fleet","Stock"),
          definition = function(Fleet,Prey,opt,num,step){
            tmp <- Fleet@suitability[Fleet@suitability$stock == Prey@name,]
            mutual.areas <-
              Fleet@livesonareas[Fleet@livesonareas %in% Prey@livesonareas]
            
            Prey@consumed[mutual.areas,,,step,Fleet@name] <-
              catch(Prey@stock[mutual.areas,,,step],
                    num,
                    Fleet@fy,
                    tmp$alpha,
                    tmp$beta,
                    length(Fleet@timesteps),
                    opt@numobs,
                    Prey@lt)
            
            return(Prey)
          }
          )
setGeneric('adjust.update.con.Stock',
           function(.Object,opt,i){standardGeneric('adjust.update.con.Stock')}
           )
setMethod(f = 'adjust.update.con.Stock',
          signature = "Stock",
          definition = function(.Object,opt,i){
            catch <- apply(.Object@consumed,1:4,sum)
            tmp <- adjustconsumption(C = catch[,,,i],
                                     S = NULL,
                                     E = .Object@Eat[,,,i],
                                     N = .Object@stock[,,,i],
                                     .Object@maxratioconsumed,
                                     opt@numofareas)
            .Object@stock[,,,i] <- .Object@stock[,,,i] - tmp$E - tmp$C
            return(.Object)
          }
          
          )

setGeneric('updateLength.Stock',
           function(.Object,i){standardGeneric('updateLength.Stock')}
           )
setMethod(f ='updateLength.Stock',
          signature = "Stock",
          definition = function(.Object,i){
            if(.Object@doesgrow==1){
              for(area in .Object@livesonareas){
                .Object@stock[area,,,i] <-
                  t(.Object@G)%*%.Object@stock[area,,,i]%*%.Object@M
              }
            }
            return(.Object)
          }
)
RGadget.Stock <- function(stocks,fleets,opt) {
  
  for(year in 1:opt@numobs) {
    for(timestep in 1:opt@numoftimesteps) {
      i <- (year - 1)*opt@numoftimesteps + timestep
      if(timestep!=1){ ########### if we are not in timestep 1  #########
        for(stock in names(stocks)){
          stocks[[stock]] <- nextstep.Stock(stocks[[stock]],i)
        }
      } else if(i==1){ ### we have a special update in the 1st timestep ###
        for(stock in names(stocks)){
          stocks[[stock]] <- firststep.Stock(stocks[[stock]])
        }
      } else { ###### if we are in timestep 1 we have to update age ######
        
#############
        ## Age update
        ## NOTE this is the last step of the
        ## calculations done in previous timestep
        for(stock in names(stocks)){
          stocks[[stock]] <- updateAge.Stock(stocks[[stock]],i)
        }
        
        ## Adding the ones which have moved between stocks
        for(stock in names(stocks)){
          if(stocks[[stock]]@doesmove==1){
            stocks[[stocks[[stock]]@movesTo]] <-
              move.Stock(stocks[[stocks[[stock]]@movesTo]],stocks[[stock]],i)
          }
        }
      }
############
      ## Migration 
      for(stock in names(stocks)){
        if(stocks[[stock]]@doesmigrate==1){
          stocks[[stock]] <- migrate.Stock(stocks[[stock]],timestep,i)
        }
      }
############
      ## Consumption calculations
      for(stock in names(stocks)){
        if(stocks[[stock]]@doeseat==1){
          for(prey in stocks[[stock]]@suitability$stocks){
            stocks[[prey]] <- eat.Stock(stocks[[prey]],stocks[[stock]],opt,i)
          }
        }
      }
      
############
      ## Catch calculations
      for(fleet in names(fleets)){
        if(timestep %in% fleets[[fleet]]@timesteps){
          for(stock in fleets[[fleet]]@suitability$stocks){
            stocks[[stock]] <- catch.Stock(fleets[[fleet]],stocks[[stock]],opt,timestep,i)
          }
        }
      }
      
#############
                                        # Overconsumption check
      for(stock in names(stocks)){
        stocks[[stock]] <- adjust.update.con.Stock(stocks[[stock]],opt,i)
        stocks[[stock]] <- updateLength.Stock(stocks[[stock]],i)
        stocks[[stock]] <- recruits.Stock(stocks[[stock]],year,timestep)
      } 
    }
  }
  return(list(stocks=stocks,fleets=fleets,opt=opt))
}


opt <- new('Gadget.setup','Iceland',1:2,c(2e5,2e5),
           10,4,5,8000,0.8,c('imm','mat'),
           c('surv','comm'),1)
surv <- new('Fleet','surv',2, '', 0.1, 1:2, c('imm','mat'), -4.5, 0.3)
comm <- new('Fleet','comm',1:4, '', 0.7, 1:2, c('imm','mat'), -8, 0.22)
sigma <- c(2.2472, 2.8982, 4.0705, 4.9276,
           5.5404, 5.8072, 6.0233, 8, 9, 9)
imm <- new('Stock','imm',1:2,opt,movesTo = "mat",doesmove = 1,maxage=3,
           sigma=sigma[1:3])
mat <- new('Stock','mat',1:2,opt,doeseat = 1,
           stocks.eaten='imm', s.alpha=4.5,
           s.beta=-.2, s.gamma=-.3, s.delta=1,
           minage=4,sigma=sigma[4:10])
stocks <- list(imm=imm,mat=mat)
fleets <- list(surv=surv,comm=comm)
tmp <- RGadget.Stock(stocks,fleets,opt)


#imm <- firststep.Stock(imm)
#blu <- get.Stock(imm,1,1)
#imm <- recruits.Stock(imm,1,1)
#imm <- nextstep.Stock(imm,2)
#imm <- updateAge.Stock(imm,2)
#mat <- move.Stock(mat,imm,2)
#imm <- migrate.Stock(imm,1,1)
#imm <- eat.Stock(imm,mat,1)
                                        #blu <- as.data.frame(imm)
#rest <- blu$step==1&blu$year==1
#tmp <- aggregate(Num.indiv~length + area,blu[rest,],sum)
#xyplot(Num.indiv~length|area,blu,type='l',
#       panel=function(x,y,...){
#         tmp <- aggregate(y,by=list(x),sum)
#         names(tmp) <- c('x','y')
#         panel.xyplot(tmp$x,tmp$y,...)
#       }
#        ) 
