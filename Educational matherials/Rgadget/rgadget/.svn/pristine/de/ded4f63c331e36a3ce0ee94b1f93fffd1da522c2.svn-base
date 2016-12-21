setClass('gadget-growth',
         representation(growthfunction = 'character',
                        growthimplementation = 'character',
                        ## growthfunction parameters
                        growthparameters = 'vector',
                        wgrowthparameters = 'vector',
                        lgrowthparameters = 'vector',
                        weigthgrowthdata = 'data.frame',
                        yeareffect = 'vector',
                        stepeffect = 'vector',
                        areaeffect = 'vector',
                        ## growth implementation
                        beta = 'vector',
                        maxlengthgroupgrowth = 'vector'                        
                        ),
         prototype(growthfunction = '',
                        growthimplementation = 'betabinomial',
                        ## growthfunction parameters
                        growthparameters = vector(),
                        wgrowthparameters = vector(),
                        lgrowthparameters = vector(),
                        weigthgrowthdata = data.frame(),
                        yeareffect = vector(),
                        stepeffect = vector(),
                        areaeffect = vector(),
                        ## growth implementation
                        beta = vector(),
                        maxlengthgroupgrowth = vector()                        
         ),
         #package = 'rgadget',
#         validity = function(x){
#           if(x@growthfunction == 'lengthvbsimple')
#             if(length(x@growthparameters)!=4)
#               stop('Growth-parameters misspecified for lengthvbsimple
#                    should be 4')
#           else 
#             return(TRUE)
#         }
         )

setClass('gadget-prey',
         representation(
                        name = 'character',
                        preylengths = 'data.frame',
                        energycontent = 'numeric'))
         ##package = 'rgadget')

setClass('gadget-predator',
         representation(suitability = 'list',
                        preference = 'data.frame',
                        maxconsumption = 'numeric',
                        halffeedingvalue = 'numeric'))
         ##package = 'rgadget')

setClass('gadget-fleet',
         representation(name = 'character',
                        type = 'character',
                        livesonareas = 'numeric',
                        multiplicative = 'numeric',
                        suitability = 'list',
                        catchability = 'data.frame',
                        quotafunction = 'character',
                        biomasslevel = 'numeric',
                        quotalevel = 'numeric',
                        amount = 'data.frame'),
         prototype(name = 'fleet',
                 type = '',
                 multiplicative = 1,
                 suitability = list(),
                 catchability = data.frame(),
                 quotafunction = '',
                 biomasslevel = 0,
                 amount = data.frame()),
         ##package = 'rgadget'
         )

setClass('gadget-time',
         representation(firstyear = 'numeric', ## first year of simulation
                        firststep = 'numeric', ## first time step within first
                                               ## year of simulation.
                        lastyear = 'numeric', ## last year of simulation
                        laststep = 'numeric', ## last step of last year
                        notimesteps = 'numeric'), ## vector of lengths of timeintervals
         ###package = 'rgadget',
         validity = function(x){
           if(x@firstyear > x@lastyear)
             stop('Firstyear after lastyear')
           if(sum(x@notimesteps) != 12)
             stop('notimesteps should sum up to 12')
           if(!(x@firststep %in% seq(along = x@notimesteps)) )
             stop('firststep not in the range of timesteps')
           if(!(x@laststep %in% seq(along = x@notimesteps)) )
             stop('laststep not in the range of timesteps')
           return(TRUE)
         }
      ) 

setClass('gadget-area',
         representation(areas = 'numeric', ## vector of area identifiers
                        size = 'numeric', ## vector of area sizes
                        temperature = 'data.frame'),
         ###package = 'rgadget',
         validity = function(x){
           if(x@area < 1)
             stop('illegal area')
           if(x@size < 0)
             stop('negative size')
           return(TRUE)
         }
         )
setMethod('initialize','gadget-area',
          function(.Object,areas,size,temperature){
            .Object@areas <- areas
            if(length(size)==1)
              .Object@size <- rep(size,length(areas))
            else
              .Object@size <- size
            .Object@temperature <- temperature[c('year','step','area','temperature')]
            return(.Object)
          })

setClass('gadget-otherfood',
         representation(foodname = 'character',
                        livesonaareas = 'character',
                        lengths = 'numeric',
                        energycontent = 'numeric',
                        amount = 'data.frame'))
         ##package = 'rgadget')


setClass('gadget-spawning',
         representation(spawnsteps = 'numeric',
                        spawnareas = 'numeric',
                        firstspawnyear = 'numeric',
                        lastspawnyear = 'numeric',
                        spawnstocksandratio = 'data.frame',
                        proportionfunction = 'vector',
                        mortalityfunction = 'vector',
                        weightlossfunction = 'vector',
                        recruitment = 'vector',
                        stockparameters = 'data.frame'),         
         prototype(spawnsteps = 0,
                   spawnareas = 0,
                   firstspawnyear = 0,
                   lastspawnyear = 0,
                   spawnstocksandratio = data.frame(stock = '',ratio = 1),
                   proportionfunction = c(func = 'constant', alpha = 1),
                   mortalityfunction = c(func = 'constant', alpha = 0),
                   weightlossfunction = c(func = 'constant', alpha = 0),
                   recruitment = c(func = 'simplessb', mu = 1),
                   stockparameters = data.frame(mean = NULL, sttdev = NULL, 
                                                alpha = NULL, beta = NULL))
         )


setClass('gadget-stock',
         representation(stockname = 'character',
                        ## setup
                        livesonareas = 'numeric',
                        minage = 'numeric',
                        maxage = 'numeric',
                        minlength = 'numeric',
                        maxlength = 'numeric',
                        dl = 'numeric',
                        refweight  = 'data.frame',
                        growthandeatlengths = 'data.frame',
                        ## growth
                        doesgrow = 'numeric',
                        growth = 'gadget-growth',
                        naturalmortality = 'numeric',
                        ## consumption
                        iseaten = 'numeric',
                        preyinfo = 'gadget-prey',
                        doeseat = 'numeric',
                        predator = 'gadget-predator',
                        ## init stuff
                        initialconditions = 'list',
                        initialdata = 'data.frame',
                        ## migration
                        doesmigrate = 'numeric',
                        yearstep = 'data.frame',
                        migrationmatrix = 'array',
                        migrationratio = 'list',
                        ## Maturation
                        doesmature = 'numeric',
                        maturityfunction = 'character',
                        maturestocksandratios = 'list',
                        coefficients = 'list',
                        maturitysteps = 'numeric',
                        maturitylengths = 'numeric',
                        ## movement between stocks
                        doesmove = 'numeric',
                        transitionstockandratios = 'list',
                        transitionstep = 'numeric',
                        ## renewal
                        doesrenew = 'numeric',
                        renewal = 'list',
                        renewal.data = 'data.frame',
                        ## spawning 
                        doesspawn = 'numeric',
                        spawning = 'gadget-spawning',
                        ## straying -- to be implemented
                        doesstray = 'numeric'
                        ),
         prototype(stockname = '',
                        ## setup
                        livesonareas = NULL,
                        minage = NULL,
                        maxage = NULL,
                        minlength = NULL,
                        maxlength = NULL,
                        dl = NULL,
                        refweight  = data.frame(),
                        growthandeatlengths = data.frame(),
                        ## growth
                        doesgrow = 0,
                        growth = new('gadget-growth'),
                        naturalmortality = NULL,
                        ## consumption
                        iseaten = 0,
                        preyinfo = new('gadget-prey'),
                        doeseat = 0,
                        predator = new('gadget-predator'),
                        ## init stuff
                        initialconditions = list(),
                        initialdata = data.frame(),
                        ## migration
                        doesmigrate = 0,
                        yearstep = data.frame(),
                        migrationmatrix = array(),
                        migrationratio = data.frame(),
                        ## Maturation
                        doesmature = 0,
                        maturityfunction = '',
                        maturestocksandratios = list(),
                        coefficients = list(),
                        maturitysteps = 0,
                        maturitylengths = 0,
                        ## movement between stocks
                        doesmove = 0,
                        transitionstockandratios = list(),
                        transitionstep = 0,
                        ## renewal
                        doesrenew = 0,
                        renewal = list(),
                        renewal.data = data.frame(),
                        ## spawning
                        doesspawn = 0,
                        spawning = new('gadget-spawning'),
                        ## straying -- to be implemented
                        doesstray = 0
                   )
         ##package = 'rgadget'
         )


setClass('gadget-tagging',
         representation(tag.experiments = 'data.frame',
                        tag.numbers = 'data.frame'),
         prototype(tag.experiments = data.frame(tagid = NULL,stock = NULL,
                                                tagarea = NULL, endyear = NULL,
                                                tagloss = NULL, numbers = NULL),
                   tag.numbers = data.frame(tagid = NULL,
                                            year = NULL,
                                            step = NULL,
                                            length = NULL,
                                            number = NULL)
         )
)

setClass('gadget-main',
         representation(model.name='character',
                        time = 'gadget-time',
                        area = 'gadget-area',
                        print = 'list',
                        stocks = 'list',
                        tags = 'gadget-tagging',
                        otherfood = 'list',
                        fleets = 'list',
                        likelihood = 'list'),
         ##package = 'rgadget'
         )

                        


