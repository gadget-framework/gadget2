Rgadget <- function(gadget.model, parameters){
  ### intial setup
  ### stock abundance
  age.range <- minage(gadget.model):maxage(gadget.model)

  stocks <- array(0, c(num.of.stocks(gadget.model),
                       num.of.areas(gadget.model),
                       maxage(gadget.model) - minage(gadget.model)+1,
                       num.lengthgroups(gadget.model),
                       num.timesteps(gadget.model),
                       num.years(gadget.model))
                  )
  dimnames(stocks) <- list(stock = names.of.stocks(gadget.model),
                           area = areas(gadget.model),
                           age = minage(gadget.model):maxage(gadget.model),
                           length = 1:lengthgroups(gadget.model),
                           step = timesteps(gadget.model),
                           year = years(gadget.model))

  
  ### Fleet catches
  catch.operation <- FALSE
  if(num.fleets(gadget.model)>0){
    catch.operation <- TRUE
    catch <- array(0, c(num.fleets(gadget.model),
                        num.of.stocks(gadget.model),
                        num.of.areas(gadget.model),
                        maxage(gadget.model) - minage(gadget.model)+1,
                        num.lengthgroups(gadget.model),
                        num.timesteps(gadget.model),
                        num.years(gadget.model))
                   )
    dimnames(catch) <- list(fleet = fleets(gadget.models),
                            stock = names.of.stocks(gadget.model),
                            area = areas(gadget.model),
                            age = minage(gadget.model):maxage(gadget.model),
                            step = timesteps(gadget.model),
                            year = years(gadget.model))
  } 

  ### Tagged population
  taggging <- FALSE
  if(num.tagging(gadget.model) >0){
    tagging <- TRUE
    tagged <- array(0, c(num.tagging(gadget.model),
                        num.of.stocks(gadget.model),
                        num.of.areas(gadget.model),
                        maxage(gadget.model) - minage(gadget.model)+1,
                        num.lengthgroups(gadget.model),
                        num.timesteps(gadget.model),
                        num.years(gadget.model))
                   )
    dimnames(tagged) <- list(taggroup = tagging(gadget.models),
                            stock = names.of.stocks(gadget.model),
                            area = areas(gadget.model),
                            age = minage(gadget.model):maxage(gadget.model),
                            step = timesteps(gadget.model),
                            year = years(gadget.model))

    ### tagged catch
    tagged.catch <- array(0, c(num.fleets(gadget.model),
                               num.tagging(gadget.model),
                               num.of.stocks(gadget.model),
                               num.of.areas(gadget.model),
                               maxage(gadget.model) - minage(gadget.model)+1,
                               num.lengthgroups(gadget.model),
                               num.timesteps(gadget.model),
                               num.years(gadget.model))
                          )
    dimnames(tagged.catch) <-
      list(fleet = fleets(gadget.model),
           taggroup = tagging(gadget.models),
           stock = names.of.stocks(gadget.model),
           area = areas(gadget.model),
           age = minage(gadget.model):maxage(gadget.model),
           step = timesteps(gadget.model),
           year = years(gadget.model))
    
  }

  ## Predation, only prey length are stored
  if(num.pred(gadget.model)>0){
    Eat <- array(0, c(num.predators(gadget.model),
                      num.of.stocks(gadget.model),
                      num.of.areas(gadget.model),
                      maxage(gadget.model) - minage(gadget.model)+1,
                      num.lengthgroups(gadget.model),
                      num.timesteps(gadget.model),
                      num.years(gadget.model))
                 )
    dimnames(Eat) <- list(predator = predators(gadget.models),
                          stock = names.of.stocks(gadget.model),
                          area = areas(gadget.model),
                          age = minage(gadget.model):maxage(gadget.model),
                          step = timesteps(gadget.model),
                          year = years(gadget.model))
  } 
  
  ### Growth by length and natural mortality by age
  G <- getGrowth(gadget.model, parameters)
  M <- getMortality(gadget.model, parameters)
  
  ### Fleet selection by age, length and
  selection <- getFleetSelection(gadget.model, parameters)

  ### Predator selection by prey length and time
  eat.selection <- getPredatorSelection(gadget.model, parameters)

  ### Initial setup
  stocks[,,,,1,1] <- intialize(gadget.model, parameters)

  ### Recruitment
  stocks[recStocks(gadget.model),,1,,] <- recruitment(gadget.model, parameters)

  for(year in years(gadget.model)){
    for(step in timesteps(gadget.model)){

      ## migration
      for(stock in migratingStocks(gadget.model)){
        stocks[stock,,,,year,step] <-
          migration.matrix(gadget.model,year,step)%*%stocks[stock,,,,year,step]
        if(tagging){
          tagged[stock,,,,year,step] <-
            migration.matrix(gadget.model,year,step)%*%
              tagged[stock,,,,year,step]
        }
      }
      
      ## consumption
      for(pred in predators(gadget.model)) {
        for(prey in stocks(gadget.model)) {
          Eat[prey,,,,year,step] <- eat(stock[prey,,,,year,step],
                                        stock[pred,,,,year,step],
                                        eat.selection[pred,year,step,,])
        }
      }

      for(fleet in fleets(gadget.model)) {
        for(prey in stocks(gadget.model)) {
          catches[fleet,prey,,,,year,step] <-
            catch(stocks[prey,,,,year,step],selection[fleet,year,step,,])
        }
      }

      ## overconsumption check
      tmpC <- adjustconsumption(catches[,,,,year,step],
                                E=Eat[,,,,year,step],
                                N=stocks[,,,year,step],
                                gadget.model)
        

      ## substract catches and consumption from abundance
      catches[,,,,year,step] <- catches[,,,,year,step] - tmpC
      
      ## natural mortality and growth
      catches[,,,,year,step] <- G%*%catches[,,,,year,step]%*%M

      ## spawning
      if(step %in% spawnSteps(gadget.model)){
        for(stock %in% spawnstocks(gadget.model,step)){
          stocks[stock,,,1,year,step] <-
            spawn(gadget.model,stock,step)
        }
      }
      
      
      ## maturation (not implemented)
      

      ## stock straying      
      stocks[stock,,,,year,step] <-
        straying(stocks,stock,gadget.model,year,step)

      
      ## update time step
      if(step < laststep(gadget.model))
        stocks[,,,,step + 1] <- stocks[,,,,step]
    
      
      
    }

    
    ## update age
    stock[,,,age.range[-1], year + 1, 1] <-
      stock[,,,head(age.range,-1),year,step]
    stock[,,,maxage(gadget.model),year + 1,1] <-
      stock[,,,maxage(gadget.model),year,step]
    
    ## stock movement
    
    
  }
  
  return('sucess')
  
}
