\name{Rgadget.summary}
\alias{Rgadget.summary}
\alias{plot.Rgadget}
\alias{plot.gadget.options}
\alias{summmary.gadget.options}
\title{Rgadget summary functions}
\usage{\method{plot}{Rgadget}(dat, compare.alk=TRUE)
\method{plot}{gadget.options}(opt)
\method{summary}{gadget.options}(opt)}
\description{Summary and plot functions for Rgadget. For a 
  gadget.options object plotting it results in a plot of the suitability
  functions for the fleets, the length weight relationship and the
  growth function for both substocks. For an Rgadget object a lattice
  plot is produced of the relevant index by the appropriate dimensions. }
\author{Bjarki Thor Elvarsson}
\arguments{\item{dat}{A Rgadget object}
  \item{opt}{A gadget options object}}
\examples{
## create the options list
opt <- gadget.options()
plot(opt)
## new simulation
sim <- Rgadget(opt)
## calculate a survey index
sI <- survey.index(sim)
plot(sI)
}
