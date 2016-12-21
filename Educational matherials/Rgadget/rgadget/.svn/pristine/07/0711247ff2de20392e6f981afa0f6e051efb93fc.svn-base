## Time file methods

setMethod("write",
    signature(x = "gadget-time"),
    function (x, file = "data", ncolumns = if (is.character(x)) 1 else 5, 
        append = FALSE, sep = " ") 
    {
      header <- sprintf('; time file created in Rgadget\n; %s - %s',file,Sys.Date())
      time.file <-
        paste(header,
              paste('firstyear',x@firstyear,sep='\t'),
              paste('firststep',x@firststep,sep='\t'),
              paste('lastyear',x@lastyear,sep='\t'),
              paste('laststep',x@laststep,sep='\t'),
              paste('notimesteps',
                    paste(length(x@notimesteps),
                          paste(x@notimesteps,collapse=' ')),
                    sep='\t'),
              sep='\n')
      write(time.file,file=file)
    }
)
setGeneric('getTimeSteps',def=function(object){standardGeneric("getTimeSteps")})
setMethod('getTimeSteps','gadget-time',
          function(x) {
            year <- x@firstyear:x@lastyear
            data.frame(year = rep(year,each = length(x@notimesteps)),
                                 step = rep(seq(along = x@notimesteps), length(year)))
          })

## area-file methods

setMethod("write",
    signature(x = "gadget-area"),
    function (x, file = "data", ncolumns = if (is.character(x)) 1 else 5, 
        append = FALSE, sep = " ") 
    {
      header <- sprintf('; time file created in Rgadget\n; %s - %s',file,Sys.Date())
      area.file <-
        paste(header,
              paste('areas',paste(x@areas,collapse=' '),sep='\t'),
              paste('size',paste(x@size,collapse=' '),sep='\t'),
              'temperature',
              '; year - step - area - temperature',
              sep='\n')
      write(area.file,file=file)
      write.table(x@temperature,file=file,col.names=FALSE,append=TRUE,
                  quote=FALSE,sep='\t',row.names=FALSE)
      
    }
)

## stockfile methods

setMethod("write",
    signature(x = "gadget-prey"),
    function (x, file = "data", ncolumns = if (is.character(x)) 1 else 5, 
        append = FALSE, sep = " ") 
    {
      dir.create(sprintf('%s/aggfiles',file), showWarnings = FALSE, recursive = TRUE)
      header <- paste(sprintf('; prey aggregation file for %s',x@name),
                      sprintf('; created using rgadget at %s', Sys.Date()),
                      sep = '\n')
      write(header,file = sprintf('%s/aggfiles/%s.prey.agg',file,x@name))
      write.table(x@preylengths,file = sprintf('%s/aggfiles/%s.prey.agg',file,x@name),
            col.names=FALSE,append=TRUE,
            quote=FALSE,sep='\t',row.names=FALSE)
      paste(sprintf('preylengths\t%s/aggfiles/%s.prey.agg',file,x@name),
            sprintf('energycontent\t%s',x@energycontent),
            sep = '\n')    
    }
)


setMethod("write",
    signature(x = "gadget-stock"),
    function (x, file = "data", ncolumns = if (is.character(x)) 1 else 5, 
        append = FALSE, sep = " ") 
    {
        dir.create(sprintf('%s/Data', file), showWarnings = FALSE, recursive = TRUE)
        dir.create(sprintf('%s/aggfiles', file), showWarnings = FALSE, recursive = TRUE)
        
        ref.head <- paste(sprintf('; refweight file for %s created using rgadget at %s',
                                  x@stockname,Sys.Date()),
                          paste(c('; ',names(x@refweight)),collapse = '\t'),
                          sep = '\n')
        write(ref.head,file = sprintf('%s/Data/%s.refweigthfile',file,x@stockname))
        tmp <- x@refweight
#        tmp[,2] <- round(tmp[,2])
        write.table(tmp,
                    file = sprintf('%s/Data/%s.refweigthfile',file,x@stockname),
                    col.names=FALSE,append=TRUE,
                    quote=FALSE,sep='\t',row.names=FALSE)
        
        ## length aggregation
        lengths <- seq(x@minlength,x@maxlength,by = x@dl)
        lenAgg <- data.frame(length = paste('len',tail(lengths,-1), sep = ''),
                             min = head(lengths,-1),
                             max = tail(lengths,-1)
                             )
        alllenAgg <- data.frame(length = 'alllen',
                                min = min(lengths),
                                max = max(lengths))
        agg.head <- paste(sprintf('; length aggregation file for %s created using rgadget at %s',
                                  x@stockname,Sys.Date()),
                          paste(c('; ',names(lenAgg)),collapse = '\t'),
                          sep = '\n')
        write(agg.head,file = sprintf('%s/aggfiles/%s.len.agg',file,x@stockname))
        
        write.table(lenAgg,
                    file = sprintf('%s/aggfiles/%s.len.agg',file,x@stockname),
                    col.names=FALSE,append=TRUE,
                    quote=FALSE,sep='\t',row.names=FALSE)
        
        write(agg.head,file = sprintf('%s/aggfiles/%s.alllen.agg',file,x@stockname))
        write.table(alllenAgg,
                    file = sprintf('%s/aggfiles/%s.alllen.agg',file,x@stockname),
                    col.names=FALSE,append=TRUE,
                    quote=FALSE,sep='\t',row.names=FALSE)
 
        ## age agg file
        ageAgg <- data.frame(label = x@minage:x@maxage,
                             age = x@minage:x@maxage)
        write(agg.head,file = sprintf('%s/aggfiles/%s.age.agg',file,x@stockname))
        write.table(ageAgg,
                    file = sprintf('%s/aggfiles/%s.age.agg',file,x@stockname),
                    col.names=FALSE,append=TRUE,
                    quote=FALSE,sep='\t',row.names=FALSE)
        allagesAgg <- data.frame(label = 'allages',
                                 age = paste(x@minage:x@maxage,collapse = '\t'))
        write(agg.head,file = sprintf('%s/aggfiles/%s.allages.agg',file,x@stockname))
        write.table(allagesAgg,
                    file = sprintf('%s/aggfiles/%s.allages.agg',file,x@stockname),
                    col.names=FALSE,append=TRUE,
                    quote=FALSE,sep='\t',row.names=FALSE)
        
        ## initial data
        init.head <- paste(sprintf('; initial (normalcond) file for %s created using rgadget at %s',
                                  x@stockname,Sys.Date()),
                          paste(c('; ',names(x@initialdata)),collapse = '\t'),
                          sep = '\n')
        write(init.head,file = sprintf('%s/Data/%s.normalcond',file,x@stockname))
        write.table(x@initialdata,
                    file = sprintf('%s/Data/%s.normalcond',file,x@stockname),
                    col.names=FALSE,append=TRUE,
                    quote=FALSE,sep='\t',row.names=FALSE)
        
        
        stock.text <- 
          c(sprintf('; stock definition file for %s created using rgadget',x@stockname),
            sprintf('; at %s',Sys.Date()),
            ';',
            sprintf('stockname\t%s',x@stockname),
            sprintf('livesonareas\t%s',paste(x@livesonareas,collapse = '\n')),
            sprintf('minage\t%s',x@minage),
            sprintf('maxage\t%s',x@maxage),
            sprintf('minlength\t%s',x@minlength),
            sprintf('maxlength\t%s',x@maxlength),
            sprintf('dl\t%s',x@dl),
            sprintf('refweightfile\t%s/Data/%s.refweigthfile',file,x@stockname),
            sprintf('growthandeatlengths\t%s/aggfiles/%s.len.agg',file,x@stockname),
            sprintf('doesgrow\t%s',x@doesgrow),
            growth = ';',
            sprintf('naturalmortality\t%s',paste(x@naturalmortality,collapse = '\t')),
            sprintf('iseaten\t%s',x@iseaten),
            eaten = ';',
            sprintf('doeseat\t%s',x@doeseat),
            eat = ';',
            'Initialconditions',
            paste(c('minage', 'maxage', 'minlength',
                    'maxlength', 'dl', 'sdev'),
                  x@initialconditions,sep ='\t',collapse = '\n'),
            sprintf('normalcondfile\t%s/Data/%s.normalcond',file,x@stockname),
            sprintf('doesmigrate\t%s',x@doesmigrate),
            migration = ';',
            sprintf('doesmature\t%s',x@doesmature),
            maturity = ';',
            sprintf('doesmove\t%s',x@doesmove),
            movement = ';',
            sprintf('doesrenew\t%s',x@doesrenew),
            renewal = ';',
            sprintf('doesspawn\t%s',x@doesspawn),
            spawning = ';',
            sprintf('doesstray\t%s',x@doesstray),
            straying = ';')
        if(x@doesgrow == 1){
          stock.text['growth'] <- toString(x@growth)
        }
        if(x@iseaten == 1){
          stock.text['eaten'] <- write(x@preyinfo,file=file)
        }
        if(x@doeseat == 1){
          stock.text['eat'] <- toString(x@predator)
        }
        if(x@doesspawn == 1){
          stock.text['spawning'] <- sprintf('spawnfile\t%s/Data/%s.spawnfile',
                                            file, x@stockname)
          write(x@spawning,file = sprintf('%s/Data/%s.spawnfile',file,
                             x@stockname))
        }
        if(x@doesmigrate == 1){
          stock.text['migration'] <- 
            paste(sprintf('yearstepfile\t%s/Data/%s.yearstep',file,x@stockname),
                  sprintf('defineratios\t%s/Data/%s.migratio',file,x@stockname),
                  sep = '\n')
          write.table(x@yearstep,
                      file = sprintf('%s/Data/%s.yearstep',file,x@stockname),
                      row.names = FALSE,
                      col.names = FALSE,
                      quote=FALSE)
          file.remove(sprintf('%s/Data/%s.migratio',file,x@stockname))
          l_ply(names(x@migrationratio),
                function(y){ 
                  migratio <- sprintf('%s/Data/%s.migratio',file,x@stockname)
                  write(sprintf('[migrationmatrix]\nname\t%s',y),
                        file = migratio, append = TRUE)
                  write.table(x@migrationratio[[y]],file= migratio,
                              append = TRUE,
                              row.names = FALSE,
                              col.names = FALSE,
                              quote=FALSE)
                  })
        }

        if(x@doesrenew == 1){
          stock.text['renewal'] <-
            paste(paste(names(x@renewal),
                        laply(x@renewal,
                              function(x)
                              paste(x,collapse=' ')),
                        sep='\t',collapse = '\n'),
                  sprintf('normalparamfile\t%s/Data/%s.rec',file,x@stockname),
                  sep='\n')
          write.table(x@renewal.data,
                      file = sprintf('%s/Data/%s.rec',
                        file,x@stockname),
                      row.names = FALSE,
                      col.names = FALSE,
                      quote=FALSE)
        }
        
        write(paste(stock.text,collapse = '\n'),
              file = sprintf('%s/%s',file,x@stockname))
    }
)

setMethod("toString",
          signature(x = "gadget-predator"),
          function (x, ...) 
          {
            tmp <- x@suitability
            tmp <- paste(tmp$stock,tmp$func,sapply(tmp$parameters,
                                            function(x) paste(x,collapse='\t')),
                         sep = '\t',collapse = '\n')
            pred.text <- 
              paste(sprintf('suitabiliy\n%s',tmp),
                    sprintf('preference\n%s',
                            paste(x@preference$stock,
                                  x@preference$preference,
                                  sep = '\t',
                                  collapse = '\n')),
                    sprintf('maxconsumption\t%s',
                            paste(x@maxconsumption,collapse = '\t')),
                    sprintf('halffeedingvalue\t%s',x@halffeedingvalue),
                    sep = '\n')
            return(pred.text)
          }
)

setMethod("toString",
          signature(x = "gadget-growth"),
          function (x, ...) 
          {
            growth.text <- 
              c(sprintf('growthfunction\t%s',x@growthfunction),
                params = '',
                sprintf('beta\t%s',x@beta),
                sprintf('maxlengthgroupgrowth\t%s',x@maxlengthgroupgrowth))
            if(x@growthfunction == 'lengthvbsimple')
              growth.text['params'] <- 
                paste(sprintf('growthparameters\t%s',
                              paste(x@growthparameters,collapse = '\t')))
            else 
              stop('other growth updates currently not supported by Rgadget')
            return(paste(growth.text,collapse = '\n'))
          }
          )


setMethod("write",
    signature(x = "gadget-fleet"),
    function (x, file = "data", ncolumns = if (is.character(x)) 1 else 5, 
        append = FALSE, sep = " ") 
    {
      header <- sprintf('; fleet file created in Rgadget\n; %s - %s\n[fleetcomponent]',file,Sys.Date())
      ## default text
      fleet.text <- 
        c(header = header,
          name = sprintf('%s\t%s',x@type,x@name),
          area = sprintf('livesonareas\t%s',x@livesonareas),
          mulit = sprintf('multiplicative\t%s',x@multiplicative),
          suit = sprintf('suitability\n%s',
                  paste(x@suitability$preyname,
                        'function',
                        x@suitability$func,
                        paste(x@suitability$parameters,collapse = '\t'))),
          empty = '; empty space -- move along nothing to see here',
          amount = sprintf('amount\tData/%s.amount',x@name),
          ';')

      if(tolower(x@type) == 'quotafleet')
        fleet.text['empty'] <- 
          paste(sprintf('quotafunction\t%s',x@quotafunction),
                sprintf('biomasslevel\t%s',paste(x@biomasslevel,collapse = '\t')),
                sprintf('quotalevel\t%s',paste(x@quotalevel,collapse = '\t')),
                sep = '\n')
      else if(tolower(x@type) == 'effortfleet')
        fleet.text['empty'] <- 
          sprintf('catchability\n%s',
                  paste(paste(x@catchability$stock,
                              x@catchability$catchabilty,sep='\t'),collapse = '\n'))
      else 
        fleet.text <- fleet.text[names(fleet.text) != 'empty']
      
      write.table(x@amount,file=sprintf('%s/Data/%s.amount',file,x@name),
                  col.names=FALSE,
                  quote=FALSE,sep='\t',row.names=FALSE)
      if(file.exists(sprintf('%s/fleets',file))){
        write(paste(fleet.text,collapse='\n'),file=sprintf('%s/fleets',file),
              append = TRUE)
      } else {
        write(paste(fleet.text,collapse='\n'),file=sprintf('%s/fleets',file))
      }
      invisible(fleet.text)
    }
)


setMethod("write",
    signature(x = "gadget-main"),
    function (x, file = "gadget-models", ncolumns = if (is.character(x)) 1 else 5, 
        append = FALSE, sep = " ") 
    {
      loc <- sprintf('%s/%s',file,x@model.name)
      dir.create(loc, showWarnings = FALSE, recursive = TRUE)
      ## writing ecosystem files
      write(x@area, file = sprintf('%s/area',loc))
      ## area aggregation files
      allareasAgg <- data.frame(label = 'allareas',areas = paste(x@area@areas,collapse = '\t'))
      write.table(allareasAgg,
                  file = sprintf('%s/aggfiles/allareas.agg',loc),
                  col.names=FALSE,append=FALSE,
                  quote=FALSE,sep='\t',row.names=FALSE)
      write(x@time, file = sprintf('%s/time',loc))
      if(length(x@print) > 0)
        write(x@print, file = sprintf('%s/printfile',loc))
      for(stock in x@stocks)
        write(stock,file = loc)
#      write(x@tags,file = sprintf('%s/tagfile',loc))
#      write(x@otherfood, file = sprintf('%s/otherfood',loc))
      file.remove(sprintf('%s/fleets',loc))
      for(fleet in x@fleets)
        write(fleet,file=loc)
      ## Likelihood files
      ##write(x@likelhood, file = sprintf('%s/likelihood'))
      
      main.text <-
        paste(sprintf('; main file for the %s model',x@model.name),
              sprintf('; created using rgadget at %s',Sys.Date()),
              'timefile\ttime',
              'areafile\tarea',
              sprintf('printfiles\t%s',
                      ifelse(length(x@print)>0,'printfile',';')),
              '[stock]',
              sprintf('stockfiles\t%s',
                      paste(sapply(x@stocks,function(x) x@stockname),
                            collapse='\t')),
              '[tagging]',
              ifelse(length(x@tags)>0,'tagfiles\ttags',';'),
              '[otherfood]',
              ifelse(length(x@otherfood)>0,'otherfoodfiles\totherfood',';'),
              '[fleet]',
              ifelse(length(x@fleets)>0,'fleetfiles\tfleets',';'),
              '[likelihood]',
              ifelse(length(x@likelihood)>0,
                     'likelihoodfiles\tlikelihood',';'),
              sep='\n'
              )
      write(main.text,file=sprintf('%s/main',loc))
      invisible(main.text)
            }
)

setMethod("write",
          signature(x = "gadget-spawning"),
          function (x, file = "data", ncolumns = if (is.character(x)) 1 else 5, 
                    append = FALSE, sep = " ") 
          {
            spawn.text <- 
              paste(sprintf('; spawning file created using Rgadget at %s',Sys.Date()),
                    sprintf('spawnsteps\t%s', x@spawnsteps),
                    sprintf('spawnareas\t%s', x@spawnareas),
                    sprintf('firstspawnyear\t%s', x@firstspawnyear),
                    sprintf('lastspawnyear\t%s',  x@lastspawnyear),
                    paste('spawnstocksandratios',x@spawnstocksandratio$stock,
                          x@spawnstocksandratio$ratio,sep='\t',collapse ='\n'),
                    sprintf('proportionfunction\t%s\t%s',
                          x@proportionfunction['func'],paste(x@proportionfunction[-1],collapse='\t')),
                    sprintf('mortalityfunction\t%s\t%s',
                            x@mortalityfunction['func'],paste(x@mortalityfunction[-1],collapse='\t')),
                    sprintf('weightlossfunction\t%s\t%s',
                            x@weightlossfunction['func'],paste(x@weightlossfunction[-1],collapse='\t')),
                    sprintf('recruitment\t%s\t%s',
                            x@recruitment['func'],paste(x@recruitment[-1],collapse='\t')),
                    
                    sprintf('stockparameters\t%s\t%s\t%s\t%s',
                            x@stockparameters$mean, 
                            x@stockparameters$std.dev,
                            x@stockparameters$alpha, 
                            x@stockparameters$beta),
                    sep = '\n')
              write(spawn.text,file = file)
          }
          )


setGeneric('getMaxage',def=function(object){standardGeneric("getMaxage")})
setMethod('getMaxage','gadget-stock', function(object) return(object@maxage))
setMethod('getMaxage','gadget-main', 
          function(object){
            maxage <- max(laply(object@stocks,getMaxage))
            return(maxage)
          })

setGeneric('getMinage',def=function(object){standardGeneric("getMinage")})
setMethod('getMinage','gadget-stock', function(object) return(object@minage))
setMethod('getMinage','gadget-main', 
          function(object){
            minage <- min(laply(object@stocks,getMinage))
            return(minage)
          })

setGeneric('getNumOfStocks',def=function(object){standardGeneric("getNumOfStocks")})
setMethod('getNumOfStocks','gadget-main',
          function(object) length(object@stocks))

setGeneric('getStockNames',def=function(object){standardGeneric("getStocksNames")})
setMethod('getStockNames','gadget-stock',
          function(object) return(object@name))
setMethod('getMinage','gadget-main', 
          function(object){
            stockNames <- laply(object@stocks,getStockNames)
            return(stockNames)
          })


setGeneric('getNumOfAreas',def=function(object){standardGeneric("getNumOfAreas")})
setMethod('getNumOfAreas','gadget-area',
          function(object) length(object@areas))
setMethod('getNumOfAreas','gadget-main',
          function(object) getNumOfAreas(object@area))

setGeneric('getAreas',def=function(object){standardGeneric("getAreas")})
setMethod('getAreas','gadget-area',
          function(object) object@areas)
setMethod('getAreas','gadget-main',
          function(object) getAreas(object@area))

setGeneric('getNumLengthGroups',def=function(object){standardGeneric("getNumLengthGroups")})
setMethod('getNumLengthGroups', 'gadget-stock',
          function(object){
            ceiling((object@maxlength - object@minlength )/object@dl)
          })
setMethod('getNumLengthGroups', 'gadget-main',
          function(object) max(ladply(object@stocks,getNumLengthGroups)))

setGeneric('getLengthGroups',def=function(object){standardGeneric("getLengthGroups")})
setMethod('getLengthGroups', 'gadget-stock',
          function(object) seq(object@minlength, object@maxlength,by=object@dl))
setMethod('getLengthGroups', 'gadget-main',
          function(object) llply(object@stocks,getLengthGroups))


setGeneric('getNumTimeSteps',def=function(object){standardGeneric("getNumTimesteps")})
setMethod('getNumTimeSteps', 'gadget-time',
          function(object) length(object@notimesteps))
setMethod('getNumTimeSteps', 'gadget-main',
          function(object) getNumTimeSteps(object@time))

#setGeneric('getTimesteps',def=function(object){standardGeneric("getTimesteps")})
setGeneric('getNumYears',def=function(object){standardGeneric("getNumYears")})
setMethod('getNumYears', 'gadget-time',
          function(object) object@lastyear - object@firstyear + 1)
setMethod('getNumYears', 'gadget-main',
          function(object) getNumYears(object@time))

setGeneric('getYears',def=function(object){standardGeneric("getYears")})
setMethod('getYears', 'gadget-time',
          function(object) object@firstyear:object@lastyear)
setMethod('getYears', 'gadget-main',
          function(object) getYears(object@time))


setGeneric('getNumFleets',def=function(object){standardGeneric("getNumFleets")})
setMethod('getNumFleets','gadget-main',
          function(object) length(object@fleets))

setGeneric('getFleetNames',def=function(object){standardGeneric("getFleetNames")})
setMethod('getFleetNames','gadget-fleet',
          function(object) object@name)
setMethod('getFleetNames','gadget-main',
          function(object) laply(object@fleets,getFleetNames))

setGeneric('getNumTagging',def=function(object){standardGeneric("getNumTagging")})
setMethod('getNumTagging', 'gadget-tagging',
          function(object) nrow(object@tag.experiments))
setMethod('getNumTagging', 'gadget-main',
          function(object) getNumTagging(object@tagging))

setGeneric('getTagID',def=function(object){standardGeneric("getTagID")})
setMethod('getTagID','gadget-tagging',
          function(object) object@tag.experiments)
setMethod('getTagID','gadget-main',
          function(object) getTagID(object@tagging))

setGeneric('isPredator',def=function(object){standardGeneric("getNumPredators")})
setMethod('isPredator', 'gadget-stock',
          function(object) object@doeseat )
setGeneric('getNumPredators',def=function(object){standardGeneric("getNumPredators")})
setMethod('getNumPredators', 'gadget-main',
          function(object) sum(laply(object@stocks,isPredator)))

setGeneric('getPredatorNames',def=function(object){standardGeneric("getPredators")})
setMethod('getPredatorNames','gadget-main',
          function(object){
            tmp <- laply(object@stocks,
                         function(x){
                           if(isPredator(x)==1)
                             getStockNames(x)
                           else
                             ''                    
                         })                  
            tmp[tmp!='']  
          })

setGeneric('getGrowth',def=function(object, par){standardGeneric("getGrowth")})
setMethod('getGrowth', 'gadget-growth',
          function(object,par){
            if(tolower(object@growthfunction) == 'lengthbvsimple'){
              params <- eval.gadget.formula(object@growthparameters,par)

              beta <- eval.gadget.formula(object@beta,par)
              nbin <- eval.gadget.formula(object@maxlengthgroupgrowth,par)
              function(lt,lengthgrouplen,dt){
                growthprob(lt,beta,params[1],params[2],dt,lengthgrouplen,
                           nbin)
              }
            } else {
              stop(sprintf('Error in Growth -- Only lengthvbsimple is supported, %s was supplied',
                           object@growthfunction))
            }
          })
setMethod('getGrowth','gadget-stock',
          function(object, par){
            func <- getGrowth(object@growth,par)
            function(dt){
              func(getLengthGroups(object), object@dl, dt)
            }
          })

setMethod('getGrowth', 'gadget-main',
          function(object){
            yearstep <- getTimeSteps(object@time)
            yearstep$dt <- yearstep$step/12
            llply(object@stocks,
                  function(x){
                    func <- getGrowth(x)
                    daply(yearstep,c('year','step'),
                          function(y){
                            func(y[1])
                          })
                  }
                  )
          })


setGeneric('getMortality',def=function(object, par){standardGeneric("getMortality")})
setMethod('getMortality','gadget-stock',
          function(object,par){
            eval.gadget.formula(object@naturalmortality)
          }
          )
setMethod('getMortality','gadget-main',
          function(object,par){
            llply(object@stocks,function(x) getMortality(x,par))
          })

setGeneric('getFleetSelection',def=function(object, par){standardGeneric("getFleetSelection")})
setMethod('getFleetSelection','gadget-fleet',
          function(object,par){
            ldply(object@suitability,
                  function(x) eval.gadget.formula(x,par))
          }
          )

setGeneric('getPredatorSelection',def=function(object, par){standardGeneric("getPredatorSelection")})

setGeneric('getInitData',def=function(object, par){standardGeneric("getInitData")})
setGeneric('getRecStocks',def=function(object, par){standardGeneric("getRecStocks")})
setGeneric('getRecruitment',def=function(object, par){standardGeneric("getRecruitment")})

setGeneric('getMigratingStocks',def=function(object){standardGeneric("migratingStocks")})
setGeneric('getMigrationMatrix',def=function(object, par){standardGeneric("getMigrationMatrix")})

setGeneric('checkOverconsumption',def=function(object){standardGeneric("checkOverconsumption")})

setGeneric('spawn',def=function(object, par){standardGeneric("spawn")})
setGeneric('getSpawnSteps',def=function(object){standardGeneric("getSpawnSteps")})

setGeneric('straying',def=function(object,par){standardGeneric("straying")})

setGeneric('getLaststep',def=function(object){standardGeneric("laststep")})


setGeneric('writeAggfiles',
           def=function(x,folder){standardGeneric("writeAggfiles")})
setMethod('writeAggfiles','gadget-stock',
          function(x,folder){
            
            ## length aggregation file
            lengths <- seq(x@minlength,x@maxlength,by = x@dl)
            lenAgg <- data.frame(length = paste('len',tail(lengths,-1),
                                   sep = ''),
                                 min = head(lengths,-1),
                                 max = tail(lengths,-1)
                                 )
            
            agg.head <-
              paste(sprintf('; aggregation file for %s created using rgadget at %s',
                            x@stockname,Sys.Date()),
                    paste(c('; ',names(lenAgg)),collapse = '\t'),
                    sep = '\n')
            write(agg.head,file = sprintf('%s/%s.len.agg',folder,
                             x@stockname))
            
            write.table(lenAgg,
                        file = sprintf('%s/%s.len.agg',folder,
                          x@stockname),
                        col.names=FALSE,append=TRUE,
                        quote=FALSE,sep='\t',row.names=FALSE)

            ## all length agg file
            alllenAgg <- data.frame(length = 'alllen',
                                    min = min(lengths),
                                    max = max(lengths))
            write(agg.head,file = sprintf('%s/%s.alllen.agg',folder,
                             x@stockname))
            write.table(alllenAgg,
                        file = sprintf('%s/%s.alllen.agg',folder,x@stockname),
                        col.names=FALSE,append=TRUE,
                        quote=FALSE,sep='\t',row.names=FALSE)
            
            ## age agg file
            ageAgg <- data.frame(label = x@minage:x@maxage,
                                 age = x@minage:x@maxage)
            write(agg.head,file = sprintf('%s/%s.age.agg',folder,x@stockname))
            write.table(ageAgg,
                        file = sprintf('%s/%s.age.agg',folder,x@stockname),
                        col.names=FALSE,append=TRUE,
                        quote=FALSE,sep='\t',row.names=FALSE)

            ## allages.agg
            allagesAgg <- data.frame(label = 'allages',
                                     age = paste(x@minage:x@maxage,
                                       collapse = '\t'))
            write(agg.head,file = sprintf('%s/%s.allages.agg',folder,
                             x@stockname))
            write.table(allagesAgg,
                        file = sprintf('%s/%s.allages.agg',folder,x@stockname),
                        col.names=FALSE,append=TRUE,
                        quote=FALSE,sep='\t',row.names=FALSE)
            ## Area agg file
            areaAgg <- data.frame(label=paste('area',
                                    x@livesonareas,
                                    sep = ''),
                                  area = x@livesonareas)
            write(agg.head,file = sprintf('%s/%s.area.agg',folder,
                             x@stockname))
            write.table(areaAgg,
                        file = sprintf('%s/%s.area.agg',folder,x@stockname),
                        col.names=FALSE,append=TRUE,
                        quote=FALSE,sep='\t',row.names=FALSE)
            
          })





























