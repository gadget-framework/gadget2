source('~/Documents/rgadget/trunk/gadgetFileIO.R')
source('~/Documents/rgadget/trunk/gadgetfunctions.R')
source('~/Documents/rgadget/trunk/gadgetClass.R')
source('~/Documents/rgadget/trunk/gadgetMethods.R')
source('~/Documents/rgadget/trunk/function.R')

library(plyr)
library(dplyr)
library(ggplot2)
library(grid)

tmp <- gadget.iterative(rew.sI=TRUE,
                        grouping=list(sind=c('si2039','si4069','si70110'),
                          survey=c('ldist.survey','alkeys.survey'),
                          catch=c('ldist.catch','alkeys.catch')),                       
                        params.file='params.base',
                        wgts='WGTS')


