conf.gadget.hess <- function(params.out = 'params.out',
                             likelihood = 'likelihood',
                             quantiles = c(0.025,0.25,0.5,0.75,0.975),
                             hess
                             ){
  #hess <- run.hessegadget(h=h)
  likelihood <- read.gadget.likelihood(likelihood)
  lik.dat <- read.gadget.data(likelihood)

  params.out <- read.gadget.parameters(params.out)
  params.out <- subset(params.out,optimise == 1)
  df <- sum(unlist(lik.dat$df)) - length(params.out$value) 
  fish.mat <- solve(hess$hessian[params.out$switch,
                                 params.out$switch])
  res.total <-
    params.out$value +
      sqrt(diag(fish.mat)*hess$sum.sq/df) %o% qnorm(quantiles)
  res.total <- as.data.frame(cbind('total score',
                                   rownames(res.total),
                                   res.total))
  names(res.total)[-c(1:2)] <- paste('q',quantiles,sep='')
if(FALSE){
  lik.out <- read.gadget.lik.out('lik.hess')
  restr <- !(lik.out$weights$Component %in%
             c('alkeys.igfs', 'alkeys.comm','alkeys.aut'))
  utkoma <- as.matrix(lik.out$data[lik.out$weights$Component[restr]])%*%as.numeric(lik.out$weights$Weight[restr])
  hesmat <- hesse(lik.out$data[params.out$switch],utkoma,hess$h)
  dimnames(hesmat) <- list(x=params.out$switch,y=params.out$switch)
  fish.mat <- solve(hesmat[params.out$switch,
                           params.out$switch])
  res.alk <- params.out$value +
    sqrt(diag(fish.mat)*utkoma[1]/df) %o% qnorm(quantiles)
  res.alk <- as.data.frame(cbind('w/o alk',
                                 rownames(res.alk),
                                 res.alk))
  names(res.alk)[-c(1:2)] <- paste('q',quantiles,sep='')
  res <- rbind(res.total,res.alk)
  names(res)[1] <- 'method'
  names(res)[2] <- 'parameter'
}
  return(list(res=res,hess=hess))
}


##' This function uses all the functions in the file hessegadget.
##' @title run.hessegadget
##' @param file.in is the gadget file that contains the parameters
##' @param result the name of the file that the results are wanted in
##' @param h is a number or a vector that contains the interval lenght in the grid.If nothing is chosen for h
##' the h will be set to sqrt(eps)*vec where eps is the machine epsilon. Eps is about 2.2e-16 in R.
##' @param location output file location
##' @return is the hessian matrix
run.hessegadget <- function(file.in = "params.out",
                            file.out = 'hess.in',
                            params.hess = '.hess',
                            result = "lik.hess",
                            location='.',
                            main.file = 'main',
                            h=1e-4){
  options(digits=19)
  tmp <- read.gadget.parameters(file.in)
  gogn <- matrix(tmp$value,ncol=length(tmp$value),nrow=1,
                 dimnames=list(value='value',swithes=tmp$switch))
  if(length(h) == 1)
    h <- abs(h*tmp$value)
  hgrid <- hesse.grid(tmp$value,h=h)
  grid.gadget(gogn,hgrid,location=location,file=file.out)
  callGadget(s=1,i=file.out,p=params.hess,o=result,
             main=main.file)
  y <- read.gadget.lik.out(result)$data$score
  hesmat <- hesse(tmp,y,h=h)

  return(list(sum.sq=y[1],df.used=dim(tmp)[1],hessian=hesmat,h=h))
}
##' hesse.grid
##'
##' This function takes in a vector and makes a grid out of it. Its main purpose is to use it for the hesse function.
##' The hesse function uses the function values of the lines in the grid.
##' @param vec is a vector 
##' @param h is a number or a vector that contains the interval lenght in the grid. If nothing is chosen for h
##' the h will be set to sqrt(eps)*vec where eps is the machine epsilon. Eps is about 2.2e-16 in R.
##' @return the grid

hesse.grid <- function(vec,h=TRUE){
  options(digits = 19)
  
  if (length(h) == 1){
    if (h==TRUE){
      h <- sqrt(.Machine$double.eps)*vec
    } else {
      h <- rep(h,length(vec))
    }
  }

q<-length(vec)
#The first part of the matrix.
#This part is used for second order derivatives.
  m<-t(matrix(rep(vec,2*q+1),q,2*q+1))  
  for(i in 2*(1:q)){
    m[i,i/2]<-m[i,i/2]-h[i/2]
    m[i+1,i/2]<-m[i+1,i/2]+h[i/2]
  }

#Zero matrix
  r=0
  for (i in 1:(q-1)){
    r=r+i
  }
  m<-rbind(m,matrix(rep(0,q*r*4),r*4,q))

#Add the second part of the matrix.
#This part is used for second order mixed derivatives.
  for(i in 1:q){
     for(j in (2*q+2):(r*4+2*q+1)){
     m[j,i]<-vec[i]
     }
  }

  p=0
  i= 2*q+2
  for (j in 1:(q-1)){
    s=q-j
    while (s>0){
      m[i+p,j]<-m[i+p,j]+h[j]
      m[i+1+p,j]<-m[i+1+p,j]+h[j]
      m[i+2+p,j]<-m[i+2+p,j]-h[j]
      m[i+3+p,j]<-m[i+3+p,j]-h[j]
      p=p+4
      s=s-1
    }
  }
  s=2
  p=0
  while (s<=q){
    for (j in s:q){
      m[i+p,j]<-m[i+p,j]+h[j]
      m[i+1+p,j]<-m[i+p+1,j]-h[j]
      m[i+2+p,j]<-m[i+p+2,j]+h[j]
      m[i+3+p,j]<-m[i+p+3,j]-h[j]
      p=p+4
    }
    s=s+1
  }
  
if(FALSE){
  
  q <- length(vec)
  
  ## The first part of the matrix.
  ## This part is used for second order derivatives.
  m <- matrix(rep(vec,each=2*q+1),2*q+1,q)
  m[2*(1:q),] <- m[2*(1:q),] + diag(h)
  m[2*(1:q)+1,] <- m[2*(1:q)+1,] - diag(h)

  ## Add the second part of the matrix.
  ## This part is used for second order mixed derivatives.  
  r <- sum(1:(q-1))
  m <- rbind(m,matrix(rep(vec,each=r*4),r*4,q))


  p <- 0
  i <- 2*q+2
  
  for (j in 1:(q-1)){
    s <- 4*((q - j - 1):0) + p  ## update these elements in colum j
    p <- 4*(q - j)
    m[i+s,j] <- m[i+s,j]+h[j]
    m[i+1+s,j] <- m[i+1+s,j]+h[j]
    m[i+2+s,j] <- m[i+2+s,j]-h[j]
    m[i+3+s,j] <- m[i+3+s,j]-h[j]
  }

  p <- 0
  for (s in 2:q){
    for (j in s:q){
#    j <- s:q
    m[i+p,j] <- m[i+p,j]+h[j]
    m[i+1+p,j] <- m[i+p+1,j]-h[j]
    m[i+2+p,j] <- m[i+p+2,j]+h[j]
    m[i+3+p,j] <- m[i+p+3,j]-h[j]
    p=p+4
    }

  }
}
  return(m)
} 
 
##' hesse
##'
##' This function takes in a vector and makes a grid out of it. Its main purpose is to use it for the hesse function.
##' The hesse function uses the function values of the lines in the grid.
##' @param vec is a vector with the points that the hesse matrix is to be calculated at 
##' @param y is a vector with the function values of the grid that is made from the vector vec and
##'        the function hesse.grid
##' @param h is a number or a vector that contains the interval lenght in the grid. If nothing is chosen for h
##' the h will be set to sqrt(eps)*vec where eps is the machine epsilon. Eps is about 2.2e-16 in R.
##' @return the hessian matrix
hesse <- function(vec,y,h=TRUE){
  options(digits=19)
  q <- length(vec)
  if(length(h) == 1){
    if (h==TRUE)
      h <- sqrt(.Machine$double.eps)*vec
    else
      h <- rep(h,q)
  }
  denom <- 4*h%o%h
  diag(denom) <- h^2
  
  ##The hessian matrix
  ##Start with the diagonal
  hmat <- diag(y[2*(1:q)] + y[2*(1:q) +1] - 2*y[1])
  #grad <- (y[2*(1:q)] - y[2*(1:q) +1])/(2*h)

  ##Add the mixed partial derivatives
  p <- 2*q+2
  tmp <- tail(y,-p)
  hmat[lower.tri(hmat)] <- -diff(diff(tmp))[4*(1:sum(1:(q-1))) - 3]
#  for (i in 1:(q-1)){
#    for (j in (i+1):q){
#      ## hmat df(...,x_i,...,x_j,...)/dx_i dx_j
#      hmat[i,j]<- y[p]-y[p+1]-y[p+2]+y[p+3])
#      p <- p+4
#    }
#  }
  
  ##Copy to the lower part of the diagonal matrix
  hmattrans<-t(hmat)
  diag(hmattrans) <- 0
  hmat <- (hmat + hmattrans)/denom
}
  dimnames(hmat) <- list(x=vec$switch,y=vec$switch)

  return(hmat)
}


##' read.gadget
##'
##' This function reads gadget files both files that are input to gadget and output.
##' @param file is the gadget file that is supposed to be read
##' @param input is a TRUE or FALSE parameter. If the gadget file is a output from gadget
##' the parameter is needed to be switched to FALSE.
##' @return the gadget file as a matrix or a vector
read.gadget <- function(file,input=TRUE){
     tmp <- readLines(file)
     {
     if (input==TRUE){
         tmp1 <- unlist(strsplit(tmp,split=" +"))
         tmp2 <- tmp1[2:length(tmp1)]
         ncol=length(tmp2)/length(tmp)
         nrow=length(tmp)
         data1 <- matrix(tmp2,ncol=ncol,nrow=nrow,byrow=TRUE)
         data <- matrix(as.numeric(data1[-1,]),ncol=ncol,
                        nrow=nrow-1,byrow=TRUE,
                        dimnames=list(rep("NULL",nrow(data1)-1),data1[1,]))
       } else {
         lengths <- sapply(strsplit(tmp,' '),length)
         skip <- max(which(lengths==min(lengths)))
         tmp <- tmp[(skip+1):length(tmp)]
         tmp1 <- strsplit(tmp,split="\t")
         row1 <- unlist(lapply(tmp1, function(x) x[lapply(tmp1,length)[[1]]]))
         data <- as.numeric(row1)
       }
     }
     return(data)
}

##' grid.gadget
##'
##' This function adds a grid to the input file for gadget so it will be in the right format to
##' be read into gadget
##' @param gfile is the former gadget file that the grid is supposed to be added to
##' @param ggrid is the grid that is to be added to the gadget file
##' @param location is the the folder that the file should be saved in
##' @param file is a string containing the name of the file

grid.gadget<-function(gfile,ggrid,location='',file='tmp'){
  param.names <- unlist(list("switches",dimnames(gfile)[[2]]))
  tmp <- data.frame(ggrid)
  write.table(data.frame(param.names),file=paste('.',location,file,sep='/'),
              quote=FALSE, row.names=FALSE, col.names=FALSE, sep="\n",eol = "\t",)
  write.table('',file=paste('.',location,file,sep='/'),quote=FALSE, row.names=FALSE, col.names=FALSE,append=TRUE)
  write.table(data.frame(tmp),file=paste('.',location,file,sep='/'),row.names=FALSE,col.names=FALSE,append=TRUE)

}
