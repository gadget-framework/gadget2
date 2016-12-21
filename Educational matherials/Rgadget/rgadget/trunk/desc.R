#' Multispecies model in R
#'
#' \tabular{ll}{
#' Package: \tab RGadget\cr
#' Type: \tab Package\cr
#' Version: \tab 0.1\cr
#' Date: \tab 2009-06-19\cr
#' License: \tab GPL (>= 2)\cr
#' LazyLoad: \tab yes\cr
#' }
#'
#' This program imitates a Gadget model with the possibility of having
#' at most 2 stocks, movement, 2 fleets (catch and survey),
#' 1 areas and consumption.
#' Surveys are made at predefined timesteps, and movements between
#' immature and the mature stock are in the last timestep of
#' each year.
#' Other calculations like growth, natural mortality and catch
#' are done in every timestep.
#'
#' @name RGadget
#' @aliases RGadget
#' @docType package
#' @title Gadget in R
#' @author Bjarki Thor Elvarsson
#' @references \url{http://www.hafro.is/gadget}
#' @keywords package multispecies modeling
#' @seealso \code{DO SOME STUFF}
#' @examples
#' sim <- Rgadget()
roxygen()
