\name{Rgadget-package}
\alias{Rgadget-package}
\alias{Rgadget}
\docType{package}
\title{
  Ecosystem simulator
}
\description{
  Rgadget is a simple ecosystem simulator that can simulate a multi-area,
  two-stock ecosystem with optional fleet operations in each
  area. Rgadget is an independent implementation of the ecosystem
  simulator in Gadget (http://www.hafro.is/gadget).
}
\details{
  \tabular{ll}{
    Package: \tab Rgadget\cr
    Type: \tab Package\cr
    Version: \tab 0.1\cr
    Date: \tab 2011-02-21\cr
    License: \tab GPL\cr
    LazyLoad: \tab yes\cr
  }
  Rgadget is a simple two substock simulator that allows the user to
  define the principle processies in the ecosystem such as migration,
  harvesting, predation and growth. The main motivation behind the
  package is twofold:

  Scenario analysis: it could be used to study the different assumptions
  of the interaction between the two substocks and harvesting
  strategies.

  Input generation: it could be used to generate input data to more
  complicated modelling frameworks, such as Gadget.

  The simulation parameters are defined in the function 'gadget.options'
  and the user can change those parameters in any way. The simulation is
  run in the function 'Rgadget' and the output from the simulation can
  be studied via various built-in index functions or plots.
}
\author{
  Bjarki Thor Elvarsson, Asta Jenny Sigurdardottir, Elinborg Ingunn
  Olafssdottir and Lorna Taylor

  Maintainer: Bjarki Thor Elvarsson <bthe@hafro.is>
}

\keyword{ package, Rgadget, Ecosystem, multi-species }
\examples{
## initialize the simulation parameters
opt <- gadget.options()
summary(opt)
## Run the simulation
sim <- Rgadget(opt)
## View the output
sI <- survey.index(sim)
plot(sI)
}
