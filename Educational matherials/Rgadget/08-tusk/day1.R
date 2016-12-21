source('/home/nataliia/Desktop/Gadget/Educational matherials/Rgadget/rgadget/trunk/gadgetFileIO.R')
source('/home/nataliia/Desktop/Gadget/Educational matherials/Rgadget/rgadget/trunk/gadgetfunctions.R')
source('/home/nataliia/Desktop/Gadget/Educational matherials/Rgadget/rgadget/trunk/gadgetClass.R')
source('/home/nataliia/Desktop/Gadget/Educational matherials/Rgadget/rgadget/trunk/gadgetMethods.R')
source('/home/nataliia/Desktop/Gadget/Educational matherials/Rgadget/rgadget/trunk/function.R')


library(plyr)
library(dplyr)
library(ggplot2)
library(grid)
library(gridExtra)

fit <- gadget.fit(mat.par = c(-8.6512,0.1403))

F.plot <-
  ggplot(head(fit$res.by.year,-1),aes(year,F)) + geom_line() + ylim(c(0,0.7)) +
  theme_bw()

ssb.plot <-
  ggplot(fit$res.by.year,aes(year,ssb/1e6)) + geom_line() +
  theme_bw() + ylab("SSB (in '000 tons)") + xlab('Year')

catch.plot <-
  ggplot(fit$res.by.year,aes(year,catch/1000)) + geom_bar(stat='identity') +
  theme_bw() + ylab('Landings (in tons)') + xlab('Year')

rec.plot <-
  ggplot(fit$res.by.year,aes(year,recruitment/1e6)) +
  geom_bar(stat='identity') +
  theme_bw() + ylim(c(0,30)) + ylab('Recruitment (in millions)') +
  xlab('Year')


grid.arrange(catch.plot,rec.plot,F.plot,ssb.plot,ncol=2)
