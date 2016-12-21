Vmig6 <- function(XfU,XfL,YfU,YfL,XiU,XiL,YiU,YiL){

  f1x <- function(w,u,D,betax,delt){
    return((1/sqrt(pi))*exp(-(w-betax*delt-u)*(w-betax*delt-u)/(4*D*delt)))
  }
  
  f2x <- function(w,u,D,betax,delt){
    erf <- function(x) 2 * pnorm(x * sqrt(2)) - 1
    return((w-betax*delt-u)*erf((w-betax*delt-u)/(2*sqrt(D*delt))))
  }
  
  
  ##' Notations
  ##' 
  ##' LX = distance between exterior "walls", on OX
  ##' LY = same, on OY
  ##' XiL, XiU = x-coordinates ("Low and Upp") of the initial area ("small rectangle!"")
  ##' XfL, XfU = x-coordinates ("Low and Upp") of the final area ("small rectangle!")
  ##' YiL, YiU = y-coordinates ("Low and Upp") of the initial area ("small rectangle!")
  ##' YfL, YfU = y-coordinates ("Low and Upp") of the final area ("small rectangle!")
  ##' Delt = time interval
  ##' DD = first parameter (diffusion coefficient)
  ##' betax = second parameter (drift along Ox)
  ##' betay = third parameter (drift along Oy)
  migration <- function(xiU,xiL,yiU,yiL,xfU,xfL,
                         yfU,yfL,lx,ly,D,betax,betay,delt){
    lambda <- 1
    ##if dist between exterior walls is: lx
    fx <- 0
    fy <- 0
    for(j in -30:30){
      betax1 <- betax - (2*j*lx)/delt
      ##correcting signs : 
      fx <- fx + ( -f1x(xfU,xiU,D,betax1,delt)+
                  f1x(xfU,xiL,D,betax1,delt) +
                  f1x(xfL,xiU,D,betax1,delt) -
                  f1x(xfL,xiL, D,betax1,delt) -
                  f2x(xfU,xiU,D,betax1,delt) +
                  f2x(xfU,xiL,D,betax1,delt) +
                  f2x(xfL,xiU,D,betax1,delt) -
                  f2x(xfL,xiL, D,betax1,delt) ) +
                    (-f1x(xfU,xiU,D,-betax1,delt)+
                     f1x(xfU,xiL,D,-betax1,delt) +
                     f1x(xfL,xiU,D,-betax1,delt) -
                     f1x(xfL,xiL, D,-betax1,delt) -
                     f2x(xfU,xiU,D,-betax1,delt) +
                     f2x(xfU,xiL,D,-betax1,delt) +
                     f2x(xfL,xiU,D,-betax1,delt) -
                     f2x(xfL,xiL, D,-betax1,delt) )
    }
    
    for(j in -30:30){
      betay1 <- betay - (2*j*ly)/delt
      fy <- fy + (  -f1x(yfU,yiU,D,betay1,delt) +
                  f1x(yfU,yiL,D,betay1,delt) +
                  f1x(yfL,yiU,D,betay1,delt) -
                  f1x(yfL,yiL,D,betay1,delt) -
                  f2x(yfU,yiU,D,betay1,delt) +
                  f2x(yfU,yiL,D,betay1,delt) +
                  f2x(yfL,yiU,D,betay1,delt) -
                  f2x(yfL,yiL,D,betay1,delt) ) + 
                    ( -f1x(yfU,yiU,D,-betay1,delt) +
                     f1x(yfU,yiL,D,-betay1,delt) +
                     f1x(yfL,yiU,D,-betay1,delt) -
                     f1x(yfL,yiL,D,-betay1,delt) -
                     f2x(yfU,yiU,D,-betay1,delt) +
                     f2x(yfU,yiL,D,-betay1,delt) +
                     f2x(yfL,yiU,D,-betay1,delt) -
                     f2x(yfL,yiL,D,-betay1,delt) )
    }
  return(fx*fy)
  }
  
  lambda <- 1
  
  ## FIRST: give the information about how far the walls are (and this is roughly correct position) Also, this is always the same thing, never changes:    
  
  XXL <- 5
  XXU <- 45
  
  YYL <- 55
  YYU <- 75
  
  LX <- XXU-XXL
  LY <- YYU-YYL  
  
  ## and about "Iceland" - forbidden area, probability of migration here is 0: 
  
  
  XIsL <- 15
  XIsU <- 24
  YIsL <- 64
  YIsU <- 66
  
  
  ##   THEN: give the information about the areas between which migration is studied:
  
  ## final "small rectangle":
  
                                        #  XfU <- 22
                                        #  XfL <- 5 
                                        #  YfU <- 58
                                        #  YfL <- 55 
  
  
  
  ## initial "small rectangle":
  
                                        #  XiU <- 40
                                        #  XiL <- 26
                                        #  YiU <- 75
                                        #  YiL <- 65
  
  
  ## (let us say we have decided about the measuring units) 
  
  Betax <- 0.002
  Betay <- 0.0048
  DD <- 0.0053
  Delt <- 40000
  
  ## these parameters are initialized and then changed during 
  ## optimization, but for a test... */
  
  ##  hints about parameter values:
  
  ## the product Betax*Delt should have a comparable order of magnitude with XiU(L) or XfU(L);
  ## same for Betay*Delt and YiU(L), YfU(L);
  ## sqrt(DD*Delt) should also be not too much smaller than some (xf-xi - betax*Delt)
  
  
  
  
  ff <- migration(XiU, XiL, YiU, YiL, XfU, XfL, YfU, YfL, LX,LY, DD, Betax, Betay, Delt)
  ## new normaliz. : to have a measure!, so integral on all domain is 1!  
  
  normaliz <- migration(XiU, XiL, YiU, YiL, XXU, XXL, YYU, YYL, LX,LY, DD, Betax, Betay, Delt)
  return(ff/normaliz)
}
