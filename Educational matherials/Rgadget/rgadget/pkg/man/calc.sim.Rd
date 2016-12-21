\name{calc.sim}
\alias{calc.sim}
\alias{age.length.key}
\alias{catch.in.kilos}
\alias{as.data.frame.gadget.sim}
\alias{length.at.age}
\alias{lengthDist}
\alias{survey.index}
\alias{survey.indexlen}

\title{Calculated indices}
\usage{\method{age.length.key}{gadget.sim}(sim, age.agg, len.agg)
\method{catch.in.kilos}{gadget.sim}(sim)
\method{as.data.frame}{gadget.sim}(sim)
\method{length.at.age}{gadget.sim}(sim)
\method{lengthDist}{gadget.sim}(sim, sigma=0)
\method{survey.index}{gadget.sim}(sim, sigma=0)
\method{survey.indexlen}{gadget.sim}(sim, length.groups=c(4, 14, 90), sigma=0)}

\description{These functions calculate various indices or aggregations
  of the output from the \emph{Rgadget} simulation function. The output
  from \emph{Rgadget} is denoted as \eqn{C_{fsralt}},  where \eqn{C} is
  the catch from fleet \eqn{f}, \eqn{s} is the substock, \eqn{a} age,
  \eqn{r} area (region), \eqn{l} lengthgroup and \eqn{t} time.  
  \emph{age.length.key} calculates the age-length-key, \eqn{K_{fsrALt}},
  for the survey and commercial fleet based on given length \eqn{L} and
  \eqn{A} age aggregates formulated as:
  \deqn{K_{fsrALt} = \sum_{a\in A}\sum_{l \in L} C_{fsralt}}

  \emph{catch.in.kilos} calculates the overall weight of the landings by
  fleet, time and area according to the length weight relationship
  defined in the \emph{gadget.options} list. 

  \emph{as.data.frame} formats the output from RGadget to a dataframe
  and calculates the weight according to the length-weight relationship
  defined by the \emph{gadget.options} object.

  \emph{length.at.age} produces the average length at age,
  \eqn{L_{fsrat}}, calculated according to
  \deqn{L_{fsrat} = \frac{\sum_{l} l C_{fsralt}}{\sum_{l} C_{fsralt}}}

  \emph{lengthDist} calculates the length distribution,
  \eqn{\lambda_{fsrlt}} from the fleets by lengthgroups and time
  according to:
  \deqn{\lambda_{fsrlt} = \sum_{a} C_{fsralt}}

  \emph{survey.index} calculates the
  survey indices for the simulated stocks based on abundance at age 1
  and abundance of the rest of the population. The age 1 index is the
  key source of information on the relative abundance of the recruits,
  whereas the index of older fish (given information on growth and
  mortality) helps determine the overall abundance of recruitment. These
  indices represent the first timestep of each year and were generated
  with varying degrees of error. Index data are output separately for
  all areas with the two age groups. If \eqn{U^e _{fArt}} is the survey index
  in area \eqn{r} at timestep \eqn{t}, agegrouping \eqn{A} (either 1
  year or \eqn{2^+}), with error applied, then
  \deqn{U^{e}_{fArt} = e^{\left(\varepsilon -\frac{\sigma^2}{2}\right)}
  \sum_l \sum_{a\in A} C_{fsralt}}
  where \eqn{\varepsilon \sim n(0,\sigma^2)} with fleet \eqn{f} fixed to
  the survey fleet.

  \emph{survey.indexlen} similarly calculates the survey length index
  based on user provided length-aggregation.} 
\value{Dataframe containing the desired results}
\author{Bjarki Thor Elvarsson and Lorna Taylor}
\arguments{\item{sim}{Results from a Rgadget simulation}
\item{age.agg}{The desired age aggregation}
\item{len.agg}{The desired length aggregation}
\item{sigma}{sigma for a log-normal noise for the indices}
\item{length.groups}{A vector of cutoff points for the length groups}}
\examples{
## set up
opt <- gadget.options()
sim <- Rgadget(opt)
## dataframe
simDat <- as.data.frame(sim)
## survey index
sI <- survey.index(sim)
}
