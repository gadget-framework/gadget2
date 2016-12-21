library(plyr)
library(reshape2)
library(lubridate)
library(stringr)
library(data.table)

##' This function attempts to read in the gadget output files defined in
##' printfiles. This is a quick and dirty implementation that has been 
##' designed to read a few nice examples, so it may work for some instances
##' and it may not. It assumes that the last line containing a comment ';'
##' is the line describing the column names and is the last comment line.
##' @title Read gadget printfiles
##' @param path a character string with the name of the folder containing the printfiles
##' @return a list containing the data that has been read in named after the files found in path.
read.printfiles <- function(path='.',suppress=FALSE){
##' worker function
##' @title 
##' @param file 
##' @return 
##' @author Bjarki Thor Elvarsson
  read.printfile <- function(file){
#    file <- paste(path,file,sep='/')
    tmp <- readLines(file)
    if(length(tmp) == 0){
      if(!suppress)
        warning(sprintf('Warning in read.printfile -- %s is of length 0',file))
      return(NULL)
    }
    skip <- max(grep(';',tmp[1:7]))
    header <- gsub('; ','',tmp[skip])
    header <- gsub(' ','.',unlist(strsplit(header,'-')))
    data <- tryCatch(read.table(file,comment.char=';',header=FALSE,
                                stringsAsFactors = FALSE),
                     error = function(e){
                       if(!suppress)
                         print(sprintf('file corrupted -- %s', file))
                       return(NULL)
                     })
    if(is.null(data))
      return(NULL)
    if(length(names(data)) != length(header)){
      if(!suppress)
        warning(sprintf('Error in read.printfile -- Header could not be read from file %s',file))
    } else {
      names(data) <- header
    }
    pos <- grep('Regression information',tmp)
    if(length(pos)!=0){
      regr <- read.table(text=gsub('; ','',
                           tmp[(pos+1):length(tmp)]))[c(1,3,5,7)]
      names(regr) <- c('label','intercept','slope','sse')
      data <- merge(data,regr)
      data <- mutate(data,
                     predict = exp(intercept)*number^slope) ## 1000 hmm
    }
    pos <- grep('; Standard output file for the stock',tmp)
    if(length(pos) != 0){
      step1 <- data[c('year','step','age','area','number')]
      next1 <- mutate(subset(step1,age>1),year=year-1,age=as.integer(age-1))
      names(next1)[5] <- 'num.after.harv'
      tmp <- merge(step1,next1)
      tmp$Z <- log(tmp$number) - log(tmp$num.after.harv)
      data <- merge(data,tmp[c('year','step','age','area','Z')],all.x=TRUE)
    }
  
    return(data)
  }
  out.files <- list.files(path=path,
                          full.names=TRUE,recursive=TRUE)
  
  printfiles <- llply(out.files,read.printfile)
  names(printfiles) <- gsub('/','',gsub(path.expand(path),'',
                                        out.files),fixed=TRUE)
  class(printfiles) <- c('gadgetOut','list')
  return(printfiles)
}
##' This functions reads the likelihood (input) file for gadget. The format of
##' the likelihood file is described in gadget's user manual. 
##' @title Read likelihood
##' @param files a vector of character strings containing the names of the likelihood files 
##' @return object of class gadget.likelihood, i.e. a list containing the various likelihood components
##' @author Bjarki ??r Elvarsson
read.gadget.likelihood <- function(files='likelihood'){
  lik <- NULL
  for(file in files){
    lik <- c(lik,sub(' +$','',gsub('\t',' ',readLines(file))))
  }
  lik <- lik[lik!='']
  lik <- lik[!grepl(';',substring(lik,1,1))]
  lik <- sapply(strsplit(lik,';'),function(x) sub(' +$','',x[1]))
  
  comp.loc <- grep('component',lik)
  name.loc <- comp.loc+3
  weights <- NULL
  common <- c('name','weight','type','datafile','areaaggfile','lenaggfile',
              'ageaggfile','sitype','function')
  tmp.func <- function(comp){
    loc <- grep(paste('[ \t]',tolower(comp),sep=''),tolower(lik[name.loc]))  
    if(sum(loc)==0){
      return(NULL)
    } else {
      
      dat <- ldply(loc, function(dd){
        if(dd < length(comp.loc)) {
          restr <- (comp.loc[dd] + 1):(comp.loc[dd+1]-1)
        } else {
          restr <- 1:length(lik) > comp.loc[dd]
        }
        
        tmp <- sapply(strsplit(sapply(strsplit(lik[restr],'[ \t]'),
                                      function(x) {
                                        paste(x[!(x==''|x=='\t')],
                                              collapse=' ')
                                      }),' '),
                      function(x) as.character(x))
        if(class(tmp)!='list'){
          names.tmp <- head(tmp,1)
          tmp <- as.data.frame(tmp,stringsAsFactors=FALSE)[2,]
          names(tmp) <- names.tmp
          row.names(tmp) <- tmp$name
          tmp$type <- tolower(tmp$type)
        } else {
          names.tmp <- sapply(tmp,function(x) x[1])
          tmp <- sapply(tmp,function(x) paste(x[-1], collapse='\t'))
          names(tmp) <- names.tmp
          tmp <- as.data.frame(t(tmp),stringsAsFactors=FALSE)
          tmp$type <- tolower(tmp$type)
        }
        return(tmp)
      })
      if(is.null(weights)){
        weights <<- dat[intersect(common, unique(c(names(weights),names(dat))))]
      } else {
        weights <<-
          rbind.fill(dat, weights)[intersect(common, unique(c(names(weights),
                                                              names(dat))))]
      }
      dat$weight <- NULL                   
      return(dat)
    }
  }

  likelihood <- list(penalty = tmp.func('penalty'),
                     understocking = tmp.func('understocking'),
                     migrationpenalty = tmp.func('migrationpenalty'),
                     surveyindices = tmp.func('surveyindices'),
                     catchdistribution = tmp.func('catchdistribution'),
                     catchstatistics = tmp.func('catchstatistics'),
                     surveydistribution = tmp.func('surveydistribution'),
                     stockdistribution = tmp.func('stockdistribution'),
                     stomachcontent = tmp.func('stomachcontent'),
                     recaptures = tmp.func('recaptures'),
                     recstatistics = tmp.func('recstatistics'),
                     catchinkilos = tmp.func('catchinkilos')
                     )
  likelihood$weights <- weights
  row.names(likelihood$weights) <- weights$name
  likelihood$weights$weight <- as.numeric(weights$weight)
  likelihood <- likelihood[c('weights',unique(likelihood$weights$type))]
  class(likelihood) <- c('gadget.likelihood',class(likelihood))
  return(likelihood)
}
##' Write a likelihood object to file
##' @title Write likelihood
##' @param lik object of class gadget.likelihood
##' @param file name of the likelihood file
##' @param data.folder 
##' @param bs.sample 
##' @return character string corresponding to the likelihood file (if desired)
##' @author Bjarki ??r Elvarsson
write.gadget.likelihood <- function(lik,file='likelihood',
                                    data.folder=NULL, bs.sample=NULL){
  lik.text <- sprintf('; Likelihood file - created in Rgadget\n; %s - %s',
                      file, Sys.Date())
  weights <- lik$weights[c('name','weight')]
  lik$weights <- NULL
  for(comp in lik){ 
    if(!is.null(data.folder)){
      comp$datafile <- paste(data.folder,comp$datafile,sep='/')
    }
    ## reorder surveyindices columns 
    if('surveyindices' %in% comp$type){
      comp <-
        comp[intersect(c('name','type','datafile','sitype','biomass',
                         'areaaggfile','lenaggfile','surveynames','fleetnames',
                         'stocknames','fittype','slope','intercept'),
                       names(comp))]
    }
    comp <- na.omit(melt(merge(weights,comp,by='name',sort=FALSE),
                         id.vars = 'name'))
    comp.text <- ddply(comp,'name',function(x){
      paste('[component]',
            sprintf('name\t\t%s',x$name[1]),
            paste(x$variable,x$value, sep = '\t\t',
                  collapse = '\n'),
            ';', sep = '\n')
    })
    
    lik.text <- paste(lik.text,
                      paste(comp.text$V1,
                            collapse='\n'),
                      sep='\n')
  }
  if(!is.null(bs.sample))
    write(sprintf(lik.text,bs.sample),file=file)
  else
    write(lik.text,file=file)
  invisible(lik.text)
}

##' <description>
##'
##' <details>
##' @title 
##' @param lik1 
##' @param lik2 
##' @return 
##' @author Bjarki Thor Elvarsson
merge.gadget.likelihood <- function(lik1,lik2){
  tmp <- within(list(),
                for(comp in unique(c(names(lik1),names(lik2)))){
                  assign(comp,
                         unique(rbind.fill(lik1[[comp]],lik2[[comp]])))
                })
  class(tmp) <- c('gadget.likelihood',class(tmp))
  return(tmp)
}
##' <description>
##'
##' <details>
##' @title 
##' @param likelihood 
##' @param comp
##' @param inverse
##' @return 
##' @author Bjarki Thor Elvarsson
get.gadget.likelihood <- function(likelihood,comp,inverse=FALSE){
  if(inverse)
    weights <- subset(likelihood$weights,!(name %in% comp))
  else
    weights <- subset(likelihood$weights,name %in% comp)
  tmp <-
    within(list(),
           for(type in weights$type){
             restr <- likelihood[[type]][['name']] %in% comp
             if(inverse)
               restr <- !restr
             assign(type,
                    likelihood[[type]][restr,])
           }
           )
  tmp$restr <- NULL
  tmp$type <- NULL
  tmp$weights <- weights
  class(tmp) <- c('gadget.likelihood',class(tmp))
  return(tmp)
}


new.gadget.main <- function(){
  main <-
    list(timefile = '',
         areafile = '',
         printfiles = '',
         stockfiles = '',
         tagfiles = '',
         otherfoodfiles = '',
         fleetfiles = '',
         likelihoodfiles = '')
  class(main) <- c('gadget.main',class(main))
  return(main)
}

##' Read gadget's main file
##' @title Read main
##' @param file main file location
##' @return object of class gadget.main
##' @author Bjarki ??r Elvarsson
read.gadget.main <- function(file='main'){
  main <- sub(' +$','',readLines(file))
  if(length(main) == 0)
    stop(sprintf('Error in read.gadget.main, file %s is empty',file))
  main <- main[main!='']
  main <- main[!grepl(';',substring(main,1,1))]
  main <- sapply(strsplit(main,';'),function(x) x[1])
  main <- clear.spaces(main)
  tmp <- sapply(main[sapply(main,length)!=1],function(x) x[2:length(x)])
  names(tmp) <-  sapply(main[sapply(main,length)!=1],function(x) x[1])
  main <- as.list(tmp)
  class(main) <- c('gadget.main',class(main))
  return(main)
}
##' Write gadget.main object to file
##' @title Write main
##' @param main gadget.main object
##' @param file name of main file 
##' @return text of the main file (if desired)
##' @author Bjarki ??r Elvarsson
write.gadget.main <- function(main,file='main'){
  main.text <- sprintf('; main file for gadget - created in Rgadget\n; %s - %s',
                       file,date())
  if(is.null(main$printfiles)){
    main$printfiles <- '; no printfile supplied'  
  }
  main.text <-
    paste(main.text,
          paste('timefile',main$timefile),
          paste('areafile',main$areafile),
          paste('printfiles',paste(main$printfiles,collapse='\t')),
          '[stock]',
          paste('stockfiles',paste(main$stockfiles,collapse='\t')),
          ifelse(is.null(main$tagfiles), #| main$tagfiles == '',
                 '[tagging]',
                 paste('[tagging]\ntagfiles',paste(main$tagfiles,
                                                   collapse='\t'))),
          ifelse(is.null(main$otherfoodfiles), #| main$otherfoodfiles == '',
                 '[otherfood]',
                 paste('[otherfood]\notherfoodfiles',
                       paste(main$otherfoodfiles,collapse='\t'))),
          ifelse(is.null(main$likelihoodfiles), # | main$likelihoodfiles == '',
                 '[fleet]',
                 paste('[fleet]\nfleetfiles',
                       paste(main$fleetfiles,collapse='\t'))),
          '[likelihood]',
          paste('likelihoodfiles',
                paste(main$likelihoodfiles,collapse='\t')),
          sep='\n')
  write(main.text,file=file)
  invisible(main.text)
}
  
##' Clear tab and spaces from a string and return a list or a matrix of values 
##' @title Clear spaces
##' @param text string 
##' @return list or matrix containing the (non-empty) values from the string
##' @author Bjarki ??r Elvarsson
clear.spaces <- function(text){
  sapply(strsplit(sapply(strsplit(text,'[ \t]'),
                         function(x) {
                           paste(x[!(x==''|x=='\t')],
                                 collapse=' ')
                         }),' '),
         function(x) x)
}
##' Read gadget parameter file
##' @title Read param
##' @param file parameter file
##' @return dataframe
##' @author Bjarki ??r Elvarsson
read.gadget.parameters <- function(file='params.in'){

  params <- tryCatch(read.table(file,header=TRUE,
                       comment.char=';',
                       stringsAsFactors=FALSE),
                     error = function(e){
                       print(sprintf('Error in read.gadget.parameters -- %s cannot be read', file))
                       return(NULL)
                     })
  if(is.null(params))
    return(params)
  row.names(params) <- params$switch
  ## digg through the data written in the header
  header <- readLines(file)
  header <- header[grepl(';',substring(header,1,1))]

  num.func <- function(pre){
    post <- ' function evaluations'
    num <- as.numeric(gsub(post,'',gsub(pre,'',header[grepl(pre,header)])))
    num <- ifelse(length(num) == 0,NA,num)
    return(num)
  }

  ## Number of function evaluations
  sim.func.str <- '; Simulated Annealing algorithm ran for '
  sim.pos <- grep(sim.func.str,header)
  
  hook.func.str <- '; Hooke & Jeeves algorithm ran for '
  hook.pos <- grep(hook.func.str,header)
  
  bfgs.func.str <- '; BFGS algorithm ran for '
  bfgs.pos <- grep(bfgs.func.str,header)
  

  
  ## final likelihood values from each component
  lik.func <- function(i){
    lik <- as.numeric(gsub('; and stopped when the likelihood value was ','',
                           header[i]))
    lik <- ifelse(length(lik) == 0,NA,lik)
    return(lik)
  }
  
  ## convergence
  conv.func <- function(i){
    error <- '; because an error occured during the optimisation'
    converged <- '; because the convergence criteria were met'
    maxiter <-
      '; because the maximum number of function evaluations was reached'
    msg <- ifelse(header[i]==error,'Error in optimisation',
                  ifelse(header[i]==converged,'Convergence criteria were met',
                         ifelse(header[i]==maxiter,
                                'Maximum number of iterations',
                                'No information')))
    ifelse(length(msg)==0,NA,msg)
  }
  
  
  tmp <- list(simann=data.frame(numFunc=num.func(sim.func.str),
                lik.val=lik.func(sim.pos+1),
                convergence=conv.func(sim.pos+2),
                stringsAsFactors=FALSE),
              hooke=data.frame(numFunc=num.func(hook.func.str),
                lik.val=lik.func(hook.pos+1),
                convergence=conv.func(hook.pos+2),
                stringsAsFactors=FALSE),
              bfgs=data.frame(numFunc=num.func(bfgs.func.str),
                lik.val=lik.func(bfgs.pos+1),
                convergence=conv.func(bfgs.pos+2),
                stringsAsFactors=FALSE))
  class(params) <- c('gadget.parameters',class(params))
  attr(params,'optim.info') <- tmp
  return(params)
}
##' Write gadget input parameters
##' @title Write params
##' @param params params dataframe
##' @param file a string naming the file to write to
##' @param columns should a conventional column based parameter file be written out or
##' should a line based parameter (used when evaluating gadget on a matrix) be used.
##' @return a string containing the text of the params file (if desired)
##' @author Bjarki ??r Elvarsson
write.gadget.parameters <- function(params,file='params.out',columns=TRUE){
  input.text <-
    paste("; input file for the gadget model",
          "; created automatically from Rgadget",
          sprintf('; %s - %s',file,date()),
          paste(names(params),collapse='\t'),
          sep='\n')
  
  if(!columns)
    write(paste(c('switches',names(params)),collapse='\t'),file=file)
  else
    write(input.text,file)
  write.table(params,file=file,
              quote=FALSE, row.names=FALSE, col.names=FALSE,
              append=TRUE, sep="\t")

}
##' .. content for \description{} (no empty lines) ..
##'
##' .. content for \details{} ..
##' @title 
##' @param lik 
##' @param output 
##' @param file 
##' @return 
##' @author Bjarki Thor Elvarsson
make.gadget.printfile <- function(main='main',output='out',
                                  aggfiles='print.aggfiles',
                                  file='printfile'){
  main <- read.gadget.main(main)
  lik <- read.gadget.likelihood(main$likelihoodfiles)
  stocks <- read.gadget.stockfiles(main$stockfiles)
  fleets <- read.gadget.fleet(main$fleetfiles)
  
  header <-
    paste(sprintf('; gadget printfile, created in %s',Sys.Date()),
          '[component]',
          'type\tlikelihoodsummaryprinter',
          sprintf('printfile\t%s/likelihoodsummary', output),
          ';',sep='\n')

  lik.template <-
    paste('[component]',
          'type\tlikelihoodprinter',
          'likelihood\t%1$s',
          sprintf('printfile\t%s/%%1$s',output),
          ';', sep='\n')

  stock.std <-
    paste('[component]',
          'type\tstockstdprinter',
          'stockname\t%1$s',
          sprintf('printfile\t%s/%%1$s.std',output),
          'yearsandsteps\t all 1',sep='\n')

  stock.full <-
    paste('[component]',
          'type\tstockprinter',
          'stocknames\t%1$s',
          sprintf('areaaggfile\t%s/%%1$s.area.agg',aggfiles),
          sprintf('ageaggfile\t%s/%%1$s.allages.agg',aggfiles),
          sprintf('lenaggfile\t%s/%%1$s.len.agg',aggfiles),
          sprintf('printfile\t%s/%%1$s.full',output),
          'yearsandsteps\t all 1',sep='\n')

  predator <-
    paste('[component]',
          'type\tpredatorpreyprinter',
          'predatornames\t%2$s',
          'preynames\t%1$s',
          sprintf('areaaggfile\t%s/%%1$s.area.agg',aggfiles),
          sprintf('ageaggfile\t%s/%%1$s.age.agg',aggfiles),
          sprintf('lenaggfile\t%s/%%1$s.alllen.agg',aggfiles),
          sprintf('printfile\t%s/%%1$s.prey',output),
          'yearsandsteps\tall all',
          sep = '\n')

  dir.create(aggfiles, showWarnings = FALSE)
  dir.create(output, showWarnings = FALSE)

  l_ply(stocks,
        function(x){
          writeAggfiles(x,folder=aggfiles)          
        })
  
  txt <- sprintf(lik.template,
                 subset(lik$weights,
                        !(type %in% c('understocking','penalty',
                                      'migrationpenalty')))[['name']])
  write(paste(header,paste(txt,collapse='\n'),
              paste(sprintf(stock.std,laply(stocks,
                                            function(x) x@stockname)),
                    collapse='\n'),
              paste(sprintf(stock.full,laply(stocks,
                                            function(x) x@stockname)),
                    collapse='\n'),
              paste(sprintf(predator,laply(stocks,
                                           function(x) x@stockname),
                            paste(fleets$fleet$fleet,collapse = ' ')),
                    collapse='\n'),
              sep='\n'),
        file=file)

}

##' Read gadget printfile
##' @title Read gadget printfile
##' @param file string containing the path to the printfile
##' @return list of the prinfile components.
##' @author Bjarki Thor Elvarsson
read.gadget.printfile <- function(file='printfile'){
  printfile <- strip.comments(file)
  comp.loc <- grep('component',printfile)
  name.loc <- grep('printfile',printfile)
  name.print <- sapply(printfile[grep('printfile',printfile)],function(x) x[2])
  name.print <- sapply(strsplit(name.print,'/'),function(x) tail(x,1))
  diff.comp <- diff(c(comp.loc,length(printfile)+1))-1
#  type.loc <- grep('type',printfile)
#  types <- unique(sapply(printfile[type.loc],function(x) x[2]))
#  print.types <- llply(types,function(x) grep(x,printfile))
#  names(print.types) <- types
  
  tmp.func <- function(restr){
    names.tmp <- sapply(printfile[restr],       
                        function(x) x[1])
    tmp <- lapply(sapply(printfile[restr],                                
                         function(x) x[-1]),unlist)
    names(tmp) <- names.tmp
    return(tmp)
  }
  
  print <- llply(1:length(comp.loc),
                 function(x) tmp.func(comp.loc[x]+1:diff.comp[x]))
  names(print) <- name.print
  return(print)
}
##' Write the gadget prinfile to file, optionally changing the output directory
##' of the printfile components.
##' @title Write Gadget printfile
##' @param print printfile object
##' @param file string containing the desired location of the printfile
##' @param output.dir where should the output from the prinfile components be written, defaults to 'out'.
##' @return (invisible) text of the printfile if desired.
##' @author Bjarki Thor Elvarsson
write.gadget.printfile <- function(print,file='prinfile',output.dir='out'){
  print.text <- sprintf('; Printfile for gadget, created by Rgadget\n; %s',file)
  for(name in names(print)){
    tmp <- print[name][[name]]
    tmp[['printfile']] <- paste(output.dir,name,sep='/')
    print.text <- paste(print.text,
                        ';\n[component]',
                        paste(names(tmp),sapply(tmp,function(x) paste(x,collapse='\t')),
                              sep='\t',collapse='\n'),
                        sep='\n')
  }
  write(print.text,file)
  invisible(print.text)
}


read.gadget.results <- function(grouping=list(),
                                final=list(final='final'),
                                wgts='WGTS',
                                normalize = FALSE                               
                                ){

  read.gadget.SS <- function(file='lik.out'){
    lik.out <- readLines(file)
    SS <- as.numeric(clear.spaces(strsplit(lik.out[length(lik.out)],
                                           '\t\t')[[1]][2]))
    return(SS)
  }
  likelihood <- read.gadget.likelihood(sprintf('%s/likelihood.final',wgts))
  grouping <- read.gadget.grouping(lik=likelihood,wgts=wgts)
  comp.tmp <- subset(likelihood$weights,
                     !(type %in% c('penalty','understocking',
                                   'migrationpenalty'))&
                     !(name %in% unlist(grouping)))$name
  comp <- within(grouping,
                 for(item in comp.tmp){
                   assign(item, item)
                 })
  comp$item <- NULL
  res <-
    rbind.fill(ldply(comp,
                     function(x)
                     read.gadget.SS(paste(wgts,
                                          paste('lik',
                                                paste(x,collapse='.'),
                                                sep='.'),sep='/'))),
               ldply(final,
                     function(x)
                     read.gadget.SS(paste(wgts,
                                          paste('lik',
                                                paste(x,collapse='.'),
                                                sep='.'),sep='/'))))
  names(res)[-1] <- likelihood$weights$name
  rownames(res) <- res$.id
  if(normalize){
    for(group in names(grouping)){
      for(comp in grouping[[group]]){
        res[,comp] <- res[,comp]/res[group,comp]
      }
    }
  }
     
  return(res)
}


##' Read data used by the various components
##' @title Read likelihood data
##' @param likelihood object of class gadget.likelihood
##' @return list of dataframes and degress of freedom
##' @author Bjarki ??r Elvarsson
read.gadget.data <- function(likelihood){
  read.agg <- function(x, first = FALSE){

    if(first){
      return(sapply(strsplit(readLines(x),'[\t ]'),function(x) x[1])) 
    }  else {
      return(read.table(x,stringsAsFactors=FALSE,comment.char=';'))      
    }
  }
  read.func <- function(x){
    
    dat <- tryCatch(read.table(x$datafile,comment.char=';'),
                    error = function(x) NULL)
    
    area.agg <- tryCatch(read.agg(x$areaaggfile, first = TRUE),
                         warning = function(x) NULL,
                         error = function(x) NULL)
    age.agg <- tryCatch(read.agg(x$ageaggfile, first = TRUE),
                        warning = function(x) NULL,
                        error = function(x) NULL)
    len.agg <- tryCatch(read.agg(x$lenaggfile),
                        warning = function(x) NULL,
                        error = function(x) NULL)
    
    if(x$type=='catchdistribution'){
      names(dat) <- c('year','step','area','age','length','number')
    }
    if(x$type=='catchstatistics'){
      if(x[['function']] %in%
         c('lengthcalcstddev','weightnostddev','lengthnostddev'))
        names(dat) <- c('year','step','area','age','number','mean')
      if(x[['function']] %in% c('lengthgivenstddev','weightgivenstddev',
                           'lengthgivenvar'))
        names(dat) <- c('year','step','area','age','number','mean','stddev') 
    }
    if(x$type=='stockdistribution'){
      names(dat) <- c('year','step','area','stock','age','length','number')
    }
    if(x$type=='surveyindices'){
      if(x$sitype %in% c('lengths','fleets') )
        names(dat) <- c('year','step','area','length','number')
      if(x$sitype=='ages')
        names(dat) <- c('year','step','area','age','number')
      if(x$sitype=='acoustic')
        names(dat) <- c('year','step','area','survey','acoustic')
      if(x$sitype=='effort')
        names(dat) <- c('year','step','area','fleet','effort')
    }
    if(x$type == 'surveydistribution'){
      names(dat) <- c('year','step','area','age','length','number')
    }
    if(x$type=='stomachcontent'){
      names(dat) <- c('year','step','area','predator','prey','ratio')
    }
    if(x$type=='recaptures'){
      names(dat) <- c('tagid','year','step','area','length','number')
    }
    if(x$type=='recstatistics'){
      if(x[['function']]=='lengthgivenstddev')
        names(dat) <- c('tagid','year','step','area','number','mean','stddev')
      else
        names(dat) <- c('tagid','year','step','area','number','mean')
    }
    if(x$type=='catchinkilos'){
#      if(x$aggregationlevel==1)
        names(dat) <- c('year','area','fleet','biomass')
#      else
#        names(dat) <- c('year','step','area','fleet','biomass')
    }
    
    restr.area <- (dat$area %in% area.agg)
    if(length(restr.area)==0)
      restr.area <- TRUE
    restr.age <- (dat$age %in% age.agg)
    if(length(restr.age)==0)
      restr.age <- TRUE
    restr.len <- (dat$length %in% len.agg[,1])
    if(length(restr.len)==0)
      restr.len <- TRUE
    dat <- dat[restr.area&restr.age&restr.len,]
    if('length' %in% names(dat)){
      names(len.agg)[1:3] <- c('length','lower','upper')
      dat <- merge(dat,len.agg,all.x=TRUE)
    }
    return(dat)
  }

  lik.dat <- dlply(subset(likelihood$weights,
                          !(type %in% c('penalty', 'understocking',
                                        'migrationpenalty'))),
                   'type',
                   function(x) dlply(x,'name',read.func))

  df <- lapply(lik.dat,function(x)
              sapply(x,function(x){
                x <- na.omit(x)
                tmp <- 0
                if(length(intersect(c('lower','upper'),names(x)))>0){
                  tmp <- 2
                }
                nrow(x[x[,ncol(x)-tmp]>0,])
              }))
  gadDat <- list(dat=lik.dat,df=df)
  class(gadDat) <- c('gadgetData','list')
  return(gadDat)
}

##' Read optinfo parameters from file
##' @title Read gadget  
##' @param file location of the optinfofile
##' @return optinfo object
##' @author Bjarki ??r Elvarsson
read.gadget.optinfo <- function(file='optinfofile'){
  optinfo <- readLines(file)
  optinfo <- na.omit(sapply(strsplit(optinfo,';'),function(x) x[1]))
  simann <- (1:length(optinfo))[(optinfo == '[simann]')]
  hooke <- (1:length(optinfo))[(optinfo == '[hooke]')]
  bfgs <- (1:length(optinfo))[(optinfo == '[bfgs]')]

  vars <- c(simann-1,hooke-1,bfgs-1,length(optinfo))
  simann.end <- min(vars[vars>simann])
  hooke.end <-  min(vars[vars>hooke])
  bfgs.end <- min(vars[vars>bfgs])
  tmp.func <- function(start,end){
    x <-  as.data.frame(clear.spaces(optinfo[start:end]),
                        stringsAsFactors=FALSE)
    names(x) <- x[1,]
    x <- x[2,]
    return(x)
  }
  optinfo <- list(simann = tmp.func(simann+1,simann.end),
                  hooke = tmp.func(hooke+1,hooke.end),
                  bfgs = tmp.func(bfgs+1,bfgs.end))
  class(optinfo) <- c('gadget.optinfo',class(optinfo))
  return(optinfo)
}
##' Write optinfo to file
##' @title Write gadget optinfo
##' @param optinfo optinfo object
##' @param file file
##' @param location location
##' @return text of the optinfofile (if desired)
##' @author Bjarki ??r Elvarsson
write.gadget.optinfo<-function(optinfo,file='optinfofile'){
  opt.text <- 
    paste("; optimisation file for gadget",
          "; created in R-gadget",
          sprint('; %s - %s',file,date()),
          sep='\n')
  for(comp in names(optinfo)){
    opt.text <-
      paste(opt.text,
            sprintf('[%s]',comp),
            paste(names(optinfo[[comp]]),
                  optinfo[[comp]],
                  sep='\t\t',collapse='\n'),
            sep='\n')
  }
  write(opt.text,file=file)
  invisible(opt.text)
}



##' Read in the gadget likelihood output.
##' @title Read gadget lik.out 
##' @param file string containing the name of the file
##' @return a list containing the swicthes (names of variable), weigths
##' (l?kelihood components) and data (dataframe with the parameter values,
##' likelihood component values and the final score.
##' @author Bjarki Thor Elvarsson
read.gadget.lik.out <- function(file='lik.out',suppress=FALSE){
  if(!file.exists(file)){
    return(NULL)
  }     
  lik <-  tryCatch(readLines(file),
                   error = function(e){
                     if(!suppress)
                       print(sprintf('file corrupted -- %s', file))
                     return(NULL)
                   })
  
  i <- grep("Listing of the switches",lik)
  i1 <- grep("Listing of the likelihood components",lik)
  i2 <- grep("Listing of the output from the likelihood",lik)

  if(is.null(i)|is.null(i1)|is.null(i2)){
    warning(sprintf('file %s is corrupt',file))
    return(NULL)
  }
  
  switches <- tryCatch(lapply(strsplit(lik[(i+1):(i1-2)],'\t'),unique),
                       error = function(e){
                         if(!suppress)
                           print(sprintf('file corrupted -- %s', file))
                         return(NULL)
                       })
  if(is.null(switches)){
    return(NULL)
  }
  names(switches) <- sapply(switches,function(x) x[1])
  switches <- lapply(switches,function(x) x[-1])

  weights <- t(sapply(strsplit(lik[(i1+3):(i2-2)],'\t'),function(x) x))
  weights <- as.data.frame(weights,stringsAsFactors=FALSE)
  weights$V2 <- as.numeric(weights$V2)
  weights$V3 <- as.numeric(weights$V3)
  names(weights) <- c('Component','Type','Weight')
  
  data <- read.table(file,skip=(i2+1))
  names(data) <- c('iteration',names(switches),weights$Component,'score')
  attr(data,'Likelihood components') <- weights$Component
  attr(data,'Parameters') <- names(switches)
  lik.out <- list(switches=switches,weights=weights,data=data)
  class(lik.out) <- c('gadget.lik.out',class(lik.out))
  return(lik.out)
}



##' Helper function created to clear out all comments (indicated by ';') and 
##' unwanted spaces from gadget input and output files.
##' @title Strip comments
##' @param file location of the gadget input file
##' @return list containing the lines from the file stripped of unwanted text.
##' @author Bjarki Thor Elvarsson
strip.comments <- function(file='main'){
  tmp <- unlist(llply(file,readLines))
  main <- sub('\t+$',' ',tmp)
  main <- sub(' +$','',main)
  main <- gsub('(','( ',main,fixed=TRUE)
  main <- gsub(')',' )',main,fixed=TRUE)
  main <- main[main!='']
  comments <- main[grepl(';',substring(main,1,1))]
  main <- main[!grepl(';',substring(main,1,1))]
  main <- sapply(strsplit(main,';'),function(x) x[1])  
  main <- clear.spaces(main)
#  attr(main,'comments') <- comments
  return(main)
}

##' <description>
##'
##' <details>
##' @title 
##' @param main.file 
##' @return 
##' @author Bjarki Thor Elvarsson
read.gadget.model <- function(main.file='main'){
  gadget.model <-
    within(list(),
           main <- read.gadget.main(main.file),
           time <- read.gadget.time(main$timefile),
           area <- read.gadget.area(main$areafile),
           print <- read.gadget.printfile(main$printfile),
           stocks <- read.gadget.stockfiles(main$stockfiles),
           tagging <- read.gadget.tagfiles(main$tagfiles),
           otherfood <- read.gadget.otherfood(main$otherfoodfiles),
           fleets <- read.gadget.fleet(main$fleetfiles),
           likelihood <- read.gadget.likelihood(main$likelihoodfiles)
           )
  class(gadget.model) <- c('gadget.model',class(gadget.model))
}

##' <description>
##'
##' <details>
##' @title 
##' @param stock.files 
##' @return 
##' @author Bjarki Thor Elvarsson
read.gadget.stockfiles <- function(stock.files){
  tmp.func <- function(sf){
    stock <- strip.comments(sf)

    growth.loc <- grep('doesgrow', stock, ignore.case = TRUE)
    mort.loc <- grep('naturalmortality', stock, ignore.case = TRUE)
    init.loc <- grep('initialconditions', stock, ignore.case = TRUE)
    pred.loc <- init.loc - 1
    if(tolower(stock[init.loc+1])=='numbers'){
      init.loc <- init.loc + 1
    }
#    initfile.loc <- #grep('normalcondfile',stock, ignore.case =TRUE)
#      c(grep('normalcondfile',stock, ignore.case =TRUE),
#        grep('normalparamfile',stock, ignore.case =TRUE),
#        grep('numberfile',stock, ignore.case =TRUE))
    eat.loc <- grep('doeseat', stock, ignore.case = TRUE)
    migrate.loc <- grep('doesmigrate', stock, ignore.case = TRUE)
    initfile.loc <- migrate.loc -1 
    mature.loc <- grep('doesmature', stock, ignore.case = TRUE)
    move.loc <- grep('doesmove', stock, ignore.case = TRUE)
    renew.loc <- grep('doesrenew', stock, ignore.case = TRUE)
    spawn.loc <- grep('doesspawn', stock, ignore.case = TRUE)
    stray.loc <- grep('doesstray', stock, ignore.case = TRUE)

    growth.info <- function(tmp){
      if(length(tmp)==1)
        tmp <- new('gadget-growth')
      else {
        names.tmp <- sapply(tmp,function(x) x[1])
        tmp <- llply(tmp,function(x) paste(x[-1],collapse=' '))
        names(tmp) <- names.tmp

        if(is.null(tmp$growthfunction))
          tmp$growthfunction <- vector()
        if(is.null(tmp$wgrowthfunction))
          tmp$wgrowthfunction <- vector()
        if(is.null(tmp$lgrowthfunction))
          tmp$lgrowthfunction <- vector()
        if(is.null(tmp$yeareffect))
          tmp$yeareffect <- vector()
        if(is.null(tmp$stepeffect))
          tmp$stepeffect <- vector()
        if(is.null(tmp$areaeffect))
          tmp$areaeffect <- vector()
        
        tmp <- new('gadget-growth',
                   growthfunction = tmp$growthfunction,
                   ## growthfunction parameters
                   growthparameters = tmp$growthparameters,
#                   wgrowthparameters = tmp$wgrowthparameters,
#                   lgrowthparameters = tmp$lgrowthparameters,
#                   yeareffect = tmp$yeareffect,
#                   stepeffect = tmp$stepeffect,
#                   areaeffect = tmp$areaeffect,
                   ## growth implementation
                   beta = tmp$beta,
                   maxlengthgroupgrowth = tmp$maxlengthgroupgrowth)
      }
      return(tmp)
    }

    prey.info <- function(tmp){
      if(length(tmp)==1){
        tmp <- new('gadget-prey')
      } else {
        tmp <- new('gadget-prey',
                   name = stock[[1]][2],
                   preylengths = read.table(tmp[[2]][2],comment.char=';'),
                   energycontent = ifelse(length(tmp)==3,as.numeric(tmp[[3]][2]),
                     1))
      }
      return(tmp)
    }

    pred.info <- function(tmp){
      if(length(tmp)==1){
        tmp <- new('gadget-predator')
      } else {
        pref.loc <- grep('preference',tmp)
        maxcon.loc <- grep('maxconsumption',tmp)
        half.loc <- grep('halffeedingvalue',tmp)
        suit <- ldply(2:(pref.loc-1),
                      function(x){
                        c(stock = tmp[[x]][1],
                          suitability = paste(tmp[[x]][-1],collapse=' '))
                      })
        pref <- ldply(pref.loc,
                      function(x){
                        c(stock = tmp[[x]][1],
                          preference = paste(tmp[[x]][-1],collapse=' '))
                      })
        tmp <- new('gadget-predator',
                   suitability = suit,
                   preferene = pref,
                   maxconsumption = as.numeric(tmp[[maxcon.loc]][2]),
                   halffeedingvalue = as.numeric(tmp[[half.loc]][2]))
      }
      return(tmp)
    }

    
    
    st <-
      new('gadget-stock',
          stockname = stock[[1]][2],
          livesonareas = as.numeric(stock[[2]][-1]),
          minage = as.numeric(stock[[3]][2]),
          maxage = as.numeric(stock[[4]][2]),
          minlength = as.numeric(stock[[5]][2]),
          maxlength = as.numeric(stock[[6]][2]),
          dl = as.numeric(stock[[7]][2]),
          refweight = read.table(stock[[8]][2],comment.char=';'),
          growthandeatlengths = read.table(stock[[9]][2],comment.char=';'),
          doesgrow = as.numeric(stock[[growth.loc]][2]),
          growth = growth.info(stock[growth.loc:(mort.loc-1)]),
          naturalmortality = as.numeric(stock[[mort.loc]][-1]),
          iseaten = as.numeric(stock[[mort.loc+1]][2]),
          preyinfo = prey.info(stock[(mort.loc+1):(eat.loc-1)]),
          doeseat = as.numeric(stock[[eat.loc]][2]),
          predator = pred.info(stock[eat.loc:(pred.loc)]),
          initialconditions = list(minage = stock[[init.loc + 1]][2],
            maxage = stock[[init.loc + 2]][2],
            minlength = stock[[init.loc + 3]][2],
            maxlength = stock[[init.loc + 4]][2],
            dl = ifelse(tolower(stock[[init.loc + 5]][1])=='dl',
              stock[[init.loc + 5]][2],1),
            sdev = ifelse(tolower(stock[[init.loc + 6]][1])=='sdev',
              stock[[init.loc + 6]][2], 1)),
          initialdata = read.gadget.table(stock[[initfile.loc]][2]),
          doesmigrate = as.numeric(stock[[migrate.loc]][2]),
          doesmature =  as.numeric(stock[[mature.loc]][2]),
          doesmove = as.numeric(stock[[move.loc]][2]),
          doesrenew =  as.numeric(stock[[renew.loc]][2]),
          renewal = list(
            minlength = ifelse(as.numeric(stock[[renew.loc]][2]) == 0,
              '',
              as.numeric(stock[[renew.loc + 1]][2])),
            maxlength = ifelse(as.numeric(stock[[renew.loc]][2]) == 0,
              '',
              as.numeric(stock[[renew.loc + 2]][2]))),
          renewal.data = tryCatch(read.gadget.table(stock[[renew.loc+3]][2]),
            error=function(x){
              tryCatch(read.gadget.table(stock[[renew.loc+4]][2]),
                       error=function(x) data.frame(text='No renewal data'))
            }),
          doesspawn = as.numeric(stock[[spawn.loc]][2]),
          doesstray = ifelse(length(stray.loc)==0,
            0,as.numeric(stock[[stray.loc]][2]))
          )
        
    return(st)
  }
  stocks <- llply(stock.files,tmp.func)
  return(stocks)
}

##' <description>
##'
##' <details>
##' @title 
##' @param area.file 
##' @return 
##' @author Bjarki Thor Elvarsson
read.gadget.area <- function(area.file='area'){
  area <- strip.comments(area.file)
  areas <- area[[1]][-1]
  size <- area[[2]][-1]
  temperature <-
    as.data.frame(t(sapply(area[-c(1:3)],function(x) as.numeric(x))))
  names(temperature) <- c('year','step','area','temperature')
  area <- list(areas=areas,size=size,temperature=temperature)
  class(area) <- c('gadget.area',class(area))
  return(area)
}

##' <description>
##'
##' <details>
##' @title 
##' @param area 
##' @param file 
##' @return 
##' @author Bjarki Thor Elvarsson
write.gadget.area <- function(area,file='area'){
  header <- sprintf('; time file created in Rgadget\n; %s - %s',file,date())
  area.file <-
    paste(header,
          paste('areas',paste(area$areas,collapse=' '),sep='\t'),
          paste('size',paste(area$size,collapse=' '),sep='\t'),
          'temperature',
          '; year - step - area - temperature',
          sep='\n')
  write(area.file,file=file)
  write.table(area$temperature,file=file,col.names=FALSE,append=TRUE,
              quote=FALSE,sep='\t',row.names=FALSE)
}
##' <description>
##'
##' <details>
##' @title 
##' @param time.file 
##' @return 
##' @author Bjarki Thor Elvarsson
read.gadget.time <- function(time.file='time'){
  time <- strip.comments(time.file)
  time.names <- sapply(time,function(x) x[1])
  time <- sapply(time,function(x) as.numeric(x[-1]))
  names(time) <- time.names
  if(sum(time$notimesteps[-1])!=12)
    warning('Error in timefile - notimesteps does not sum to 12')
  if(length(time$notimesteps[-1])!=time$notimesteps[1])
    warning('Error in timefile - notimesteps does not contain the right number of timesteps')
  time$notimesteps <- time$notimesteps[-1]
  class(time) <- c('gadget.time',class(time))
  return(time)
}

##' <description>
##'
##' <details>
##' @title 
##' @param time 
##' @param file 
##' @return 
##' @author Bjarki Thor Elvarsson
write.gadget.time <- function(time,file='time'){
  header <- sprintf('; time file created in Rgadget\n; %s - %s',file,date())
  time.file <-
    paste(header,
          paste('firstyear',time$firstyear,sep='\t'),
          paste('firststep',time$firststep,sep='\t'),
          paste('lastyear',time$lastyear,sep='\t'),
          paste('laststep',time$laststep,sep='\t'),
          paste('notimesteps',
                paste(length(time$notimesteps),
                      paste(time$notimesteps,collapse=' ')),
                sep='\t'),
          sep='\n')
  write(time.file,file=file)
}

##' Gadget Penalty file
##' @param file name of the file that is to contain the penalty
write.gadget.penalty <- function(file='penaltyfile'){
  penalty <- paste("; penalty file for the gadget example",
                   sprintf('; %s created at %s using Rgadget',file,date()),
                   "; switch - power - lower - upper",
                   "default\t2\t10000\t10000 ; defaults",
                   sep='\n')
  write(penalty,file=file)
}



##' .. content for \description{} (no empty lines) ..
##'
##' .. content for \details{} ..
##' @title 
##' @param params.file 
##' @param bs.wgts 
##' @param bs.samples 
##' @param bs.lik 
##' @param lik.pre 
##' @param params.pre 
##' @param parallel 
##' @return 
##' @author Bjarki Thor Elvarsson
read.gadget.bootstrap <- function(params.file='params.in',
                                 bs.wgts='BS.WGTS',
                                 bs.samples=1:100,
                                 bs.lik='likelihood',
                                 lik.pre = 'lik.',
                                 params.pre = 'params.',
                                 parallel=FALSE
                                 ){
  wgts <- sprintf('%s/BS.%s',bs.wgts,bs.samples)
  dboot <- read.gadget.wgts(params.file,wgts,
                            bs.lik,lik.pre,params.pre,parallel)
  return(dboot)
}

##' .. content for \description{} (no empty lines) ..
##'
##' .. content for \details{} ..
##' @title 
##' @param params.file base parameter file
##' @param wgts location of the reweighting folder
##' @param likelihood likelihood file
##' @param lik.pre strings matching the likelihood output
##' @param params.pre strings matching the parameter estimates
##' @param parallel should the files be read in parallel
##' @return data.frame with parameter estimates and likelihood output from the iterative reweighting folder.
##' @author Bjarki Thor Elvarsson
read.gadget.wgts <- function(params.file = 'params.in',
                             wgts = 'WGTS',
                             likelihood = 'likelihood',
                             lik.pre = 'lik.',
                             params.pre = 'params.',
                             parallel=FALSE){
  
  params.in <- read.gadget.parameters(params.file)
  bs.lik <- read.gadget.likelihood(likelihood)
  files <- unique(list.files(wgts))

  lik.pre.g <- paste('^',gsub('.','[^a-zA-Z]',lik.pre,fixed=TRUE),sep='')
  params.pre.g <- paste('^',gsub('.','[^a-zA-Z]',params.pre,fixed=TRUE),sep='')
  
  liks <- unique(files[grep(lik.pre.g,files)])
  comps <- gsub(lik.pre.g,'',liks)

  tmp.func <- function(path){
    read.gadget.SS <- function(file='lik.out'){
      lik.out <- read.gadget.lik.out(file)

      if(is.null(lik.out)){
        SS <- as.data.frame(t(rep(NA,length(bs.lik$weights$weight))))
        names(SS) <- bs.lik$weights$name
      } else {
        SS <- lik.out$data[intersect(bs.lik$weights$name,names(lik.out$data))]
      }
      return(SS)
    }
    path.f <- list.files(path)
    liks <- path.f[grep(lik.pre,path.f)]
    params <- path.f[grep(params.pre.g,path.f)]
    ldply(intersect(comps,unique(c(gsub(params.pre.g,'',params),
                                   'init'))),
          function(x){
            if(x=='init')
              tmp <- params.in
            else
              tmp <- read.gadget.parameters(sprintf('%s/%s%s',
                                                    path,params.pre,x))
            if(is.null(tmp)){
              tmp <- params.in
              tmp$value <- NA*tmp$value
              ss <- as.data.frame(t(rep(NA,length(bs.lik$weights$weight))))
              names(ss) <- bs.lik$weights$name
            } else {
              ss <- read.gadget.SS(sprintf('%s/%s%s',path,lik.pre,x))
            }
            optim  <- ldply(attributes(tmp)$optim.info,
                            function(x) cbind(fake.id=1,x))
            optim <- reshape(optim,idvar='fake.id',
                             timevar='.id',direction='wide')
            optim$fake.id <- NULL
            dtmp <- cbind(bs.data=tail(unlist(strsplit(path,'/')),1),
                          comp=x,
                          t(tmp['value']),
                          ss,
                          optim)
            
            return(dtmp)
          }
          )
  }
  dparam <- ldply(wgts,tmp.func,.parallel=parallel)
  attr(dparam,'init.param') <- params.in
  return(dparam)
}

read.gadget.wgtsprint <- function(wgts = 'WGTS',
                                  comp = 'final',
                                  out.pre = 'out.',
                                  parallel = FALSE){
  bs.print <- llply(sprintf('%s/%s%s',wgts,out.pre,comp),
                    read.printfiles,.parallel=parallel) 
  names(bs.print) <- comp
  tmp <- llply(names(bs.print[[1]]),
               function(x) ldply(bs.print,function(y) y[[x]]),.parallel=TRUE)
  names(tmp) <- names(bs.print[[1]])
  return(tmp)
}

read.gadget.bootprint <- function(bs.wgts='BS.WGTS',
                                  bs.samples=1:1000,
                                  printfile='printfile',
                                  final = 'final'){
  printfile <- read.gadget.printfile(printfile)
  run.func <- function(bs.data){
    path <- sprintf('%s/BS.%s',bs.wgts,bs.data)
    
    dir.create(paste(path,sprintf('out.%s',final),sep='/'))
    main.print <- read.gadget.main(sprintf('%s/main.%s',path,final))
    main.print$printfiles <- sprintf('%s/print.%s',path,final)
    write.gadget.main(main.print,sprintf('%s/main.print',path))
    write.gadget.printfile(printfile,file=sprintf('%s/print.%s',path,final),
                           output.dir=paste(path,sprintf('out.%s',final),
                             sep='/'))
    callGadget(s=1,main=sprintf('%s/main.print',path),
               i=sprintf('%s/params.%s',path,final),
               o=sprintf('%s/lik.print',path))
    out <- read.printfiles(sprintf('%s/out.%s',path,final))
    return(out)
  }
  bs.print <- llply(bs.samples,
                    run.func,.parallel=TRUE)
  names(bs.print) <- sprintf('BS.%s',bs.samples)
  tmp <- llply(names(printfile),function(x) ldply(bs.print,function(y) y[[x]]))
  names(tmp) <- names(printfile)
  return(tmp)
}


##' .. content for \description{} (no empty lines) ..
##'
##' .. content for \details{} ..
##' @title 
##' @param txt 
##' @return 
##' @author Bjarki Thor Elvarsson
merge.formula <- function(txt){
  openP <- grep('(',txt,fixed=TRUE)
  closeP <- grep(')',txt,fixed=TRUE)
  if(length(openP) + length(closeP) == 0)
    return(txt)
  
  if(length(openP) != length(closeP))
    stop('numbers of paranthesis dont match in gadget formula')

  braces <- data.frame(begin=openP,end=closeP,group=openP)
  for(i in 1:length(openP)){
    braces$end[i] <- closeP[i]
    braces$begin[i] <- openP[max(which(openP < closeP[i]))]
    openP[max(which(openP < closeP[i]))] <- length(txt)
  }
  braces <- arrange(braces, begin)
  for(i in 1:length(openP)){
    braces$group[braces$end<braces$end[i] & braces$end>braces$begin[i]] <-
      braces$group[i]
  }

  braces <- ddply(braces,'group',function(x) head(x,1))
  for(i in length(braces$group):1){
    txt[braces$begin[i]] <- paste(txt[braces$begin[i]:braces$end[i]],
                                  collapse=' ')
    txt <- txt[-c((braces$begin[i]+1):braces$end[i])]
  }
  return(txt)
}



##' Evaluate gadget formulas, which are in reverse polish notation, ie
##' '(* x y)' which is equivalent to 'x*y'. The evaluation supports the following
##' symbols '*','/','+','-','exp','log','sqrt'. The evaluation uses a gadget
##' parameter object for its evaluation.
##' @title eval.gadget.formula
##' @param gad.for gadget formula
##' @param par gadget parameters object
##' @return a vector of evaluated gadget formulas
##' @author Bjarki Thor Elvarsson
eval.gadget.formula <- function(gad.for,par){
  tmp <- strsplit(gsub(')',' )',gsub('(','',gad.for,fixed=TRUE)),' ')  
  ldply(tmp,
        function(x){
          x <- x[!x=='']
          par.ind <- grep('#',x,fixed=TRUE)
          x <- gsub("*","'*'(",x,fixed=TRUE)
          x <- gsub("/","'/'(",x,fixed=TRUE)
          x <- gsub("+","'+'(",x,fixed=TRUE)
          x <- gsub("- ","'-'(",x,fixed=TRUE)
          x <- gsub('exp','exp(',x,fixed = TRUE)
          x <- gsub('log','log(',x,fixed = TRUE)
          x <- gsub('sqrt','sqrt(',x,fixed = TRUE)
          x <- gsub('[0-9]+.[0-9]+#|[0-9]+#','#',x)
          x[par.ind] <- par[gsub('#','',x[par.ind],fixed=TRUE),'value']          
          x <- gsub(',)',')',gsub('(,','(',paste(x,collapse=','),fixed=TRUE),
                    fixed=TRUE)
          return(eval(parse(text=x)))
        })
}
##' .. content for \description{} (no empty lines) ..
##'
##' .. content for \details{} ..
##' @title read.gadget.table
##' @param file path to file
##' @param header logical, should the header be read from the file
##' @return data.frame with 
##' @author Bjarki Thor Elvarsson
read.gadget.table <- function(file,header=FALSE){
  dat <- strip.comments(file)
  if(class(dat) == 'list')
    gad.tab <- ldply(dat,merge.formula)
  else {
    gad.tab <- adply(dat,2,merge.formula)
    gad.tab$X1 <- NULL
  }
  if(header){
    comments <- attr(dat,'comments')
    header <- tail(comments,1)
    ## unfinised business
  }

  return(gad.tab)
}

##' <description>
##'
##' function only tested for linear and totalfleets>
##' @title 
##' @param fleet.file 
##' @return 
##' @author Bjarki Þór Elvarsson
read.gadget.fleet <- function(fleet.file='fleet'){
  fleet <- strip.comments(fleet.file)
  comp.loc <- grep('fleetcomponent',fleet)
  suit.loc <- grep('suitability',fleet)
  fleet.dat <-
    data.frame(fleet = laply(fleet[comp.loc+1],function(x) x[2]),
               type = laply(fleet[comp.loc+1],function(x) x[1]),
               livesonareas = laply(fleet[comp.loc+2],
                 function(x) paste(x[-1],collapse=' ')),
               multiplicative = laply(fleet[comp.loc+3],
                 function(x) as.numeric(x[2])),
               amount =  laply(fleet[c(comp.loc[-1]-1,
                 length(fleet))],
                 function(x) x[2]),
               stringsAsFactors=FALSE
               )
  diff.suit <- data.frame(fleet=laply(fleet[comp.loc+1],function(x) x[2]),
                          begin=suit.loc+1,
                          end=c(comp.loc[-1]-2,length(fleet)-1))
  prey <- ddply(diff.suit,'fleet',
                function(x){
                  ldply(fleet[x$begin:x$end],
                        function(x)
                        c(stock=x[1],suitability=x[3],
                          params=paste(tail(x,-3),collapse=' ')))
                  
                })
  return(list(fleet=fleet.dat,prey=prey))
}
##' <description>
##'
##' <details>
##' @title 
##' @param fleet 
##' @param file 
##' @return 
##' @author Bjarki Þór Elvarsson
write.gadget.fleet <- function(fleet,file='fleet'){
  base.text <-
    paste('[fleetcomponent]',
          '%s\t%s',
          'livesonareas\t%s',
          'multiplicative\t%s',
          'suitability',
          '%s',
          'amount\t%s',
          sep='\n')

  suit.text <- ddply(fleet$prey,'fleet',
                     function(x){
                       c(suitability=paste(x$stock,'function',
                           x$suitability,x$params,
                           sep='\t', collapse='\n'))
                     })
  tmp <- merge(fleet$fleet,suit.text,by='fleet')
  tmp$suitability <- ifelse(tmp$type=='quotafleet',
                            paste(tmp$suitability,
                                  sprintf('quotafunction\t%s\nbiomasslevel\t%s\nquotalevel\t%s\nselectstocks\t%s',
                                          tmp$quotafunction,tmp$biomasslevel,
                                          tmp$quotalevel,
                                          tmp$selectstocks),
                                  sep='\n'),
                            tmp$suitability)
  
  write(sprintf(base.text,tmp$type,tmp$fleet,tmp$livesonareas,
                tmp$multiplicative,tmp$suitability, tmp$amount),
        file=file)
  
}

get.gadget.suitability <- function(fleets,params,lengths,normalize=FALSE){
  ddply(fleets$prey,~fleet+stock,
        function(x){
          txt.split <- merge.formula(unlist(strsplit(x$params[1],' ')))
          suit.par <- eval.gadget.formula(txt.split,params)$V1
          suitability(suit.par,l = lengths, type = x$suitability,
                      to.data.frame = TRUE, normalize = normalize)
        })
}

get.gadget.growth <- function(stocks,params,dt=0.25,age.based=FALSE){
  ldply(stocks,function(x){
    txt.split <- merge.formula(unlist(strsplit(x@growth@growthparameters,' ')))
    txt.split <- c(txt.split,x@growth@beta,x@growth@maxlengthgroupgrowth)
    suit.par <- eval.gadget.formula(txt.split,params)$V1
    lt <- getLengthGroups(x)
    if(age.based){
      age <- x@minage:x@maxage
      data.frame(stock=x@stockname,age=age,
                 length=suit.par[1]*(1-exp(-suit.par[2]*age)))
    } else {
      melt(growthprob(lt,suit.par[5],suit.par[1],suit.par[2],dt,
                      suit.par[6],max(diff(lt))),
           varnames = c('lfrom','lto','lgrowth'))
    }
  })
}

get.gadget.recruitment <- function(stocks,params){
  ldply(stocks, function(x){
    if(x@doesrenew == 1){
      na.omit(data.frame(stock = x@stockname,
                         year=x@renewal.data$V1,
                         recruitment =
                         10000*unlist(eval.gadget.formula(x@renewal.data$V5,
                                                          params))))
    } else {
      data.frame(stock = x@stockname,year=NA,recruitment=NA)
    }
  })
}


get.gadget.catches <- function(fleets,params){
  tmp <- ddply(fleets$fleet,~fleet,
               function(x){
                 subset(read.gadget.table(x$amount),
                        V4 == x$fleet)
               })
  tmp$fleet <- NULL
  names(tmp) <- c('year','step','area','fleet','catch')
  tmp <- merge(merge(fleets$prey[c('fleet','stock')],tmp),
               fleets$fleet[c('fleet','type')])
  tmp$catch <- unlist(eval.gadget.formula(tmp$catch,params))
  return(tmp)
}

read.gadget.grouping <- function(lik = read.gadget.likelihood(),
                                 wgts = 'WGTS'){
  lik.tmp <- subset(lik$weights,
                    !(type %in% c('penalty','understocking',
                                  'migrationpenalty')))
  
  tmp <-
    ldply(lik.tmp$name,
          function(x){
            text <- gsub('params.','',
                        grep('params',list.files(wgts),value = TRUE))
            x1 <- gsub('.','\\.',x,fixed=TRUE)
            x1 <- paste('(',x1,'[[:punct:]]','|',x1,'$)',sep='')  
            pos <- grep(x1,text)            
            data.frame(name = x,
                       pos = pos,
                       ord = regexpr(x,text[pos])[1],
                       stringsAsFactors=FALSE)
          })
  tmp <- arrange(tmp,pos,ord)
  grouping <- dlply(tmp,~pos,function(x) as.character(x$name))
  names(grouping) <- unlist(llply(grouping,function(x) paste(x,collapse='.')))
  attributes(grouping)$split_labels <- NULL
  return(grouping)
}


gadget.fit <- function(wgts = 'WGTS', main.file = 'main',
                       fleet.predict = data.frame(fleet='comm',ratio=1),
                       mat.par=NULL){

  resTable <- read.gadget.results(wgts=wgts)
  nesTable <- read.gadget.results(wgts=wgts,normalize = TRUE)
  params <- read.gadget.parameters(sprintf('%s/params.final',wgts))
  lik <- read.gadget.likelihood(sprintf('%s/likelihood.final',wgts))
  lik.dat <- read.gadget.data(lik)
  
  ## model output, i.e printfiles
  make.gadget.printfile(main = main.file,
                        file = sprintf('%s/printfile.fit',wgts),
                        out = sprintf('%s/out.fit',wgts),
                        aggfiles = sprintf('%s/print.aggfiles',wgts))
  main <- read.gadget.main(file = main.file)
  main$printfiles <- sprintf('%s/printfile.fit',wgts)
  write.gadget.main(main,file = sprintf('%s/main.print',wgts))
  callGadget(s=1,
             i = sprintf('%s/params.final',wgts),
             main = sprintf('%s/main.print',wgts))
  out <- read.printfiles(sprintf('%s/out.fit',wgts))

  stocks <- read.gadget.stockfiles(main$stockfiles)
  fleets <- read.gadget.fleet(main$fleetfiles)
  catches <- get.gadget.catches(fleets,params)
  gss.suit <- ldply(stocks,
                    function(x){
                      get.gadget.suitability(fleets,params,
                                             getLengthGroups(x))
                    })
  stock.growth <- get.gadget.growth(stocks,params,age.based=TRUE)
  stock.recruitment <- get.gadget.recruitment(stocks,params)

  harv.suit <- function(l){
    ddply(merge(get.gadget.suitability(fleets,params,l),fleet.predict),~l,
          summarise, harv=sum(ratio*suit))$harv
  }
  
  ## merge data and estimates
  if('surveyindices' %in% names(lik.dat$dat)){
    sidat <- ldply(names(lik.dat$dat$surveyindices),
                   function(x){
                     sidat <-
                       merge(lik.dat$dat$surveyindices[[x]],
                             out[[x]],
                             by.y=c('year','label','step','area'),
                             by.x=c('year','length','step','area'),
                             all.y=TRUE)
                     sidat$length <- paste(sidat$lower,
                                           sidat$upper, sep = ' - ')
                     sidat$name <- x
                     sidat <- merge(sidat,
                                    subset(lik$surveyindices,
                                           select=c(name,stocknames)),
                                    by='name')
                     return(sidat)
                   })
  } else {
    sidat <- NULL
  }



  if('catchdistribution' %in% names(lik.dat$dat)){
    catchdist.fleets <-
      ldply(names(lik.dat$dat$catchdistribution),
            function(x){

              ldist <-
                merge(lik.dat$dat$catchdistribution[[x]],
                      out[[x]],
                      by=c('length', 'year',
                        'step', 'area','age'),
                      all.y=TRUE)
              ldist$name <- x
              ldist$age <- as.character(ldist$age)
              ldist$area <- as.character(ldist$area)
              ldist <- data.table(ldist)
              ldist <-
                ldist[,c('total.catch','total.pred', 'observed', 'predicted') :=
                      list(total.catch = sum(number.x,na.rm=TRUE),
                           total.pred = sum(number.y,na.rm=TRUE),
                           observed = number.x/sum(number.x,na.rm=TRUE),
                           predicted = number.y/sum(number.y,na.rm=TRUE)
                           ),
                by=list(year, step,  area)]
            ldist <- ldist[,c('upper','lower','avg.length','residuals') :=
                           list(upper = as.integer(max(ifelse(is.na(upper),0.0,
                                  upper))),
                                lower = as.integer(max(ifelse(is.na(lower),0.0,
                                  lower))),
                                avg.length = as.numeric((lower+upper)/2),
                                residuals = as.numeric(observed - predicted)),
                           by = list(length,age)]              
              ldist <- merge(ldist,
                             subset(lik$catchdistribution,
                                    select=c(name,fleetnames,stocknames)),
                             by = 'name')          
              return(ldist)
            })
  } else {
    catchdist.fleets <- NULL
  }


  if(sum(grepl('.std',names(out),fixed = TRUE))>0){
    res.by.year <- 
      ldply(laply(stocks,function(x) x@stockname),function(x){
        f.by.year <- ddply(subset(out[[sprintf('%s.std',x)]],
                                  step == 1 & year < 2014), #### ATH!!!!
                           ~year,
                           summarise,
                           F=max(Z-0.15,na.rm=TRUE)) ### ATH!!!!!!!!
        bio.by.year <- ddply(subset(out[[sprintf('%s.full',x)]],
                                    step == 1),
                             ~year + area,
                             plyr::here(summarise),
                             total.biomass = sum(number*mean.weight),
                             harv.biomass =
                             sum(mean.weight*
                                 harv.suit(as.numeric(gsub('len','',length)))*
                                 number),
                             ssb = sum(mean.weight*logit(mat.par[1],
                               mat.par[2],as.numeric(gsub('len','',length)))*
                             number))
        
        bio <- merge(f.by.year,bio.by.year)
        bio$stock <- x
        return(bio)
      })
    res.by.year <- merge(res.by.year,stock.recruitment,all.x = TRUE)
    annual.catch <- ddply(catches,~year+stock, summarise,
                          catch = sum(catch))
                          
    res.by.year <- merge(res.by.year,
                         annual.catch,all.x = TRUE)
  } else {
    res.by.year <- NULL
  }

  
  
  if('stockdistribution' %in% names(lik.dat$dat)){
    stockdist <-
      ldply(names(lik.dat$dat$stockdistribution),
            function(x){
              stockdist <-
                merge(lik.dat$dat$stockdistribution[[x]],
                      out[[x]],
                      by=c('length', 'year',
                        'step', 'area','age','stock'),
                      all.y=TRUE)
              len.agg <-  tryCatch(read.table(lik$stockdistribution$lenaggfile,
                                              stringsAsFactors=FALSE,
                                              comment.char=';'),
                                   warning = function(x) NULL,
                                   error = function(x) NULL)
              names(len.agg)[1:3] <- c('length','agg.lower','agg.upper')
              stockdist <- merge(stockdist,len.agg)
              stockdist$name <- x
              stockdist <- data.table(stockdist)
              stockdist <-
                stockdist[,c('obs.ratio','pred.ratio','upper',
                             'lower','avg.length') :=
                          list(obs.ratio = number.x/sum(number.x,na.rm=TRUE),
                               pred.ratio = number.y/sum(number.y),
                               upper = max(ifelse(is.na(upper),
                                 agg.upper,upper)),
                               lower = max(ifelse(is.na(lower),
                                 agg.lower,lower)),
                               length2 = (lower+upper)/2),
                          by = list(year, step, area, age, length)]
              stockdist <- stockdist[,c('agg.upper','agg.lower'):=NULL,]
              stockdist <- merge(stockdist,
                                 subset(lik$stockdistribution,
                                        select=c(name,fleetnames,stocknames)),
                                 by='name')
              return(stockdist)
            })
  } else {
    stockdist <- NULL
  }

 
  out <- list(sidat = sidat, resTable = resTable, nesTable = nesTable,
              suitability = gss.suit, stock.growth = stock.growth,
              stock.recruiment = stock.recruitment, 
              res.by.year = res.by.year,
              catchdist.fleets = catchdist.fleets, stockdist = stockdist)

  save(out,file=sprintf('%s/WGTS.Rdata',wgts))
  return(out)
}

gadget.bootfit <- function(main = 'main', dparam.file = 'bsres_v1.RData',
                           bsprint.file = 'bsprint.RData',
                           fleet.predict = data.frame(fleet='comm',ratio=1),
                           mat.par=NULL, .parallel = TRUE
                           ){


  load(dparam.file)
  load(bsprint.file)
  main <- read.gadget.main(main)
  lik <- read.gadget.likelihood(main$likelihoodfiles)
  lik.dat <- read.gadget.data(lik)

  fleets <- read.gadget.fleet(main$fleetfiles)
  stocks <- read.gadget.stockfiles(main$stockfiles)
  dfinal <- subset(dparam,comp=='final')
  dfinal$comp <- NULL
  boot.rec <-
    ddply(melt(dfinal,id.vars='bs.data',variable.name = 'switch'),
          ~bs.data,function(x){
            rownames(x) <- x$switch
            get.gadget.recruitment(stocks,x)
          })
  boot.sel <-
    ddply(melt(dfinal,id.vars='bs.data',variable.name = 'switch'),
          ~bs.data,function(x){
            rownames(x) <- x$switch
            get.gadget.suitability(fleets,x,getLengthGroups(stocks[[1]]))
          })
  boot.growth <- 
    ddply(melt(dfinal,id.vars='bs.data',variable.name = 'switch'),
          ~bs.data,function(x){
            rownames(x) <- x$switch
            get.gadget.growth(stocks,x,age.based = TRUE)
          })

#  boot.ldistfit <-
#    rbindlist(llply(names(lik.dat$dat$catchdistribution),
#                    function(x){
#                      si <-
#                        data.table(noageprint[[x]]) %.%
#                        group_by(.id,year,step,area) %.%
#                        mutate(proportion = number/sum(number)) %.%
#                        group_by(year,step,age,length,add=FALSE) %.%
#                        summarise(upper = quantile(proportion,0.975,na.rm=TRUE),
#                                  lower = quantile(proportion,0.025,na.rm=TRUE))
#                      si$fleet <- x
#                      return(si)
#                    }))
  
  harv.suit <- function(l, .id){
    x <- subset(melt(dfinal,id.vars='bs.data',variable.name = 'switch'),
                bs.data == .id)
    rownames(x) <- x$switch
    ddply(merge(get.gadget.suitability(fleets,x,l),fleet.predict),~l,
          summarise, harv=sum(ratio*suit))$harv
  }
  
  res.by.year <- 
    ldply(laply(stocks,function(x) x@stockname),function(x){
      f.by.year <- ddply(bsprint[[sprintf('%s.std',x)]],
                         ~year + .id,
                         summarise,
                         F=max(Z-0.15,na.rm=TRUE)) ### ATH!!!!!!!!
      ## making sure this works for a relic from the good old times:)
      txt <- ifelse(sum(grepl('.full',names(bsprint),fixed=TRUE))==1,
                    sprintf('%s.full',x), sprintf('%s.lw',x))  
      
      bio.by.year <- ddply(subset(bsprint[[txt]],
                                  step == 1),
                           ~year + area + .id,
                           plyr::here(summarise),
                           total.biomass = sum(number*mean.weight),
                           harv.biomass =
                           sum(mean.weight*
                               harv.suit(as.numeric(gsub('len','',length)),
                                         .id[1])*
                               number),
                           ssb = sum(mean.weight*logit(mat.par[1],
                             mat.par[2],as.numeric(gsub('len','',length)))*
                             number),
                           .parallel = .parallel)
      
      bio <- merge(f.by.year,bio.by.year)
      bio$stock <- x
      return(bio)
    })
  res.by.year <- merge(res.by.year,boot.rec,all.x = TRUE)
  boot.fit <- list(bootparams = dfinal,res.by.year = res.by.year,
                   boot.rec = boot.rec, boot.sel = boot.sel,
                   boot.growth = boot.growth)
  save(boot.fit,file='digestedBoot.RData')
  invisible(boot.fit)
}
