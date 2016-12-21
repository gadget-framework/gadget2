source('../rgadget/trunk/gadgetFileIO.R')
source('../rgadget/trunk/gadgetfunctions.R')
source('../rgadget/trunk/gadgetClass.R')
source('../rgadget/trunk/gadgetMethods.R')
source('../rgadget/trunk/function.R')

library(plyr)
library(dplyr)
library(ggplot2)
library(grid)
mat.par = c(-8.6512,0.1403)
fit <- gadget.fit(mat.par = c(-8.6512,0.1403))
ypr <- gadget.ypr(params.file='WGTS/params.final',
                  ypr='WGTS/YPR')
prognFmax <-
  gadget.forward(years=6,params.file='WGTS/params.final',
                 pre='WGTS/PROGN', stochastic=FALSE,
                 num.trials=1,
                 effort=ypr$fmax)

## to calculate biomass index

tmp <- rbind.fill(fit$sidat,
                  ddply(fit$sidat,~year, summarise,
                        number.x = sum(number.x*0.00000659*lower^3.01721 ),
                        predict = sum(predict*0.00000659*lower^3.01721 ),
                        upper = sum(upper*0.00000659*lower^3.01721 ),
                        lower = sum(lower*0.00000659*lower^3.01721 ),
                        lower = 110, lower = 180,
                        length = 'Biomass'))

si.fit.survey <-
  ggplot(tmp, aes(year,number.x)) +
  geom_point() +
  geom_line(aes(year,predict)) +
  geom_linerange(data=subset(tmp,year==max(year)),
                 aes(year,ymax=number.x,ymin=predict),col='green')+
  geom_text(data=mutate(subset(tmp,year==min(year)),y=Inf),
            aes(year,y,label=length), vjust = 2,hjust = -1)+
  facet_wrap(~length,scale='free_y',ncol=2) + theme_bw() +
  ylab('Index') + xlab('Year') +
  theme (panel.margin = unit(0,'cm'), plot.margin = unit(c(0,0,0,0),'cm'),
         strip.background = element_blank(), strip.text.x = element_blank())

ldist.fit.survey <-
  ggplot(subset(fit$catchdist.fleets,name == 'ldist.survey'),
         aes(lower,predicted)) +
  geom_line(aes(lower,observed),col='gray') +
  facet_wrap(~year+step) + theme_bw() + geom_line() +
  geom_text(data=mutate(subset(fit$catchdist.fleets,
              name == 'ldist.survey' & lower==min(lower)),y=Inf),
            aes(lower,y,label=year), vjust = 2,hjust = -1)+
  ylab('Proportion') + xlab('length') +
  theme (axis.text.y = element_blank(), axis.ticks.y = element_blank(),
         panel.margin = unit(0,'cm'), plot.margin = unit(c(0,0,0,0),'cm'),
         strip.background = element_blank(), strip.text.x = element_blank())

ldist.fit.catch <-
  ggplot(subset(fit$catchdist.fleets,name == 'ldist.catch'),
         aes(lower,predicted)) +
  geom_line(aes(lower,observed),col='gray') +
  facet_wrap(~year+step) + theme_bw() + geom_line() +
  geom_text(data=mutate(subset(fit$catchdist.fleets,
              name == 'ldist.catch' & lower==min(lower)),y=Inf),
            aes(lower,y,label=year), vjust = 2,hjust = -1)+
  ylab('Proportion') + xlab('length') +
  theme (axis.text.y = element_blank(), axis.ticks.y = element_blank(),
         panel.margin = unit(0,'cm'), plot.margin = unit(c(0,0,0,0),'cm'),
         strip.background = element_blank(), strip.text.x = element_blank())


selection.plot <-
  ggplot(fit$suitability,
         aes(l,suit,lty=fleet)) +
  geom_line() +
  theme_bw() + ylab('Suitability') + xlab('Length') +
  theme(legend.position = c(0.8,0.25), legend.title = element_blank(),
        plot.margin = unit(c(0,0,0,0),'cm')) 


gr.plot <-
  ggplot(fit$stock.growth,
         aes(age,length)) + 
  geom_line() +
  theme_bw() + ylab('Length') + xlab('Age') +
  theme(legend.position = c(0.25,0.75), legend.title = element_blank(),
        plot.margin = unit(c(0,0,0,0),'cm'))


rec.plot <-
  ggplot(fit$res.by.year,aes(year,recruitment/1e6)) +
  geom_bar(stat='identity') +
  ylab("Recruitment (in millions)") + xlab('Year') +  theme_bw() +
  theme(legend.position = c(0.25,0.75), legend.title = element_blank(),
        plot.margin = unit(c(0,0,0,0),'cm'))

catch.plot
  ggplot(fit$res.by.year,aes(year,catch/1000)) +
  geom_bar(stat='identity') +
  ylab("Catches (in tons)") + xlab('Year') +  theme_bw() +
  theme(legend.position = c(0.25,0.75), legend.title = element_blank(),
        plot.margin = unit(c(0,0,0,0),'cm'))

biomass.plot <- 
  ggplot(fit$res.by.year,aes(year,total.biomass/1000)) +
  geom_bar(stat='identity') +
  ylab("Total biomass (in tons)") + xlab('Year') +  theme_bw() +
  theme(legend.position = c(0.25,0.75), legend.title = element_blank(),
        plot.margin = unit(c(0,0,0,0),'cm'))


harv.plot <- 
  ggplot(fit$res.by.year,aes(year,harv.biomass/1000)) +
  geom_bar(stat='identity') +
  ylab("Harvestable biomass (in tons)") + xlab('Year') +  theme_bw() +
  theme(legend.position = c(0.25,0.75), legend.title = element_blank(),
        plot.margin = unit(c(0,0,0,0),'cm'))

ssb.plot <- 
  ggplot(fit$res.by.year,aes(year,ssb/1000)) +
  geom_bar(stat='identity') +
  ylab("SSB (in tons)") + xlab('Year') +  theme_bw() +
  theme(legend.position = c(0.25,0.75), legend.title = element_blank(),
        plot.margin = unit(c(0,0,0,0),'cm'))


ypr.plot <- 
  ggplot(ypr$ypr,aes(effort,bio)) +
  geom_line() +
  geom_segment(aes(x = effort,xend=effort,y=-Inf,yend=bio),
               data=subset(ypr$ypr, effort == ypr$fmax)) +
  geom_segment(aes(x = effort,xend=effort,y=-Inf,yend=bio),
               data=subset(ypr$ypr, effort == ypr$f0.1$f0.1)) +
  geom_text(data=subset(ypr$ypr,effort == ypr$fmax),
            aes(label = sprintf('Fmax = %s',effort),
                x = effort+0.04,y=0.2,angle=90)) +
  geom_text(data=subset(ypr$ypr,effort == ypr$f0.1$f0.1),
            aes(label = sprintf('F0.1 = %s',effort),
                x = effort+0.04,y=0.2,angle=90)) +
  theme_bw() +  xlab('Fishing mortality') + ylab('Yield per recruit') +
  theme(legend.position='none',plot.margin = unit(c(0,0,0,0),'cm'))


progn.ssb <-
  prognFmax$lw %>%
  filter(step == 1) %>%
  group_by(year) %>%
  summarise(ssb = sum(weight*logit(mat.par[1],
              mat.par[2],length)*
              number),
            total.biomass = sum(number*weight))#,

progn.by.year <-
  left_join(prognFmax$catch %>%
            group_by(year) %>%
            summarise(catch=sum(biomass.consumed)),
            progn.ssb)


prog.bio.plot <-
  ggplot(progn.by.year,aes(year,ssb/1e6)) +
  geom_rect(aes(xmin=max(fit$res.by.year$year),
                xmax=Inf,ymin=-Inf,ymax=Inf),
            fill = 'gray90', alpha=0.1) +
  geom_line() +
  theme_bw() + xlab('Year') + ylab('SSB (\'000 tons)') +
  theme(plot.margin = unit(c(0,0,0,0),'cm'),
        legend.title = element_blank(),
        legend.position = c(0.2,0.7))


prog.catch.plot <-
  ggplot(progn.quant,aes(year,catch/1000)) +
  geom_rect(aes(xmin=max(fit$res.by.year$year),
                xmax=Inf,ymin=-Inf,ymax=Inf),
            fill = 'gray90', alpha=0.1) +
  geom_line() +
  theme_bw() + xlab('Year') + ylab('Catch (\'000 tons)') +
  theme(plot.margin = unit(c(0,0,0,0),'cm'),
        legend.title = element_blank(),
        legend.position = c(0.2,0.7))

prog.rec.plot <- rec.plot + geom_bar(aes(year,10*recruitment),
                                     data=prognFmax$recruitment,
                                     fill='red',stat='identity')
