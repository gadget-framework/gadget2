##' A flesxible barplot routine: gad.bar
##'
##' Adds a barplot to a plot.
##' 
##' @title gad.bar 
##' @param x The x values
##' @param y the y values
##' @param br width of the bars.  The default is 0.5 which creates barplot with no spaces
##' between the bars.  The width refers to how much should the bar be extended in each direction.
##' @param botn The 'bottom' value.  The default is 0.  That is the bar is plotted from 0 to y[i].
##' @param fill Should the bar be filled.
##' @param bar.col The colour of the bars
##' @param bar.d Density, see '?polygon' for details.
##' @param bar.ang Angle of lines
##' @param line.col Colour of outer lines of the bars
##' @param line.lwd Width of the outer lines of the bars
##' @return Adds bars to the current plot.
##' @author Gudmundur Thordarson
gad.bar<-function (x, y, br = 0.5, botn = 0, fill = T, bar.col = "lightblue", 
    bar.d = -1, bar.ang = 45, line.col = "black", line.lwd = 1) 
{
    tmp <- data.frame(cbind(x, y))
    if (fill == T) {
        for (i in x) {
            polygon(c(i - br, i - br, i - br, i + br, i + br, 
                i + br, i - br), c(botn, tmp$y[tmp$x == i], tmp$y[tmp$x == 
                i], tmp$y[tmp$x == i], tmp$y[tmp$x == i], botn, 
                botn), col = bar.col, density = bar.d, angle = bar.ang)
        }
    }
    for (i in x) {
        lines(c(i - br, i - br), c(botn, tmp$y[tmp$x == i]), 
            col = line.col, lwd = line.lwd)
        lines(c(i + br, i + br), c(botn, tmp$y[tmp$x == i]), 
            col = line.col, lwd = line.lwd)
        lines(c(i - br, i + br), c(tmp$y[tmp$x == i], tmp$y[tmp$x == 
            i]), col = line.col, lwd = line.lwd)
        lines(c(i - br, i + br), c(botn, botn), col = line.col, 
            lwd = line.lwd)
    }
}
##' Gadget: Make catchdistribution input file
##' Previously known as: 'newbc.catchdistribution'
##' @title gadget.make.catchdist
##' @param sfile Optional, a id file to extract a subset of 'korlfile'
##' @param korlfile The main data file, has to have the following columns:
##' 'fjoldi' (numbers), 'ar' (year), 
##' @param areanames Optional, can also be supplied in the 'korlfile'
##' @param agenames Optional, can also be supplied in the 'korlfile'
##' @param lengthnames Optional, can also be supplied in the 'korlfile'
##' @return a dataframe that can be used as an input file in gadget
##' @author Hoskuldur Bjornsson (I presume)
##' @note Requires the function 'apply.shrink' from the geo package
gadget.make.catchdist <- function(sfile,korlfile,areanames,agenames,lengthnames) {
  tmp <- korlfile
  if(!missing(sfile)) tmp <- join(tmp,sfile[,c("reg","ar","step","synis.id")],"synis.id")
  if(is.na(match("fjoldi",names(tmp)))) tmp$fjoldi <- rep(1,nrow(tmp)) 
  x <- apply.shrink(tmp$fjoldi,list(tmp$ar,tmp$step,tmp$reg,tmp$agegroup,tmp$lengthgroup),
                    sum,names=c("year","step","area","age","length","number"))
  x <- x[order(x$year*1e8+x$step*1e6+x$area*1e4),]
  if(!missing(areanames)) x$area <- areanames[x$area]
  if(!missing(agenames)) x$age <- agenames[x$age] 
  if(!missing(lengthnames)) x$length <- lengthnames[x$length]
  return(invisible(x))
} 


##' Gadget: Make catchdistribution input file
##'
##' Use gadget.make.catchdist
##' @title newbc.catchdistribution
##' @param ... 
##' @return Nothing
##' @author Ali Baba
newbc.catchdistribution<-function(...){
  cat('\n OLD!! PLEASE USE: gadget.make.catchdist \n\n')
}

##' Gadget: make mean length by age input file
##' Previously known as: 'newbc.meanlength.by.age'
##' 
##' @title gadget.make.meanl.by.age
##' @param sfile a id file to extract a subset of 'kfile'
##' @param kfile The main datafile
##' @param Stddev 
##' @param functionnumber 
##' @param areanames 
##' @param agenames 
##' @return a dataframe containing the columns 'year', 'step', 'area',
##' 'age', 'number', 'meanlength' and 'stddev'
##' @author Hoskuldur Bjornsson (I presume)
##' @note Requires the function 'apply.shrink' from the geo package
gadget.make.meanl.by.age <- function(sfile,kfile,Stddev,functionnumber,areanames,agenames) {
  kfile <- join(kfile,sfile[,c("reg","ar","step","synis.id")],"synis.id")
  x <- apply.shrink(kfile$lengd,list(kfile$ar,kfile$step,kfile$reg,kfile$aldur),mean,
                    names=c("year","step","area","age","meanlength"))
  x1 <- apply.shrink(rep(1,nrow(kfile)),list(kfile$ar,kfile$step,kfile$reg,kfile$aldur),sum,
                     names=c("year","step","area","age","number"))
  x <- merge(x1,x)
  if(missing(Stddev)){
    x2 <- apply.shrink(kfile$lengd,list(kfile$ar,kfile$step,kfile$reg,kfile$aldur),sd,
                       names=c("year","step","area","age","stddev"))
    x <- merge(x,x2)
  }
  else {
    x <- merge(x,Stddev)
  }  
  i <- is.na(x$stddev)
  if(any(i)) x$stddev[i] <- x$meanlength[i]
  x <- x[order(x$year*1e8+x$step*1e6+x$area*1e4,x$age*1e2),]
  x$meanlength <- round(x$meanlength,3)
  x$stddev <- round(x$stddev,3)
  if(!missing(areanames)) x$area <- areanames[x$area]
  if(!missing(agenames)) x$age <- agenames[x$age]
  x <- x[,c("year","step","area","age","number","meanlength","stddev")]
  write.table(x,file="tmp",sep="\t", col.names=FALSE, row.names=FALSE) 
  return(invisible(x)) 
}

##' Gadget: Make meanlength at age input file
##'
##' Use gadget.make.meanl.by.age
##' @title newbc.meanlength.by.ate
##' @param ... 
##' @return Nothing
##' @author Ali Baba
newbc.meanlength.by.age <- function(...){
  cat('\n OLD!! PLEASE USE: gadget.make.meanl.by.age \n\n')
}


##' gadgetR: ICES standard graph emulator
##'
##' 
##' @title gadget.plot.ices
##' @param ResYr A data frame containing the following columns 'year', 'catch',
##' 'rec', 'ssb' and 'Fbar', where catch and ssb are in thous. tonnes, rec is in
##' millions.
##' @param export  Default is 'F'. If one whants to export the graph then a name has to be
##' given.  The function will add '.jpg' to the file.  This will create a jpg that has the
##' same dimensions as the 'ICES standard graph' in the Summary sheets.
##' @param land.tr Should landings from the last year be skipped,  the default is to skip
##' the last year.  If one does not whant that then land.est=0.
##' @param rec.tr The same for recruitment as land.tr is for catches.
##' @param Rec.est For estimated recruitment.  Default is false.  Has to be supplied as a data frame
##' with the column 'year' and 'rec.est'
##' @param Rage Age of recruitment to be included in the title
##' @param Fbar the upper and lower age of the Fbar, default is c(7,13).
##' @param Fpa The Fpa reference point, comes as a horizontal line (lty=4)
##' @param Flim Flim reference point, comes as a horizontal line (lty=3)
##' @param Fmsy Fmsy reference point, comes as a horizontal line (lty=5)
##' @param Bpa Bpa reference point, comes as a horizontal line (lty=4)
##' @param Blim Blim reference point, comes as a horizontal line (lty=3)
##' @param Bmsy Bmsy reference point, comes as a horizontal line (lty=5)
##' @param Biomass.title, The default is 'Spawning Stock Biomass' but can be
##' changed to something different as 'Harvestable Biomass'.  Note that the function still
##' uses the $ssb column to plot with.
##' @param Fleg.pos Position of the F-reference points legend, default is 'topright',
##' see '?legend' for details
##' @param Bleg.pos Position of the Biomass-reference points legend, default is 'topleft',
##' see '?legend' for details
##' @param title.cex Size of the title
##' @param lab.cex Size of labels
##' @param axis.cex Size of axis labels
##' @param leg.cex Size of legend (Fbar and SSB plots)
##' @return A graph
##' @author Gudmundur Thordarson
gadget.plot.ices<-function(ResYr, export=F, land.tr=1, rec.tr=1, Rec.est=F, Rage=3, Fbar=c(7,13), Fpa=F, Flim=F, Fmsy=F,
                           Bpa=F, Blim=F, Bmsy=F,Biomass.title='Spawning Stock Biomass',Fleg.pos='topright',
                           Bleg.pos="topleft",
                           title.cex=1.2, lab.cex=0.9, axis.cex=0.8, leg.cex=0.8){
  x<-ResYr
  if(export!=F){
    jpeg(filename = paste(export,'.jpg', sep=''),
          width = 17.2, height = 8.4, units = 'cm',
          pointsize = 12, quality = 100, bg = "white", res=550)
  }
  par(mfrow=c(2,2))
  par(mar=c(1.5,2.5,1.7,0.5))
  par(omi=rep(0.05,4))

  plot(x$year, x$catch,
       xlim=c(min(x$year), max(x$year)), ylim=c(0,max(x$catch)*1.2),
       type="n", bty='l', yaxs='i', las=1, mgp=c(1.4,0.1,0), tck=-0.02, xlab="",
       ylab='Landings in 1000t', yaxt='n', cex.lab=lab.cex, cex.axis=axis.cex)
  axis(2, labels=T,mgp=c(0,0.2,0), tck=-0.02, adj=0, las=1, cex.axis=axis.cex)
  title('Landings', cex.main=title.cex, line=0)
  gad.bar(x$year[1:(length(x$year)-land.tr)], x$catch[1:(length(x$year)-land.tr)],
          bar.col='white', br=0.3)
   box(which='figure')

  plot(x$year, x$rec,
       xlim=c(min(x$year), max(x$year)), ylim=c(0,max(x$rec)*1.2),
       type="n", bty='l', yaxs='i', las=1, mgp=c(1.4,0.1,0), tck=-0.02, xlab="",
       ylab='Recruitment in millions', yaxt='n', cex.lab=lab.cex, cex.axis=axis.cex)
  axis(2, labels=T,mgp=c(0,0.2,0), tck=-0.02, adj=0, las=1, cex.axis=axis.cex)
  title(paste('Recruitment (age ',Rage,')', sep=''), cex.main=title.cex, line=0)
  gad.bar(x$year[1:(length(x$year)-rec.tr)], x$rec[1:(length(x$year)-rec.tr)],
          bar.col='white', br=0.3)
  if(is.data.frame(Rec.est)==TRUE){
    gad.bar(Rec.est$year, Rec.est$rec.est, bar.col='grey', br=0.3)
  }
  box(which='figure')
  Fleg<-paste('expression(italic(bar(F))[(',Fbar[1],'-',Fbar[2],')])',sep='')

  plot(x$year, x$Fbar,
       xlim=c(min(x$year), max(x$year)), ylim=c(0,max(x$Fbar)*1.2),
       type="n", bty='l', yaxs='i', las=1, mgp=c(1.4,0.1,0), tck=-0.02, xlab="",
       ylab=eval(parse(text=Fleg)), yaxt='n', cex.lab=lab.cex, cex.axis=axis.cex)
  axis(2, labels=T,mgp=c(0,0.2,0), tck=-0.02, adj=0, las=1, cex.axis=axis.cex)
  title('Fishing Mortality', cex.main=title.cex, line=0)
  #mtext(side=2, expression(F[7-13]),
  lines(x$year[1:(length(x$year)-1)], x$Fbar[1:(length(x$year)-1)], lwd=2)
  if(Flim!=F){
    abline(h=Flim, lty=3)
  }
  if(Fpa!=F){
    abline(h=Fpa, lty=4)
  }
  if(Fmsy!=F){
    abline(h=Fmsy, lty=5)
  }
  if(Flim!=F & Fpa!=F & Fmsy==F){
    legend(x=Fleg.pos, c(expression(F[lim]),expression(F[pa])), lty=c(3,4), cex=leg.cex, bty='n')
  }
  if(Flim!=F & Fpa==F  & Fmsy==F){
    legend(x=Fleg.pos, c(expression(F[lim])), lty=c(3), cex=leg.cex, bty='n')
  }
  if(Flim==F & Fpa!=F & Fmsy==F){
    legend(x=Fleg.pos, c(expression(F[pa])), lty=c(4), cex=leg.cex, bty='n')
  }
  if(Flim!=F & Fpa!=F  & Fmsy!=F){
    legend(x=Fleg.pos, c(expression(F[lim]), expression(F[pa]), expression(F[msy])), lty=c(3,4,5), cex=leg.cex, bty='n')
  }
  if(Flim==F & Fpa!=F  & Fmsy!=F){
    legend(x=Fleg.pos, c(expression(F[pa]), expression(F[msy])), lty=c(4,5), cex=leg.cex, bty='n')
  }
  if(Flim!=F & Fpa==F  & Fmsy!=F){
    legend(x=Fleg.pos, c(expression(F[lim]), expression(F[msy])), lty=c(3,5), cex=leg.cex, bty='n')
  }
    
   box(which='figure')
  
  plot(x$year, x$ssb,
       xlim=c(min(x$year), max(x$year)), ylim=c(0,max(x$ssb)*1.2),
       type="n", bty='l', yaxs='i', las=1, mgp=c(1.4,0.1,0), tck=-0.02, xlab="",
       ylab='SSB in 1000t', yaxt='n', cex.lab=lab.cex, cex.axis=axis.cex)
  axis(2, labels=T,mgp=c(0,0.2,0), tck=-0.02, adj=0, las=1, cex.axis=axis.cex)
  title(Biomass.title, cex.main=title.cex, line=0)
  lines(x$year, x$ssb,  lwd=2)
  if(Blim!=F){
    abline(h=Blim, lty=3)
  }
  if(Bpa!=F){
    abline(h=Bpa, lty=4)
  }
  if(Bmsy!=F){
    abline(h=Bmsy, lty=5)
  }
  if(Blim!=F & Bpa!=F & Bmsy==F){
    legend(x=Bleg.pos, c(expression(B[lim]),expression(B[pa])), lty=c(3,4), cex=leg.cex, bty='n')
  }
  if(Blim!=F & Bpa==F & Bmsy==F){
    legend(x=Bleg.pos, c(expression(B[lim])), lty=c(3), cex=leg.cex, bty='n')
  }
  if(Blim==F & Bpa!=F & Bmsy==F){
    legend(x=Bleg.pos, c(expression(B[pa])), lty=c(4), cex=leg.cex, bty='n')
  }
  if(Blim!=F & Bpa!=F & Bmsy!=F){
    legend(x=Bleg.pos, c(expression(B[lim]),expression(B[pa]),expression(B[msy])), lty=c(3,4,5), cex=leg.cex, bty='n')
  }
   if(Blim!=F & Bpa==F & Bmsy!=F){
    legend(x=Bleg.pos, c(expression(B[lim]),expression(B[msy])), lty=c(3,5), cex=leg.cex, bty='n')
  }
  if(Blim==F & Bpa!=F & Bmsy!=F){
    legend(x=Bleg.pos, c(expression(B[pa]),expression(B[msy])), lty=c(4,5), cex=leg.cex, bty='n')
  }
    
   box(which='figure')
  if(export!=F){
   dev.off()
  }
}


##' Gadget: Plot of fit between survey indices and model
##' Plots the fit, either as a xy-scatter with a fitted line
##' (plot=1) or with data and fit on y-axis and years on x-axis
##'
##' 
##' @title gadget.plot.likefit 
##' @param likefit a data frmae created with gadget.result.likefit()
##' @param plot type of plot.  plot=1 creates a xy-scatter and plot=2 creates a
##' the same data with years on the x-axis.
##' @param add  Should the plot be added to existing plot.
##' @param axlab.cex Size of axis labels
##' @param lab.cex Size of 'length-group' text in upper left corner of plot.
##' @param yr.cex Size of year points on plot 1
##' @param fitline.col Colour of the fitline
##' @param fitline.lty Lintype of the fitline
##' @param fitline.lwd Width/thickness of fit line
##' @param corr Add the correlation to the plot.  The correlation is calculated as
##' cor(index, predN).
##' @param corr.cex Size of the correlation text
##' @param corr.col Colour of the correlation text
##' @param yr.bin Interval between labels on x-axis on plot=2
##' @param xlegend Legend on X-axis, usually 'Number in stock' on plot=1 or 'Year'
##' on plot=2
##' @param ylegend Legend on Y axis, usually 'Index' or something of that sort. 
##' @param leg.cex Size of the legend (xlegend, ylegend)
##' @param xline Line of the xlegend 
##' @param yline Line of the ylegend 
##' @param tcross Should a terminal year cross (tcross) be plotted.  On plot=1 tcross
##' comes as a cross at the terminal year but as a line between the terminal fitted line
##' and the terminal index value.
##' @param tcross.col Colour of the t.cross
##' @param tcross.lwd Width (thickness) of the tcross
##' @param tcross.lty Type of line for the tcross
##' @param index.line Should a line be plotted along with the index data in plot=2
##' @param ind.line.lty  The type of index line
##' @param ind.line.col  The colour of the index.line
##' @param mp2 The positions of the labels on the axes, mpg=c(0,mp2,0)
##' @param mfrow Number of panels (subplots) on the graphsheet (window), see ?par
##' @param omi Outer margin, see ?par
##' @param mar Margin of subplots, see ?par
##' @return A plot
##' @author Gudmundur Thordarson
gadget.plot.likefit<-function(likefit, plot=1, add=F, axlab.cex=0.6, lab.cex=1.2, yr.cex=0.7,
                              fitline.col='red', fitline.lty=1, fitline.lwd=3,
                              corr=T, corr.cex=0.9, corr.col='black', yr.bin=3,
                              xlegend="Stock", ylegend="Index", leg.cex=1.5,
                              xline=0.4, yline=0.6, tcross=T, tcross.col='green',
                              tcross.lwd=3, tcross.lty=1, index.line=TRUE, ind.line.lty=2,
				ind.line.col='black',mp2=0.2,
                              mfrow=c(3,3), omi=rep(0.4,4), mar=c(2,2,0,0)){
if(add==F){ 
  par(omi=omi, mar=mar)
  lab<-unique(likefit$label)
  par(mfrow=mfrow)
}
  if(plot==1){
    for(i in 1:length(lab)) {
      tmp1 <- likefit[likefit$label==lab[i],]
      plot(tmp1$number,tmp1$index,type="n",xlim=c(0,max(tmp1$number)),
           ylim=c(0,max(tmp1$index)),xlab="",ylab="",cex.axis=axlab.cex, mgp=c(0,mp2,0), tck=0.03)
      if(tcross==T){
        abline(v=tmp1$number[tmp1$year==max(tmp1$year)], col=tcross.col, lty=tcross.lty, lwd=tcross.lwd)
        abline(h=tmp1$index[tmp1$year==max(tmp1$year)], col=tcross.col, lty=tcross.lty, lwd=tcross.lwd)
      }
      abline(lsfit(tmp1$number,tmp1$index,intercept=F),lwd=fitline.lwd ,col=fitline.col, lty=fitline.lty)
      text(tmp1$number,tmp1$index,substring(tmp1$year,3,4),cex=yr.cex)
      if(corr==T){
      text(0.8*max(tmp1$number), 0.05*max(tmp1$index), paste('Corr=',round(cor(tmp1$number,tmp1$index),3),sep=""),
           cex=corr.cex, col=corr.col)
    }
      text(0.01*max(tmp1$number), 0.95*max(tmp1$index), lab[i], cex=lab.cex, adj=0)
    }
    mtext(side=1, xlegend, cex=leg.cex, line=xline, outer=T)
    mtext(side=2, ylegend, cex=leg.cex, line=yline, outer=T)
  }
  if(plot==2){
    for(i in 1:length(lab)) {
      tmp1 <- likefit[likefit$label==lab[i],]
      plot(tmp1$year,tmp1$index,type="n",
           ylim=c(0,max(tmp1$index)*1.4), xlab="",ylab="",cex.axis=axlab.cex, xaxt="n", mgp=c(0,mp2,0), tck=0.03)
      yr<-seq(min(tmp1$year), max(tmp1$year), yr.bin)
      axis(1, labels=substr(yr,3,4), at=yr, cex.axis=axlab.cex, mgp=c(0,mp2,0), tck=0.03)
      text(min(tmp1$year)+round(length(tmp1$year)*0.05), max(tmp1$index)*1.3, lab[i],
       cex=lab.cex, adj=0)
    if(tcross==T){
      lines(c(rep(tmp1$year[tmp1$year==max(tmp1$year)],2)),c(tmp1$index[tmp1$year==max(tmp1$year)],
                              tmp1$predN[tmp1$year==max(tmp1$year)]), col=tcross.col,
            lty=tcross.lty, lwd=tcross.lwd)
    }
    lines(tmp1$year, tmp1$index, lty=ind.line.lty, col=ind.line.col)
    points(tmp1$year, tmp1$index, pch=16, cex=yr.cex, col='black')
    lines(tmp1$year, tmp1$predN, lwd=fitline.lwd, col=fitline.col, lty=fitline.lty)
      if(corr==T){
      text(min(tmp1$year)+round(length(tmp1$year)*0.05), 0.07*max(tmp1$index),
           paste('Corr=',round(cor(tmp1$number,tmp1$index),3),sep=""), adj=0,
           cex=corr.cex, col=corr.col)
    }
    }
    mtext(side=1, xlegend, cex=leg.cex, line=xline, outer=T)
    mtext(side=2, ylegend, cex=leg.cex, line=yline, outer=T)
  }
}


##' Gadget: Predict selection function 'expsuitfuncL50'
##'
##' 
##' @title gadget.predict.expsuitfuncL50
##' @param length a vector of lengths to predict selection for
##' @param par parameters of the function alpha and L50
##' @return a vector of proportions
##' @author Gudmundur Thordarson
##' @note This selection function is the typical s-shaped
##' selection curve often assumed to describe selection in fisheries
##' with one noteble exception that the alpha parameter is multiplied
##' with 4
##' @examples
##' sel.par<-c(35,0.03)
##' le<-1:100
##' plot(le,gadget.predict.expsuitfuncL50(le, sel.par), type='l')
gadget.predict.expsuitfuncL50 <- function(length,par)
  return(1/(1+exp(-4*par[2]*(length-par[1]))))

##' Gadget: read likelihood file
##'
##' 
##' @title gadget.read.likeli 
##' @param file a gadget likelihood file (lik.out)
##' @param boundfile 
##' @return a list containing the following elements
##' 
##' $ncomp the number of likelihood components
##' 
##' $components' A table that lists the components, their type,
##' initial weight, start sse, the percent of the initial SSE, the minimum
##' SSE and the percent of the minimum.
##' 
##' $switchnames
##' 
##' $nvar Number of parameters
##' 
##' $minimum The estimated parameter values after minimisation
##' 
##' $funcval
##' 
##' $start  The parameter values before minimisation
##' @author Hoskuldur Bjornsson (Most likely)
gadget.read.likeli<- function(file, boundfile) {
    tmpfile <- tempfile("lik")
    on.exit(unlink(tmpfile))
    x <- scan(file, what = character(),sep="\n")
    i <- grep("Listing of the switches",x)
    i1 <- grep("Listing of the likelihood components",x)
    i2 <- grep("Listing of the output from the likelihood",x)
    ind  <- x[(i+1):(i1-2)]
    nvar <- length(ind)
    switchnames <- matrix(" ",length(ind),2)
    for(i in 1:length(ind)) switchnames[i,] <- Split.text.in.two(ind[i])
    ind <- x[(i1+3):(i2-2)]
    write(ind,file=tmpfile)
    components <- read.table(tmpfile,row.names=NULL,col.names=c("component","type","weight"),as.is=T)
    ncomp <- length(ind)
    data <- read.table(file,skip=(i2+1))
#    names(data)[ncol(data)] <- "total"
    names(data) <- c("nr",switchnames[,1],components$component,"total")
    start <- data[1,]
    minimum <- data[data$total==min(data$total),]
    if(nrow(minimum) > 1) minimum <- minimum[1,]
    print(components);print(start[(nvar+2):(length(start)-1)])
    components$start <- unlist(start[(nvar+2):(length(start)-1)])
    components$start <- components$start*components$weight
    components$percent.start <- components$start/sum(components$start)*100
    components$minimum <- unlist(minimum[(nvar+2):(length(minimum)-1)])
    components$minimum <- components$minimum*components$weight
    components$percent.minimum <- components$minimum/sum(components$minimum)*100
    
    funcval <- data$total
    result <- list(ncomp=ncomp,components=components,switchnames=switchnames,
                   nvar=nvar,minimum=minimum,funcval=funcval,start=start)
    write.table(minimum[2:(nvar+1)],file="tmpminimum",sep=" ", row.names=FALSE, col.names=FALSE)
    if(!missing(boundfile)){
      tmp <- read.table(boundfile,col.names=c("switches","value","lower","upper","optimize"),
                        row.names=NULL,as.is=T,skip=1)
      tmp1 <- data.frame(switches=switchnames[,1],newvalue=c(unlist(minimum[2:(nvar+1)])))
      tmp <- merge(tmp,tmp1)
      tmp <- tmp[,c("switches","newvalue","lower","upper","optimize")]
      write("switch  value   lower   upper   optimize",file="refinp.tmp")
      write.table(tmp,file="refinp.tmp",sep="\t",append=T,dimnames.write=F)
    }
    return(result)
  }

Split.text.in.two  <- function(txt,sep="\t") {
  res <- rep("",2)
  n <- nchar(txt)
  for( i in 1:n) {
    if(!is.na(match(substring(txt,i,i),sep))){
      res[1] <- substring(txt,1,i-1)
      res[2] <- substring(txt,i+1,n-1)
      return(res)
    }
  }
}

##' Read output from gadget likelihoodprinter
##'
##' 
##' @title gadget.read.likelihoodprinter
##' @param path Path to the file
##' @param file Name of the file
##' @return Dataframe containing the result of a likelihood printer
##' along with estimates of slopem intercept and SSE
##' @author Hoskuldur Bjornsson, Gudmundur Thordarson
gadget.read.likelihoodprinter <- function(file, path=F) {
  if(path!=F){
  mfile <- paste(path,file,sep="")
	}else{
	     mfile<-file
		}
  x <- scan(mfile,what=character(),sep="\n")
  n1 <- length(x) 
  i <- grep(";",x)
  x <- x[-i]
   x <- matrix(x,length(x),1)
  write.table(x,file="tmp",sep="\t", row.names = FALSE,
                 col.names = FALSE,  quote = FALSE)
 
  tmp <- read.table("tmp",col.names = c("year", "step", "area", "label", "number"))
  n <- length(unique(tmp$label))
  x <- read.table(mfile,skip=n1-n,row.names=NULL)
  x <- x[,c("V2","V4","V6","V8")]
  names(x) <- c("label","intercept","slope","sse")
  tmp <- join(tmp,x,"label")
  return(tmp)
}

##' Read GADGET parameter file
##'
##' 
##' @title gadget.read.par 
##' @param path path to the file
##' @param para the file, usually called something like 'params.in'
##' @param ro Should the parameter values be rounded, if ro=T
##' then the values are ronded to 2 digits
##' @return data.frame
##' @author Gudmundur Thordarson
gadget.read.par<-function(path, para, ro = F)
{
	tmp <- scan(paste(path, para, sep = ""), what='numeric', multi.line = F)
	st <- grep("switch", tmp)
	tmp2 <- tmp[st:length(tmp)]
	nam <- tmp2[1:5]
	tmp2 <- matrix(tmp2[6:length(tmp2)], ncol = 5, byrow = T)
	k <- data.frame(matrix(as.numeric(tmp2[, 2:5]), ncol = 4, byrow = F))
	if(ro == T) {
		round(k)
	}
	j <- data.frame(tmp2[, 1])
	jk <- cbind(j, k)
	dimnames(jk)[2] <- list(nam)
	return(jk)
}

##' Gadget: read otput from predatorpreyprinter
##'
##' 
##' @title gadget.read.predatorpreyprinter
##' @param predfile from a gadget predatorpreyprinter
##' @return dataframe
##' @author Gudmundur Thordarson
gadget.read.predatorpreyprinter<-function(predfile){
  tmp<-scan(predfile, sep="\n", what=character())
  j<-grep('year-step', tmp)
  cn<-c('year','step','area','age','length','number.consumed','biomass.consumed','mortality')
  tmp <- read.table(predfile,skip=j,col.names=cn)
  return(tmp)
}
##' Gadget, read output from a stockstdprinter
##'
##' 
##' @title gadget.read.stdstock
##' @param stockfiles print out from a stockstdprinter
##' @param stock Name of stock
##' @return a dataframe
##' @author Hoskuldur Bjornsson (most likely)
gadget.read.stdstock <- function(stockfiles,stock=rep(1,length(stockfiles))){
  res <- NULL
  for(i in 1:length(stockfiles)) {
    print(i)
    tmp <- scan( stockfiles[i], sep = "\n", what = character())
     j <- grep("year-step",tmp)
     cn <- c("year","step","area","age","number","mean.length","mean.weight",
             "stddev.length","number.consumed","biomass.consumed")
     tmp <- read.table(stockfiles[i],skip=j,col.names=cn)
     tmp$stock <- rep(stock[i],nrow(tmp))
     res <- rbind(res,tmp)
   }
  return(res)
}
##' Gadget, read output from a stockprinter
##'
##' 
##' @title gadget.read.stock
##' @param stockfiles print out from a stockprinter
##' @param stock Name of stock
##' @return a dataframe
##' @author Hoskuldur Bjornsson (most likely)
gadget.read.stock <- function(stockfiles,stock=rep(1,length(stockfiles))){
  res <- NULL
  for(i in 1:length(stockfiles)) {
    print(i)
    tmp <- scan( stockfiles[i], sep = "\n", what = character())
     j <- grep("year-step",tmp)
     cn <- c("year","step","area","age","length","number","weight")
     tmp <- read.table(stockfiles[i],skip=j,col.names=cn)
     tmp$biomass <- tmp$number*tmp$weight
     tmp$stock <- rep(stock[i],nrow(tmp))
     res <- rbind(res,tmp)
   }
  return(res)
}

##' Gadget:  Function that prepares the result from a likelihoodprinter
##' for plotting fit to the data or residual plot.
##'
##' 
##' @title gadget.result.likefit
##' @param likecomp Name of the likelihood components to be read.  Usually in the '/out'
##' folder
##' @param data The survey data file
##' @param path Path to where the likelihood and data files reside
##' @param label.substr This is to extract the first value in a length group.  For example
##' 'len20-29' becomes 20.  This is then used as bases for assaigning length for the lengthgroup
##' @param le.add Number to be added to label.substr.  Example if 5 then $le will become 25.
##' @return A dataframe
##' @author Gudmundur Thordarson
gadget.result.likefit<-function(likecomp, data, path, label.substr=c(4,5),le.add=5){
  lc<-NULL
  for(i in 1:length(likecomp)){
    j<-gadget.read.likelihoodprinter(path=paste(path,sep=""),file=likecomp[i])
    lc<-rbind(lc, j)
  }
  surdat <- read.table(paste(path, data, sep="/"),
                   col.names=c("year","step","area","label","index"))
  tmp <- merge(lc,surdat)
  tmp$predN <- exp(tmp$intercept)*tmp$number^tmp$slope
  tmp$number <- tmp$number/1e6
  tmp$index <- tmp$index/1000
  tmp$predN <- tmp$predN/1000
  tmp$resid <- log(tmp$index/tmp$predN)
  tmp$le<-as.numeric(substring(tmp$label,label.substr[1],label.substr[2]))+le.add

  return(tmp)
}



##' Gadget:  Get typical fisheries information
##' by year, such as SSB, Harvestable biomass,
##' total biomass, recruitment and fishing
##' mortality (Fbar)
##'
##' @title gadget.result.year 
##' @param std.file Either a R-object from reading in
##' stdstockprinter throug the function 'gadget.read.stdstock'
##' or simply the path to the output from gadget stdstockprinter
##' usually found in 'out' folder.
##' @param step At which step should the summary be prepeared for, default step=1
##' @param Rage Age at gecruitment
##' @param bioplus Should the biomass be total biomass (bioplus=0) or maybe something
##' else like b4+ (bioplus=4).
##' @param trim.yr Should the last years be trimmed off, useful for
##' getting rid of future years (prognosis)
##' @param mat.par Maturation parameters from a logistic curve (glm, binomial) on the
##' form of c(intercept, slope)
##' @param harv.par Selection parameters for the expsuitfuncL50 used to define the
##' part of the stock that is available to the fleet.  The input form is c(L50, alpha).
##' @param Fbar Over which age-groups should the the average F be calculated.  Takes the form
##' of c(lower.age, higher.age)
##' @param ro Rounding of SSB, Harv and Bio.  Default is three digits.
##' @param roF Rounding of Fbar, default is 4 digits.
##' @param M Value of natural mortality (M), default is 0.2
##' @param import Should the stdstock file be imported, see 'std.file'
##' @return A data frame containing the columns: 'year', 'biomass', 'harv', 'ssb',
##' 'recr', 'catch' and 'Fbar'
##' 
##' @author Gudmundur Thordarson
gadget.result.year<-function(std.file, step=1, Rage=3, bioplus=0, trim.yr=F,mat.par, harv.par,
                             Fbar=c(4,8), ro=3, roF=4, M=0.2, import=F){
  if (import == T) {
        tmp <- gadget.read.stdstock(std.file)
    }
    else {
        tmp <- std.file
    }
    tmp<-tmp[tmp$age>0,]
    yr <- sort(unique(tmp$year))
    catch <- tapply(tmp$biomass.consumed, tmp$year, sum)
    i <- tmp$step == step
    ii <- tmp$step == step & tmp$age >= bioplus
    biomass <- tapply(tmp$number[ii] * tmp$mean.weight[ii], tmp$year[ii], 
        sum)
    ssb <- tapply(tmp$number[i] * mat.fuct(tmp$mean.length[i], 
        mat.par[1], mat.par[2]), tmp$year[i], sum)
    harv <- tapply(tmp$number[i] * gadget.predict.expsuitfuncL50(tmp$mean.length[i], 
        harv.par) * tmp$mean.weight[i], tmp$year[i], sum)
    rec <- tmp$number[tmp$step == step & tmp$age == Rage]
    tusk.F <- NULL
    for (i in yr[1:(length(yr)-1)]) {
        N0 <- tmp[tmp$year == i & tmp$step == 1, ]
        N1 <- tmp[tmp$year == i + 1 & tmp$step == 1, ]
        Fy <- round(log(N0$number[1:(nrow(N0)-1)]) -
                    log(N1$number[2:(nrow(N1))]), 5) - M
        Fy <- ifelse(Fy < 0, 0, Fy)
        N0$Fy <- c(Fy, Fy[length(Fy)])
        tusk.F <- rbind(tusk.F, N0)
    }
    i <- tusk.F$age >= Fbar[1] & tusk.F$age <= Fbar[2]
    F.bar <- c(tapply(tusk.F$Fy[i], tusk.F$year[i], mean),NA)
    result <- data.frame(year = yr, biomass = round(biomass/1e+06, 
        ro), harv = round(harv/1e+06, ro), ssb = round(ssb/1e+06, 
        ro), recr = round(rec/1e+06, ro), catch = round(catch/1e+06, 
        ro), Fbar = round(F.bar, roF))
    if (trim.yr != F) {
        result <- result[result$year <= trim.yr, ]
    }
    return(result)
}


##' Gadget: Logistic curve
##'
##' Mainly used internaly.
##' 
##' @title mat.fuct
##' @param Lengd a vector of lengths
##' @param a The intercept from a logistic regression
##' @param b The slope from a logistic regression
##' @return A vector of predicted proportions
##' @author Gudmundur Thordarson
mat.fuct<-function(Lengd, a = -8.8, b = 0.25)
{
	sel <- exp(a + Lengd * b)/(1. + exp(a + Lengd * b))
}


##' Residual plot (Bubbles)
##'
##' 
##' @title residplot
##' @param residuals A dataframe containing the residuals to be plotted 
##' @param xpos 
##' @param ypos 
##' @param maxsize maximum size of bubbles, default 0.2 but looks better at 0.1
##' @param poscol Colour of positive residuals (solid circles)
##' @param linecol Clours of the circles, outer margin of circles, default black
##' @param lwd Width of circle lines
##' @param n 'Resolution of circles'
##' @param maxn 
##' @param negcol Colour of negative residuals (open circles)
##' @param txt Sould the values be printed inside the circles (bubbles)
##' @param csi Size of the 'txt' letters
##' @param ro rounding of 'txt'
##' @param txt.col Colour of 'txt'
##' @param xlab X-axis legend
##' @param ylab Y-axis legend
##' @param axes Should axes be plotted, default=T
##' @param arg Should diagonal lines be plotted, good for age based residuals
##' @param argcol Colur of the line
##' @param arglty Line type of the diagonal lines
##' @param cn Column names to be used in the residplot (optional), see example
##' @param append Add to existing plot
##' @return A residual (bubble) plot
##' @author Hoskuldur Bjornsson (I presume)
##' @examples
##' yr<-1980:1990; agegr=1:10
##' tmp<-data.frame(yr=rep(yr, each=10),age=rep(agegr, length(yr)))
##' tmp$resid<-rnorm(nrow(tmp), 0, 0.3)
##' residplot(tmp,cn=c("yr","age","resid"),maxn=1,poscol='red', negcol='blue', ylab="Age",
##'           n=50, arg=T, argcol='black',maxsize=0.3, txt=T, xlab='Year')
##' 
residplot<-function(residuals, xpos, ypos, maxsize = 0.2, poscol = 'black', linecol = 'black', lwd = 1,
	n = 50, maxn, negcol, txt = F, csi = 1, ro=1,txt.col='black', xlab = "", ylab = "", axes = 
	T, arg = T, argcol = 'lightblue', arglty = 2, cn = c("x", "y", "z"), append = F
	)
{
	par(err = -1)
	if(is.data.frame(residuals)) {
		x <- residuals[, cn[1]]
		y <- residuals[, cn[2]]
		residuals <- residuals[, cn[3]]
	}
	else {
		residuals <- t(residuals)
		if(missing(maxn))
			maxn <- max(abs(residuals), na.rm = T)
		if(missing(xpos))
			xpos <- 1:nrow(residuals)
		if(missing(ypos))
			ypos <- 1:ncol(residuals)
		x <- matrix(xpos, length(xpos), length(ypos))
		y <- matrix(ypos, length(xpos), length(ypos), byrow = T)
	}
	if(!append)
		plot(x, y, type = "n", xlab = xlab, ylab = ylab, axes = axes)
	x.bck <- x
	y.bck <- y
	if(arg) {
		r <- x.bck - y.bck
		tmp <- unique(r)
		for(i in 1:length(tmp)) {
			j <- r == tmp[i]
			lines(x.bck[j], y.bck[j], col = argcol, lty = arglty)
		}
	}
	plt <- par()$pin
	xscale <- (par()$usr[2] - par()$usr[1])/plt[1] * maxsize
	yscale <- (par()$usr[4] - par()$usr[3])/plt[2] * maxsize
	rx <- c(unlist(sqrt(abs(residuals)/maxn) * xscale))
	ry <- c(unlist(sqrt(abs(residuals)/maxn) * yscale))
	theta <- seq(0, 2 * pi, length = n)
	n1 <- length(rx)
	theta <- matrix(theta, n1, n, byrow = T)
	x <- matrix(x, n1, n)
	y <- matrix(y, n1, n)
	rx <- matrix(rx, n1, n)
	ry <- matrix(ry, n1, n)
	x <- x + rx * cos(theta)
	y <- y + ry * sin(theta)
	x <- cbind(x, rep(NA, nrow(x)))
	y <- cbind(y, rep(NA, nrow(y)))
	i <- residuals > 0
	if(any(i)) {
		polygon(c(t(x[i,  ])), c(t(y[i,  ])), col = poscol)
		lines(c(t(x[i,  ])), c(t(y[i,  ])), col = linecol, lwd = lwd)
	}
	i <- residuals < 0
	if(any(i)) {
		if(!missing(negcol))
			polygon(c(t(x[i,  ])), c(t(y[i,  ])), col = negcol)
		lines(c(t(x[i,  ])), c(t(y[i,  ])), col = linecol, lwd = lwd)
	}
	if(txt)
		text(x.bck, y.bck, as.character(round(residuals, ro)), cex = csi, col=txt.col)
	return(invisible())
}

