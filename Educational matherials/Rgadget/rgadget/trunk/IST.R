source('function.R')
source('whaleStock.R')
source('summaryFunc.R')
source('gadgetoptions.R')
library(ggplot2)
library(gridExtra)
library(xtable)
library(plyr)
library(reshape2)
library(aod)
library(multicore)
library(doMC)
registerDoMC()
opt <- gadget.options()
opt$stocks <- c('W','C1','C2','C3','E','S')

## time and space
opt$areas <- c('CE','WG','EG','WI','EIF','N','SP')
opt$numofareas <- length(opt$areas)

opt$numoftimesteps <- 1
opt$dt <- 1

## length and age
opt$maxage <- 25
opt$numofagegroups <- opt$maxage + 1

opt$minlen <- 1
opt$maxlen <- 2
opt$numoflgroups <- 1
opt$z <- 0.08

## migration and dispersion
opt$gamma.mix <- 0.8
opt$alpha.mix <- 0
opt$mixing <- matrix(0,ncol=6,nrow=7,dimnames=list(areas=opt$areas,stocks=opt$stocks))
opt$mixing['CE','W'] <- opt$gamma.mix
opt$mixing['WG','W'] <- 1-opt$gamma.mix
opt$mixing[,'C1'] <- c(opt$alpha.mix*opt$gamma.mix,
                       opt$alpha.mix*(1-opt$gamma.mix),
                       1-2*opt$alpha.mix,
                       opt$alpha.mix,0,0,0)
opt$mixing[c('EG','WI','EIF'),'C2'] <- c(opt$alpha.mix,
                                         1-2*opt$alpha.mix,
                                         opt$alpha.mix)
opt$mixing[c('WI','EIF','N'),'C3'] <- c(opt$alpha.mix,
                                         1-2*opt$alpha.mix,
                                         opt$alpha.mix)
opt$mixing['N','E'] <- 1
opt$mixing['SP','S'] <- 1

## dispersion
opt$dispersion <- matrix(0,nrow=length(opt$stocks),ncol=length(opt$stocks),
                         dimnames = list(To = opt$stocks,
                           From = opt$stocks))
diag(opt$dispersion) <- 1
opt$dispersion['C1',sprintf('C%s',1:3)] <- c(0.95,0.05,0)
opt$dispersion['C2',sprintf('C%s',1:3)] <- c(0.05572320,1-0.35572320,0.3)
opt$dispersion['C3',sprintf('C%s',1:3)] <- c(0,0.00157109,1-0.00157109)

## harvest
opt$fleets <- 'Comm'
opt$doescatchcomm <- 'WI'
opt$doescathcsurv <- ''
opt$quota <- 150
names(opt$quota) <- 'WI'
opt$salpha <- c(Male=-3.6/0.57,Female=-4.1)
opt$sbeta <- c(Male=1/0.57,Female=1)

opt$doeseat <- 0
opt$doesgrow <- 0

opt$mort <- rep(opt$z,opt$maxage+1)
opt$doescatchsurv <- NULL

opt$num.tags <- 1000
names(opt$num.tags) <- 'EG'

opt$tag.loss <- 1
opt$recapture.lambda <- 2
opt$gender.division <- c(Male=1/2,Female=1/2)

## birth parameters obtained in the IST-trial
opt$density.z <- 2.38980
opt$age.of.parturation <- 6
opt$resiliance.a <- c(r1=0.17031,r2=0.42577,r4=0.68123)
opt$avg.B <- 2/(opt$maxage-opt$age.of.parturation)
opt$msyl <- 0.72
opt$msyr <- c(r1=0.01,r2=0.02,r4=0.04)

## migration and dispersion
opt$gamma.mix <- 0.8

opt.h4 <- opt
opt.h3 <- opt
opt.h4$alpha.mix <- 0.05
opt.h3$alpha.mix <- 0
mixing.matrix <- function(opt){
  opt$mixing <- matrix(0,ncol=6,nrow=7,
                       dimnames=list(area=opt$areas,stock=opt$stocks))
  opt$mixing['CE','W'] <- opt$gamma.mix
  opt$mixing['WG','W'] <- 1-opt$gamma.mix
  opt$mixing[,'C1'] <- c(opt$alpha.mix*opt$gamma.mix,
                         opt$alpha.mix*(1-opt$gamma.mix),
                         1-2*opt$alpha.mix,
                         opt$alpha.mix,0,0,0)
  opt$mixing[c('EG','WI','EIF'),'C2'] <- c(opt$alpha.mix,
                                         1-2*opt$alpha.mix,
                                           opt$alpha.mix)
  opt$mixing[c('WI','EIF','N'),'C3'] <- c(opt$alpha.mix,
                                          1-2*opt$alpha.mix,
                                          opt$alpha.mix)
  opt$mixing['N','E'] <- 1
  opt$mixing['SP','S'] <- 1
  return(opt)
}

opt.h4 <- mixing.matrix(opt.h4)
opt.h3 <- mixing.matrix(opt.h3)
opt.h3$mixing[c('CE','WG','EG'),'C1'] <- c(0.1*opt$gamma.mix,
                                           0.1*(1-opt$gamma.mix),
                                           0.9)

## dispersion
dispersion.matrix <- function(opt){
  opt$dispersion <- matrix(0,nrow=length(opt$stocks),
                           ncol=length(opt$stocks),
                           dimnames = list(To = opt$stocks,
                             From = opt$stocks))
  diag(opt$dispersion) <- 1
  c1c2 <-
    min(0.99,0.05*0.5*(opt$init.abund['C1'] +
                       opt$init.abund['C2'])/opt$init.abund['C1'])
  c2c1 <-
    min(0.99,0.05*0.5*(opt$init.abund['C1'] +
                       opt$init.abund['C2'])/opt$init.abund['C2'])
  c2c3 <-
    min(0.99,0.05*0.5*(opt$init.abund['C3'] +
                      opt$init.abund['C2'])/opt$init.abund['C2'])
  c3c2 <-
    min(0.99,0.05*0.5*(opt$init.abund['C3'] +
                      opt$init.abund['C2'])/opt$init.abund['C3'])


  opt$dispersion['C1',sprintf('C%s',1:3)] <- c(1-c1c2,c1c2,0)
  opt$dispersion['C2',sprintf('C%s',1:3)] <- c(c2c1,1-(c2c1+c2c3),c2c3)
  opt$dispersion['C3',sprintf('C%s',1:3)] <- c(0,c3c2,1-c3c2)
  return(opt)
}


opt.h3$init.abund <- rep(7000,6)#c(7595, 5260, 3362,  8183, 6613,  7841)
opt.h4$init.abund <- rep(7000,6)#c(7266,  3317,  5422,  7730,  7064,  7995)
names(opt.h3$init.abund) <- opt$stocks
names(opt.h4$init.abund) <- opt$stocks
opt.h3 <- dispersion.matrix(opt.h3)
opt.h4$dispersion <- NULL


power.analysis <- function(rec){
#  tmp.func <- function(x){
#    tmp.year <- negbin(value~year,~1,x,method='SANN')
#    tmp.0 <- negbin(value~1,~1,x,method='SANN')
#    return(anova(tmp.year,tmp.0)@anova.table$P[2])
#  }

#  AIC.year <- ddply(rec,'variable', tmp.year)
#  AIC.0 <- ddply(rec,'variable', tmp.0)

  tagLik <- function(y,theta){

    sum(-(lgamma(y+theta)+theta*log(1/3) + y*log(2/3) - (lgamma(y+1) + lgamma(theta))))
  }

  tagMin <- function(x,y){
    if(x[1] < 0 | x[2] < 0)
      1e6
    else
      tagLik(y,2:10*x[1]+x[2])
  }

  tagMin0 <- function(x,y){
    if(x < 0)
      1e6
    else
      tagLik(y,x)
  }

  tmp.func <- function(x){
    tmp.year <- optim(c(1,1),tagMin,y=x$value,method='SANN')
    tmp.0 <- optim(1,tagMin0,y=x$value,method='SANN')
    lrt <- 2*(tmp.0$value - tmp.year$value)
    return(1-pchisq(lrt,df=1))
  }

  return(ddply(rec,'variable', tmp.func))

}
if(FALSE){
  p.h3 <- power.analysis(rec.h3)
  p.h4 <- power.analysis(rec.h4)

  print(sprintf('Hypothesis 4 rejected when true: %s',sum(p.h4<1)))
  print(sprintf('Hypothesis 3 rejected when true: %s',sum(p.h3>1)))
}



run.tag.experiment <- function(num.tags=100,num.trials=1000,lambda=2){
  opt.h4$num.tags <- num.tags
  names(opt.h4$num.tags) <- 'EG'
  opt.h3$num.tags <- num.tags
  names(opt.h3$num.tags) <- 'EG'
  ## run the simulation
  sim.h4 <- Rgadget(opt.h4)
  sim.h3 <- Rgadget(opt.h3)

  ## tags..
  rec.h4 <- tagging.recaptures(sim.h4,lambda,num.trials)
  rec.h4$rec$year <- 2:10
  rec.h4$rec$Hypothesis <- 'Mixing'

  rec.h3 <- tagging.recaptures(sim.h3,lambda,num.trials)
  rec.h3$rec$year <- 2:10
  rec.h3$rec$Hypothesis <- 'Dispersion'

  ## test power of a regular tagging experiment
  recaptures <- rbind.fill(rec.h3$rec,rec.h4$rec)
  recstat <- ddply(recaptures,~Hypothesis + variable,
                   function(x){
                     tmp <- drop1(glm(value~year + offset(-0.08*year),
                                      family=poisson,
                                      data=x))
                     data.frame(diffAIC = diff(tmp$AIC),
                                diffDev = diff(tmp$Deviance),
                                rec = sum(x$value))
                   })
                                        #power.analysis(rec.h3$rec)
  recstat$rho <- c(rec.h3$rho,rec.h4$rho)
  Ri <- rbind.fill(data.frame(C2=rec.h3$Ri),
                   as.data.frame(rec.h4$Ri))
  names(Ri) <- paste('Rel',names(Ri),sep='.')
  ci <- rbind.fill(data.frame(C2=rep(rec.h3$ci,num.trials)),
                   as.data.frame(rec.h4$ci))
  names(ci) <- paste('Catch',names(ci),sep='.')
  recstat <- cbind(recstat,Ri,ci)
  names(recstat)[2] <- 'Trial'
  recstat$Trial <- as.numeric(gsub('V','',as.character(recstat$Trial)))
  recstat$num.tags <- num.tags
  recaptures$num.tags <- num.tags
  save(recaptures,recstat, sim.h3, sim.h4,
       file=sprintf('tag%s.RData',num.tags))
  return(recstat)
}

hypo.test <- ldply(100*(1:15),run.tag.experiment,.parallel=TRUE)

recaptures <- ldply(100*(1:15),
                    function(i){
                      load(sprintf('tag%s.RData',i))
                      recaptures
                    })

sim.h4 <- Rgadget(opt.h4)
sim.h3 <- Rgadget(opt.h3)

Tnum <-
  rbind.fill(mutate(subset(adply(sim.h4$Tagged,c(3,6),sum),
                           area %in% c('EG','WI','EIF')),
                    Year = as.numeric(time),
                    Hypothesis = 'Mixing'),
             mutate(subset(adply(sim.h3$Tagged,c(3,6),sum),
                           area %in% c('EG','WI','EIF')),
                    Year = as.numeric(time),
                    Hypothesis = 'Dispersion'))

tag.plot <-
  ggplot(Tnum,aes(Year,V1/1000,lty=Hypothesis)) + geom_line() +
  facet_wrap(~area, scale='free_y') + theme_bw() +
  geom_point(data=data.frame(Year=0,V1=0,
               Hypothesis=c('Mixing','Dispersion')),col='white') +
  ylab('Proportion tags remaining') +
  theme(legend.position = c(0.1,0.2))

pdf(file='fig00-tagplot.pdf',width=10,height=4)
print(tag.plot)
dev.off()

power.anal <-
  ddply(hypo.test,~num.tags,
        function(x){
          h4 <- subset(x,Hypothesis == 'Mixing')
          h3 <- subset(x,Hypothesis != 'Mixing')
          devQ <- quantile(h4$diffDev,0.95)
          devh3 <- sum(h3$diffDev>devQ)/1000
          recQ <- quantile(h4$rec,0.95)
          rech3 <- sum(h3$rec>recQ)/1000
          rhoQ <- quantile(h4$rho,0.95)
          rhoh3 <- sum(h3$rho>rhoQ)/1000
          data.frame(poisson=devh3,rec=rech3,rho=rhoh3)
        })


recapture.plot <-
  ggplot(subset(recaptures, num.tags %in% (c(2*(0:7)+1)*100)),
                aes(year,value,fill=Hypothesis,
                        group=interaction(year,Hypothesis))) +
  geom_boxplot() + facet_wrap(~num.tags,scale='free_y') +
  theme_bw() + ylab('Number of recaptures') + xlab('Year') +
  theme(legend.position = c(0.8,0.15)) +
#        strip.background = element_blank()) + #,
#        panel.margin = unit(-1,'lines'),
#        strip.text.x = element_text(vjust=-2,hjust=0.8)) +
  scale_fill_manual(values = c('gray40','gray70'))

f <- function(x) {
  r <- quantile(x, probs = c(0.025, 0.25, 0.5, 0.75, 0.975))
  names(r) <- c("ymin", "lower", "middle", "upper", "ymax")
  r
}

o <- function(x) {
  subset(x, x < quantile(x,probs=0.025) | quantile(x,probs=0.975) < x)
}



pdf(file='fig01-rec.pdf',width=10,height=7)
print(recapture.plot)
dev.off()

poisson.plot <-
  ggplot(hypo.test,aes(num.tags,diffDev,
                       fill=Hypothesis,
                       group=interaction(num.tags,Hypothesis))) +
  stat_summary(fun.data=f, geom="boxplot", position = 'dodge')+
#  geom_boxplot() +
  theme_bw() + ylab('Difference in deviance') + xlab('Number of tags') +
  theme(legend.position = c(0.2,0.8)) +
#        strip.background = element_blank()) + #,
#        panel.margin = unit(-1,'lines'),
#        strip.text.x = element_text(vjust=-2,hjust=0.8)) +
  scale_fill_manual(values = c('gray40','gray70')) 

trendsig.plot <-
  ggplot(ddply(hypo.test,~Hypothesis + num.tags,
               summarise, app=mean(diffDev>1.96)),
         aes(num.tags,app,col=Hypothesis,group=Hypothesis)) + 
  geom_line()+
  scale_colour_manual(values = c('gray40','gray70')) +
  theme_bw() + ylab('Proportion significant') + xlab('Number of tags') +
  theme(legend.position = c(0.2,0.8)) + ylim(c(0,1))


pdf('fig02-poisson.pdf',width=10,height=14)
grid.arrange(poisson.plot,trendsig.plot,ncol=1)
dev.off()

rec.plot <-
  ggplot(hypo.test,aes(num.tags,rec,
                       fill=Hypothesis,
                       group=interaction(num.tags,Hypothesis))) +
  stat_summary(fun.data=f, geom="boxplot", position = 'dodge')+
  theme_bw() + ylab('Total recaptures') + xlab('Number of tags') +
  theme(legend.position = c(0.2,0.8)) +
#        strip.background = element_blank()) + #,
#        panel.margin = unit(-1,'lines'),
#        strip.text.x = element_text(vjust=-2,hjust=0.8)) +
  scale_fill_manual(values = c('gray40','gray70'))

pdf('fig03-recaptures.pdf',width=10,height=7)
print(rec.plot)
dev.off()

rho.plot <-
  ggplot(hypo.test,aes(num.tags,rho,
                       fill=Hypothesis,
                       group=interaction(num.tags,Hypothesis))) +
  stat_summary(fun.data=f, geom="boxplot", position = 'dodge')+
  theme_bw() + ylab('Ratio of recaptures to relatives in catch') + xlab('Number of tags') +
  theme(legend.position = c(0.2,0.8)) +
#        strip.background = element_blank()) + #,
#        panel.margin = unit(-1,'lines'),
#        strip.text.x = element_text(vjust=-2,hjust=0.8)) +
  scale_fill_manual(values = c('gray40','gray70'))

pdf('fig04-rho.pdf',width=10,height=7)
print(rho.plot)
dev.off()


relatives <-
  subset(melt(hypo.test, id.vars=c('Hypothesis', 'Trial','num.tags')),
         grepl('Rel',variable) & !is.na(value) & value > 0)

ddply(ddply(relatives,~Hypothesis + num.tags+Trial,summarise,value=sum(value)),
      ~Hypothesis,summarise,m=median(value),q.upper=quantile(value,0.975),
      q.lower=quantile(value,0.025))
