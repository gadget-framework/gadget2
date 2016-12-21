\name{Rgadget}
\alias{Rgadget}
\title{Rgadget}
\usage{Rgadget(opt=gadget.options())}
\description{Rgadget is the driver function for the ecosystem simulation. It takes a
gadget options list, where all parameters of the simulation are set, as
input and runs the simulation for a set period of time (the length of the
timesteps and number of observations (years) are also defined in gadget
options).}
\details{The Rgadget simulator replicates some of the more commonly used features of
Gadget, with use of some of these features optional. The most complex model
possible consists of two substocks, with the younger substock optionally
maturing into the older substock at a given age and timestep, ie the
maturation process is not modelled. Both substocks are subject to growth
and natural mortality and live on a number of areas. Two fleets are possible, eg
one representing the commercial fleet and the other an annual survey, with
the timesteps on which these fleets operate optional. There can be
migration between the areas. The mature substock can also predate upon the
immature stock. As with Gadget, the fleets are modelled as predators. The
order in which the population processes are modelled on each timestep is
identical to that in Gadget.

As Gadget is a forward simulation model, it is started with an initial
population. The length distribution of each age group of the population
is assumed to be normally distributed with a given mean and standard
deviation (specified by the user). The youngest age group in each year
is entered into the population in a similar manner.
Fleets are modelled as predators, as in Gadget, and operate on some or all
areas and at some or all timesteps which are defined by the user.
Consumption (predation or harvesting) implemented through length based
suitability functions of the form:
\deqn{S_{pred,prey}(L,l) = \frac{\delta}{1+e^{-\alpha-\beta l-\gamma L}}}
where of l is the prey length and L predator length. For fleets L should be
irrelevant and therefore \eqn{\gamma = 0}{gamma  = 0} for fleets.

Maturation from stock A to stock B is modelled by moving the oldest
agegroup of A into with the age increasing if done on the last timestep
of the year. This replicates the Gadget process \emph{doesmove}.

Growth follows a beta-binomial distribution with mean update length as
stipulated by the von Bertanlanffy curve.

The order of calculations is the same as in Gadget and is as follows:

1. Migration between areas

2. Consumption, including catch by the fleets

3. Natural mortality

4. Growth

5. Recruitment

6. Remove the stock, here immature, that will move

7. Increase the age

8. Replace the stock, here to the mature, that has moved and increase the age.}
\value{a list of arrays:
\item{Rec}{Recruits for all years}
\item{immStart}{Initial starting population for the immature stock age 2 and older}
\item{matStart}{Initial starting population for the mature stock age 2 and older}
\item{immNumRec}{Immature stock population for all timesteps, areas, ages and lengths}
\item{matNumRec}{Mature stock population for all timesteps, areas, ages and lengths}
\item{immCsurv}{Survey catches of the immature stock}
\item{matCsurv}{Survey catches of the mature stock}
\item{immCcomm}{Commercial catches of the immature stock}
\item{matCcomm}{Commercial catches of the mature stock}
\item{Eat}{Amount consumed of immatures by matures}
\item{GrowthProb}{Growthprobability matrix}
\item{immMort}{Natural mortality for the immature stock}
\item{matMort}{Natural mortality for the mature stock}
\item{opt}{Gadget options list used in the simulation}}
\author{Bjarki Thor Elvarsson, Asta Jenny Sigurdardottir and Elinborg Ingunn Olafsdottir}
\arguments{\item{opt}{gadget options list, as defined by 'gadget.options'}}
\examples{opt <- gadget.options()
sim <- Rgadget(sim)}
