source('~/Desktop/Gadget/Educational matherials/Rgadget/rgadget/trunk/gadgetFileIO.R')
source('~/Desktop/Gadget/Educational matherials/Rgadget/rgadget/trunk/gadgetfunctions.R')
source('~/Desktop/Gadget/Educational matherials/Rgadget/rgadget/trunk/gadgetClass.R')
source('~/Desktop/Gadget/Educational matherials/Rgadget/rgadget/trunk/gadgetMethods.R')
source('~/Desktop/Gadget/Educational matherials/Rgadget/rgadget/trunk/function.R')

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


